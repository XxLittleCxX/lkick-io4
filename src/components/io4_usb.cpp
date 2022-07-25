#include "stdinclude.h"

#include "config.h"
#include "Effects/Marquee.hpp"

namespace component {
    namespace io4_usb {
        output_t output_data;
        output_keyboard_t output_keyboard_data;

        void usb_init() {
            xTaskCreate(tud, "tud", 2048, NULL, 10, NULL);
        }

        [[noreturn]] void tud(void *pVoid) {
            using namespace component::config;
            uint8_t last_mode = 0;

            auto aime_led = aime_reader::getController();
            std::vector<PicoLed::Color> rainbow_palette;
            rainbow_palette.push_back(PicoLed::RGB(255, 0, 0));
            rainbow_palette.push_back(PicoLed::RGB(255, 255, 0));
            rainbow_palette.push_back(PicoLed::RGB(0, 255, 0));
            rainbow_palette.push_back(PicoLed::RGB(0, 255, 255));
            rainbow_palette.push_back(PicoLed::RGB(0, 0, 255));
            rainbow_palette.push_back(PicoLed::RGB(255, 0, 255));
            PicoLed::Marquee effect_marquee(aime_led, rainbow_palette, 3.0, -2.0, 1.0);

            tusb_init();
            while (true) {
                tud_task();
                
                uint8_t this_mode = get_mode();
                switch (this_mode) {
                case MODE::KEYBOARD:
                    if (last_mode != this_mode) {
                        aime_led.setBrightness(0x2f);
                        led_board::set_color(0xff, 0xff, 0xff, 0xff, 0xff, 0xff);
                        using namespace component::ongeki_hardware;
                        set_led(0xffffff);
                        set_led_brightness(0x4f);
                    }
                    if (effect_marquee.animate())
                        aime_led.show();

                    component::ongeki_hardware::update_keyboard(&output_keyboard_data);

                    if (tud_hid_ready())
                        tud_hid_report(0x02, &output_keyboard_data, sizeof(output_keyboard_data));
                    break;
                case MODE::IO4:
                    if (last_mode != this_mode){
                        aime_led.setBrightness(0xff);
                        aime_reader::set_card_light(0, 0, 0);
                        led_board::init_color();
                        using namespace component::ongeki_hardware;
                        set_led(0xffffff);
                        set_led_brightness(0xff);
                    }                  

                    component::ongeki_hardware::update_hardware(&output_data);

                    if (tud_hid_ready())
                        tud_hid_report(0x01, &output_data, sizeof(output_data));
                    break;
                default:
                    break;
                }
                last_mode = this_mode;
                vTaskDelay(6 / portTICK_PERIOD_MS);
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
                    output_data.coin[0].count = 0;
                    output_data.coin[0].condition = io4_usb::coin_condition_t::normal;
                    output_data.coin[1].count = 0;
                    output_data.coin[1].condition = io4_usb::coin_condition_t::normal;
                    output_data.system_status = 0x00;
                    break;
                case io4_usb::SET_GENERAL_OUTPUT: {
                    uint32_t ledData = data->payload[0] << 16 | data->payload[1] << 8 | data->payload[2];
                    component::ongeki_hardware::set_led(ledData);
                    //uart_puts(uart1,"IO4: Set General Output\n");
                    break;
                }
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