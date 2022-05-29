#include "stdinclude.h"

int main() {
    board_init();
    uart_init(uart0, BAUD_RATE);
    i2c_init(i2c_default, 100*1000);
    gpio_set_function(21, GPIO_FUNC_I2C);
    gpio_set_function(20, GPIO_FUNC_I2C);
    gpio_pull_up(21);
    gpio_pull_up(20);
    gpio_set_function(0, GPIO_FUNC_UART);
    gpio_set_function(1, GPIO_FUNC_UART);
    component::config::init();
    component::ongeki_hardware::init();
    component::io4_usb::usb_init();
    component::serial::init();
    vTaskStartScheduler();
}