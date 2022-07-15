#include "stdinclude.h"

namespace component {
    namespace io4_usb {
        enum class coin_condition_t: uint8_t {
            normal = 0x0,
            jam=0x1,
            disconnect=0x2,
            busy=0x3
        };

        struct coin_data_t {
            coin_condition_t condition;
            uint8_t count;
        } __attribute((packed));

        struct output_t {
            int16_t analog[8];
            int16_t rotary[4];
            // 10bit be number
            coin_data_t coin[2];
            uint16_t switches[2];
            uint8_t system_status;
            uint8_t usb_status;
            uint8_t _unused[29];
        } __attribute((packed)) ;

        enum cmd_t : uint8_t {
            SET_COMM_TIMEOUT = 0x01,
            SET_SAMPLING_COUNT = 0x02,
            CLEAR_BOARD_STATUS = 0x03,
            SET_GENERAL_OUTPUT = 0x04,
            SET_PWM_OUTPUT = 0x05,
            UPDATE_FIRMWARE = 0x85,
        };

        struct input_t {
            uint8_t report_id;
            cmd_t cmd;
            uint8_t payload[62];
        } __attribute((packed));

        // keyboard data
        struct output_keyboard_t {
            uint8_t modifier;
            uint8_t reserved;
            uint8_t keycodes[6];
        } __attribute((packed));
        
        void usb_init();

        [[noreturn]] void tud(void *pVoid);

        void process_data(const input_t *data);
    }
}