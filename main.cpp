#include "stdinclude.h"

int main() {
    stdio_uart_init_full(uart1, 115200, 4,5);

    board_init();
    uart_init(uart0, BAUD_RATE);
    gpio_set_function(0, GPIO_FUNC_UART);
    gpio_set_function(1, GPIO_FUNC_UART);

    uart_init(uart1, 115200);

    // Set the GPIO pin mux to the UART - 0 is TX, 1 is RX
    gpio_set_function(4, GPIO_FUNC_UART);
    gpio_set_function(5, GPIO_FUNC_UART);

//    uint8_t data[] = {0x55,0x55,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0x03,0xfd,0xd4,0x14,0x01,0x17,0x00};
//    uart_write_blocking(uart0, data, 52);
//    while(1){
//        if(uart_is_readable(uart0)){
//            char read = uart_getc(uart0);
//            uart_putc(uart1, read);
//        }
//    }

    //uart_puts(uart1, "Hello world!\n");
    printf("hello\n");
    component::io4_usb::usb_init();
    component::serial::init();
    vTaskStartScheduler();
}