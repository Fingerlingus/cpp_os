#include "stdlib.h"

#ifdef __cplusplus
extern "C" {
#endif

void* kmalloc(size_t);
void* kfree(void*);

void* malloc(size_t size) {
    return kmalloc(size);
}

void* calloc(size_t size) {
    return kmalloc(size);
}

void* realloc(void* ptr, size_t size) {
    void* p = malloc(size);
    memcpy(p, ptr, size);
    return p;
}

void free(void* ptr) {
    kfree(ptr);
}

void* memset(void* str, int c, size_t n) {
    for(size_t i = 0; i < n; i++)
        ((int*)str)[i] = c;

    return str;
}

void* memcpy(void* dest, const void* src, size_t n) {
    for(size_t i = 0; i < n; i++)
        ((char*)dest)[i] = ((char*)src)[i];

    return dest;
}

void* memmove(void* dest, const void* src, size_t n) {
    char buf[n];
    for(size_t i = 0; i < n; i++)
        buf[i] = ((char*)src)[i];

    for(size_t i = 0; i < n; i++)
        ((char*)dest)[i] = buf[i];

    return dest;
}

size_t strlen(const char* str) {
    const char* s = str;
    while(*str++ != '\0');
    return str - s;
}

size_t strnlen(const char* str, size_t maxlen) {
    for(size_t i = 0; i < maxlen; i++) {
        if(str[i] == '\0')
            return i;
    }

    return maxlen;
}

int strncmp(const char* str1, const char* str2, size_t num) {
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

int memcmp(const void* str1, const void* str2, size_t num) {
    for(size_t i = 0; i < num; i++) {
        if(((const char*)str1)[i] != ((const char*)str2)[i]) 
            return str1 > str2 ? 1 : -1;
    }

    return 0;
}

#ifdef __cplusplus
NO_RETURN void abort() noexcept {
#else 
void abort() {
#endif
    for(;;)
        asm("cli\n\thlt\n\t");
}

#ifdef __cplusplus
} // extern "C"
#endif
