#pragma once

#include <stdint.h>

extern "C" void* memcpy(void* dest, const void* src, size_t n) {
    for(size_t i = 0; i < n; i++)
        ((char*)dest)[i] = ((char*)src)[i];

    return dest;
}

extern "C" void* memmove(void* dest, const void* src, size_t n) {
    char buf[n];
    for(size_t i = 0; i < n; i++)
        buf[i] = ((char*)src)[i];

    for(size_t i = 0; i < n; i++)
        ((char*)dest)[i] = buf[i];

    return dest;
}

extern "C" size_t strnlen(const char* s, size_t maxlen) {
    for(size_t i = 0; i < maxlen; i++) {
        if(s[i] == '\0')
            return i;
    }

    return maxlen;
}

extern "C" int strncmp(const char* str1, const char* str2, size_t num) {
    int s1_len = strnlen(str1, num);
    int s2_len = strnlen(str2, num);

    if(s1_len != s2_len)
        return str1 > str2 ? 1 : -1;

    for(size_t i = 0; i < num; i++) {
        if(str1[i] != str2[i]) 
            return str1 > str2 ? 1 : -1;
    }

    return 0;
}

extern "C" int memcmp(const void* str1, const void* str2, size_t num) {
    for(size_t i = 0; i < num; i++) {
        if(((const char*)str1)[i] != ((const char*)str2)[i]) 
            return str1 > str2 ? 1 : -1;
    }

    return 0;
}
