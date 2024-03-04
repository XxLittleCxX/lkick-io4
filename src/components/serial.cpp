#include "stdinclude.h"

namespace component {
    namespace serial {
        stream streams[CFG_TUD_CDC] = {
                stream(0),
                stream(1),
        };


        [[noreturn]] void led_board(void *pVoid){
            led_board::init(&component::serial::streams[1]);
            while(true){
                led_board::update();
                vTaskDelay(2 / portTICK_PERIOD_MS);
            }
        }

        void init() {
            constexpr UBaseType_t core1_CoreAffinityMask = (1 << 1);
            xTaskCreateAffinitySet(component::serial::aimeUpdate, "aime", 2048, nullptr, 6, core1_CoreAffinityMask, nullptr);
            xTaskCreateAffinitySet(component::serial::led_board, "led_board", 4096, nullptr, 6, core1_CoreAffinityMask, nullptr);
        }

        [[noreturn]] void aimeUpdate(void *pVoid) {
            aime_reader::init(&streams[0]);
            while(true){
                aime_reader::update();
                vTaskDelay(2 / portTICK_PERIOD_MS);
            }
        }

        stream::stream(int itf) {
            m_itf = itf;

            //printf("stream::ctor(%d)\n", itf);
        }

        void stream::write(uint8_t byte) const {
            if(byte == 0xE0 || byte == 0xD0) {
                tud_cdc_n_write_char(m_itf, (char)0xD0);
                tud_cdc_n_write_char(m_itf, (char)(byte - 1));
            } else {
                tud_cdc_n_write_char(m_itf, (char)byte);
            }
        }

        void stream::write_head() const {
            tud_cdc_n_write_char(m_itf, (char)0xE0);
        }

        bool stream::read(uint8_t &out) const {
            auto byte = (uint8_t) tud_cdc_n_read_char(m_itf);

            if(byte == 0xD0) {
                out = (uint8_t)(tud_cdc_n_read_char(m_itf) + 1);
                return true;
            }

            out = byte;
            return false;
        }

        bool stream::available() const {
            auto avail = tud_cdc_n_available(m_itf);
            if(avail == 1) {
                uint8_t peek;
                tud_cdc_n_peek(m_itf, &peek);

                if(peek == 0xD0)
                    return false;

                return true;
            } else if(avail > 0) {
                return true;
            }

            return false;
        }

        void stream::flush() const {
            tud_cdc_n_write_flush(m_itf);
        }
    }
}