#pragma once

#if defined __GNU_C__
#   define IS_GCC 1
#elif defined _MSC_VER
#   define IS_MSVC 1
#endif

#define _STR(s) #s
#define STR(s) _STR(s)

#define PRAGMA(x) _Pragma(STR(x))

#define PACK(s) PRAGMA(pack(push, 1)) s; PRAGMA(pack(pop))
#define ALIGN(n) alignas(n)

#if IS_GCC
#   define ABI(a) __attribute__((a))
#elif IS_MSVC
#   define ABI(a) __declspec(a)
#endif

#define NO_DISCARD [[nodiscard]]
