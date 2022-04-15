#include "stdinclude.h"

namespace component {
    namespace io4_usb {
        output_t output_data;

        void usb_init() {
            xTaskCreate(tud, "tud", 1024, NULL, 10, NULL);
            //xTaskCreate(update_task, "io4", 256, NULL, 10, NULL);
        }

        [[noreturn]] void tud(void *pVoid) {
            tusb_init();
            while (true) {
                tud_task();

                //component::ongeki_hardware::update_hardware(&output_data);

                if (tud_hid_ready()) {
                    tud_hid_report(0x01, &output_data, sizeof(output_data));
                }
                vTaskDelay(5 / portTICK_PERIOD_MS);
            }
        }

        [[noreturn]] void update_task(void *pVoid) {
            while (true) {
                component::ongeki_hardware::update_hardware(&output_data);
                vTaskDelay(2 / portTICK_PERIOD_MS);
            }
        }

        void process_data(const input_t *data) {
            switch (data->cmd) {
                case io4_usb::SET_COMM_TIMEOUT:
                    //uart_puts(uart1,"IO4: Set Communicate Timeout\n");
                    output_data.system_status = 0x30;
                    break;
                case io4_usb::SET_SAMPLING_COUNT:
                    //uart_puts(uart1,"IO4: Set Sampling Count\n");
                    output_data.system_status = 0x30;
                    break;
                case io4_usb::CLEAR_BOARD_STATUS:
                   // uart_puts(uart1,"IO4: Clear Board Status\n");
                    output_data.system_status = 0x00;
                    break;
                case io4_usb::SET_GENERAL_OUTPUT:
                    //uart_puts(uart1,"IO4: Set General Output\n");
                    break;
                default:
                    break;
            }
        }

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
        extern "C" uint16_t
        tud_hid_get_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer,
                              uint16_t reqlen) {
            return 0;
        }

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
        extern "C" void
        tud_hid_set_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer,
                              uint16_t buf_size) {
            auto data = reinterpret_cast<const component::io4_usb::input_t *>(buffer);
            if (data->report_id== 0x10) {
                process_data(data);
            }
        }
    }
}