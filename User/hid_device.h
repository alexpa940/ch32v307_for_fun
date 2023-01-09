/*
 * hid_device.h
 *
 *  Created on: Jan 8, 2023
 *      Author: Alex
 */

#ifndef USER_HID_DEVICE_H_
#define USER_HID_DEVICE_H_

#include <stdint.h>

#define HID_KEYBOARD_REPORT_DESC_SIZE (63)

#define HID_INT_EP          0x83
#define HID_INT_EP_SIZE     8
#define HID_INT_EP_INTERVAL 10

#define HID_STATE_IDLE 0
#define HID_STATE_BUSY 1

extern void add_hid_device(void);

#endif /* USER_HID_DEVICE_H_ */
