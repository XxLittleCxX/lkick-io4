#include "stdinclude.h"


enum  {
    BLINK_NOT_MOUNTED = 250,
    BLINK_MOUNTED = 1000,
    BLINK_SUSPENDED = 2500,
};

static uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;

void led_blinking_task(void);
void hid_task(void);

static void cdc_task(void);

[[noreturn]] void update(void *pVoid);
struct output_t {
    int16_t analog[8];
    int16_t rotary[4];
    int16_t coin[2];
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


static output_t output_data;


[[noreturn]] void tud(void *pVoid){

    printf("tusb before init\n");
    tusb_init();
    printf("tusb init\n");
    while(true){
        tud_task();
        //printf("tusb task\n");
        if (tud_hid_ready()) {
            tud_hid_report(0x01, &output_data, sizeof(output_data));
        }
        vTaskDelay(2 / portTICK_PERIOD_MS);
    }
}

int main() {
    stdio_init_all();

    board_init();
    uart_init(UART_ID, BAUD_RATE);

    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    xTaskCreate(tud, "tud",USBD_STACK_SIZE, NULL,5,NULL);
    xTaskCreate(update, "io4",256, NULL,10,NULL);
    vTaskStartScheduler();
    while(1){

    };
}

// echo to either Serial0 or Serial1
// with Serial0 as all lower case, Serial1 as all upper case
static void echo_serial_port(uint8_t itf, uint8_t buf[], uint32_t count)
{
    for(uint32_t i=0; i<count; i++)
    {
        if (itf == 0)
        {
            // echo back 1st port as lower case
            if (isupper(buf[i])) buf[i] += 'a' - 'A';
        }
        else
        {
            // echo back 2nd port as upper case
            if (islower(buf[i])) buf[i] -= 'a' - 'A';
        }

        tud_cdc_n_write_char(itf, buf[i]);
    }
    tud_cdc_n_write_flush(itf);
}

//--------------------------------------------------------------------+
// USB CDC
//--------------------------------------------------------------------+
static void cdc_task(void)
{
    uint8_t itf;

    for (itf = 0; itf < CFG_TUD_CDC; itf++)
    {
        // connected() check for DTR bit
        // Most but not all terminal client set this when making connection
        // if ( tud_cdc_n_connected(itf) )
        {
            if ( tud_cdc_n_available(itf) )
            {
                uint8_t buf[64];

                uint32_t count = tud_cdc_n_read(itf, buf, sizeof(buf));

                // echo back to both serial ports
                echo_serial_port(0, buf, count);
                echo_serial_port(1, buf, count);
            }
        }
    }
}

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
    // TODO not Implemented
    (void) itf;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) reqlen;

    return 0;
}

[[noreturn]] void update(void *pVoid) {
    output_data.system_status = 0x02;
    while(true){
            output_data.switches[0] = rand();
            output_data.switches[1] = rand();
        vTaskDelay(2 / portTICK_PERIOD_MS);
    }
}

void process_data(const input_t *data) {
    switch (data->cmd) {
        case SET_COMM_TIMEOUT:
            printf("IO4: SET COMMUNICATE TIMEOUT\n");
            output_data.system_status = 0x30;
            break;
        case SET_SAMPLING_COUNT:
            printf("IO4: SET SAMPLING COUNT\n");
            output_data.system_status = 0x30;
            break;
        case CLEAR_BOARD_STATUS:
            printf("IO4: CLEAR BOARD STATUS\n");
            output_data.system_status = 0x00;
            break;
        case SET_GENERAL_OUTPUT:
            printf("IO4: SET GENERAL OUTPUT\n");

            break;
        default:
            break;
    }
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t buf_size)
{
    // This example doesn't use multiple report and report ID
    (void) itf;
    (void) report_id;
    (void) report_type;

    printf("recv %d bytes at itf %d\n", buf_size, itf);
    for(auto i = 0; i < buf_size && i < 32; i++) {
        if(i > 0 && i % 0x10 == 0) {
            printf("\n");
        }

        printf("%02hhX ", buffer[i]);
    }

    printf("\n\n");

    auto data = reinterpret_cast<const input_t *>(buffer);
    if(data->report_id == 0x10) {
        process_data(data);
    }

    // echo back anything we received from host
//    tud_hid_report(0, buffer, bufsize);
}


//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void)
{
    blink_interval_ms = BLINK_MOUNTED;
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
    blink_interval_ms = BLINK_NOT_MOUNTED;
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
    (void) remote_wakeup_en;
    blink_interval_ms = BLINK_SUSPENDED;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
    blink_interval_ms = BLINK_MOUNTED;
}