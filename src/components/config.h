#include "stdinclude.h"

// the size of flash on pico is PICO_FLASH_SIZE_BYTES (2 * 1024 * 1024)
// here we use the last 48 * 1024 bytes
#define OFFSET 2000 * 1024

namespace component {
    namespace config {
        enum MODE {
            IO4 = 1,
            CUSTOM_HID,
            KEYBOARD,
            BUCKET
        };

        void init();

        void read_config(uint8_t *buf, uint16_t size);

        void erase_config();

        void write_config(uint8_t *buf, uint16_t size);

        void set_mode(uint8_t mode);

        void cycle_mode();
    }
}