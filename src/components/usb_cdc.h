#pragma once
#ifndef LKICK_SERIAL_H
#define LKICK_SERIAL_H
#endif //LKICK_SERIAL_H

#include "stdinclude.h"

namespace component {
    namespace usb_cdc {
        static void echo_serial_port(uint8_t itf, uint8_t buf[], uint32_t count);

        [[noreturn]] static void cdc_task(void *pVoid);
    }
}