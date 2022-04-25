#pragma once

namespace nonstd {

template <typename T> typename std::remove_reference_t<T>&& move(T&& arg) {
	return static_cast<typename std::remove_reference_t<T>&&>(arg);
}

template<typename T> constexpr bool always_false = false;
 
template<typename T>
typename std::add_rvalue_reference<T>::type declval() noexcept {
    static_assert(always_false<T>, "declval not allowed in an evaluated context");
}

template<typename T> T&& forward(typename std::remove_reference_t<T>& t) noexcept {
    return static_cast<T&&>(t);
}

template<typename T> T&& forward(typename std::remove_reference_t<T>&& t) noexcept {
    static_assert(!std::is_lvalue_reference_v<T>,
                  "Can not forward an rvalue as an lvalue.");
    return static_cast<T&&>(t);
}

} // namespace nonstd

