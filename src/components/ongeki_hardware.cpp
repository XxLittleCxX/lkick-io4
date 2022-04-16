#include "stdinclude.h"

#include "hardware/adc.h"
#include "pico/bootrom.h"

namespace component {
    // hello 5
    const uint8_t PIN_MAP[10] = {
            // L: A B C SIDE MENU
            2, 4, 6, 14, 3,
            // R: A B C SIDE MENU
            10, 12, 13, 8, 7};

    const uint8_t PIN_BIT[10] = {
            // L: A B C SIDE MENU
            1, 1, 1, 1, 1,
            1, 1, 1, 1, 1};

    const uint8_t SWITCH_INDEX[10] = {
            0, 0, 0, 1, 1,
            0, 1, 0, 0, 0
    };

    const uint8_t SWITCH_OFFSET[10] = {
            0, 5, 4, 15, 14,
            1, 0, 15, 14, 13
    };

    auto card_light = PicoLed::addLeds<PicoLed::WS2812B>(pio1, 0,
                                                         CARD_LIGHT_PIN, 16, PicoLed::FORMAT_GRB);

    auto lightColors = PicoLed::addLeds<PicoLed::WS2812B>(pio0, 3,
                                                          11, 6, PicoLed::FORMAT_GRB);

    namespace ongeki_hardware {
        void init() {
            card_light.setBrightness(0x10);
            card_light.fill(PicoLed::RGB(255, 255, 255));
            card_light.show();

            for (unsigned char i: PIN_MAP) {
                gpio_init(i);
                gpio_set_dir(i, GPIO_IN);
                gpio_pull_up(i);
            }
            gpio_init(5);
            gpio_set_dir(5, GPIO_IN);
            gpio_pull_up(5);
            adc_init();
            adc_gpio_init(LEVER_PIN);

            adc_select_input(2);

            lightColors.fillGradient(PicoLed::RGB(255, 0, 0), PicoLed::RGB(0, 0, 255));
            lightColors.show();
        }

        uint bitPosMap[] =
                {
                        23, 20, 22, 19, 21, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6
                };

        void set_led(uint ledData){
            for(auto i=0;i<3;i++){
                // Left1, Left2, Left3, Right1, Right2, Right3
                lightColors.setPixelColor(i, PicoLed::RGB(
                        ((ledData >> bitPosMap[9+i*3]) & 1) * 255,
                        ((ledData >> bitPosMap[9+i*3+1]) & 1) * 255,
                        ((ledData >> bitPosMap[9+i*3+2]) & 1) * 255
                        )); // r
                lightColors.setPixelColor(i+3, PicoLed::RGB(
                        ((ledData >> bitPosMap[i*3]) & 1) * 255,
                        ((ledData >> bitPosMap[i*3+1]) & 1) * 255,
                        ((ledData >> bitPosMap[i*3+2]) & 1) * 255
                )); // l
            }
            lightColors.show();
        }


        void update_hardware(component::io4_usb::output_t *data) {
            bool inHello = !gpio_get(5);

            if (inHello) {
//                lightColors.clear();
//                lightColors.setPixelColor(2, PicoLed::RGB(255, 0, 0));
//                lightColors.setPixelColor(3, PicoLed::RGB(255, 255, 255));
//                lightColors.setPixelColor(4, PicoLed::RGB(255, 255, 0));
//                lightColors.show();
                if (!gpio_get(PIN_MAP[7])) {
                    reset_usb_boot(0, 0);
                }
                if (!gpio_get(PIN_MAP[0])) {
                    data->switches[0] += (1 << 9) + (1 << 6);
                }
                if (!gpio_get(PIN_MAP[1])) {
                    data->coin[0].count++;
                    data->coin[1].count++;
                }
            }else{
                data->switches[0] = 0;
                data->switches[1] = 0;
                for (auto i = 0; i < 10; i++) {
                    auto read = gpio_get(PIN_MAP[i]) ^ PIN_BIT[i];
                    if (read) {
                        data->switches[SWITCH_INDEX[i]] += 1 << SWITCH_OFFSET[i];
                    }
                }
            }

            uint16_t raw = adc_read();
            data->analog[0] = *(int16_t *) &raw;
            data->rotary[0] = *(int16_t *) &raw;
        }
    }
}