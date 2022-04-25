#include <cstdint>

#include "new.hpp"

#ifndef NO_DISCARD
#	define NO_DISCARD [[nodiscard]]
#endif

extern "C" NO_DISCARD void* kmalloc(std::size_t count);
extern "C" void kfree(void* ptr);

NO_DISCARD void* operator new(std::size_t count) {
	return kmalloc(count);
}

NO_DISCARD void* operator new[](std::size_t count) {
	return kmalloc(count);
}

NO_DISCARD void* operator new(std::size_t count, align_val_t) {
	return kmalloc(count);
}

NO_DISCARD void* operator new[](std::size_t count, align_val_t) {
	return kmalloc(count);
}

NO_DISCARD void* operator new(std::size_t count, const nothrow_t&) noexcept {
	return kmalloc(count);
}

NO_DISCARD void* operator new[](std::size_t count, const nothrow_t&) noexcept {
	return kmalloc(count);
}

NO_DISCARD void* operator new(std::size_t count, align_val_t, const nothrow_t&) noexcept {
	return kmalloc(count);
}

NO_DISCARD void* operator new[](std::size_t count, align_val_t, const nothrow_t&) noexcept {
	return kmalloc(count);
}

void operator delete(void* ptr) noexcept {
	kfree(ptr);
}

void operator delete[](void* ptr) noexcept {
	kfree(ptr);
}

void operator delete[](void* ptr, align_val_t) noexcept {
	kfree(ptr);
}

void operator delete(void* ptr, std::size_t) noexcept {
	kfree(ptr);
}

void operator delete[](void* ptr, std::size_t) noexcept {
	kfree(ptr);
}

void operator delete(void* ptr, std::size_t, align_val_t) noexcept {
	kfree(ptr);
}

void operator delete[](void* ptr, std::size_t, align_val_t) noexcept {
	kfree(ptr);
}

void operator delete(void* ptr, const nothrow_t&) noexcept {
	kfree(ptr);
}

void operator delete[](void* ptr, const nothrow_t&) noexcept {
	kfree(ptr);
}

void operator delete(void* ptr, align_val_t, const nothrow_t&) noexcept {
	kfree(ptr);
}

void operator delete[](void* ptr, align_val_t, const nothrow_t&) noexcept {
	kfree(ptr);
}

