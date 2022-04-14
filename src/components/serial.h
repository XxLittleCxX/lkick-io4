#pragma once
#include "stdinclude.h"
namespace component {
    namespace serial {
        class stream {
        public:
            explicit stream(int itf);

            bool read(uint8_t &out) const;
            void write(uint8_t byte) const;
            void write_head() const;
            bool available() const;

            void flush() const;

        private:
            int m_itf;
        };

        typedef void (* update_cb_t)(const stream &stream);

        void init();

        [[noreturn]] void update(void *abc);
        [[noreturn]] void aimeUpdate(void *abc);
    }
}