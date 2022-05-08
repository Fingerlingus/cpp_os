#include <climits>

#include "stdlib/cstdlib.hpp"

#include "page_table.hpp"

#include "util.hpp"

namespace mem {

#ifdef K_PAGE_SIZE
    inline static constexpr std::size_t PAGE_SIZE = K_PAGE_SIZE;
#else
    inline static constexpr std::size_t PAGE_SIZE = 4096;
#endif

inline static constexpr std::size_t PT_SIZE = 512 * PAGE_SIZE;

void page_table::init() {
	// zero all tables
	memset_safe(&m_ptes, 0);
	memset_safe(&m_pdtes, 0);
	memset_safe(&m_pdptes, 0);
	memset_safe(&m_pml4tes, 0);
	//memset_safe(&m_pml5tes, 0);
	// ...and zero phys_addr_map
	memset_safe(m_phys_addr_map, 0);

	// identity map first 16MB, leave the rest to be allocated on demand
	m_pml4tes[0]    = ((uint64_t)(&m_pdptes[0][0]) & ~0xFFF) | 0x03;
	m_pdptes[0][0]  = ((uint64_t)(&m_pdtes[0][0][0]) & ~0xFFF) | 0x03;
	for (int i = 0; i < 8; i++)
		m_pdtes[0][0][i] = ((uint64_t)(&m_ptes[0][0][i][0]) & ~0xFFF) | 0x03;

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 512; j++) {
			m_ptes[0][0][i][j] = 
                (((i * PT_SIZE) + (j * PAGE_SIZE)) & ~0xFFF) | 0x03;
        }
	}
}

bool page_table::is_physically_allocated(physical_address phys_addr) const {
	if (phys_addr & 0xFFF)
		return true;

	if (phys_addr < 16_mb) // memory under 16MB guaranteed to be identity mapped
		return true;

	if (phys_addr == m_last_mapped_phys_addr)
		return true;

	phys_addr &= ~0xFFF; // 4KB-align address
	phys_addr /= PAGE_SIZE;
	uintptr_t page_index = reinterpret_cast<uintptr_t>(phys_addr / m_phys_addr_map.size());
	uintptr_t page_pos   = reinterpret_cast<uintptr_t>(phys_addr % SIZE_IN_BITS(*m_phys_addr_map.data()));
	return (m_phys_addr_map[page_index] & (1 << page_pos)) != 0x00000000;
}

bool page_table::is_virtually_allocated(virtual_address virt_addr) const {
	if (virt_addr & 0xFFF)
		return true;

	if (virt_addr < 0x1000000)
        return 1; // memory under 16MB guaranteed to be identity mapped

	uint16_t pml4t_index = (virt_addr >> 39) & 0x01FF;
	uint16_t pdpt_index  = (virt_addr >> 30) & 0x01FF;
	uint16_t pdt_index   = (virt_addr >> 21) & 0x01FF;
	uint16_t pt_index    = (virt_addr >> 12) & 0x01FF;

	// if entries aren't present, page can't be allocated
	if ((m_pml4tes[pml4t_index] & 0x01) == 0x00)
		return false;

	if ((m_pdptes[pml4t_index][pdpt_index] & 0x01) == 0x00)
		return false;

	if ((m_pdtes[pml4t_index][pdpt_index][pdt_index] & 0x01) == 0x00)
		return false;

	if ((m_ptes[pml4t_index][pdpt_index][pdt_index][pt_index] & 0x01) == 0x00)
		return false;

	return true;
}

