cmake_minimum_required(VERSION 3.16)

include(pico-sdk/pico_sdk_init.cmake)

project(LKICK)

pico_sdk_init()

add_executable(LKICK main.cpp)

target_sources(LKICK PUBLIC ${CMAKE_CURRENT_LIST_DIR}/main.cpp
        ${CMAKE_CURRENT_LIST_DIR}/usb_descriptors.c)

target_include_directories(LKICK PUBLIC
        ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(LKICK pico_stdlib hardware_adc tinyusb_device tinyusb_board)

pico_add_extra_outputs(LKICK)