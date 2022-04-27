#include <cstdint>
#include <new>

#include "stdlib/list.hpp"
#include "stdlib/expected.hpp"

#include "memory.hpp"
#include "page_table.hpp"

#ifndef NO_DISCARD
#	define NO_DISCARD [[nodiscard]]
#endif

bool kheap_init() {
    std::size_t num_pages = KHEAP_SIZE / 4096;
    if(KHEAP_SIZE % 4096 != 0) num_pages++;

    char* p = (char*)KHEAP_BEGIN;
    while(p != KHEAP_END) {
        if(pt->alloc_page(p) == nullptr)
            return false;
    }
    return true;
}

#ifdef __cplusplus
	extern "C" {
#endif

NO_DISCARD void* kmalloc(std::size_t size) {
    return nullptr;
}

void kfree(void* ptr) {

}

#ifdef __cplusplus
	}
#endif
