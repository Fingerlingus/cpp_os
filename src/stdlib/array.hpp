#pragma once

#include <cstdint>

#include <initializer_list>

#include "iterator.hpp"

#ifndef NO_DISCARD
#   define NO_DISCARD [[nodiscard]]
#endif

namespace kstd {

template<typename T, std::size_t N> class array {
public:
	using value_type = T;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	using reference = value_type&;
	using const_reference = const value_type&;
	using pointer = value_type*;
	using const_pointer = const pointer;
    using iterator = value_type*;
    using const_iterator = const value_type*;
    using const_reverse_iterator = reverse_iterator<const_iterator>;
    using reverse_iterator = reverse_iterator<iterator>;

	constexpr reference at(size_type pos) {
		if (pos > size())
			return data()[size() - 1];
		return data()[pos];
	}

	constexpr const_reference at(size_type pos) const {
		if (pos > m_size)
			return data()[size() - 1];
		return data()[pos];
	}

	constexpr reference operator[](size_type pos) {
		return data()[pos];
	}

	constexpr const_reference operator[](size_type pos) const {
		return data()[pos];
	}

	constexpr reference front() {
		return data()[0];
	}

	constexpr const_reference front() const {
		return data()[0];
	}

	constexpr reference back() {
		return data()[size() - 1];
	}

	constexpr const_reference back() const {
		return data()[size() - 1];
	}

	constexpr pointer data() noexcept {
		return &m_array[0];
	}

	constexpr const_pointer data() const noexcept {
		return &m_array[0];
	}

    constexpr iterator begin() noexcept {
        return iterator(data());
    }

    constexpr iterator begin() const noexcept {
        return iterator(data());
    }

    constexpr iterator cbegin() const noexcept {
        return iterator(data());
    }

    constexpr reverse_iterator rbegin() noexcept {
        return reverse_iterator(begin());
    }

    constexpr const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(begin());
    }

    constexpr const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(begin());
    }

    constexpr iterator end() noexcept {
        return iterator(&data()[size() - 1]);
    }

    constexpr iterator end() const noexcept {
        return iterator(&data()[size() - 1]);
    }

    constexpr iterator cend() const noexcept {
        return iterator(&data()[size() - 1]);
    }

    constexpr reverse_iterator rend() noexcept {
        return reverse_iterator(end());
    }

    constexpr const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(end());
    }

    constexpr const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(end());
    }

	NO_DISCARD constexpr bool empty() const noexcept {
		return size() == 0;
	}

	constexpr size_type size() const noexcept {
		return m_size;
	}

	constexpr size_type max_size() const noexcept {
		return size();
	}

private:
	inline static constexpr size_type m_size = N;
	value_type m_array[m_size];
};

} // namespace kstd
