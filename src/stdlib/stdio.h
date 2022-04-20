#pragma once

#include "char_traits.hpp"

#ifndef __cplusplus
#   define EOF -1 
#   define WEOF -1
#else
#   define EOF char_traits<char>::eof()
#   define WEOF char_traits<wchar_t>::eof()
#endif
