#pragma once

#include <type_traits>

namespace kstd {

template<typename T>
typename std::enable_if<std::is_object<T>::value, T*>::type 
addressof(T& arg) noexcept {
    return reinterpret_cast<T*>(
        &const_cast<char&>(
            reinterpret_cast<const volatile char&>(arg)
        )
    );
}
 
template<typename T> 
typename std::enable_if<!std::is_object<T>::value, T*>::type 
addressof(T& arg) noexcept {
    return &arg;
}

template<typename T, typename ...Args> 
constexpr T* construct_at(T* p, Args&&... args) {
	return ::new(const_cast<void*>(
        static_cast<const volatile void*>(p))) T(forward<Args>(args)...);
}

template<class T> constexpr void destroy_at(T* p) {
	if constexpr (std::is_array_v<T>) {
		for (auto& elem : *p)
			destroy_at(addressof(elem));
	}
	else
		p->~T();
}

template<class Alloc> struct allocator_traits {
	using allocator_type = Alloc;
	using value_type = typename allocator_type::value_type;
	using pointer = value_type*;
	using const_pointer = const value_type*;
	using void_pointer = void*;
	using const_void_pointer = const void*;
	using difference_type = typename allocator_type::difference_type;
	using size_type = typename allocator_type::size_type;
	using propagate_on_container_copy_assignment = std::false_type;
	using propagate_on_container_move_assignment = 
        typename allocator_type::propagate_on_container_move_assignment;
	using propagate_on_container_swap = std::false_type;
	using is_always_equal = typename std::is_empty<Alloc>::type;

	static constexpr pointer allocate(size_type n) {
		return allocator_type().allocate(n);
	}

	static constexpr void deallocate(pointer p, size_type n) {
		return allocator_type().deallocate(p, n);
	}

	template<typename T, typename ...Args> 
    static constexpr void construct(allocator_type&, T* p, Args&&... args) {
		construct_at(p, forward<Args>(args)...);
	}

	template<typename T> 
    static constexpr void destroy(allocator_type&, T* p) {
		destroy_at(p);
	}
};

} // namespace kstd
