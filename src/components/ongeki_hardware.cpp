#include "stdinclude.h"

#include "hardware/adc.h"
#include "pico/bootrom.h"
#include "pico/error.h"
#include "../analogRead/analog_read.h"
#include "scancode.h"

namespace component {
    // hello 5
    const uint8_t PIN_MAP[10] = {
            // L: A B C SIDE MENU
            2, 4, 6, 14, 3,
            // R: A B C SIDE MENU
            10, 12, 13, 8, 7};

    const uint8_t PIN_BIT[10] = {
            // L: A B C SIDE MENU
            1, 1, 1, 0, 1,
            1, 1, 1, 0, 1};

    const uint8_t SWITCH_INDEX[10] = {
            0, 0, 0, 1, 1,
            0, 1, 0, 0, 0
    };

    const uint8_t SWITCH_OFFSET[10] = {
            0, 5, 4, 15, 14,
            1, 0, 15, 14, 13
    };

    const uint8_t KEYBOARD_KEYS[10] = {
        // L: A B C SIDE MENU
        USB_HID_SCANCODE_S, USB_HID_SCANCODE_D, USB_HID_SCANCODE_F, USB_HID_SCANCODE_R, USB_HID_SCANCODE_Q,
        // L: A B C SIDE MENU
        USB_HID_SCANCODE_J, USB_HID_SCANCODE_K, USB_HID_SCANCODE_L, USB_HID_SCANCODE_U, USB_HID_SCANCODE_P
    };

    auto lightColors = PicoLed::addLeds<PicoLed::WS2812B>(pio0, 3,
                                                          11, 6, PicoLed::FORMAT_GRB);

    bool hasI2cLever = false;
    uint8_t addr = 0b0000110;
    uint8_t reg1 = 0x03, reg2 = 0x04;
    ResponsiveAnalogRead analog(LEVER_PIN, false , 0.00005);
    namespace ongeki_hardware {
        void init() {
            for (unsigned char i: PIN_MAP) {
                gpio_init(i);
                gpio_set_dir(i, GPIO_IN);
                gpio_pull_up(i);
            }
            gpio_init(5);
            gpio_set_dir(5, GPIO_IN);
            gpio_pull_up(5);
            gpio_init(PICO_DEFAULT_LED_PIN);
            gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

            lightColors.fill(PicoLed::RGB(255, 255, 255));
            lightColors.show();

            // check i2c lever

            auto writeResult = i2c_write_blocking_until(i2c_default, addr,
                                                        &reg1, 1, true, delayed_by_ms(get_absolute_time(), 10));
            if (writeResult == PICO_ERROR_GENERIC || writeResult == PICO_ERROR_TIMEOUT) {
                // no i2c lever;
                gpio_put(PICO_DEFAULT_LED_PIN, false);
            } else {
                hasI2cLever = true;
                gpio_put(PICO_DEFAULT_LED_PIN, true);
            }
        }

        uint bitPosMap[] =
                {
                        23, 20, 22, 19, 21, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6
                };
        bool inHello = false;

        void set_led(uint ledData) {
            for (auto i = 0; i < 3; i++) {
                // Left1, Left2, Left3, Right1, Right2, Right3
                lightColors.setPixelColor(i, PicoLed::RGB(
                        ((ledData >> bitPosMap[9 + i * 3]) & 1) * 255,
                        ((ledData >> bitPosMap[9 + i * 3 + 1]) & 1) * 255,
                        ((ledData >> bitPosMap[9 + i * 3 + 2]) & 1) * 255
                )); // r
                lightColors.setPixelColor(i + 3, PicoLed::RGB(
                        ((ledData >> bitPosMap[i * 3]) & 1) * 255,
                        ((ledData >> bitPosMap[i * 3 + 1]) & 1) * 255,
                        ((ledData >> bitPosMap[i * 3 + 2]) & 1) * 255
                )); // l
            }
            lightColors.show();
        }

        void set_led_brightness(uint8_t brightness) {
            lightColors.setBrightness(brightness);
        }

