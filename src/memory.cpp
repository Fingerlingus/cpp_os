#include "memory.hpp"

#include <cstdint>
#include <new>

#ifndef NO_DISCARD
#	define NO_DISCARD [[nodiscard]]
#endif

#ifdef __cplusplus
	extern "C" {
#endif

NO_DISCARD void* kmalloc(std::size_t size) {
	static char* free_memory_base = HEAP_BASE;
	size = (size + 7) / 8 * 8;
	free_memory_base += size;
	return free_memory_base - size;
}

void kfree(void*) {
    return;
}

#ifdef __cplusplus
	}
#endif
