//
// Created by i on 2022/4/13.
//

#ifndef LKICK_STDINCLUDE_H
#define LKICK_STDINCLUDE_H

#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bsp/board.h"
#include "hardware/uart.h"
#include "pico/stdlib.h"
#include <ctype.h>
#include "tusb.h"

#include "FreeRTOS.h"
#include "task.h"

#include "io4_usb.h"
#include "ongeki_hardware.h"

#include "led_board.h"
#include "comio.h"
#include "serial.h"
#include "aime_reader.h"

#include <PicoLed.hpp>
#define USBD_STACK_SIZE    (3*configMINIMAL_STACK_SIZE/2) * (CFG_TUSB_DEBUG ? 2 : 1)

#define LEVER_PIN 28
#define ONBOARD_RGB_PIN 16 // waveshare rp2040-zero
#define LSIDE_RGB_PIN 15
#define RSIDE_RGB_PIN 9
//#define LED_PIN 11
#define CARD_LIGHT_PIN ONBOARD_RGB_PIN

#define UART_TX_PIN 0
#define UART_RX_PIN 1
#define UART_ID uart0
#define BAUD_RATE 115200

#endif //LKICK_STDINCLUDE_H
