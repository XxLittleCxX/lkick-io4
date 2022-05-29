#include "stdinclude.h"

namespace component {
    namespace config {
        const uint8_t *flash_target_contents = (const uint8_t *) (XIP_BASE + OFFSET);
        const uint8_t *mode = flash_target_contents;

        void init() {
//            uint8_t id[8];
//            flash_get_unique_id(id);
            if (*mode == 0) { // first time booting
                set_mode(MODE::IO4);
            }
        }

        void read_config(uint8_t *buf, uint16_t size) {
            memcpy(buf, flash_target_contents, size);
        }

        void erase_config() {
            // erase 4kB
            uint32_t ints = save_and_disable_interrupts();
            flash_range_erase(OFFSET, FLASH_SECTOR_SIZE);
            restore_interrupts(ints);
        }

        void write_config(uint8_t *buf, uint16_t size) {
            erase_config();
            uint32_t ints = save_and_disable_interrupts();
            uint8_t buffer[FLASH_PAGE_SIZE];
            memcpy(buffer, buf, size);
            flash_range_program(OFFSET, buffer, FLASH_PAGE_SIZE);
            restore_interrupts(ints);
        }

        void set_mode(uint8_t m) {
            uint8_t buffer[FLASH_PAGE_SIZE];
            read_config(buffer, FLASH_PAGE_SIZE);
            buffer[0] = m;
            write_config(buffer, FLASH_PAGE_SIZE);
        }

        void cycle_mode() {
            switch (*mode) {
                case MODE::IO4:
                    set_mode(MODE::CUSTOM_HID);
                    break;
                case MODE::CUSTOM_HID:
                    set_mode(MODE::KEYBOARD);
                    break;
                case MODE::KEYBOARD:
                    set_mode(MODE::IO4);
                    break;
                default:
                    gpio_put(PICO_DEFAULT_LED_PIN, true);
                    vTaskDelay(1000 / portTICK_PERIOD_MS);
                    gpio_put(PICO_DEFAULT_LED_PIN, false);
                    set_mode(MODE::IO4);
                    return;
            }
            for (int i = 0; i < *mode; ++i) {
                gpio_put(PICO_DEFAULT_LED_PIN, true);
                vTaskDelay(100 / portTICK_PERIOD_MS);
                gpio_put(PICO_DEFAULT_LED_PIN, false);
                vTaskDelay(100 / portTICK_PERIOD_MS);
            }
        }
    }
}