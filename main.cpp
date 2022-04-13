#include "stdinclude.h"

int main() {
    stdio_init_all();

    board_init();
    uart_init(UART_ID, BAUD_RATE);

    component::io4_usb::usb_init();
    component::serial::init();
    vTaskStartScheduler();

//    while(true){
//        component::serial::update();
//        vTaskDelay(2 / portTICK_PERIOD_MS);
//    }
}