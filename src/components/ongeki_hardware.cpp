#include "stdinclude.h"
#include <PicoLed.hpp>

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
            0,0,0,1,1,
            0,1,0,0,0
    };

    const uint8_t SWITCH_OFFSET[10] = {
            0,5,4,15,14,
            1,0,15,14,13
    };

    auto card_light = PicoLed::addLeds<PicoLed::WS2812B>(pio1, 0,
                                                       CARD_LIGHT_PIN, 16, PicoLed::FORMAT_GRB);
    auto lightColors = PicoLed::addLeds<PicoLed::WS2812B>(pio1, 1,
                                                         LSIDE_RGB_PIN, 6, PicoLed::FORMAT_GRB);
    auto leftColors = PicoLed::addLeds<PicoLed::WS2812B>(pio1, 2,
                                                         RSIDE_RGB_PIN, 6, PicoLed::FORMAT_GRB);
    auto rightColors = PicoLed::addLeds<PicoLed::WS2812B>(pio1, 3,
                                                         11, 6, PicoLed::FORMAT_GRB);

    namespace ongeki_hardware {
        void init(){
            card_light.setBrightness(0x10);
            card_light.fill(PicoLed::RGB(255,255,255));
            card_light.show();

            for (unsigned char i : PIN_MAP)
            {
                //uart_putc(uart0, PIN_MAP[i]);
                gpio_init(i);
                gpio_set_dir(i, GPIO_IN);
                gpio_pull_up(i);
            }
            gpio_init(5);
            gpio_set_dir(5, GPIO_IN);
            gpio_pull_up(5);
            // hello
        }
        void update_hardware(component::io4_usb::output_t *data) {
            data->switches[0] = 0;
            data->switches[1] = 0;
            for(auto i=0;i<10;i++)
            {
                auto read = gpio_get(PIN_MAP[i]) ^ PIN_BIT[i];
               if(read){
                   data->switches[SWITCH_INDEX[i]] += 1<< SWITCH_OFFSET[i];
               }
            }
            if(!gpio_get(5)){
                data->switches[0] += (1 << 9) + (1 << 6);
            }


//            data->switches[0] = rand();
//            data->switches[1] = rand();
            // output_data.coin[0] = rand();
            // output_data.coin[1] = rand();
        }
    }
}