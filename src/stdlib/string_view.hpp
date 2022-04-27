#pragma once

#include <climits>
#include <cstdint>
#include <cstddef>

#include "char_traits.hpp"
#include "memory.hpp"

#ifndef NO_DISCARD
#   define NO_DISCARD [[nodiscard]]
#endif

namespace kstd {

template<typename CharT, class Traits = char_traits<CharT>> 
class basic_string_view {
public:
    using traits_type = Traits;
    using value_type = CharT;
    using pointer = CharT*;
    using const_pointer = const CharT*;
    using reference = CharT&;
    using const_reference = const CharT&;
    using const_iterator = const CharT*;
    using iterator = const_iterator;
    // using const_reverse_iterator = reverse_iterator<const_iterator>;
    // using reverse_iterator = const_reverse_iterator;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    static constexpr size_type npos = size_type(-1);

    constexpr basic_string_view() noexcept : m_data(nullptr), m_size(0) { }

    constexpr basic_string_view( const basic_string_view& other ) noexcept 
        = default;

    constexpr basic_string_view(const_pointer s, size_type count) 
        : m_data(s), 
          m_size(count) 
    { 

    }

    constexpr basic_string_view(const_pointer s) 
        : m_data(s), 
          m_size(traits_type::length(s))
    {
    
    }

    template<class It, class End> constexpr basic_string_view(It first, 
                                                              End last) 
        : m_data(to_address(first)), 
          m_size(last - first)
    {
        
    }
    
    // TODO - C++20 range constructor
    
    constexpr basic_string_view(std::nullptr_t) = delete;

    constexpr basic_string_view& operator=(const basic_string_view& view) 
        noexcept = default;

    constexpr const_iterator cbegin() const noexcept {
        return const_iterator(data());
    }

    constexpr const_iterator begin() const noexcept {
        return cbegin();
    }

    constexpr const_iterator cend() const noexcept {
        return const_iterator(data() + length());
    }

    constexpr const_iterator end() const noexcept {
        return cend();
    }

    // TODO - rbegin, rend, crbegin, crend
    
    constexpr const_reference operator[](size_type pos) const {
        return data()[pos];
    }

    constexpr const_reference at(size_type pos) const {
        if(pos > length() - 1)
            return data()[length() - 1];

        return data()[pos];
    }

    constexpr const_reference front() const {
        return *begin();
    }

    constexpr const_reference back() const {
        return *end();
    }

    constexpr const_pointer data() const noexcept {
        return m_data;
    }

    constexpr size_type size() const noexcept {
        return m_size;
    }

    constexpr size_type length() const noexcept {
        return size();
    }

    constexpr size_type max_size() const noexcept {
        return UINT_MAX;
    }

    NO_DISCARD constexpr bool empty() const noexcept {
        return length() == 0;
    }

    constexpr void remove_prefix(size_type n) {
        if(n < length())
            m_data += n;
    }

    constexpr void remove_suffix(size_type n) {
        if(n < length())
            m_size -= n;
    }

    constexpr void swap( basic_string_view& v ) noexcept {
        basic_string_view tmp = v;
        v.m_data = data();
        v.m_size = size();
        m_data = tmp.data();
        m_size = tmp.size();
    }

    constexpr size_type copy(const_pointer dest, 
                             size_type count,
                             size_type pos = 0) const
    {
        size_type rcount = count < size() - pos ? count : size() - pos;
        return traits_type::copy(dest, data() + pos, rcount);
    }

    constexpr basic_string_view substr(size_type pos = 0,
                                       size_type count = npos) const
    {
        size_type rcount = count < size() - pos ? count : size() - pos;
        return basic_string_view(data() + pos, rcount);
    }

    constexpr int compare(basic_string_view v) const noexcept {
        size_type rlen = size() < v.size() ? size() : v.size();
        return traits_type::compare(data(), v.data(), rlen);
    }

