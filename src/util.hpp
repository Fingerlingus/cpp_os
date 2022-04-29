#pragma once

#include <cstdint>
#include <climits>

#include "stdlib/cstdlib.hpp"

template<typename T> constexpr uint8_t SIZE_IN_BITS() {
	return sizeof(T) * CHAR_BIT;
}

template<typename T> constexpr uint8_t SIZE_IN_BITS(T) {
	return SIZE_IN_BITS<T>();
}

template<typename T, std::size_t N> 
inline void* memset_safe(T (*p)[N], int c) {
	return memset((void*)p, c, N);
}

template<typename T, std::size_t N>
inline void* memset_safe(kstd::array<T, N>& a, int c) {
	return memset((void*)a.data(), c, N);
}

template<typename T, std::size_t N> 
inline void* memcpy_safe(T (*dest)[N], T (*src)[N]) {
	return memcpy((void*)dest, (void*)src, N);
}

template<typename T, std::size_t N> 
inline void* memmove_safe(T (*dest)[N], T (*src)[N]) {
	return memmove((void*)dest, (void*)src, N);
}

template<typename T, std::size_t N> 
inline int memcmp_safe(T (*dest)[N], T (*src)[N]) {
	return memcmp((void*)dest, (void*)src, N);
}