#pragma once

#include <cstdint>

#include <initializer_list>

#include "utility.hpp"

namespace kstd {

struct nullopt_t {
	explicit constexpr nullopt_t(int) { }
};

template<typename T> class optional {
public:
	using value_type = T;
	using reference = value_type&;
	using const_reference = const value_type&;
	using pointer = value_type*;
	using const_pointer = const value_type*;
	using self_type = optional<value_type>;

	constexpr optional() noexcept
		: m_has_value(false) 
	{
			
	}

	constexpr optional(nullopt_t)
		: m_has_value(false)
	{
	
	}

	constexpr optional(const self_type& other) 
		: m_value(other.m_value), 
		  m_has_value(other.m_has_value) 
	{
	
	}

	constexpr optional(self_type&& other) 
		: m_value(move(other.m_value)),
		  m_has_value(move(other.m_has_value)) 
	{
		other.m_value = value_type();
		other.m_has_value = false;
	}

	template <class U> constexpr optional(const optional<U>& other) 
		: m_value(other.m_value),
		  m_has_value(other.m_has_value) 
	{

	}

	template<class U> constexpr optional(optional<U>&& other)
		: m_value(move(other.m_value)),
		  m_has_value(move(other.m_has_value)) 
	{
		other.m_value = value_type();
		other.m_has_value = false;
	}

	//template<class... Args> 
	// constexpr explicit optional(std::in_place_t, Args&&... args);
	//template<class U, class ...Args> 
	// constexpr explicit optional(std::in_place_t, 
	//							   std::initializer_list<U> ilist, 
	//							   Args&&... args);

	template<class U = T> constexpr optional(U&& value) {
		m_value = move(value);
		m_has_value = true;
		value = U();
	}

	constexpr self_type& operator=(nullopt_t) {
		reset();
	}

	constexpr self_type& operator=(const self_type& other) {
		if (other.m_has_value == false && m_has_value == false)
			return *this;

		if (other.m_has_value == false && m_has_value == true) {
			reset();
			return *this;
		}

		if (other.m_has_value == true && m_has_value == false) {
			m_value = other.m_value;
			m_has_value = true;
			return *this;
		}

		m_value = other.m_value;
		m_has_value = true;
		return *this;
	}

	constexpr optional& operator=(optional&& other) {
		if (other.m_has_value == false && m_has_value == false)
			return *this;

		if (other.m_has_value == false && m_has_value == true) {
			reset();
			return *this;
		}

		if (other.m_has_value == true && m_has_value == false) {
			m_value = other.m_value;
			m_has_value = true;
			return *this;
		}

		m_value = move(other.m_value);
		m_has_value = true;
		return *this;
	}

	template<class U = T> constexpr optional& operator=(U&& value) {
		if (m_has_value == false) {
			m_value = forward<U>(value);
			m_has_value = true;
			return *this;
		}

		m_value = forward<U>(value);
		return *this;

	}

	template<class U> constexpr optional& operator=(const optional<U>& other) {
		if (other.m_has_value == false && m_has_value == false)
			return *this;

		if (other.m_has_value == false && m_has_value == true) {
			reset();
			return *this;
		}

		if (other.m_has_value == true && m_has_value == false) {
			m_value = other.m_value;
			m_has_value = true;
			return *this;
		}

		m_value = other.m_value;
		m_has_value = true;
		return *this;
	}

	template<class U> constexpr optional& operator=(optional<U>&& other) {
		if (other.m_has_value == false && m_has_value == false)
			return *this;

		if (other.m_has_value == false && m_has_value == true) {
			reset();
			return *this;
		}

		if (other.m_has_value == true && m_has_value == false) {
			m_value = other.m_value;
			m_has_value = true;
			return *this;
		}

		m_value = move(other.m_value);
		m_has_value = true;
		return *this;
	}

	reference operator=(reference rhs) {
		m_value = rhs;
		m_has_value = true;
		return m_value;
	}

	constexpr pointer operator->() {
		return &m_value;
	}

	constexpr const_pointer operator->() const {
		return &m_value;
	}

	constexpr reference operator*() {
		return m_value;
	}

	/*
	constexpr const_reference operator*() const& {
		return m_value;
	}
	*/

	/*
	constexpr value_type&& operator*()&& {
		return m_value;
	}
	*/

	/*
	constexpr const value_type&& operator*() const&& {
		return m_value;
	}
	*/

	constexpr bool has_value() const {
		return m_has_value;
	}

	constexpr explicit operator bool() const {
		return has_value();
	}

	constexpr reference value()& {
		return m_value;
	}

	constexpr const_reference value() const& {
		return m_value;
	}

	constexpr value_type&& value()&& {
		return m_value;
	}

	constexpr const value_type&& value() const&& {
		return m_value;
	}

	template<class U> constexpr value_type value_or(U&& default_value)&& {
		return bool(*this) ? 
			   **this : 
			   static_cast<T>(forward<U>(default_value));
	}

	template<class U> constexpr value_type value_or(U&& default_value) const& {
		return bool(*this) ? 
			   move(**this) : 
			   static_cast<T>(forward<U>(default_value));
	}

	constexpr void swap(optional& other) {
		self_type temp = *this;
		m_value = other.m_value;
		m_has_value = other.m_has_value;
		other.m_value = temp.m_value;
		other.m_has_value = temp.m_has_value;
	}

	constexpr void reset() {
		value().value_type::~value_type();
		m_has_value = false;
	}

	template<class ...Args> constexpr reference emplace(Args&&... args) {
		m_value(forward<Args>(args)...);
		return m_value;
	}

	template<class U, class ...Args> 
	constexpr reference emplace(std::initializer_list<U> ilist, 
							   Args&&... args) 
	{
		m_value(ilist, forward<Args>(args)...);
		return m_value;
	}

private:
	value_type m_value;
	bool m_has_value = false;
};

template<typename T, typename U = T> 
bool operator==(const optional<T>& lhs, const optional<U>& rhs) {
	return lhs.value() == rhs.value();
}

template<typename T, typename U = T> 
bool operator==(const optional<T>& lhs, const U& rhs) {
	return lhs.value() == rhs;
}

template<typename T, typename U = T> 
bool operator!=(const optional<T>& lhs, const optional<U>& rhs) {
	return lhs.value() != rhs.value();
}

template<typename T, typename U = T> 
bool operator!=(const optional<T>& lhs, const U& rhs) {
	return lhs.value() != rhs;
}

template<typename T, typename U = T> 
bool operator<(const optional<T>& lhs, const optional<U>& rhs) {
	return lhs.value() < rhs.value();
}

template<typename T, typename U = T> 
bool operator<(const optional<T>& lhs, const U& rhs) {
	return lhs.value() < rhs;
}

template<typename T, typename U = T> 
bool operator>(const optional<T>& lhs, const optional<U>& rhs) {
	return lhs.value() > rhs.value();
}

template<typename T, typename U = T> 
bool operator>(const optional<T>& lhs, const U& rhs) {
	return lhs.value() > rhs;
}

template<typename T, typename U = T> 
bool operator<=(const optional<T>& lhs, const optional<U>& rhs) {
	return lhs.value() <= rhs.value();
}

template<typename T, typename U = T> 
bool operator<=(const optional<T>& lhs, const U& rhs) {
	return lhs.value() <= rhs;
}

template<typename T, typename U = T> 
bool operator>=(const optional<T>& lhs, const optional<U>& rhs) {
	return lhs.value() >= rhs.value();
}

template<typename T, typename U = T> 
bool operator>=(const optional<T>& lhs, const U& rhs) {
	return lhs.value() >= rhs;
}

template<typename T> optional<T> make_optional(T&& t) {
	return optional<T>(move(t));
}

template<class T, class ...Args> constexpr optional<T> make_optional(Args&&... args) {
	return optional<T>(forward<Args>(args)...);
}

template<class T, class U, class ...Args> constexpr optional<T> make_optional(std::initializer_list<U> ilist, Args&&... args) {
	return optional<T>(ilist, forward<Args>(args)...);
}

} // namespace kstd