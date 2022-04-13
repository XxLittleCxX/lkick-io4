#include "stdinclude.h"

namespace component {
    namespace io4_usb {
        output_t output_data;

        void usb_init() {
            xTaskCreate(tud, "tud", USBD_STACK_SIZE, NULL, 5, NULL);
            xTaskCreate(update_task, "io4", 256, NULL, 10, NULL);
        }

        [[noreturn]] void tud(void *pVoid) {
            tusb_init();
            while (true) {
                tud_task();

                if (tud_hid_ready()) {
                    tud_hid_report(0x01, &output_data, sizeof(output_data));
                }
                vTaskDelay(2 / portTICK_PERIOD_MS);
            }
        }

        [[noreturn]] void update_task(void *pVoid) {
            while (true) {
                memset(&output_data, 0, sizeof(output_data));
                output_data.system_status = 0x02;
                component::ongeki_hardware::update_hardware(&output_data);
                vTaskDelay(2 / portTICK_PERIOD_MS);
            }
        }

        void process_data(const input_t *data) {
            switch (data->cmd) {
                case SET_COMM_TIMEOUT:
                    output_data.system_status = 0x30;
                    break;
                case SET_SAMPLING_COUNT:
                    output_data.system_status = 0x30;
                    break;
                case CLEAR_BOARD_STATUS:
                    output_data.system_status = 0x00;
                    break;
                case SET_GENERAL_OUTPUT:

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
            // TODO not Implemented
            (void) itf;
            (void) report_id;
            (void) report_type;
            (void) buffer;
            (void) reqlen;

            return 0;
        }

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
        extern "C" void
        tud_hid_set_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer,
                              uint16_t buf_size) {
            // This example doesn't use multiple report and report ID
            (void) itf;
            (void) report_id;
            (void) report_type;

//            printf("recv %d bytes at itf %d\n", buf_size, itf);
            for (auto i = 0; i < buf_size && i < 32; i++) {
                if (i > 0 && i % 0x10 == 0) {
                    printf("\n");
                }

//                printf("%02hhX ", buffer[i]);
            }

            printf("\n\n");

            auto data = reinterpret_cast<const input_t *>(buffer);
            if (data->report_id == 0x10) {
                process_data(data);
            }
        }

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
        void tud_mount_cb(void) {

        }

// Invoked when device is unmounted
        void tud_umount_cb(void) {

        }

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
        void tud_suspend_cb(bool remote_wakeup_en) {
            (void) remote_wakeup_en;

        }

// Invoked when usb bus is resumed
        void tud_resume_cb(void) {

        }

    }
}