bool page_table::map_phys_addr(physical_address phys_addr, virtual_address virt_addr) {
	uint16_t pml4t_index   = (virt_addr >> 39) & 0x01FF;
	uint16_t pdpt_index    = (virt_addr >> 30) & 0x01FF;
	uint16_t pdt_index     = (virt_addr >> 21) & 0x01FF;
	uint16_t pt_index      = (virt_addr >> 12) & 0x01FF;

	if (virt_addr & 0xFFF)
		return false;

	if (phys_addr & 0xFFF)
		return false;

	// bail if page is already allocated
	if (is_virtually_allocated(virt_addr))
		return false;

    if ((m_pml4tes[pml4t_index] & 0x03) != 0x03) {
		 m_pml4tes[pml4t_index] = 
            (uint64_t)(&m_pdptes[pml4t_index][pdpt_index]) | 0x03;
    }

    if ((m_pdptes[pml4t_index][pdpt_index] & 0x03) != 0x03) { 
		 m_pdptes[pml4t_index][pdpt_index] = 
            (uint64_t)(&m_pdtes[pml4t_index][pdpt_index][pdt_index]) | 0x03;
    }

    if ((m_pdtes[pml4t_index][pdpt_index][pdt_index] & 0x03) != 0x03) { 
		 m_pdtes[pml4t_index][pdpt_index][pdt_index] = 
            (uint64_t)(&m_ptes[pml4t_index][pdpt_index][pdt_index][pt_index]) |
                0x03;
    }

    if ((m_ptes[pml4t_index][pdpt_index][pdt_index][pt_index] & 0x03) != 0x03)
    {
		 m_ptes[pml4t_index][pdpt_index][pdt_index][pt_index] = 
             phys_addr | 0x03;
    }

	phys_addr /= PAGE_SIZE;
	uint16_t page_index = phys_addr / NUM_PHYS_ADDR_MAP_ENTRIES;
	uint8_t  page_pos   = phys_addr % SIZE_IN_BITS(*m_phys_addr_map.data());
	m_phys_addr_map[page_index] |= (1 << page_pos);
	return true;
}

void* page_table::alloc_page(virtual_address virt_addr) {
	// else use address supplied if possible
	if (virt_addr & 0xFFF)
		return nullptr;

	// if no specific address is supplied, get a random one
	if (IS_NULL(virt_addr)) {
		virt_addr = find_free_virt_addr();
		if (IS_NULL(virt_addr))
			return nullptr;
	}

	if (is_virtually_allocated(virt_addr))
		return const_cast<void*>(virt_addr.const_ptr());

	physical_address phys_addr = find_free_phys_addr();
	if (IS_NULL(phys_addr))
		return nullptr;
	
	// ...and map them
	if (ERROR(map_phys_addr(phys_addr, virt_addr)))
		return nullptr;
	m_last_mapped_virt_addr = const_cast<void*>(virt_addr.const_ptr());
	m_last_mapped_phys_addr = const_cast<void*>(phys_addr.const_ptr());
    for(auto& r : m_allocated_pages) {
		if(IS_NULL(r))
			r = const_cast<void*>(virt_addr.const_ptr());
    }
	return const_cast<void*>(virt_addr.const_ptr());
}

void* page_table::alloc_pages(virtual_address virt_addr, std::size_t num_pages) {
    if(num_pages == 1) {
		if (is_virtually_allocated(virt_addr))
			return const_cast<void*>(virt_addr.const_ptr());
        
        return alloc_page(virt_addr);
    }
    
    char* c_virt_addr = (char*)virt_addr.const_ptr();
    
    if(virt_addr != nullptr) {
        for(std::size_t i = 0; i < num_pages; i++) {
            if(!is_virtually_allocated(static_cast<void*>(c_virt_addr)))
                alloc_page(static_cast<void*>(c_virt_addr));
            c_virt_addr += PAGE_SIZE;
        }
		return const_cast<void*>(virt_addr.const_ptr());
    }

    while(c_virt_addr + (num_pages * PAGE_SIZE) < (char*)MAX_VIRTUAL_MEMORY) {
        bool b = false;
        for(std::size_t i = 0; i < num_pages; i++) {
            if(is_virtually_allocated(static_cast<void*>(c_virt_addr))) {
                b = true;
                break;
            }
        }
        if(b == false)
            return alloc_pages(static_cast<void*>(c_virt_addr), num_pages);
    }
    return nullptr;
}

bool page_table::unmap_virt_addr(virtual_address virt_addr) {
	if (virt_addr & 0xFFF)
		return false;

	// bail if page is already allocated
	if (is_virtually_allocated(virt_addr))
		return false;

	uint16_t pml4t_index = (virt_addr >> 39) & 0x01FF;
	uint16_t pdpt_index  = (virt_addr >> 30) & 0x01FF;
	uint16_t pdt_index   = (virt_addr >> 21) & 0x01FF;
	uint16_t pt_index    = (virt_addr >> 12) & 0x01FF;

	// clear PT entry
	m_ptes[pml4t_index][pdpt_index][pdt_index][pt_index] = 0x00000000;

	return true;
}

