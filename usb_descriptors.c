/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "tusb.h"

/* A combination of interfaces must have a unique product id, since PC will save device driver after the first plug.
 * Same VID/PID with different interface e.g MSC (first), then CDC (later) will possibly cause system error on PC.
 *
 * Auto ProductID layout's Bitmap:
 *   [MSB]       MIDI | HID | MSC | CDC          [LSB]
 */
//#define _PID_MAP(itf, n)  ( (CFG_TUD_##itf) << (n) )
//#define USB_PID           (0x4000 | _PID_MAP(CDC, 0) | _PID_MAP(MSC, 1) | _PID_MAP(HID, 2) | \
//                           _PID_MAP(MIDI, 3) | _PID_MAP(VENDOR, 4) )
//
//#define USB_VID   0xCafe
#define USB_BCD 0x0200
#define USB_VID 0x0ca3
#define USB_PID 0x0021

//--------------------------------------------------------------------+
// Device Descriptors
//--------------------------------------------------------------------+
tusb_desc_device_t const desc_device =
        {
                .bLength            = sizeof(tusb_desc_device_t),
                .bDescriptorType    = TUSB_DESC_DEVICE,
                .bcdUSB             = USB_BCD,

                // Use Interface Association Descriptor (IAD) for CDC
                // As required by USB Specs IAD's subclass must be common class (2) and protocol must be IAD (1)
                .bDeviceClass       = TUSB_CLASS_MISC,
                .bDeviceSubClass    = MISC_SUBCLASS_COMMON,
                .bDeviceProtocol    = MISC_PROTOCOL_IAD,
                .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,

                .idVendor           = USB_VID,
                .idProduct          = USB_PID,
                .bcdDevice          = 0x0100,

                .iManufacturer      = 0x01,
                .iProduct           = 0x02,
                .iSerialNumber      = 0x03,

                .bNumConfigurations = 0x01
        };

// Invoked when received GET DEVICE DESCRIPTOR
// Application return pointer to descriptor
uint8_t const * tud_descriptor_device_cb(void)
{
    return (uint8_t const *) &desc_device;
}


//--------------------------------------------------------------------+
// HID Report Descriptor
//--------------------------------------------------------------------+

uint8_t const desc_hid_report[] = {
        0x05, 0x01,                     // Usage Page (Generic Desktop Ctrls)
        0x09, 0x04,                     // Usage (Joystick)
        0xA1, 0x01,                     // Collection (Application)
        0x85, 0x01,                     //   Report ID (1)
        0x09, 0x01,                     //   Usage (Pointer)
        0xA1, 0x00,                     //   Collection (Physical)
        0x09, 0x30,                     //     Usage (X)
        0x09, 0x31,                     //     Usage (Y)
        0x09, 0x30,                     //     Usage (X)
        0x09, 0x31,                     //     Usage (Y)
        0x09, 0x30,                     //     Usage (X)
        0x09, 0x31,                     //     Usage (Y)
        0x09, 0x30,                     //     Usage (X)
        0x09, 0x31,                     //     Usage (Y)
        0x09, 0x33,                     //     Usage (Rx)
        0x09, 0x34,                     //     Usage (Ry)
        0x09, 0x33,                     //     Usage (Rx)
        0x09, 0x34,                     //     Usage (Ry)
        0x09, 0x36,                     //     Usage (Slider)
        0x09, 0x36,                     //     Usage (Slider)
        0x15, 0x00,                     //     Logical Minimum (0)
        0x27, 0xFF, 0xFF, 0x00, 0x00,   //     Logical Maximum (65534)
        0x35, 0x00,                     //     Physical Minimum (0)
        0x47, 0xFF, 0xFF, 0x00, 0x00,   //     Physical Maximum (65534)
        0x95, 0x0E,                     //     Report Count (14)
        0x75, 0x10,                     //     Report Size (16)
        0x81, 0x02,                     //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
        0xC0,                           //   End Collection
        0x05, 0x02,                     //   Usage Page (Sim Ctrls)
        0x05, 0x09,                     //   Usage Page (Button)
        0x19, 0x01,                     //   Usage Minimum (0x01)
        0x29, 0x30,                     //   Usage Maximum (0x30)
        0x15, 0x00,                     //   Logical Minimum (0)
        0x25, 0x01,                     //   Logical Maximum (1)
        0x45, 0x01,                     //   Physical Maximum (1)
        0x75, 0x01,                     //   Report Size (1)
        0x95, 0x30,                     //   Report Count (48)
        0x81, 0x02,                     //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
        0x09, 0x00,                     //   Usage (0x00)
        0x75, 0x08,                     //   Report Size (8)
        0x95, 0x1D,                     //   Report Count (29)
        0x81, 0x01,                     //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
        0x06, 0xA0, 0xFF,               //   Usage Page (Vendor Defined 0xFFA0)
        0x09, 0x00,                     //   Usage (0x00)
        0x85, 0x10,                     //   Report ID (16)
        0xA1, 0x01,                     //   Collection (Application)
        0x09, 0x00,                     //     Usage (0x00)
        0x15, 0x00,                     //     Logical Minimum (0)
        0x26, 0xFF, 0x00,               //     Logical Maximum (255)
        0x75, 0x08,                     //     Report Size (8)
        0x95, 0x3F,                     //     Report Count (63)
        0x91, 0x02,                     //     Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
        0xC0,                           //   End Collection
        0xC0,                           // End Collection
        TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(2)),
};