        void show_mode_effect(uint8_t mode) {
            using namespace component::config;
            uint key_led = 0;

            switch(mode) {
            case MODE::IO4:
                key_led = 0xA0C440;
                break;
            case MODE::KEYBOARD:
                key_led = 0x8A4900;
                break;
            }
            
            for(int i = 0; i < 2; i++) {
                set_led(key_led);
                vTaskDelay(100 / portTICK_PERIOD_MS);
                set_led(0);
                vTaskDelay(100 / portTICK_PERIOD_MS);
            }
        }


        uint16_t rawArr[6] = {};
        bool coin = false;
        bool rg = false;

        void update_hardware(component::io4_usb::output_t *data) {
            data->switches[0] = 0;
            data->switches[1] = 0;

            inHello = !gpio_get(5);
            do {
                if (!inHello) break;

                if (!gpio_get(PIN_MAP[7])) {
                    reset_usb_boot(0, 0);
                }
                if (!gpio_get(PIN_MAP[0])) {
                    data->switches[0] += (1 << 9) + (1 << 6);
                }

                if (!gpio_get(PIN_MAP[1])) {
                    if (!coin) {
                        data->coin[0].count++;
                        data->coin[1].count++;
                        coin = true;
                    }
                } else {
                    coin = false;
                }

                if (!gpio_get(PIN_MAP[6])) {
                    if (!rg) {
                        rg = true;
                        uint8_t mode = config::cycle_mode();
                        show_mode_effect(mode);
                    }
                } else {
                    rg = false;
                }

            } while (false);

            coin = false;
            rg = false;

            for (auto i = 0; i < 10; i++) {
                auto read = gpio_get(PIN_MAP[i]) ^ 1;
                if (read) {
                    data->switches[SWITCH_INDEX[i]] += 1 << SWITCH_OFFSET[i];
                }
            }


            if (hasI2cLever) {
                uint8_t result1, result2;
                i2c_write_blocking(i2c_default, addr, &reg1, 1, true);
                i2c_read_blocking(i2c_default, addr, &result1, 1, false);

                i2c_write_blocking(i2c_default, addr, &reg2, 1, true);
                i2c_read_blocking(i2c_default, addr, &result2, 1, false);

                uint16_t finalResult = (result1 << 8) + result2;
                finalResult = finalResult > 16383 ? 65535 : finalResult << 2;
                finalResult = ~finalResult;
                data->analog[0] = *(int16_t *) &finalResult;
                data->rotary[0] = *(int16_t *) &finalResult;
//                tud_cdc_write_str(std::to_string(finalResult).c_str());
//                tud_cdc_write_char(' ');
//                tud_cdc_write_str(std::to_string(result2).c_str());
//                tud_cdc_write_char('\r');
//                tud_cdc_write_char('\n');
            } else {
                uint16_t raw = analog.getValue() << 4;
                data->analog[0] = *(int16_t *) &raw;
                data->rotary[0] = *(int16_t *) &raw;
            }
        }

        void update_analog()
        {
            for (int i = 0; i < 200; i++) {
                analog.update();
            }
        }

        void add_key(uint8_t *keycodes, uint8_t key, int index) {
            if (index >= 6) return;
            keycodes[index] = key;
        }

        void update_keyboard(component::io4_usb::output_keyboard_t *data) {
            data->modifier = 0;
            memset(data->keycodes, 0, sizeof(data->keycodes));

            inHello = !gpio_get(5);
            do {
                if (!inHello) break;

                if (!gpio_get(PIN_MAP[7])) {
                    reset_usb_boot(0, 0);
                }

                if (!gpio_get(PIN_MAP[6])) {
                    if (!rg) {
                        rg = true;
                        uint8_t mode = config::cycle_mode();
                        show_mode_effect(mode);
                    }
                } else {
                    rg = false;
                }

            } while (false);

            rg = false;

            int key_index = 0;
            for (auto i = 0; i < 10; i++) {
                auto read = gpio_get(PIN_MAP[i]) ^ PIN_BIT[i];
                if (read) {
                    add_key(data->keycodes, KEYBOARD_KEYS[i], key_index++);
                }
            }
        }
    }
}
