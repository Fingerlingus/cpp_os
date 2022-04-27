#pragma once

#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif
 
constexpr uint8_t ATEXIT_MAX_FUNCS = 128;
using __guard = uint64_t;
 
struct atexit_func_entry_t {
	void (*destructor_func)(void *);
	void *obj_ptr;
	void *dso_handle;
};
 
int __cxa_atexit(void (*f)(void *), void *objptr, void *dso);
void __cxa_finalize(void *f);
int __cxa_guard_acquire (__guard *);
void __cxa_guard_release (__guard *);
void __cxa_guard_abort (__guard *);
int __cxa_guard_acquire (__guard *g);
void __cxa_guard_release (__guard *g);
void __cxa_guard_abort (__guard *);
#ifdef __cplusplus
};
#endif
