#pragma once

#include <cstdint>
#include <climits>

#include <type_traits>
#include <concepts>

#include "stdlib/cstdlib.hpp"
#include "stdlib/expected.hpp"
#include "stdlib/optional.hpp"

template<typename T, typename U = T> 
concept inequality_comparable = requires(T t, U u) {
	t != u;
};

template<typename T>
concept negatable = requires(T t) {
	!t;
};

template<typename T>
concept boolean = std::is_same_v<T, bool>;

template<typename T = void*> class memory_address {
public:
	using self_type = memory_address<T>;
	using pointer = T;
	using const_pointer = const T;

	constexpr memory_address()
		: m_p(nullptr) 
	{

	}

	constexpr memory_address(const_pointer p)
		: m_p(p) 
	{

	}

	constexpr memory_address(std::uintptr_t u)
		: m_u(u) {

	}

	template<typename U> constexpr memory_address(const U* p)
		: m_p(static_cast<const_pointer>(p)) 
	{
		
	}

	constexpr self_type& operator=(const_pointer p) {
		m_p = p;
	}

	template<typename U>
		requires std::integral<U>
	constexpr self_type& operator=(U u) {
		m_u = u;
	}

	constexpr operator const_pointer() const {
		return m_p;
	}

	template<typename U>
		requires std::integral<U>
	constexpr self_type& operator+(U u) const {
		return reinterpret_cast<const_pointer>(m_u + u);
	}

	template<typename U>
		requires std::integral<U>
	constexpr self_type& operator-(U u) const {
		return reinterpret_cast<const_pointer>(m_u - u);
	}

	template<typename U> constexpr self_type& operator+(const U* u) const {
		return reinterpret_cast<const_pointer>(static_cast<U*>(m_p) + u);
	}

	template<typename U> constexpr self_type& operator-(const U* u) const {
		return reinterpret_cast<const_pointer>(static_cast<U*>(m_p) - u);
	}

	template<typename U>
		requires std::integral<U>
	constexpr self_type& operator/(U u) const {
		return reinterpret_cast<const_pointer>(m_u / u);
	}

	template<typename U>
		requires std::integral<U>
	constexpr self_type& operator%(U u) const {
		return reinterpret_cast<const_pointer>(m_u % u);
	}

	template<typename U>
		requires std::integral<U>
	constexpr self_type& operator&(U u) const {
		return reinterpret_cast<const_pointer>(m_u & u);
	}

	template<typename U>
		requires std::integral<U>
	constexpr self_type& operator|(U u) const {
		return reinterpret_cast<const_pointer>(m_u | u);
	}

	template<typename U>
		requires std::integral<U>
	constexpr self_type& operator>>(U u) const {
		return reinterpret_cast<const_pointer>(m_u >> u);
	}

	template<typename U>
		requires std::integral<U>
	constexpr self_type& operator<<(U u) const {
		return reinterpret_cast<const_pointer>(m_u << u);
	}

	template<typename U>
		requires std::integral<U>
	constexpr self_type& operator+=(U u) {
		m_u += u;
		return *this;
	}

	template<typename U> constexpr self_type& operator+=(const U* u) {
		m_p = static_cast<const_pointer>(static_cast<U*>(m_p) + u);
		return *this;
	}

	template<typename U> 
		requires std::integral<U>
	constexpr self_type& operator/=(U u) {
		m_u /= u;
		return *this;
	}

	template<typename U>
		requires std::integral<U>
	constexpr self_type& operator&=(U u) {
		m_u &= u;
		return *this;
	}

	template<typename U>
		requires std::integral<U>
	constexpr self_type& operator|=(U u) {
		m_u |= u;
		return *this;
	}

	template<typename U>
		requires std::integral<U>
	constexpr self_type& operator>>=(U u) {
		m_u >>= u;
		return *this;
	}

	template<typename U>
		requires std::integral<U>
	constexpr self_type& operator<<=(U u) {
		m_u <<= u;
		return *this;
	}

	constexpr bool operator!() const {
		return !m_p;
	}

	constexpr self_type& operator++() const {
		return ++m_p;
	}

	constexpr self_type& operator++(int) const {
		return m_p++;
	}

	constexpr self_type& operator--() const {
		return --m_p;
	}

	constexpr self_type& operator--(int) const {
		return --m_p;
	}

	constexpr const_pointer const_ptr() const {
		return m_p;
	}

	constexpr operator pointer() = delete;
	//constexpr operator std::uintptr_t() = delete;

private:
	union {
		const_pointer m_p;
		std::uintptr_t m_u;
	};
};

