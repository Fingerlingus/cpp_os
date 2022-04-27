#include "stdlib/cstdlib.hpp"

#include "page_table.hpp"

namespace mem {

#ifdef K_PAGE_SIZE
    inline static constexpr std::size_t PAGE_SIZE = K_PAGE_SIZE;
#else
    inline static constexpr std::size_t PAGE_SIZE = 4096;
#endif

inline static constexpr std::size_t PT_SIZE = 0x200000;

void page_table::init() {
	// zero all tables
    memset(m_ptes,    0, NUM_PTES);
    memset(m_pdtes,   0, NUM_PDTES);
    memset(m_pdptes,  0, NUM_PDTES);
    memset(m_pml4tes, 0, NUM_PDTES);
    //memset(m_pml5tes,       0, NUM_PDTES);
	// ...and zero phys_addr_map
    memset(m_phys_addr_map.data(), 0, NUM_PHYS_ADDR_MAP_ENTRIES);

	// identity map first 16MB, leave the rest to be allocated on demand
	m_pml4tes[0] = ((uint64_t)&m_pdptes & ~0xFFF) | 0x03;
	m_pdptes[0]  = ((uint64_t)&m_pdtes  & ~0xFFF) | 0x03;
	for (int i = 0; i < 16; i++)
		m_pdtes[i] = ((uint64_t)&m_ptes[i * 512] & ~0xFFF) | 0x03;

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 512; j++) {
			m_ptes[i * 512 + j] = 
                (((PAGE_SIZE * j) + PT_SIZE * i) & ~0xFFF) | 0x03;
        }
	}
}

bool page_table::is_physically_allocated(const void* const phys_addr) {
	if ((uint64_t)phys_addr & 0xFFF)
		return true;

	uint64_t addr = (uint64_t)phys_addr;
	if (addr < 0x1000000) // memory under 16MB guaranteed to be identity mapped
		return true;

	if (phys_addr == m_last_mapped_phys_addr)
		return true;

	addr &= ~0xFFF; // 4KB-align address
	addr >>= 12;
	uint16_t page_index = addr / m_phys_addr_map.size();
	uint8_t  page_pos   = addr % 64;
	return (m_phys_addr_map[page_index] & (1 << page_pos)) != 0x00000000;
}

bool page_table::is_virtually_allocated(const void* const virt_addr) {
	if ((uint64_t)virt_addr & 0xFFF)
		return true;

	uint64_t virt_addr_u64 = (uint64_t)virt_addr;

	if (virt_addr_u64 < 0x1000000)
        return 1; // memory under 16MB guaranteed to be identity mapped

	uint16_t pml4t_index = (virt_addr_u64 >> 39) & 0x01FF;
	uint16_t pdpt_index  = (virt_addr_u64 >> 30) & 0x01FF;
	uint16_t pdt_index   = (virt_addr_u64 >> 21) & 0x01FF;
	uint16_t pt_index    = (virt_addr_u64 >> 12) & 0x01FF;

	// if entries aren't present, page can't be allocated
	if ((m_pml4tes[pml4t_index] & 0x01) == 0x00)
		return false;

	if ((m_pdptes[(pml4t_index * 512) + 
                   pdpt_index
                 ] & 0x01) == 0x00)
		return false;

	if ((m_pdtes[(pml4t_index * 512 * 512) + 
                 (pdpt_index * 512) + 
                 pdt_index
                ] & 0x01) == 0x00)
		return false;

	if ((m_ptes[(pml4t_index * 512 * 512 * 512) + 
                (pdpt_index * 512 * 512) + 
                (pdt_index * 512) +
                pt_index
               ] & 0x01) == 0x00)
		return false;

	return true;
}

