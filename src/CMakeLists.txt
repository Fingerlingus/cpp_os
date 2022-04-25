cmake_minimum_required(VERSION 3.16)

project(os)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_COMPILER /usr/bin/clang++)
set(CMAKE_VERBOSE_MAKEFILE OFF)

add_executable(kernel init.cpp memory.cpp stdlib/stdlib.c)
set_target_properties(kernel PROPERTIES SUFFIX ".elf")

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} --target=x86_64)
target_compile_options(kernel PUBLIC -Wall -Wextra 
                              -ffreestanding -fno-stack-protector 
                              -fno-stack-check -fPIE 
                              -mabi=sysv -mno-80387 -mno-mmx 
                              -mno-3dnow -mno-sse -mno-sse2 -mno-sse3 -mno-sse4 
                              -mcmodel=large -mno-avx -mno-avx2 -mno-red-zone
                              -fno-exceptions -fno-rtti
                              -I/usr/include/c++/11.2.0 
                              -I/usr/include/c++/11.2.0/x86_64-pc-linux-gnu 
                              -l/usr/local/lib/gcc/x86_64-elf/11.2.0/libgcc.a)

target_compile_options(kernel PUBLIC $<$<CONFIG:Debug>:-O0 -g -Wpedantic>)
target_compile_options(kernel PUBLIC $<$<CONFIG:RelWithDebInfo>:-03 -g>)
target_compile_options(kernel PUBLIC $<$<CONFIG:Release>:-03>)
target_include_directories(kernel PUBLIC ${PROJECT_SOURCE_DIR})
target_include_directories(kernel PUBLIC deps/libcxxrt/src)
target_include_directories(kernel PUBLIC deps/libunwind/include)
target_compile_definitions(kernel PUBLIC LIBCXXRT_WEAK_LOCKS)

target_link_options(kernel PUBLIC -T ${PROJECT_SOURCE_DIR}/linker.ld -nostdlib 
                                /usr/local/lib/gcc/x86_64-elf/11.3.0/libgcc.a)
