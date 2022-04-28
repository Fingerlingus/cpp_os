#pragma once

#include "stdlib/array.hpp"

#include "asm_wrappers.hpp"

namespace mem {

#ifdef K_VMEM_MAX
    inline static constexpr std::size_t MAX_VIRTUAL_MEMORY = VMEM_MAX;
#else
	inline static constexpr std::size_t MAX_VIRTUAL_MEMORY = 0x200000000;
#endif
inline static constexpr std::size_t MAX_PAGES = MAX_VIRTUAL_MEMORY / 4096;

inline constexpr std::size_t operator""_gb(unsigned long long s) {
	return s * 1024 * 1024 * 1024;
}

inline constexpr std::size_t operator""_mb(unsigned long long s) {
	return s * 1024 * 1024;
}

inline constexpr std::size_t operator""_kb(unsigned long long s) {
	return s * 1024;
}

inline constexpr std::size_t operator""_b(unsigned long long s) {
	return s;
}

class page_table {
public:
	inline page_table() { }
    void init();
	void* alloc_page(void* virt_addr = nullptr);
	void* alloc_pages(void* virt_addr, std::size_t num_pages);
	bool  dealloc_page(void* virt_addr);
	bool  dealloc_pages(void* virt_addr, std::size_t num_pages);
	bool  is_virtually_allocated(const void* const virt_addr) const;
	bool  is_physically_allocated(const void* const phys_addr) const;

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
	inline static constexpr std::size_t NUM_PML4TES =
		(MAX_VIRTUAL_MEMORY / 512_gb == 0 ? 1 : MAX_VIRTUAL_MEMORY / 512_gb) >= 512 ?
		512 : MAX_VIRTUAL_MEMORY / 512_gb == 0 ? 1 : MAX_VIRTUAL_MEMORY / 512_gb;

	inline static constexpr std::size_t NUM_PDPTES =
		(MAX_VIRTUAL_MEMORY / 1_gb == 0 ? 1 : MAX_VIRTUAL_MEMORY / 1_gb) >= 512 ?
		512 : MAX_VIRTUAL_MEMORY / 1_gb == 0 ? 1 : MAX_VIRTUAL_MEMORY / 1_gb;

	inline static constexpr std::size_t NUM_PDTES =
		(MAX_VIRTUAL_MEMORY / 2_mb == 0 ? 1 : MAX_VIRTUAL_MEMORY / 2_mb) >= 512 ?
		512 : MAX_VIRTUAL_MEMORY / 2_mb == 0 ? 1 : MAX_VIRTUAL_MEMORY / 2_mb;

    /*
    inline static constexpr NUM_PML5TES = 
        NUM_PML4TES / 512 >= 512 ? NUM_PML4TES / 512 : 512;
    */
    inline static constexpr std::size_t NUM_PHYS_ADDR_MAP_ENTRIES =
        MAX_PAGES / (8 * sizeof(uint64_t));

	uint64_t m_ptes[NUM_PML4TES][NUM_PDPTES][NUM_PDTES][512] __attribute__((aligned(4096)));
	uint64_t m_pdtes[NUM_PML4TES][NUM_PDPTES][512] __attribute__((aligned(4096)));
	uint64_t m_pdptes[NUM_PML4TES][512] __attribute__((aligned(4096)));
	uint64_t m_pml4tes[512] __attribute__((aligned(4096)));
	//uint64_t m_pml5tes[NUM_PML5TES] __attribute__((aligned(4096)));
	void* m_last_mapped_phys_addr;
	void* m_last_mapped_virt_addr;
	kstd::array<void*, MAX_PAGES> m_allocated_pages;
    kstd::array<uint64_t, NUM_PHYS_ADDR_MAP_ENTRIES> m_phys_addr_map;

	bool  unmap_virt_addr(void* virt_addr);
	bool  unmap_phys_addr(void* phys_addr);
	bool  map_phys_addr(void* phys_addr, void* virt_addr);
	void* find_free_virt_addr() const;
	void* find_free_phys_addr() const;
};

} // namespace mem

inline mem::page_table kernel_pt;
inline mem::page_table* pt = &kernel_pt;
