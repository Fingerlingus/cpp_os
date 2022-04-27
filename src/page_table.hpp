#pragma once

#include "stdlib/array.hpp"

#include "asm_wrappers.hpp"

namespace mem {

#ifdef K_VMEM_MAX
    inline static constexpr void* MAX_VIRTUAL_MEMORY = VMEM_MAX;
#else
    inline static constexpr void* MAX_VIRTUAL_MEMORY = 
        __builtin_constant_p((void*)0x10000000) ?
        (void*)0x10000000 : (void*)0x10000000;
#endif

inline static constexpr std::size_t MAX_PAGES =
        __builtin_constant_p((std::size_t)MAX_VIRTUAL_MEMORY / 4096) ?
        (std::size_t)MAX_VIRTUAL_MEMORY / 4096 : 
        (std::size_t)MAX_VIRTUAL_MEMORY / 4096;

class page_table {
public:
	inline page_table() { }
    void init();
	void* alloc_page(void* virt_addr = nullptr);
	void* alloc_pages(void* virt_addr, std::size_t num_pages);
	bool  dealloc_page(void* virt_addr);
	bool  dealloc_pages(void* virt_addr, std::size_t num_pages);
	bool  is_virtually_allocated(const void* const virt_addr);
	bool  is_physically_allocated(const void* const phys_addr);

	inline void* last_mapped_phys_addr() const {
		return m_last_mapped_phys_addr;
	}

	inline void* last_mapped_virt_addr() const {
		return m_last_mapped_virt_addr;
	}

	inline kstd::array<void*, MAX_PAGES>& allocated_pages() {
		return m_allocated_pages;
	}

	inline const kstd::array<void*, MAX_PAGES>& allocated_pages() const {
		return m_allocated_pages;
	}

	inline uint64_t* pml4t() {
		return m_pml4tes;
	}

    inline const uint64_t* pml4t() const {
		return m_pml4tes;
	}

    inline void activate() const {
		lcr3((void*)&m_pml4tes);
	}

private:
    inline static constexpr std::size_t NUM_PTES = 
        MAX_PAGES / 512 >= 512 ? MAX_PAGES / 512 : 512;
    inline static constexpr std::size_t NUM_PDTES = 
        NUM_PTES / 512 >= 512 ? NUM_PTES / 512 : 512;
    inline static constexpr std::size_t NUM_PDPTES = 
        NUM_PDTES / 512 >= 512 ? NUM_PDTES / 512 : 512;
    inline static constexpr std::size_t NUM_PML4TES = 
        NUM_PDPTES / 512 >= 512 ? NUM_PDPTES / 512 : 512;
    /*
    inline static constexpr NUM_PML5TES = 
        NUM_PML4TES / 512 >= 512 ? NUM_PML4TES / 512 : 512;
    */
    inline static constexpr std::size_t NUM_PHYS_ADDR_MAP_ENTRIES =
        MAX_PAGES / (8 * sizeof(uint64_t));

	uint64_t m_ptes[NUM_PTES]       __attribute__((aligned(4096)));
	uint64_t m_pdtes[NUM_PDTES]     __attribute__((aligned(4096)));
	uint64_t m_pdptes[NUM_PDPTES]   __attribute__((aligned(4096)));
	uint64_t m_pml4tes[NUM_PML4TES] __attribute__((aligned(4096)));
	//uint64_t m_pml5tes[NUM_PML5TES] __attribute__((aligned(4096)));
	void* m_last_mapped_phys_addr;
	void* m_last_mapped_virt_addr;
	kstd::array<void*, MAX_PAGES> m_allocated_pages;
    kstd::array<uint64_t, NUM_PHYS_ADDR_MAP_ENTRIES> m_phys_addr_map;

	bool unmap_virt_addr(void* virt_addr);
	bool map_phys_addr(void* phys_addr, void* virt_addr);
};

} // namespace mem

inline mem::page_table kernel_pt;
inline mem::page_table* pt = &kernel_pt;