    constexpr int compare(size_type pos1, 
                          size_type count1,
                          basic_string_view v) const 
    {
        return substr(pos1, count1).compare(v);
    }

    constexpr int compare(size_type pos1, 
                          size_type count1, 
                          basic_string_view v,
                          size_type pos2, 
                          size_type count2) const 
    {
        return substr(pos1, count1).compare(v.substr(pos2, count2));
    }

    constexpr int compare(const_pointer s) const {
        return compare(basic_string_view(s));
    }

    constexpr int compare(size_type pos1, 
                          size_type count1, 
                          const_pointer s) const 
    {
        return substr(pos1, count1).compare(basic_string_view(s));
    }

    constexpr int compare(size_type pos1, 
                          size_type count1,
                          const_pointer s, 
                          size_type count2) const
    {
        return substr(pos1, count1).compare(basic_string_view(s, count2));
    }

    constexpr bool starts_with(basic_string_view v) const noexcept {
        return substr(0, v.size()) == v;
    }

    constexpr bool starts_with(value_type c) const noexcept {
        return !empty() && traits_type::eq(front(), c);
    }

    constexpr bool starts_with(const_pointer s) const {
        return starts_with(basic_string_view(s));
    }

    constexpr bool ends_with(basic_string_view v) const noexcept {
        return size() >= v.size() && compare(size() - v.size(), npos, v) == 0;
    }

    constexpr bool ends_with(value_type c) const noexcept {
        return empty() && traits_type::eq(back(), c);
    }

    constexpr bool ends_with(const_pointer s) const {
        return ends_with(basic_string_view(s));
    }

    constexpr size_type find(basic_string_view v, 
                             size_type pos = 0) const noexcept
    {
        const_pointer p = data() + pos;
        while(p++ + v.length() < data() + length()) {
            if(traits_type::compare(p, v.data(), v.length()) == 0)
                return p - data();
        }
        return npos;
    }

    constexpr size_type find(value_type ch, size_type pos = 0) const noexcept {
        return find(basic_string_view(addressof(ch), 1), pos);
    }

    constexpr size_type find(const_pointer s, 
                             size_type pos, 
                             size_type count) const
    {
        return find(basic_string_view(s, count), pos);
    }

    constexpr size_type find(const_pointer s, size_type pos = 0) const {
        return find(basic_string_view(s), pos);
    }

    constexpr size_type rfind(basic_string_view v, 
                              size_type pos = npos) const noexcept 
    {
        if(pos == npos)
            pos = length() - 1;

        const_pointer p = data() + pos;
        while(p-- - v.length() > data()) {
            if(traits_type::compare(p, v.data(), v.length()) == 0) 
                return p - data();
        }
        return npos;
    }

    constexpr size_type rfind(value_type c, 
                              size_type pos = npos) const noexcept 
    {
        return rfind(basic_string_view(addressof(c), 1), pos);
    }

    constexpr size_type rfind(const_pointer s, 
                              size_type pos, 
                              size_type count) const 
    {
        return rfind(basic_string_view(s, count), pos);
    }

    constexpr size_type rfind(const_pointer s, size_type pos = npos) const {
        return rfind(basic_string_view(s), pos);
    }

    constexpr size_type find_first_of(basic_string_view v, 
                                      size_type pos = 0) const noexcept 
    {
        size_type first = npos;
        for(const auto& c : v) {
            size_type tmp = find(c, pos);
            if(tmp < first)
                first = tmp;
        }
        return first;
    }

    constexpr size_type find_first_of(value_type c, 
                                      size_type pos = 0) const noexcept 
    {
        return find_first_of(basic_string_view(addressof(c), 1), pos);
    }

    constexpr size_type find_first_of(const_pointer s, 
                                      size_type pos, 
                                      size_type count) const 
    {
        return find_first_of(basic_string_view(s, count), pos);
    }