bool page_table::map_phys_addr(void* phys_addr, void* virt_addr) {
	uint64_t phys_addr_u64 = (uint64_t)phys_addr;
	uint64_t virt_addr_u64 = (uint64_t)virt_addr;
	uint16_t pml4t_index = (virt_addr_u64 >> 39) & 0x01FF;
	uint16_t pdpt_index  = (virt_addr_u64 >> 30) & 0x01FF;
	uint16_t pdt_index   = (virt_addr_u64 >> 21) & 0x01FF;
	uint16_t pt_index    = (virt_addr_u64 >> 12) & 0x01FF;

	if (virt_addr_u64 & 0xFFF)
		return false;

	if (phys_addr_u64 & 0xFFF)
		return false;

	// bail if page is already allocated
	if (is_virtually_allocated(virt_addr))
		return false;

    if ((m_pml4tes[pml4t_index] & 0x03) != 0x03) {
		 m_pml4tes[pml4t_index] = 
            (uint64_t)&m_pdptes[(pml4t_index * 512) + pdpt_index] | 0x03;
    }

    if ((m_pdptes[(pml4t_index * 512) + pdpt_index] & 0x03) != 0x03) { 
		 m_pdptes[(pml4t_index * 512) + pdpt_index] = 
            (uint64_t)&m_pdtes[(pml4t_index * 512 * 512) + 
                               (pdpt_index * 512) +
                                pdt_index
                              ] | 0x03;
    }

    if ((m_pdtes[(pml4t_index * 512 * 512) + 
                 (pdpt_index * 512) + 
                  pdt_index] & 0x03) != 0x03) 
    { 
		 m_pdtes[(pml4t_index * 512 * 512) + 
                 (pdpt_index * 512) + 
                 pdt_index] = 
            (uint64_t)&m_ptes[(pml4t_index * 512 * 512 * 512) + 
                              (pdpt_index * 512 * 512) +
                              (pdt_index * 512) +
                              pt_index
                             ] | 0x03;
    }

    if ((m_pdtes[(pml4t_index * 512 * 512 * 512) + 
                 (pdpt_index * 512 * 512) + 
                 (pdt_index * 512) +
                 pt_index] & 0x03) != 0x03) 
    { 
		 m_pdtes[(pml4t_index * 512 * 512 * 512) + 
                 (pdpt_index * 512 * 512) + 
                 (pdt_index * 512) +
                  pt_index
                ] = phys_addr_u64 | 0x03;
    }

	phys_addr_u64 /= 4096;
	uint16_t page_index = phys_addr_u64 / 64;
	uint8_t  page_pos = phys_addr_u64 % 64;
	m_phys_addr_map[page_index] |= (1 << page_pos);
	return true;
}

void* page_table::alloc_page(void* virt_addr) {
	// if no specific address is supplied, get a random one
	if (virt_addr == nullptr) {
		uint64_t i;
		if (m_last_mapped_virt_addr == nullptr)
			i = 0x1000000;
		else
			i = (uint64_t)m_last_mapped_virt_addr + 4096;

		for (; i < reinterpret_cast<std::size_t>(MAX_VIRTUAL_MEMORY); 
               i += 4096) 
        {
			// free virtual address
			if (is_virtually_allocated((void*)i) == false) {
				uint64_t j;
				if (m_last_mapped_phys_addr == nullptr)
					j = 0x1000000;
				else
					j = (uint64_t)m_last_mapped_phys_addr + 4096;

				for (; j < reinterpret_cast<std::size_t>(MAX_VIRTUAL_MEMORY); 
                       j += 4096) 
                {
					// find free physical address
					if (is_physically_allocated((void*)j) == false) {
						// and map them
						if (map_phys_addr((void*)j, (void*)i) == false)
							return nullptr;
						m_last_mapped_virt_addr = (void*)i;
						m_last_mapped_phys_addr = (void*)j;
                        for(auto& r : m_allocated_pages) {
                            if(r == nullptr)
                                r = reinterpret_cast<void*>(i);
                        }
						return (void*)i;
					}
				}
			}
		}
	}
	// else use address supplied if possible
	else {
		if ((uint64_t)virt_addr & 0xFFF)
			return nullptr;

		if (is_virtually_allocated(virt_addr))
			return nullptr;

		uint64_t i;
		if (m_last_mapped_phys_addr == nullptr)
			i = 0x1000000;
		else
			i = (uint64_t)m_last_mapped_phys_addr + 4096;

		// find free physical address
		for (; i < reinterpret_cast<std::size_t>(MAX_VIRTUAL_MEMORY); 
               i += 4096) 
        {
			if (!is_physically_allocated((void*)i)) {
				// ...and map them
				if (map_phys_addr((void*)i, virt_addr) == false)
					return nullptr;
				m_last_mapped_virt_addr = (void*)virt_addr;
				m_last_mapped_phys_addr = (void*)i;
                for(auto& r : m_allocated_pages) {
                    if(r == nullptr)
                        r = virt_addr;
                }
				return virt_addr;
			}
		}
	}
	return nullptr;
}

