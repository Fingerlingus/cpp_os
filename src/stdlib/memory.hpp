#pragma once

#include <type_traits>

namespace nonstd {

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

} // namespace nonstd
