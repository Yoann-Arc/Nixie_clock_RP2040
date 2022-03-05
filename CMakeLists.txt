cmake_minimum_required(VERSION 3.21)
include(pico_sdk_import.cmake)
project(Prog_Rp)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

set(CMAKE_CXX_STANDARD 14)

add_executable(Prog_Rp main.cpp)

pico_enable_stdio_usb(Prog_Rp 1)
pico_enable_stdio_uart(Prog_Rp 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(Prog_Rp)

# Pull in our pico_stdlib which pulls in commonly used features
target_link_libraries(Prog_Rp pico_stdlib)