void* page_table::alloc_pages(void* virt_addr, std::size_t num_pages) {
    if(num_pages == 1) {
        if(is_virtually_allocated(virt_addr))
            return virt_addr;
        
        return alloc_page(virt_addr);
    }
    
    char* c_virt_addr = (char*)virt_addr;
    
    if(virt_addr != nullptr) {
        for(std::size_t i = 0; i < num_pages; i++) {
            if(!is_virtually_allocated((void*)c_virt_addr))
                alloc_page((void*)c_virt_addr);
            c_virt_addr += 4096;
        }
        return virt_addr;
    }

    while(c_virt_addr + (num_pages * PAGE_SIZE) < MAX_VIRTUAL_MEMORY) {
        bool b = false;
        for(std::size_t i = 0; i < num_pages; i++) {
            if(is_virtually_allocated((void*)c_virt_addr)) {
                b = true;
                break;
            }
        }
        if(b == false)
            return alloc_pages((void*)c_virt_addr, num_pages);
    }
    return nullptr;
}


bool page_table::dealloc_pages(void* virt_addr, std::size_t num_pages) {
    bool b = true;
    for(std::size_t i = 0; i < num_pages; i++) {
        if(dealloc_page(virt_addr) == false)
            b = false;
    }
    return b;
}

bool page_table::unmap_virt_addr(void* virt_addr) {
	if ((uint64_t)virt_addr & 0xFFF)
		return false;

	// bail if page is already allocated
	if (is_virtually_allocated(virt_addr))
		return false;

	uint64_t virt_addr_u64 = (uint64_t)virt_addr;
	uint16_t pml4t_index = (virt_addr_u64 >> 39) & 0x01FF;
	uint16_t pdpt_index  = (virt_addr_u64 >> 30) & 0x01FF;
	uint16_t pdt_index   = (virt_addr_u64 >> 21) & 0x01FF;
	uint16_t pt_index    = (virt_addr_u64 >> 12) & 0x01FF;

	// clear PT entry
	m_ptes[(pml4t_index * 512 * 512 * 512) +
           (pdpt_index * 512 * 512) +
           (pdt_index * 512) + 
            pt_index
          ] = 0x00000000;

	// get phys addr
	void* phys_addr = (void*)((virt_addr_u64 & 0xFFF) | 
                      ((pt_index    & 0xFFF) << 12)   | 
                      ((pdt_index   & 0x1FF) << 21)   |
                      ((pdpt_index  & 0x1FF) << 30)   |
                      ((pml4t_index & 0x1FF) << 31));
	uint64_t phys_addr_u64 = (uint64_t)phys_addr;
	phys_addr_u64 /= 4096;

	// and clear bit in phys_addr_map
	uint16_t page_index = phys_addr_u64 / m_phys_addr_map.size();
	uint8_t  page_pos   = phys_addr_u64 % 64;
	m_phys_addr_map[page_index] &= ~(1 << page_pos);

	return true;
}

bool page_table::dealloc_page(void* virt_addr) {
	if ((uint64_t)virt_addr & ~0xFFF)
		return false;

	if (!is_virtually_allocated(virt_addr))
		return false;

	uint64_t addr = (uint64_t)virt_addr;

	uint16_t pml4t_index = (addr >> 39) & 0x01FF;
	uint16_t pdpt_index  = (addr >> 30) & 0x01FF;
	uint16_t pdt_index   = (addr >> 21) & 0x01FF;
	uint16_t pt_index    = (addr >> 12) & 0x01FF;

	// bail if entries are not present
	if ((m_pml4tes[pml4t_index] & 0x01) == 0x00000000)
		return false;

	if ((m_pdptes[(pml4t_index * 512) + 
                   pdpt_index
                 ] & 0x01) == 0x00000000)
		return false;

	if ((m_pdtes[(pml4t_index * 512 * 512) +
                 (pdpt_index * 512) +
                  pdt_index
                ] & 0x01) == 0x00000000)
		return false;

	if ((m_ptes[(pml4t_index * 512 * 512 * 512) + 
                (pdpt_index * 512 * 512) +
                (pt_index * 512) + 
                 pt_index
               ] & 0x01) == 0x00000000)
		return false;

	// zero PT entry
    m_ptes[(pml4t_index * 512 * 512 * 512) + 
           (pdpt_index * 512 * 512) +
           (pt_index * 512) + 
            pt_index
           ] = 0x00000000;
	// and unmap virtual address
	return unmap_virt_addr(virt_addr);
}

} // namespace mem
