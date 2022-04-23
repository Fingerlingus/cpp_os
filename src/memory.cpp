#include "memory.hpp"

#include <cstdint>

#define HEAP_BASE nullptr
#define MEMBLOCK_MAGIC 0x5A1C068D

extern "C" void* kmalloc(std::size_t size) {
	static char* free_memory_base = HEAP_BASE;
	size = (size + 7) / 8 * 8;
	free_memory_base += size;
	return free_memory_base - size;
}

extern "C" void kfree(void*) {
    return;
}
