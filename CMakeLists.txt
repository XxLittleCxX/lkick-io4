cmake_minimum_required(VERSION 3.16)

include(pico-sdk/pico_sdk_init.cmake)

project(LKICK)


set(PICO_CXX_ENABLE_EXCEPTIONS 1)

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
        ${CMAKE_CURRENT_LIST_DIR}/src/components/config.cpp
        )

target_include_directories(LKICK PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/src/PN532
        ${CMAKE_CURRENT_LIST_DIR}/src/PN532_HSU
        ${CMAKE_CURRENT_LIST_DIR}/src/components
        ${CMAKE_CURRENT_LIST_DIR}/src/boards
        )

add_subdirectory(freertos)
add_subdirectory(src/PicoLed)
target_link_libraries(LKICK pico_stdlib pico_multicore hardware_adc hardware_i2c tinyusb_device tinyusb_board freertos PicoLed hardware_flash)

#pico_enable_stdio_uart(LKICK 1)
pico_add_extra_outputs(LKICK)