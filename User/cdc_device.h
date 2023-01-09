/*
 * cdc_device.h
 *
 *  Created on: Jan 7, 2023
 *      Author: Alex
 */

#ifndef USER_CDC_DEVICE_H_
#define USER_CDC_DEVICE_H_

#include <stdint.h>
#include <stdbool.h>
#include "usbd_core.h"
#include "Arduino.h"
#include "usbd_cdc.h"

#define CDC_COUNT 2
#define CDC_START_IDX 0

struct cdc_ep
{
    uint8_t int_ep;
    uint8_t data_ep;
};

extern struct cdc_ep cdc_eps[CDC_COUNT];

/*!< endpoint address */
#define CDC_IN_EP_1  0x81
#define CDC_OUT_EP_1 0x01
#define CDC_INT_EP_1 0x82

/*!< endpoint address */
#define CDC_IN_EP_2  0x83
#define CDC_OUT_EP_2 0x03
#define CDC_INT_EP_2 0x84

#ifdef CONFIG_USB_HS
#define CDC_MAX_MPS 512
#else
#define CDC_MAX_MPS 64
#endif

#define CDC_TRANSMIT_QUEUE_BUFFER_SIZE ((uint16_t)(CDC_MAX_MPS * 2))
#define CDC_RECEIVE_QUEUE_BUFFER_SIZE ((uint16_t)(CDC_MAX_MPS * 3))

/*
 * The value USB_CDC_TRANSMIT_TIMEOUT is defined in terms of HAL_GetTick() units.
 * Typically it is 1ms value. The timeout determines when we would consider the
 * host "too slow" and threat the USB CDC port as disconnected.
 */
#ifndef USB_CDC_TRANSMIT_TIMEOUT
  #define USB_CDC_TRANSMIT_TIMEOUT 3
#endif


typedef struct {
  __attribute__((aligned(4))) uint8_t buffer[CDC_TRANSMIT_QUEUE_BUFFER_SIZE];
  volatile uint16_t write;
  volatile uint16_t read;
  volatile uint16_t reserved;
} CDC_TransmitQueue_TypeDef;

typedef struct {
  __attribute__((aligned(4))) uint8_t buffer[CDC_RECEIVE_QUEUE_BUFFER_SIZE];
  volatile uint16_t write;
  volatile uint16_t read;
  volatile uint16_t length;
} CDC_ReceiveQueue_TypeDef;

//extern CDC_TransmitQueue_TypeDef TransmitQueue;
//extern CDC_ReceiveQueue_TypeDef ReceiveQueue;


//CDC Queue defs
void CDC_TransmitQueue_Init(CDC_TransmitQueue_TypeDef *queue);
int CDC_TransmitQueue_WriteSize(CDC_TransmitQueue_TypeDef *queue);
int CDC_TransmitQueue_ReadSize(CDC_TransmitQueue_TypeDef *queue);
void CDC_TransmitQueue_Enqueue(CDC_TransmitQueue_TypeDef *queue, const uint8_t *buffer, uint32_t size);
uint8_t *CDC_TransmitQueue_ReadBlock(CDC_TransmitQueue_TypeDef *queue, uint16_t *size);
void CDC_TransmitQueue_CommitRead(CDC_TransmitQueue_TypeDef *queue);

void CDC_ReceiveQueue_Init(CDC_ReceiveQueue_TypeDef *queue);
int CDC_ReceiveQueue_ReadSize(CDC_ReceiveQueue_TypeDef *queue);
int CDC_ReceiveQueue_Dequeue(CDC_ReceiveQueue_TypeDef *queue);
int CDC_ReceiveQueue_Peek(CDC_ReceiveQueue_TypeDef *queue);
uint16_t CDC_ReceiveQueue_Read(CDC_ReceiveQueue_TypeDef *queue, uint8_t *buffer, uint16_t size);
bool CDC_ReceiveQueue_ReadUntil(CDC_ReceiveQueue_TypeDef *queue, uint8_t terminator, uint8_t *buffer,
                                uint16_t size, uint16_t *fetched);
uint8_t *CDC_ReceiveQueue_ReserveBlock(CDC_ReceiveQueue_TypeDef *queue);
void CDC_ReceiveQueue_CommitBlock(CDC_ReceiveQueue_TypeDef *queue, uint16_t size);

struct cdc_device_s
{
    uint8_t index;

    CDC_TransmitQueue_TypeDef TransmitQueue;
    CDC_ReceiveQueue_TypeDef ReceiveQueue;
    bool receivePended = true;
    bool transmitStarted = false;
    uint32_t transmitStart = 0;

    struct cdc_line_coding line_cod;
    uint8_t dtrState = 0;
    bool CDC_DTR_enabled = false;

    uint8_t write_buffer[CDC_MAX_MPS] USB_MEM_ALIGNX;
    uint8_t* recv_block;
    uint8_t read_buffer[CDC_MAX_MPS]  USB_MEM_ALIGNX;

    uint8_t ep_data_addr;
    struct usbd_interface intf0;
    struct usbd_interface intf1;
    struct usbd_endpoint cdc_out_ep;
    struct usbd_endpoint cdc_in_ep;
};

extern void cdc_usbd_configure_done_callback(void);
extern void add_cdc_device(void);
extern bool CDC_connected(struct cdc_device_s *obj);
extern void CDC_continue_transmit(struct cdc_device_s *obj);
extern bool CDC_resume_receive(struct cdc_device_s* obj);
extern void CDC_enableDTR(struct cdc_device_s *obj, bool enable);
extern void cdc_attach_obj(struct cdc_device_s *obj);

#endif /* USER_CDC_DEVICE_H_ */