    constexpr size_type find_first_of(const_pointer s, 
                                      size_type pos = 0) const
    {
        return find_first_of(basic_string_view(s), pos);
    }

    constexpr size_type find_last_of(basic_string_view v, 
                                     size_type pos = npos) const noexcept
    {
        size_type last = 0;
        for(const auto& c : v) {
            size_type tmp = rfind(c, pos);
            if(tmp > last)
                last = tmp;
        }
        return last != 0 ? last : npos;
    }

    constexpr size_type find_last_of(value_type c, 
                                     size_type pos = npos) const noexcept 
    {
        return find_last_of(basic_string_view(addressof(c), 1), pos);
    }

    constexpr size_type find_last_of(const_pointer s, 
                                     size_type pos, 
                                     size_type count) const
    {
        return find_last_of(basic_string_view(s, count), pos);
    }

    constexpr size_type find_last_of(const_pointer s, 
                                     size_type pos = npos) const
    {
        return find_last_of(basic_string_view(s), pos);
    }

    /* TODO - implement
    constexpr size_type find_first_not_of(basic_string_view v, 
                                          size_type pos = 0) const noexcept
    {
        return npos;
    }

    constexpr size_type find_first_not_of(value_type c, 
                                          size_type pos = 0) const noexcept
    {
        return find_first_not_of(basic_string_view(addressof(c), 1), pos);
    }

    constexpr size_type find_first_not_of(const_pointer s, 
                                          size_type pos, 
                                          size_type count) const
    {
        return find_first_not_of(basic_string_view(s, count), pos);
    }

    constexpr size_type find_first_not_of(const_pointer s, 
                                          size_type pos = 0) const
    {
        find_first_not_of(basic_string_view(s), pos);
    }

    constexpr size_type find_last_not_of(basic_string_view v, 
                                         size_type pos = 0) const noexcept
    {
        return npos;
    }

    constexpr size_type find_last_not_of(value_type c, 
                                         size_type pos = 0) const noexcept
    {
        return find_last_not_of(basic_string_view(addressof(c), 1), pos);
    }

    constexpr size_type find_last_not_of(const_pointer s, 
                                         size_type pos, 
                                         size_type count) const
    {
        return find_last_not_of(basic_string_view(s, count), pos);
    }

    constexpr size_type find_last_not_of(const_pointer s, 
                                         size_type pos = 0) const
    {
        find_last_not_of(basic_string_view(s), pos);
    }
    */

protected:
    const_pointer m_data;
    size_type m_size;
};

using string_view = basic_string_view<char>;
using wstring_view = basic_string_view<wchar_t>;
using u8string_view = basic_string_view<char8_t>;
using u16string_view = basic_string_view<char16_t>;
using u32string_view = basic_string_view<char32_t>;

template<class CharT, class Traits> 
constexpr bool operator==(basic_string_view<CharT,Traits> lhs,
                          basic_string_view<CharT,Traits> rhs) noexcept
{
    return lhs.compare(rhs) == 0;
}

// TODO - spaceship operator (operator<=>)
// TODO - operator<< (get ostreams working first)

inline namespace literals {
inline namespace string_view_literals {

constexpr string_view operator "" sv(const char* str, 
                                          size_t len) noexcept 
{
    return string_view{str, len};
}

constexpr u8string_view operator "" sv(const char8_t* str, 
                                            size_t len) noexcept 
{
    return u8string_view{str, len};
}

constexpr u16string_view operator "" sv(const char16_t* str, 
                                             size_t len) noexcept 
{
    return u16string_view{str, len};
}

constexpr u32string_view operator "" sv(const char32_t* str, 
                                             size_t len) noexcept 
{
    return u32string_view{str, len};
}

constexpr wstring_view operator "" sv(const wchar_t* str, 
                                           size_t len) noexcept 
{
    return wstring_view{str, len};
}

} // namespace string_view_literals
} // namespace literals

// TODO - template deduction guides

} // namespace kstd
