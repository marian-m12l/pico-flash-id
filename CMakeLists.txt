cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include(pico_sdk_import.cmake)

project(pico-flash-id C CXX ASM)

pico_sdk_init()


add_executable(pico-flash-id main.c)

pico_set_binary_type(pico-flash-id copy_to_ram)

pico_set_program_name(pico-flash-id "pico-flash-id")
pico_set_program_version(pico-flash-id "0.1")

pico_enable_stdio_uart(pico-flash-id 0)
pico_enable_stdio_usb(pico-flash-id 1)

target_link_libraries(pico-flash-id pico_stdlib hardware_flash)

pico_add_extra_outputs(pico-flash-id)
