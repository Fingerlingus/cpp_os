#pragma once

#include <type_traits>

#include "utility.hpp"

#ifndef NO_DISCARD
#	define NO_DISCARD [[nodiscard]]
#endif

namespace kstd {

template<typename E = int> class unexpected {
public:
	using error_type = E;
	static_assert(!std::is_same<error_type, void>::value,
		      "error_type cannot be void");

	unexpected() = delete;
	constexpr unexpected(const error_type& e) : m_val(e) { }
	constexpr unexpected(error_type&& e) : m_val(move(e)) { }
	
	NO_DISCARD constexpr const error_type& value() const& { return m_val; }
	NO_DISCARD constexpr error_type&& value() && { return move(m_val); }
	NO_DISCARD constexpr const error_type&& value() const&& { 
		return move(m_val); 
	}

private:
	error_type m_val;
};

template<typename T, typename E = int> class expected {
public:
	using self_type = expected<T, E>;
	using value_type = T;
	using error_type = E;

	static_assert(!std::is_reference<value_type>::value, 
		      "value_type cannot be a reference");
	static_assert(!std::is_reference<error_type>::value, 
		      "error_type cannot be a reference");
	static_assert(!std::is_same<unexpected<error_type>, 
		      typename std::remove_cv<value_type>::type>::value,
		      "value_type cannot be unexpected<error_type>");

	expected() = delete;
	expected(const self_type&) = default;
	expected(self_type&&) = default;
	
	self_type& operator=(const self_type&) = default;
	self_type& operator=(self_type&&) = default;

	constexpr expected(const value_type& t) 
		: m_expected(t), 
		  m_is_unexpected(false)
	{

	}

	constexpr expected(value_type&& t) 
		: m_expected(move(t)), 
		  m_is_unexpected(false)
	{
		t = value_type();
	}

	constexpr expected(const unexpected<error_type>& e) 
		: m_unexpected(e),
		  m_is_unexpected(true)
	{

	}

	constexpr expected(unexpected<error_type>&& e) 
		: m_unexpected(move(e.value())),
		  m_is_unexpected(true)
	{
	
	}

	NO_DISCARD constexpr self_type& operator=(const value_type& t) {
		m_expected = t;
		m_is_unexpected = false;
		return *this;
	}
	
	NO_DISCARD constexpr self_type& operator=(value_type&& t) {
		m_expected = move(t);
		m_is_unexpected = false;
		return *this;
	}
	
	NO_DISCARD constexpr self_type& 
	operator=(const unexpected<error_type>& e) {
		m_unexpected = e;
		m_is_unexpected = true;
		return *this;
	}
	
	NO_DISCARD constexpr self_type& operator=(unexpected<error_type>&& e) {
		m_unexpected = move(e);
		m_is_unexpected = true;
		return *this;
	}

	NO_DISCARD operator value_type() const { return m_expected; }
	NO_DISCARD operator value_type&() const& { 
		return m_expected; 
	}
	NO_DISCARD operator value_type&&() const&& { 
		return move(m_expected); 
	}

	NO_DISCARD operator unexpected<error_type>() const { 
		return unexpected<error_type>(m_unexpected); 
	}
	NO_DISCARD operator unexpected<error_type>&() const& { 
		return unexpected<error_type>(m_unexpected); 
	}
	NO_DISCARD operator unexpected<error_type>&&() const&& { 
		return unexpected<error_type>(m_unexpected); 
	}

	NO_DISCARD constexpr const value_type& value() const& { return m_expected; }
	NO_DISCARD constexpr const error_type& error() const& { return m_unexpected; }

	NO_DISCARD constexpr bool is_unexpected() const { 
		return m_is_unexpected; 
	}
	NO_DISCARD constexpr bool is_error() const { return is_unexpected(); }
	NO_DISCARD constexpr operator bool() { return is_unexpected(); }
	
	NO_DISCARD constexpr value_type& operator->() & { return m_expected; }
	NO_DISCARD constexpr const value_type& operator->() const& { return m_expected; }

private:
	union {
		value_type m_expected;
		error_type m_unexpected;
	};
	bool m_is_unexpected;
};

} // namespace kstd
