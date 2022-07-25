#include "stdinclude.h"
#include "serial.h"

namespace led_board {
    void init(const component::serial::stream *stream);
    void init_color();
    void set_color(uint8_t lr, uint8_t lg, uint8_t lb, uint8_t rr, uint8_t rg, uint8_t rb);
    void update();
}