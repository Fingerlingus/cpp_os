#pragma once

#include <type_traits>

#include <stdint.h>

#ifndef NO_DISCARD
#   define NO_DISCARD [[nodiscard]]
#endif

namespace kstd {

template<typename T> struct allocator {
	using value_type = T;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	using propagate_on_container_move_assignment = std::true_type;
	using self_type = allocator<T>;

	constexpr allocator() noexcept = default;
	constexpr allocator(const self_type&) noexcept = default;
	template<typename U> constexpr allocator(const allocator<U>&) { }

	constexpr ~allocator() = default;

	NO_DISCARD constexpr T* allocate(size_type n) const {
		return new value_type[n];
	}

	constexpr void deallocate(value_type* p, std::size_t) const {
		if (p != nullptr)
			delete[] p;
	}
};

template<class T1, class T2> constexpr bool operator==(const allocator<T1>&, 
                                                       const allocator<T2>&) 
{
	return true;
}

template<class T1, class T2> constexpr bool operator!=(const allocator<T1>&, 
                                                       const allocator<T2>&) 
{
	return false;
}

} // namespace kstd

