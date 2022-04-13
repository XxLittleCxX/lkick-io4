#include "stdinclude.h"
#include "serial.h"

namespace led_board {
    void init(const component::serial::stream *stream);
    void update();
}