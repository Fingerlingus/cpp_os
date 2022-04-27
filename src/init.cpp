#include <cstdint>
#include <cstddef>

#include "stdlib/string_view.hpp"

#include "limine.h"

#include "efi.hpp"

#include "page_table.hpp"

#ifndef NO_RETURN
#   define NO_RETURN [[noreturn]]
#endif

extern const void* KERNEL_END;

// The Limine requests can be placed anywhere, but it is important that
// the compiler does not optimise them away, so, usually, they should
// be made volatile or equivalent.

volatile limine_terminal_request terminal_request = {
    .id = LIMINE_TERMINAL_REQUEST,
    .revision = 0
};

volatile limine_rsdp_request rsdp_request = {
    .id = LIMINE_RSDP_REQUEST,
    .revision = 0
};

volatile limine_smbios_request smbios_request = {
    .id = LIMINE_SMBIOS_REQUEST,
    .revision = 0
};

volatile limine_efi_system_table_request efi_system_table_request = {
    .id = LIMINE_EFI_SYSTEM_TABLE_REQUEST,
    .revision = 0
};

volatile limine_boot_time_request boot_time_request = {
    .id = LIMINE_BOOT_TIME_REQUEST,
    .revision = 0
};

volatile limine_kernel_address_request kernel_address_request = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 0
};

NO_RETURN static void done(void) {
    for (;;) {
        __asm__("cli\n\thlt\n\t");
    }
}

static void init_print(limine_terminal* terminal, 
                       limine_terminal_write write, 
                       kstd::string_view s) 
{
    write(terminal, s.data(), s.length());
}

// The following will be our kernel's entry point.
extern "C" void _start(void) {
    // Ensure we got a terminal
    if (terminal_request.response == nullptr || 
        terminal_request.response->terminal_count < 1) 
    {
        done();
    }

    auto* terminal = terminal_request.response->terminals[0];
    auto write = terminal_request.response->write;

    void* efi_system_table = nullptr;
    if(efi_system_table_request.response) {
        efi_system_table = (void*)efi_system_table_request.response->address;
        if(efi_system_table != nullptr)
            init_print(terminal, write, "+ Found EFI System Table.\n");
        else {
            init_print(terminal, 
                       write, 
                       "- FATAL: Unable to find EFI System Table (probably not in EFI mode).\n");
            done();
        }
    } else {
        init_print(terminal, 
                   write, 
                   "- FATAL: Unable to find EFI System Table (probably not in EFI mode).\n");
        done();
    }

    void* rsdp = nullptr;
    if(rsdp_request.response) {
        rsdp = (void*)rsdp_request.response->address;
        if(rsdp != nullptr)
            init_print(terminal, write, "+ Found RSDP.\n");
        else
            init_print(terminal, write, "- Unable to find RSDP.\n");
    } else init_print(terminal, write, "- Unable to find RSDP.\n");

    void* smbios_entry = nullptr;
    if(smbios_request.response) {
        smbios_entry = (void*)smbios_request.response->entry_64;
        if(smbios_entry != nullptr)
            init_print(terminal, write, "+ Found SMBIOS entry point.\n");
        else
            init_print(terminal, write, "- Unable to find SMBIOS entry point.\n");
    } else init_print(terminal, write, "- Unable to find SMBIOS entry point.\n"); 

    int64_t boot_time = 0;
    if(boot_time_request.response) {
        boot_time = (uint64_t)boot_time_request.response->boot_time;
        if(boot_time > 0)
            init_print(terminal, write, "+ Retrieved boot time.\n");
        else
            init_print(terminal, write, "- Unable to retrieve boot time.\n");
    } else init_print(terminal, write, "- Unable to retrieve boot time.\n");

    void* kernel_physical_base = nullptr;
    void* kernel_virtual_base = nullptr;
    if(kernel_address_request.response) {
        kernel_physical_base = 
            (void*)kernel_address_request.response->physical_base;
        kernel_virtual_base = 
            (void*)kernel_address_request.response->virtual_base;
    }
    std::size_t kernel_size = 
        (std::size_t)((char*)KERNEL_END - (char*)kernel_virtual_base);

    std::size_t kernel_size_in_pages = 
        kernel_size % 4096 == 0 ? kernel_size / 4096 : kernel_size / 4096 + 1;
    
    pt->init();
    pt->alloc_pages(kernel_virtual_base, kernel_size_in_pages);
    // We're done, just hang...
    done();
}
