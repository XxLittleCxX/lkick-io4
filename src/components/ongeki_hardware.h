#include "stdinclude.h"

namespace component {
    namespace ongeki_hardware {
        void init();
        void update_hardware(component::io4_usb::output_t *data);
        void update_keyboard(component::io4_usb::output_keyboard_t *data);
        void set_led(uint ledData);
        void set_led_brightness(uint8_t brightness);
    }
}