// Invoked when received GET HID REPORT DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const * tud_hid_descriptor_report_cb(uint8_t itf)
{
    (void) itf;
    return desc_hid_report;
}

//--------------------------------------------------------------------+
// Configuration Descriptor
//--------------------------------------------------------------------+
enum
{
    ITF_NUM_HID,
    ITF_NUM_CDC_0,
    ITF_NUM_CDC_0_DATA,
    ITF_NUM_CDC_1,
    ITF_NUM_CDC_1_DATA,
    ITF_NUM_TOTAL
};

#define CONFIG_TOTAL_LEN    (TUD_CONFIG_DESC_LEN + TUD_HID_INOUT_DESC_LEN +  CFG_TUD_CDC * TUD_CDC_DESC_LEN )

#define EPNUM_HID           0x81
#define EPNUM_CDC_0_NOTIF   0x82
#define EPNUM_CDC_0_OUT     0x02
#define EPNUM_CDC_0_IN      0x83

#define EPNUM_CDC_1_NOTIF   0x84
#define EPNUM_CDC_1_OUT     0x04
#define EPNUM_CDC_1_IN      0x85



uint8_t const desc_fs_configuration[] =
        {
                // Config number, interface count, string index, total length, attribute, power in mA
                TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, 0x00, 100),

                TUD_HID_INOUT_DESCRIPTOR(ITF_NUM_HID, 6, HID_ITF_PROTOCOL_NONE, sizeof(desc_hid_report), 0x01, 0x81, CFG_TUD_HID_EP_BUFSIZE, 6),
                // 1st CDC: Interface number, string index, EP notification address and size, EP data address (out, in) and size.
                TUD_CDC_DESCRIPTOR(ITF_NUM_CDC_0, 4, 0x82, 8, 0x03, 0x83, 64),
                TUD_CDC_DESCRIPTOR(ITF_NUM_CDC_1, 5, 0x84, 8, 0x05, 0x85, 64),
        };



uint8_t desc_other_speed_config[CONFIG_TOTAL_LEN];

// device qualifier is mostly similar to device descriptor since we don't change configuration based on speed
tusb_desc_device_qualifier_t const desc_device_qualifier =
        {
                .bLength            = sizeof(tusb_desc_device_qualifier_t),
                .bDescriptorType    = TUSB_DESC_DEVICE_QUALIFIER,
                .bcdUSB             = USB_BCD,

                .bDeviceClass       = TUSB_CLASS_MISC,
                .bDeviceSubClass    = MISC_SUBCLASS_COMMON,
                .bDeviceProtocol    = MISC_PROTOCOL_IAD,

                .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,
                .bNumConfigurations = 0x01,
                .bReserved          = 0x00
        };

uint8_t const* tud_descriptor_device_qualifier_cb(void)
{
    return (uint8_t const*) &desc_device_qualifier;
}

// Invoked when received GET CONFIGURATION DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const * tud_descriptor_configuration_cb(uint8_t index)
{
    (void) index; // for multiple configurations
    return desc_fs_configuration;
}

// Invoked when received GET OTHER SEED CONFIGURATION DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
// Configuration descriptor in the other speed e.g if high speed then this is for full speed and vice versa
uint8_t const* tud_descriptor_other_speed_configuration_cb(uint8_t index)
{
    (void) index; // for multiple configurations

    // other speed config is basically configuration with type = OHER_SPEED_CONFIG
    memcpy(desc_other_speed_config, desc_fs_configuration, CONFIG_TOTAL_LEN);
    desc_other_speed_config[1] = TUSB_DESC_OTHER_SPEED_CONFIG;

    // this example use the same configuration for both high and full speed mode
    return desc_other_speed_config;
}

//--------------------------------------------------------------------+
// String Descriptors
//--------------------------------------------------------------------+

// array of pointer to string descriptors
char const* string_desc_arr [] =
        {
                (const char[]) { 0x09, 0x04 },
                // 1: Manufacturer
                "SEGA",
                // 2: Product
                "LKICK H/W 5.3-G-DEV",
                // 3: Serials
                "",
                // 4: CDC1, Aime Reader
                "AIME READER @COM1",
                // 5: CDC2, Led Board
                "LED BOARD @COM3",
                // 6: IO4 Board Name
                "I/O CONTROL BD;15257;01;90;1831;6679A;00;GOUT=14_ADIN=8,E_ROTIN=4_COININ=2_SWIN=2,E_UQ1=41,6;",
        };

static uint16_t _desc_str[128];

// Invoked when received GET STRING DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
    (void) langid;

    uint8_t chr_count;

    if (index == 0)
    {
        memcpy(&_desc_str[1], string_desc_arr[0], 2);
        chr_count = 1;
    }
    else
    {
        // Note: the 0xEE index string is a Microsoft OS 1.0 Descriptors.
        // https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/microsoft-defined-usb-descriptors

        if ( !(index < sizeof(string_desc_arr)/sizeof(string_desc_arr[0])) ) return NULL;

        const char* str = string_desc_arr[index];

        // Cap at max char
        chr_count = strlen(str);

        // Convert ASCII string into UTF-16
        for(uint8_t i=0; i<chr_count; i++)
        {
            _desc_str[1+i] = str[i];
        }
    }

    // first byte is length (including header), second byte is string type
    _desc_str[0] = (TUSB_DESC_STRING << 8) | (2 * chr_count + 2);

    return _desc_str;
}
