#pragma once

#include <stddef.h>

#ifndef NO_RETURN
#   define NO_RETURN [[noreturn]]
#endif

#ifdef __cplusplus
extern "C" {
#endif

void* malloc(size_t size);
void* calloc(size_t size);
void* realloc(void* ptr, size_t size);
void free(void* ptr);
void* memcpy(void* dest, const void* src, size_t n);
void* memmove(void* dest, const void* src, size_t n);
size_t strnlen(const char* s, size_t maxlen);
int strncmp(const char* str1, const char* str2, size_t num);
int memcmp(const void* str1, const void* str2, size_t num);
#ifdef __cplusplus
NO_RETURN void abort() noexcept;
#else
void abort();
#endif

#ifdef __cplusplus
} // extern "C"
#endif
