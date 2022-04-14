cmake_minimum_required(VERSION 3.16)

include(pico-sdk/pico_sdk_init.cmake)

project(LKICK)

pico_sdk_init()

add_executable(LKICK main.cpp stdinclude.h)

target_sources(LKICK PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/main.cpp
        ${CMAKE_CURRENT_LIST_DIR}/usb_descriptors.c
        ${CMAKE_CURRENT_LIST_DIR}/src/components/io4_usb.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/components/ongeki_hardware.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/components/serial.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/boards/led_board.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/boards/aime_reader.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/PN532_HSU/PN532_HSU.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/PN532/PN532.cpp
        )

target_include_directories(LKICK PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/src/PN532
        ${CMAKE_CURRENT_LIST_DIR}/src/PN532_HSU
        ${CMAKE_CURRENT_LIST_DIR}/src/components
        ${CMAKE_CURRENT_LIST_DIR}/src/boards
        )

add_subdirectory(freertos)

target_link_libraries(LKICK pico_stdlib hardware_adc tinyusb_device tinyusb_board freertos hardware_i2c)

pico_enable_stdio_uart(LKICK 1)
pico_add_extra_outputs(LKICK)