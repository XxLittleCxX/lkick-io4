#include "stdinclude.h"

int main() {
    board_init();
    uart_init(uart0, BAUD_RATE);
    gpio_set_function(0, GPIO_FUNC_UART);
    gpio_set_function(1, GPIO_FUNC_UART);
    component::ongeki_hardware::init();
    component::io4_usb::usb_init();
    component::serial::init();
    vTaskStartScheduler();
}