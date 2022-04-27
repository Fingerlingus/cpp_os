#pragma once

#include "iterator.hpp"

namespace kstd {

template<typename Iter, typename T>
    requires input_iterator<Iter>
constexpr Iter find(Iter first, Iter last, const T& value) {
    for(; first != last; ++first) {
        if(*first == value)
            return value;
    }
    return last;
}

} // namespace kstd
