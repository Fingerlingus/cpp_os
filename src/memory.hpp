#pragma once

#include <cstdint>

#include "stdlib/list.hpp"

#ifndef NO_DISCARD
#	define NO_DISCARD [[nodiscard]]
#endif

constexpr uint32_t MEMBLOCK_MAGIC = 0x59A406B3;

enum class MEMBLOCK_FLAGS : uint32_t {

};

struct memblock {
    uint32_t magic = MEMBLOCK_MAGIC;
    void* addr;
    std::size_t size;
    MEMBLOCK_FLAGS flags;
} __attribute__((packed));

nonstd::list<memblock*> free_blocks;

extern "C" NO_DISCARD void* kmalloc(std::size_t);
extern "C" void kfree(void*);
