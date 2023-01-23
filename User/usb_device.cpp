/*
 * usb_device.cpp
 *
 *  Created on: Jan 7, 2023
 *      Author: Alex
 */
#include "usbd_core.h"
#include "usbd_cdc.h"
#include "cdc_device.h"
#include "usb_device.h"
#include "hid_device.h"
#include "usbd_hid.h"

/*!< config descriptor size */
#define USB_CONFIG_SIZE (9 + CDC_ACM_DESCRIPTOR_LEN * CDC_COUNT )//+ 25

/*!< global descriptor */
static const uint8_t usb_device_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0x00, 0x00, 0x00, USBD_VID, USBD_PID, 0x0200, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x08, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),

//    /************** Descriptor of Joystick Mouse interface ****************/
//    /* 09 */
//    0x09,                          /* bLength: Interface Descriptor size */
//    USB_DESCRIPTOR_TYPE_INTERFACE, /* bDescriptorType: Interface descriptor type */
//    0x00,                          /* bInterfaceNumber: Number of Interface */
//    0x00,                          /* bAlternateSetting: Alternate setting */
//    0x01,                          /* bNumEndpoints */
//    0x03,                          /* bInterfaceClass: HID */
//    0x01,                          /* bInterfaceSubClass : 1=BOOT, 0=no boot */
//    0x01,                          /* nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse */
//    0,                             /* iInterface: Index of string descriptor */
//    /******************** Descriptor of Joystick Mouse HID ********************/
//    /* 18 */
//    0x09,                    /* bLength: HID Descriptor size */
//    HID_DESCRIPTOR_TYPE_HID, /* bDescriptorType: HID */
//    0x11,                    /* bcdHID: HID Class Spec release number */
//    0x01,
//    0x00,                          /* bCountryCode: Hardware target country */
//    0x01,                          /* bNumDescriptors: Number of HID class descriptors to follow */
//    0x22,                          /* bDescriptorType */
//    HID_KEYBOARD_REPORT_DESC_SIZE, /* wItemLength: Total length of Report descriptor */
//    0x00,
//    /******************** Descriptor of Mouse endpoint ********************/
//    /* 27 */
//    0x07,                         /* bLength: Endpoint Descriptor size */
//    USB_DESCRIPTOR_TYPE_ENDPOINT, /* bDescriptorType: */
//    HID_INT_EP,                   /* bEndpointAddress: Endpoint Address (IN) */
//    0x03,                         /* bmAttributes: Interrupt endpoint */
//    HID_INT_EP_SIZE,              /* wMaxPacketSize: 4 Byte max */
//    0x00,
//    HID_INT_EP_INTERVAL, /* bInterval: Polling Interval */


//    CDC_ACM_DESCRIPTOR_INIT(CDC_START_IDX, ((CDC_START_IDX + 2)|0x80), (CDC_START_IDX + 1), ((CDC_START_IDX + 1)|0x80), 0x02),
//#if CDC_COUNT > 1
//    CDC_ACM_DESCRIPTOR_INIT((CDC_START_IDX + 2), ((CDC_START_IDX + 4)|0x80), (CDC_START_IDX + 3), ((CDC_START_IDX + 3)|0x80), 0x02),
//#endif
//#if CDC_COUNT > 2
//    CDC_ACM_DESCRIPTOR_INIT((CDC_START_IDX + 2 + 2), ((CDC_START_IDX + 4 + 2)|0x80), (CDC_START_IDX + 3 + 2), ((CDC_START_IDX + 3 + 2)|0x80), 0x02),
//#endif
//#if CDC_COUNT > 3
//    CDC_ACM_DESCRIPTOR_INIT((CDC_START_IDX + 2 + 4), ((CDC_START_IDX + 4 + 4)|0x80), (CDC_START_IDX + 3 + 4), ((CDC_START_IDX + 3 + 4)|0x80), 0x02),
//#endif
    CDC_ACM_DESCRIPTOR_INIT(CDC_START_IDX, ((CDC_START_IDX + 2)|0x80), (CDC_START_IDX + 1), ((CDC_START_IDX + 1)|0x80), 0x02),
