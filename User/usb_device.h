/*
 * usb_device.h
 *
 *  Created on: Jan 7, 2023
 *      Author: Alex
 */

#ifndef USER_USB_DEVICE_H_
#define USER_USB_DEVICE_H_

#define USBD_VID           0x0FFF
#define USBD_PID           0xFFFF
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

extern void usb_device_init(void);

#endif /* USER_USB_DEVICE_H_ */
