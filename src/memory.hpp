#pragma once

#include <cstdint>

#include "stdlib/array.hpp"

#ifndef NO_DISCARD
#	define NO_DISCARD [[nodiscard]]
#endif

extern const void* const KHEAP_BEGIN;

#ifdef KHEAP_INIT_SIZE
    inline static constexpr std::size_t KHEAP_SIZE = KHEAP_INIT_SIZE;
#else
    inline static constexpr std::size_t KHEAP_SIZE = 0x2000000;
#endif

inline const void* KHEAP_END = (void*)((char*)KHEAP_BEGIN + KHEAP_SIZE);

bool kheap_init();

extern "C" NO_DISCARD void* kmalloc(std::size_t size);
extern "C" void kfree(void* ptr);