bool page_table::unmap_phys_addr(physical_address phys_addr) {
	if (phys_addr & 0xFFF)
		return false;

	if (!is_physically_allocated(phys_addr))
		return false;

	phys_addr /= PAGE_SIZE;

	// and clear bit in phys_addr_map
	uint16_t page_index = phys_addr / NUM_PHYS_ADDR_MAP_ENTRIES;
	uint8_t  page_pos = phys_addr % SIZE_IN_BITS(*m_phys_addr_map.data());
	m_phys_addr_map[page_index] &= ~(1 << page_pos);
	return true;
}

bool page_table::dealloc_page(virtual_address virt_addr) {
	if (virt_addr & ~0xFFF)
		return false;

	if (!is_virtually_allocated(virt_addr))
		return false;

	uint16_t pml4t_index = (virt_addr >> 39) & 0x01FF;
	uint16_t pdpt_index  = (virt_addr >> 30) & 0x01FF;
	uint16_t pdt_index   = (virt_addr >> 21) & 0x01FF;
	uint16_t pt_index    = (virt_addr >> 12) & 0x01FF;

	// bail if entries are not present
	if ((m_pml4tes[pml4t_index] & 0x01) == 0x00000000)
		return false;

	if ((m_pdptes[pml4t_index][pdpt_index] & 0x01) == 0x00000000)
		return false;

	if ((m_pdtes[pml4t_index][pdpt_index][pdt_index] & 0x01) == 0x00000000)
		return false;

	if ((m_ptes[pml4t_index][pdpt_index][pt_index][pt_index] & 0x01) == 
            0x00000000)
		return false;

	// zero PT entry
    m_ptes[pml4t_index][pdpt_index][pt_index][pt_index] = 0x00000000;
	// unmap virtual address
	if (unmap_virt_addr(virt_addr))
		return false;
	// and unmap physical address
	return unmap_phys_addr(to_phys_addr(virt_addr));
}

bool page_table::dealloc_pages(virtual_address virt_addr, std::size_t num_pages) {
	bool b = true;
	for (std::size_t i = 0; i < num_pages; i++) {
		if (ERROR(dealloc_page(virt_addr)))
			b = false;
	}
	return b;
}

page_table::virtual_address page_table::find_free_virt_addr() const {
	virtual_address test_vaddr;
	if (m_last_mapped_virt_addr == nullptr)
		test_vaddr = 16_mb;
	else
		test_vaddr = m_last_mapped_virt_addr + 4096;

	bool b = false;
	for (; test_vaddr < MAX_VIRTUAL_MEMORY; test_vaddr += 4096) {
		if (!is_virtually_allocated(test_vaddr)) {
			b = true;
			break;
		}
	}
	if (b == false)
		return nullptr;

	return test_vaddr;
}

page_table::physical_address page_table::find_free_phys_addr() const {
	physical_address test_paddr;
	if (m_last_mapped_phys_addr == nullptr)
		test_paddr = 16_mb;
	else
		test_paddr = m_last_mapped_virt_addr + 4096;

	bool b = false;
	for (; test_paddr < MAX_VIRTUAL_MEMORY; test_paddr += 4096) {
		if (!is_physically_allocated(test_paddr)) {
			b = true;
			break;
		}
	}
	if (b == false)
		return nullptr;

	return test_paddr;
}

page_table::physical_address page_table::to_phys_addr(virtual_address virt_addr) const {
	uint16_t offset = virt_addr & 0xFFF;

	uint16_t pml4t_index = (virt_addr >> 39) & 0x01FF;
	uint16_t pdpt_index  = (virt_addr >> 30) & 0x01FF;
	uint16_t pdt_index   = (virt_addr >> 21) & 0x01FF;
	uint16_t pt_index    = (virt_addr >> 12) & 0x01FF;

	// bail if entries are not present
	if ((m_pml4tes[pml4t_index] & 0x01) == 0x00000000)
		return nullptr;

	if ((m_pdptes[pml4t_index][pdpt_index] & 0x01) == 0x00000000)
		return nullptr;

	if ((m_pdtes[pml4t_index][pdpt_index][pdt_index] & 0x01) == 0x00000000)
		return nullptr;

	physical_address phys_addr =
		(m_ptes[pml4t_index][pdpt_index][pt_index][pt_index] & ~0xFFF) + offset;

	return phys_addr;
}

} // namespace mem