template<typename T, typename U = T> 
bool operator==(const memory_address<T>& lhs, const memory_address<U>& rhs) {
	return lhs.const_ptr() == rhs.const_ptr();
}

template<typename T, typename U = T>
bool operator==(const memory_address<T>& lhs, const U* const rhs) {
	return lhs.const_ptr() == rhs;
}

template<typename T, typename U = T>
bool operator!=(const memory_address<T>& lhs, const memory_address<U>& rhs) {
	return lhs.const_ptr() != rhs.const_ptr();
}

template<typename T, typename U = T>
bool operator!=(const memory_address<T>& lhs, const U* const rhs) {
	return lhs.const_ptr() != rhs;
}

template<typename T, typename U = T>
bool operator<(const memory_address<T>& lhs, const memory_address<U>& rhs) {
	return lhs.const_ptr() < rhs.const_ptr();
}

template<typename T, typename U = T>
bool operator<(const memory_address<T>& lhs, const U* const rhs) {
	return lhs.const_ptr() < rhs;
}

template<typename T, typename U>
	requires std::integral<U>
bool operator<(const memory_address<T>& lhs, U rhs) {
	return reinterpret_cast<U>(lhs.const_ptr()) < rhs;
}

template<typename T, typename U = T>
bool operator>(const memory_address<T>& lhs, const memory_address<U>& rhs) {
	return lhs.const_ptr() > rhs.const_ptr();
}

template<typename T, typename U = T>
bool operator>(const memory_address<T>& lhs, const U* const rhs) {
	return lhs.const_ptr() > rhs;
}

template<typename T, typename U>
	requires std::integral<U>
bool operator>(const memory_address<T>& lhs, U rhs) {
	return reinterpret_cast<U>(lhs.const_ptr()) > rhs;
}

template<typename T> inline constexpr uint8_t SIZE_IN_BITS() {
	return sizeof(T) * CHAR_BIT;
}

template<typename T> inline constexpr uint8_t SIZE_IN_BITS(T) {
	return SIZE_IN_BITS<T>();
}

template<typename T> inline constexpr bool IS_NULL(const T* const p) {
	return p == nullptr;
}

template<typename T> 
	requires std::equality_comparable_with<T, decltype(nullptr)>
inline constexpr bool IS_NULL(const T& t) {
	return t == nullptr;
}

template<typename T> 
	requires inequality_comparable<T, decltype(0)> && (!boolean<T>)
inline constexpr bool ERROR(const T& t) {
	return t != 0;
}

template<typename T>
	requires negatable<T> && (!boolean<T>)
inline constexpr bool ERROR(const T& t) {
	return !t ? true : false;
}

template<typename T>
	requires inequality_comparable<T, decltype(0)> && 
			 (!negatable<T>) && 
			 (!boolean<T>)
inline constexpr bool ERROR(const T& t) {
	return t != 0;
}

template<typename T> inline constexpr bool ERROR(const T* const p) {
	return IS_NULL(p);
}

inline constexpr bool ERROR(bool b) {
	return b == false;
}

template<typename T, typename E> 
inline constexpr bool ERROR(const kstd::expected<T, E>& e) {
	return e.is_unexpected();
}

template<typename T>
inline constexpr bool ERROR(const kstd::optional<T>& t) {
	return !t.has_value();
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