#pragma once

#include <cstdint>

#include "ios.hpp"

#include "cuchar.hpp"
#include "cwchar.hpp"
#include "cstdlib.hpp"

namespace nonstd {

template<typename CharT> class char_traits {
public:
    using char_type = CharT;
    using int_type = int;
    using off_type = streamoff;
    using state_type = mbstate_t;
    using pos_type = state_type;
    // using comparison_category = strong_ordering;

    static constexpr void assign(char_type& r, const char_type& a) noexcept {
        r = a;
    }

    static constexpr char_type* assign(char_type* p,
                                       std::size_t count, 
                                       char_type a) 
    {
        for(size_t i = 0; i < count; i++) 
            p[i] = a;

        return p;
    }

    static constexpr bool eq(char_type a, char_type b) noexcept {
        return a == b;
    }

    static constexpr bool lt(char_type a, char_type b) noexcept {
        return a < b;
    }

    static constexpr char_type* move(char_type* dest, 
                                     const char_type* src, 
                                     std::size_t count) 
    {
        return (char_type*)memmove(dest, src, count);
    }

    static constexpr char_type* copy(char_type* dest, 
                                     const char_type* src, 
                                     size_t count) 
    {
        return (char_type*)memcpy(dest, src, count);
    }
    
    static constexpr int compare(const char_type* s1, 
                                 const char_type* s2, 
                                 std::size_t count) 
    {
        return strncmp(s1, s2, count);
    }

    static constexpr size_t length(const char_type* s) {
        const char_type* str = s;
        while(*s++ != char_type());
        return (size_t)(s - str);
    }

    static constexpr const char_type* find(const char_type* p, 
                                           std::size_t count, 
                                           const char_type& ch)
    {
        for(size_t i = 0; i < count; i++) {
            if(p[i] == ch)
                return &p[i];
        }

        return nullptr;
    }

    static constexpr char_type to_char_type(int_type c) noexcept {
        return static_cast<char_type>(c);
    }

    static constexpr int_type to_int_type(char_type c) noexcept {
        if(c == eof())
            return -1;

        return (int_type)c;
    }

    static constexpr bool eq_int_type(int_type c1, int_type c2) noexcept {
        if(to_int_type(c1) > 0 && to_int_type(c2) > 0)
            return eq(static_cast<char_type>(c1), static_cast<char_type>(c2));

        else if(c1 == eof() && c2 == eof())
            return true;

        else return false;
    }

    static constexpr int_type eof() noexcept {
        return -1;
        // specialize
    }

    static constexpr int_type not_eof(int_type e) noexcept {
        if(!eq_int_type(e, eof()))
            return e;

        return int_type();
    }

};

template<> class char_traits<char> {
public:
    using char_type = char;
    using int_type = int;
    using off_type = streamoff;
    using state_type = mbstate_t;
    using pos_type = state_type;
    // using comparison_category = strong_ordering;

    static constexpr void assign(char_type& r, const char_type& a) noexcept {
        r = a;
    }

    static constexpr char_type* assign(char_type* p,
                                       std::size_t count, 
                                       char_type a) 
    {
        for(size_t i = 0; i < count; i++) 
            p[i] = a;

        return p;
    }

    static constexpr bool eq(char_type a, char_type b) noexcept {
        return a == b;
    }

    static constexpr bool lt(char_type a, char_type b) noexcept {
        return a < b;
    }

    static constexpr char_type* move(char_type* dest, 
                                     const char_type* src, 
                                     std::size_t count) 
    {
        return (char_type*)memmove(dest, src, count);
    }

    static constexpr char_type* copy(char_type* dest, 
                                     const char_type* src, 
                                     size_t count) 
    {
        return (char_type*)memcpy(dest, src, count);
    }
    
    static constexpr int compare(const char_type* s1, 
                                 const char_type* s2, 
                                 std::size_t count) 
    {
        return strncmp(s1, s2, count);
    }

    static constexpr size_t length(const char_type* s) {
        const char_type* str = s;
        while(*s++ != char_type());
        return (size_t)(s - str);
    }

    static constexpr const char_type* find(const char_type* p, 
                                           std::size_t count, 
                                           const char_type& ch)
    {
        for(size_t i = 0; i < count; i++) {
            if(p[i] == ch)
                return &p[i];
        }

        return nullptr;
    }

    static constexpr char_type to_char_type(int_type c) noexcept {
        return static_cast<char_type>(c);
    }

    static constexpr int_type to_int_type(char_type c) noexcept {
        if(c == eof())
            return -1;

        return (int_type)c;
    }

    static constexpr bool eq_int_type(int_type c1, int_type c2) noexcept {
        if(to_int_type(c1) > 0 && to_int_type(c2) > 0)
            return eq(static_cast<char_type>(c1), static_cast<char_type>(c2));

        else if(c1 == eof() && c2 == eof())
            return true;

        else return false;
    }

    static constexpr int_type eof() noexcept {
        return -1;
    }

    static constexpr int_type not_eof(int_type e) noexcept {
        if(!eq_int_type(e, eof()))
            return e;

        return int_type();
    }

};

template<> class char_traits<wchar_t> {
public:
    using char_type = wchar_t;
    using int_type = wint_t;
    using off_type = streamoff;
    using state_type = mbstate_t;
    using pos_type = state_type;
    // using comparison_category = strong_ordering;

    static constexpr void assign(char_type& r, const char_type& a) noexcept {
        r = a;
    }

    static constexpr char_type* assign(char_type* p,
                                       std::size_t count, 
                                       char_type a) 
    {
        for(size_t i = 0; i < count; i++) 
            p[i] = a;

        return p;
    }

    static constexpr bool eq(char_type a, char_type b) noexcept {
        return a == b;
    }

    static constexpr bool lt(char_type a, char_type b) noexcept {
        return a < b;
    }

    static constexpr char_type* move(char_type* dest, 
                                     const char_type* src, 
                                     std::size_t count) 
    {
        return (char_type*)memmove(dest, src, count);
    }

    static constexpr char_type* copy(char_type* dest, 
                                     const char_type* src, 
                                     size_t count) 
    {
        return (char_type*)memcpy(dest, src, count);
    }
    
    static constexpr int compare(const char_type* s1, 
                                 const char_type* s2, 
                                 std::size_t count) 
    {
        return memcmp(s1, s2, count);
    }

    static constexpr size_t length(const char_type* s) {
        const char_type* str = s;
        while(*s++ != char_type());
        return (size_t)(s - str);
    }

    static constexpr const char_type* find(const char_type* p, 
                                           std::size_t count, 
                                           const char_type& ch)
    {
        for(size_t i = 0; i < count; i++) {
            if(p[i] == ch)
                return &p[i];
        }

        return nullptr;
    }

    static constexpr char_type to_char_type(int_type c) noexcept {
        return static_cast<char_type>(c);
    }

    static constexpr int_type to_int_type(char_type c) noexcept {
        if(c == eof())
            return -1;

        return (int_type)c;
    }

    static constexpr bool eq_int_type(int_type c1, int_type c2) noexcept {
        if(to_int_type(c1) > 0 && to_int_type(c2) > 0)
            return eq(static_cast<char_type>(c1), static_cast<char_type>(c2));

        else if(c1 == eof() && c2 == eof())
            return true;

        else return false;
    }

    static constexpr int_type eof() noexcept {
        return -1;
    }

    static constexpr int_type not_eof(int_type e) noexcept {
        if(!eq_int_type(e, eof()))
            return e;

        return int_type();
    }

};

template<> class char_traits<char8_t> {
public:
    using char_type = char8_t;
    using int_type = unsigned int;
    using off_type = streamoff;
    using state_type = mbstate_t;
    using pos_type = state_type;
    // using comparison_category = strong_ordering;

    static constexpr void assign(char_type& r, const char_type& a) noexcept {
        r = a;
    }

    static constexpr char_type* assign(char_type* p,
                                       std::size_t count, 
                                       char_type a) 
    {
        for(size_t i = 0; i < count; i++) 
            p[i] = a;

        return p;
    }

    static constexpr bool eq(char_type a, char_type b) noexcept {
        return a == b;
    }

    static constexpr bool lt(char_type a, char_type b) noexcept {
        return a < b;
    }

    static constexpr char_type* move(char_type* dest, 
                                     const char_type* src, 
                                     std::size_t count) 
    {
        return (char_type*)memmove(dest, src, count);
    }

    static constexpr char_type* copy(char_type* dest, 
                                     const char_type* src, 
                                     size_t count) 
    {
        return (char_type*)memcpy(dest, src, count);
    }
    
    static constexpr int compare(const char_type* s1, 
                                 const char_type* s2, 
                                 std::size_t count) 
    {
        return memcmp(s1, s2, count);
    }

    static constexpr size_t length(const char_type* s) {
        const char_type* str = s;
        while(*s++ != char_type());
        return (size_t)(s - str);
    }

    static constexpr const char_type* find(const char_type* p, 
                                           std::size_t count, 
                                           const char_type& ch)
    {
        for(size_t i = 0; i < count; i++) {
            if(p[i] == ch)
                return &p[i];
        }

        return nullptr;
    }

    static constexpr char_type to_char_type(int_type c) noexcept {
        return static_cast<char_type>(c);
    }

    static constexpr int_type to_int_type(char_type c) noexcept {
        if(c == eof())
            return -1;

        return (int_type)c;
    }

    static constexpr bool eq_int_type(int_type c1, int_type c2) noexcept {
        if(to_int_type(c1) > 0 && to_int_type(c2) > 0)
            return eq(static_cast<char_type>(c1), static_cast<char_type>(c2));

        else if(c1 == eof() && c2 == eof())
            return true;

        else return false;
    }

    static constexpr int_type eof() noexcept {
        return 0x00EDA080;
    }

    static constexpr int_type not_eof(int_type e) noexcept {
        if(!eq_int_type(e, eof()))
            return e;

        return int_type();
    }

};

template<> class char_traits<char16_t> {
public:
    using char_type = char16_t;
    using int_type = uint_least16_t;
    using off_type = streamoff;
    using state_type = mbstate_t;
    using pos_type = state_type;
    // using comparison_category = strong_ordering;

    static constexpr void assign(char_type& r, const char_type& a) noexcept {
        r = a;
    }

    static constexpr char_type* assign(char_type* p,
                                       std::size_t count, 
                                       char_type a) 
    {
        for(size_t i = 0; i < count; i++) 
            p[i] = a;

        return p;
    }

    static constexpr bool eq(char_type a, char_type b) noexcept {
        return a == b;
    }

    static constexpr bool lt(char_type a, char_type b) noexcept {
        return a < b;
    }

    static constexpr char_type* move(char_type* dest, 
                                     const char_type* src, 
                                     std::size_t count) 
    {
        return (char_type*)memmove(dest, src, count);
    }

    static constexpr char_type* copy(char_type* dest, 
                                     const char_type* src, 
                                     size_t count) 
    {
        return (char_type*)memcpy(dest, src, count);
    }
    
    static constexpr int compare(const char_type* s1, 
                                 const char_type* s2, 
                                 std::size_t count) 
    {
        return memcmp(s1, s2, count);
    }

    static constexpr size_t length(const char_type* s) {
        const char_type* str = s;
        while(*s++ != char_type());
        return (size_t)(s - str);
    }

    static constexpr const char_type* find(const char_type* p, 
                                           std::size_t count, 
                                           const char_type& ch)
    {
        for(size_t i = 0; i < count; i++) {
            if(p[i] == ch)
                return &p[i];
        }

        return nullptr;
    }

    static constexpr char_type to_char_type(int_type c) noexcept {
        return static_cast<char_type>(c);
    }

    static constexpr int_type to_int_type(char_type c) noexcept {
        if(c == eof())
            return -1;

        return (int_type)c;
    }

    static constexpr bool eq_int_type(int_type c1, int_type c2) noexcept {
        if(to_int_type(c1) > 0 && to_int_type(c2) > 0)
            return eq(static_cast<char_type>(c1), static_cast<char_type>(c2));

        else if(c1 == eof() && c2 == eof())
            return true;

        else return false;
    }

    static constexpr int_type eof() noexcept {
        return 0xFFFF;
    }

    static constexpr int_type not_eof(int_type e) noexcept {
        if(!eq_int_type(e, eof()))
            return e;

        return int_type();
    }

};

template<> class char_traits<char32_t> {
public:
    using char_type = char32_t;
    using int_type = uint_least32_t;
    using off_type = streamoff;
    using state_type = mbstate_t;
    using pos_type = state_type;
    // using comparison_category = strong_ordering;

    static constexpr void assign(char_type& r, const char_type& a) noexcept {
        r = a;
    }

    static constexpr char_type* assign(char_type* p,
                                       std::size_t count, 
                                       char_type a) 
    {
        for(size_t i = 0; i < count; i++) 
            p[i] = a;

        return p;
    }

    static constexpr bool eq(char_type a, char_type b) noexcept {
        return a == b;
    }

    static constexpr bool lt(char_type a, char_type b) noexcept {
        return a < b;
    }

    static constexpr char_type* move(char_type* dest, 
                                     const char_type* src, 
                                     std::size_t count) 
    {
        return (char_type*)memmove(dest, src, count);
    }

    static constexpr char_type* copy(char_type* dest, 
                                     const char_type* src, 
                                     size_t count) 
    {
        return (char_type*)memcpy(dest, src, count);
    }
    
    static constexpr int compare(const char_type* s1, 
                                 const char_type* s2, 
                                 std::size_t count) 
    {
        return memcmp(s1, s2, count);
    }

    static constexpr size_t length(const char_type* s) {
        const char_type* str = s;
        while(*s++ != char_type());
        return (size_t)(s - str);
    }

    static constexpr const char_type* find(const char_type* p, 
                                           std::size_t count, 
                                           const char_type& ch)
    {
        for(size_t i = 0; i < count; i++) {
            if(p[i] == ch)
                return &p[i];
        }

        return nullptr;
    }

    static constexpr char_type to_char_type(int_type c) noexcept {
        return static_cast<char_type>(c);
    }

    static constexpr int_type to_int_type(char_type c) noexcept {
        if(c == eof())
            return -1;

        return (int_type)c;
    }

    static constexpr bool eq_int_type(int_type c1, int_type c2) noexcept {
        if(to_int_type(c1) > 0 && to_int_type(c2) > 0)
            return eq(static_cast<char_type>(c1), static_cast<char_type>(c2));

        else if(c1 == eof() && c2 == eof())
            return true;

        else return false;
    }

    static constexpr int_type eof() noexcept {
        return 0xFFFFFFFF;
    }

    static constexpr int_type not_eof(int_type e) noexcept {
        if(!eq_int_type(e, eof()))
            return e;

        return int_type();
    }

};

} // namespace nonstd
