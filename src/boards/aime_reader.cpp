#include "stdinclude.h"
#include "PN532.h"
#include "PN532_HSU.h"

namespace aime_reader {
    const component::serial::stream *stream;

    io_packet_t *out;

    io_packet_t *in;
    uint8_t in_size, checksum;

//    i2c_inst_t *i2c = i2c0;

    //PN532_I2C pn532_i2c(*i2c);
    PN532_HSU pn532_hsu;
    PN532 nfc(pn532_hsu);

    const uint LED_PIN = PICO_DEFAULT_LED_PIN;

    void init(const component::serial::stream *input) {
//        const uint sda_pin = 0;
//        const uint scl_pin = 1;
//        i2c_init(i2c, 100 * 1000);
//        gpio_set_function(sda_pin, GPIO_FUNC_I2C);
//        gpio_set_function(scl_pin, GPIO_FUNC_I2C);
//        gpio_pull_up(sda_pin);
//        gpio_pull_up(scl_pin);

        // 0x48 address
        stream = input;
        nfc.begin();
//
//
//
        uint32_t versiondata = nfc.getFirmwareVersion();
        uart_puts(uart1, (versiondata ? "y" : "n"));
        nfc.setPassiveActivationRetries(0x10);//设定等待次数
        nfc.SAMConfig();

        gpio_init(LED_PIN);
        gpio_set_dir(LED_PIN,GPIO_OUT);
//        uint8_t rxdata;
//        int ret = i2c_read_blocking(i2c, 0x48, &rxdata, 1, false);
//        uart_puts(uart1, ret < 0 ? "." : "@");
    }
    uint16_t systemCode = 0xFFFF;
    uint8_t requestCode = 0x01;
    typedef union {
        uint8_t block[18];
        struct {
            uint8_t IDm[8];
            uint8_t PMm[8];
            union {
                uint16_t SystemCode;
                uint8_t System_Code[2];
            };
        };
    } Card;
    Card card;

    void on_packet(io_packet_t *packet) {
        bool response = true;

        io_fill_data(out, packet->srcNodeId, packet->dstNodeId);
        out->response.status = ACK_OK;
        out->response.report = REPORT_OK;
        out->response.command = packet->request.command;

        switch(packet->request.command) {
            case cAiMeNFCRW::kNFCCMD_EXT_TO_NORMAL_MODE:
            {
                // 重置
                break;
            }
            default: {
                out->length = 0;
                out->response.status = ACK_INVALID;
                break;
            }
        }

        if (response) {
            io_apply_checksum(out);
            auto size = io_get_package_size(out, PACKAGE_TYPE_RESPONSE);

            stream->write_head();
            for(auto i = 1; i < size; i++) {
                stream->write(out->buffer[i]);
            }

            stream->flush();
        }
    }

    void update() {

        if (nfc.felica_Polling(systemCode, requestCode, card.IDm, card.PMm, &card.SystemCode, 80)) {
            gpio_put(LED_PIN,1);
            //stream->write(2);
        }else{
            gpio_put(LED_PIN,0);
            //stream->write(3);
        }
//        while(stream->available()) {
//            uint8_t byte;
//            bool is_escaped = stream->read(byte);
//
//            if(byte == 0xE0 && !is_escaped) {
//                in_size = 0;
//                checksum = 0;
//            }
//
//            in->buffer[in_size++] = byte;
//
//            if(in_size > 5 && in_size - 5 == in->length && checksum == byte) {
//                on_packet(in);
//            }
//
//            if(byte != 0xE0 || is_escaped) {
//                checksum += byte;
//            }
//        }
    }
}