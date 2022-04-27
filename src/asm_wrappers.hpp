// generic assembly instruction wrappers
// apologies for the AT&T syntax
// GCC inline asm doesn't play nice with the correct syntax

#include <cstdint>

#if defined __x86_64__ || defined __i386__

extern "C" inline uint8_t inb(uint16_t port) {
    uint8_t val;
    __asm__(
            "inb %1, %0\n\t"
            :"=a"(val)
            :"d"(port)
            :
           );
    return val;
}

extern "C" inline uint16_t inw(uint16_t port) {
    uint16_t val;
    __asm__(
            "inw %1, %0\n\t"
            :"=a"(val)
            :"d"(port)
            :
           );
    return val;
}

extern "C" inline uint32_t ind(uint16_t port) {
    uint32_t val;
    __asm__(
            "inl %1, %0\n\t"
            :"=a"(val)
            :"d"(port)
            :
           );
    return val;
}

extern "C" inline void outb(uint16_t port, uint8_t data) {
    __asm__(
            "outb %1, %0\n\t"
            :
            :"d"(port),"a"(data)
            :
           );
}

extern "C" inline void outw(uint16_t port, uint16_t data) {
    __asm__(
            "outw %1, %0\n\t"
            :
            :"d"(port),"a"(data)
            :
           );
}

extern "C" inline void outd(uint16_t port, uint32_t data) {
    __asm__(
            "outl %1, %0\n\t"
            :
            :"d"(port),"a"(data)
            :
           );
}

extern "C" inline void sti() {
    __asm__("sti\n\t");
}

extern "C" inline void cli() {
    __asm__("cli\n\t");
}

extern "C" inline void hlt() {
    __asm__("hlt\n\t");
}

extern "C" inline void lcr3(void* page_table) {
    __asm__("mov %0, %%cr3\n\t"
            :
            :"r"(page_table)
            :
           );
}

//extern "C" rflags_t	get_flags();

#endif