#if CDC_COUNT > 1
    CDC_ACM_DESCRIPTOR_INIT((CDC_START_IDX + 2), ((CDC_START_IDX + 2)|0x80), (CDC_START_IDX + 3), ((CDC_START_IDX + 3)|0x80), 0x02),
#endif
#if CDC_COUNT > 2
    CDC_ACM_DESCRIPTOR_INIT((CDC_START_IDX + 2 + 2), ((CDC_START_IDX + 2)|0x80), (CDC_START_IDX + 3 + 2), ((CDC_START_IDX + 3 + 2)|0x80), 0x02),
#endif
#if CDC_COUNT > 3
    CDC_ACM_DESCRIPTOR_INIT((CDC_START_IDX + 2 + 4), ((CDC_START_IDX + 2)|0x80), (CDC_START_IDX + 3 + 4), ((CDC_START_IDX + 3 + 4)|0x80), 0x02),
#endif
    ///////////////////////////////////////
    /// string0 descriptor
    ///////////////////////////////////////
    USB_LANGID_INIT(USBD_LANGID_STRING),
    ///////////////////////////////////////
    /// string1 descriptor
    ///////////////////////////////////////
    0x14,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'C', 0x00,                  /* wcChar0 */
    'h', 0x00,                  /* wcChar1 */
    'e', 0x00,                  /* wcChar2 */
    'r', 0x00,                  /* wcChar3 */
    'r', 0x00,                  /* wcChar4 */
    'y', 0x00,                  /* wcChar5 */
    'U', 0x00,                  /* wcChar6 */
    'S', 0x00,                  /* wcChar7 */
    'B', 0x00,                  /* wcChar8 */
    ///////////////////////////////////////
    /// string2 descriptor
    ///////////////////////////////////////
    0x26,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'C', 0x00,                  /* wcChar0 */
    'h', 0x00,                  /* wcChar1 */
    'e', 0x00,                  /* wcChar2 */
    'r', 0x00,                  /* wcChar3 */
    'r', 0x00,                  /* wcChar4 */
    'y', 0x00,                  /* wcChar5 */
    'U', 0x00,                  /* wcChar6 */
    'S', 0x00,                  /* wcChar7 */
    'B', 0x00,                  /* wcChar8 */
    ' ', 0x00,                  /* wcChar9 */
    'C', 0x00,                  /* wcChar10 */
    'D', 0x00,                  /* wcChar11 */
    'C', 0x00,                  /* wcChar12 */
    ' ', 0x00,                  /* wcChar13 */
    'D', 0x00,                  /* wcChar14 */
    'E', 0x00,                  /* wcChar15 */
    'M', 0x00,                  /* wcChar16 */
    'O', 0x00,                  /* wcChar17 */
    ///////////////////////////////////////
    /// string3 descriptor
    ///////////////////////////////////////
    0x16,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    '2', 0x00,                  /* wcChar0 */
    '0', 0x00,                  /* wcChar1 */
    '2', 0x00,                  /* wcChar2 */
    '2', 0x00,                  /* wcChar3 */
    '1', 0x00,                  /* wcChar4 */
    '2', 0x00,                  /* wcChar5 */
    '3', 0x00,                  /* wcChar6 */
    '4', 0x00,                  /* wcChar7 */
    '5', 0x00,                  /* wcChar8 */
    '6', 0x00,                  /* wcChar9 */
#ifdef CONFIG_USB_HS
    ///////////////////////////////////////
    /// device qualifier descriptor
    ///////////////////////////////////////
    0x0a,
    USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER,
    0x00,
    0x02,
    0x02,
    0x02,
    0x01,
    0x40,
    0x01,
    0x00,
#endif
    0x00
};

extern "C" void usbd_configure_done_callback(void)
{
    cdc_usbd_configure_done_callback();
}

void usb_device_init(void)
{
    usbd_desc_register(usb_device_descriptor);
    //add_hid_device();
    add_cdc_device();

    usbd_initialize();
}



