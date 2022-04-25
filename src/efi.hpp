#pragma once

#include <stdint.h>

#define ABI(a) __attribute__((a))
#define EFIAPI ABI(msabi)

namespace efi {

using status = size_t;
using physical_address = uint64_t;
using virtual_address = uint64_t;
using handle = struct { }*;

enum class RESET_TYPE {
    COLD,
    WARM,
    SHUTDOWN,
    PLATFORM_SPECIFIC
};

struct time;
struct time_capabilities;
struct memory_descriptor;
struct guid;
struct capsule_header;
using get_time_fn = status(EFIAPI *) (time* t, 
                                      time_capabilities* capabilities);

using set_time_fn = status(EFIAPI *) (time* t);

using get_wakeup_time_fn = status(EFIAPI *) (bool* enabled, bool* pending, 
                                             time* t);

using set_wakeup_time_fn = status(EFIAPI *) (bool* enable, time* t);

using set_virtual_address_map_fn = 
        status(EFIAPI *) (size_t memory_map_size, size_t descriptor_size, 
                          uint32_t descriptor_version, 
                          memory_descriptor* virtual_map);

using convert_ptr_fn = status(EFIAPI *) (size_t debug_disposition, 
                                         void** addr);

using get_variable_fn = status(EFIAPI *)(char16_t* variable_name, 
                                         guid* vendor_guid, uint32_t attr, 
                                         size_t data_size, void* data);

using get_next_variable_name_fn = status(EFIAPI *) (size_t* variable_name_size,
                                                   char16_t variable_name, 
                                                   guid* vendor_guid);

using set_variable_fn = status(EFIAPI *) (char16_t* variable_name, 
                                         guid* vendor_guid, uint32_t attr,
                                         size_t data_size, void* data);

using get_next_high_mono_count_fn = status(EFIAPI *) (size_t* high_count);

using reset_system_fn = status(EFIAPI *) (RESET_TYPE reset_type, 
                                          status reset_status, 
                                          size_t data_size, void* data);

using update_capsule_fn = 
        status(EFIAPI *) (capsule_header** capsule_header_array, 
                          size_t capsule_count, 
                          physical_address scatter_gather_list);

using query_capsule_capabilities_fn = 
        status(EFIAPI *) (capsule_header** capsule_header_array, 
                          size_t capsule_count, uint64_t* max_capsule_size, 
                          RESET_TYPE* reset_type);

using query_variable_info_fn = 
        status(EFIAPI *) (uint32_t attr, uint64_t* max_variable_storage_size, 
                          uint64_t remaining_variable_storage_size, 
                          uint64_t max_variable_size);

struct table_header {
    uint64_t signature;
    uint32_t revision;
    uint32_t header_size;
    uint32_t crc32;
    uint32_t : 32;
};

struct time {
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t : 8;
    uint32_t nanosecond;
    int16_t timezone;
    uint8_t daylight;
    uint8_t : 8;
};

struct time_capabilities {
    uint32_t resolution;
    uint32_t accuracy;
    bool sets_to_zero;
};

struct memory_descriptor {
    uint32_t type;
    physical_address physical_start;
    virtual_address virtual_start;
    uint64_t num_pages;
    uint64_t attr;
};

struct guid {
    uint32_t data1;
    uint16_t data2;
    uint16_t data3;
    uint8_t data4[8];
};

struct capsule_header {
    guid capsule_guid;
    uint32_t header_size;
    uint32_t flags;
    uint32_t capsule_image_size;
};

struct runtime_services_table {
    table_header header;
    get_time_fn get_time;
    set_time_fn set_time;
    get_wakeup_time_fn get_wakeup_time;
    set_wakeup_time_fn set_wakeup_time;
    set_virtual_address_map_fn set_virtual_address_map;
    convert_ptr_fn convert_ptr;
    get_variable_fn get_variable;
    get_next_variable_name_fn get_next_variable_name;
    set_variable_fn set_variable;
    get_next_high_mono_count_fn get_next_high_monotonic_count;
    reset_system_fn reset;
    update_capsule_fn update_capsule;
    query_capsule_capabilities_fn query_capsule_capabilities;
    query_variable_info_fn query_variable_info;
};

struct system_table {
    table_header header;
    char16_t* firmware_vendor;
    uint32_t firmware_revision;
    handle console_in_handle;
    void* con_in;
    handle console_out_handle;
    void* con_out;
    handle std_err_handle;
    void* std_err;
    runtime_services_table* runtime_services;
    void* boot_services;
    size_t num_table_entries;
    void* config_table;
};

} // namespace efi
