#include "stdinclude.h"

#define CONFIG_SIZE FLASH_SECTOR_SIZE

namespace component {
    namespace config {
        const uint8_t *flash_target_contents = (const uint8_t *) (XIP_BASE + OFFSET);
//        const uint8_t *mode = &flash_target_contents[0];
        uint8_t mode_field = MODE::IO4;
        const uint8_t *mode = &mode_field;

        void init() {
//            uint8_t id[8];
//            flash_get_unique_id(id);
            switch (*mode)
            {
            case MODE::IO4:
            case MODE::KEYBOARD:
                break;
            default:
                set_mode(MODE::IO4);
                break;
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
            uint32_t ints = save_and_disable_interrupts();
            uint8_t buffer[CONFIG_SIZE];
            memcpy(buffer, buf, size);
            flash_range_erase(OFFSET, CONFIG_SIZE);
            flash_range_program(OFFSET, buffer, CONFIG_SIZE);
            restore_interrupts(ints);
        }

        void set_mode(uint8_t m) {
            /*
            uint8_t buffer[CONFIG_SIZE];
            read_config(buffer, CONFIG_SIZE);
            buffer[0] = m;
            write_config(buffer, CONFIG_SIZE);
            */
            mode_field = m;
            return;
        }
        
        uint8_t get_mode() {
            return *mode;
        }

        uint8_t cycle_mode() {
            switch (*mode) {
                case MODE::IO4:
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
                    return *mode;
            }
            for (int i = 0; i < *mode; ++i) {
                gpio_put(PICO_DEFAULT_LED_PIN, true);
                vTaskDelay(100 / portTICK_PERIOD_MS);
                gpio_put(PICO_DEFAULT_LED_PIN, false);
                vTaskDelay(100 / portTICK_PERIOD_MS);
            }
            return *mode;
        }
    }
}