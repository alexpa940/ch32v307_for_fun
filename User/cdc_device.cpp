/*
 * cdc_device.cpp
 *
 *  Created on: Jan 7, 2023
 *      Author: Alex
 */


#include "cdc_device.h"

static struct cdc_device_s *cdc_obj[CDC_COUNT];
struct cdc_ep cdc_eps[CDC_COUNT];

static struct cdc_device_s * cdc_ep_to_obj(uint8_t ep)
{
    for(uint8_t i = 0; i < CDC_COUNT; i++)
    {
        if(cdc_obj[i] == NULL) {
          continue;
        }
        if(cdc_obj[i]->ep_data_addr == (ep & 0xF))
        {
            return cdc_obj[i];
        }
    }
    return NULL;
}

static struct cdc_device_s * cdc_intf_to_obj(uint8_t inf)
{
    for(uint8_t i = 0; i < CDC_COUNT; i++)
    {
        if(cdc_obj[i] == NULL) {
          continue;
        }
        if(inf == 3)
        {
            (void)0;
        }
        if(cdc_obj[i]->intf0.intf_num == inf)
        {
            return cdc_obj[i];
        }
    }
    return NULL;
}

bool CDC_resume_receive(struct cdc_device_s* obj)
{
  /*
   * TS: main and IRQ threads can't pass it at same time, because
   * IRQ may occur only if receivePended is true. So it is thread-safe!
   */
  if (!obj->receivePended)
  {
    uint8_t *block = CDC_ReceiveQueue_ReserveBlock(&obj->ReceiveQueue);
    if (block != NULL)
    {
      obj->recv_block = block;
      obj->receivePended = true;
      /* Set new buffer */
      memset(obj->read_buffer, 0, CDC_MAX_MPS);
      usbd_ep_start_read(obj->ep_data_addr, obj->read_buffer, CDC_MAX_MPS);
      return true;
    }
  }
  return false;
}

void cdc_usbd_configure_done_callback(void)
{
    for(uint8_t i = 0; i < CDC_COUNT; i++)
    {
        if(cdc_obj[i] == NULL) {
          continue;
        }
        CDC_TransmitQueue_Init(&cdc_obj[i]->TransmitQueue);
        CDC_ReceiveQueue_Init(&cdc_obj[i]->ReceiveQueue);
        cdc_obj[i]->receivePended = false;
        cdc_obj[i]->transmitStart = 0;
        /* setup first out ep read transfer */
        CDC_resume_receive(cdc_obj[i]);
    }
}

void cdc_attach_obj(struct cdc_device_s *obj)
{
  if(obj == NULL) {
    return;
  }
  cdc_obj[obj->index] = obj;
}

void usbd_cdc_acm_bulk_out(uint8_t ep, uint32_t nbytes)
{
    struct cdc_device_s *obj = cdc_ep_to_obj(ep);
    if(obj != NULL)
    {
        memcpy(obj->recv_block, obj->read_buffer, nbytes);
        CDC_ReceiveQueue_CommitBlock(&obj->ReceiveQueue, (uint16_t)nbytes);
        obj->receivePended = false;
        if (!CDC_resume_receive(obj)) {
            usbd_ep_start_read(obj->ep_data_addr, 0, 0);
        }
    }
}

void CDC_continue_transmit(struct cdc_device_s *obj)
{
  uint16_t size;
  uint8_t *buffer;
  /*
   * TS: This method can be called both in the main thread
   * (via USBSerial::write) and in the IRQ stream (via USBD_CDC_TransmistCplt),
   * BUT the main thread cannot pass this condition while waiting for a IRQ!
   * This is not possible because TxState is not zero while waiting for data
   * transfer ending! The IRQ thread is uninterrupted, since its priority
   * is higher than that of the main thread. So this method is thread safe.
   */

  if (obj->transmitStarted == false)
  {
    buffer = CDC_TransmitQueue_ReadBlock(&obj->TransmitQueue, &size);
    if (size > 0)
    {
      obj->transmitStart = millis();

      //TODO: ch32v307 must align 4
      memset(obj->write_buffer, 0, sizeof(obj->write_buffer));
      memcpy(obj->write_buffer, buffer, size);

      usbd_ep_start_write(obj->ep_data_addr | 0x80, obj->write_buffer, size);
      obj->transmitStarted = true;
    }
  }
}

void usbd_cdc_acm_bulk_in(uint8_t ep, uint32_t nbytes)
{
    struct cdc_device_s *obj = cdc_ep_to_obj(ep);
    if(obj != NULL)
    {
        obj->transmitStarted = false;
        obj->transmitStart = 0;
        CDC_TransmitQueue_CommitRead(&obj->TransmitQueue);
        CDC_continue_transmit(obj);
    }
}

extern "C" void usbd_cdc_acm_set_dtr(uint8_t intf, bool dtr)
{
    struct cdc_device_s *obj = cdc_intf_to_obj(intf);
    if(obj != NULL)
    {
        obj->dtrState = (obj->CDC_DTR_enabled) ? static_cast<bool>(dtr) : true;
        if (obj->dtrState)
        {
            obj->transmitStart = 0;
        }
    }
}

void CDC_enableDTR(struct cdc_device_s *obj, bool enable)
{
  obj->CDC_DTR_enabled = enable;
}

bool CDC_connected(struct cdc_device_s *obj)
{
  /* Save the transmitStart value in a local variable to avoid twice reading - fix #478 */
  uint32_t transmitTime = obj->transmitStart;
  if (transmitTime) {
    transmitTime = millis() - transmitTime;
  }
  return (usb_device_is_configured()
          && (transmitTime < USB_CDC_TRANSMIT_TIMEOUT));
}

void add_cdc_device(void)
{
    for(uint8_t i = 0; i < CDC_COUNT; i++)
    {
        if(cdc_obj[i] == NULL) {
          continue;
        }

        cdc_obj[i]->cdc_out_ep.ep_addr = cdc_obj[i]->ep_data_addr;
        cdc_obj[i]->cdc_out_ep.ep_cb = usbd_cdc_acm_bulk_out;

        cdc_obj[i]->cdc_in_ep.ep_addr = cdc_obj[i]->ep_data_addr | 0x80;
        cdc_obj[i]->cdc_in_ep.ep_cb = usbd_cdc_acm_bulk_in;

        usbd_add_interface(usbd_cdc_acm_init_intf(&cdc_obj[i]->intf0));
        usbd_add_interface(usbd_cdc_acm_init_intf(&cdc_obj[i]->intf1));
        usbd_add_endpoint(&cdc_obj[i]->cdc_out_ep);
        usbd_add_endpoint(&cdc_obj[i]->cdc_in_ep);
    }
}

extern "C" void usbd_cdc_acm_set_line_coding(uint8_t intf, struct cdc_line_coding *line_coding)
{
    struct cdc_device_s *obj = cdc_intf_to_obj(intf);
    if(obj != NULL)
    {
       memcpy(&obj->line_cod, line_coding, sizeof(cdc_line_coding));
    }

}

extern "C" void usbd_cdc_acm_get_line_coding(uint8_t intf, struct cdc_line_coding *line_coding)
{
    struct cdc_device_s *obj = cdc_intf_to_obj(intf);
    if(obj != NULL)
    {
        memcpy(line_coding, &obj->line_cod, sizeof(cdc_line_coding));
    }
}

// Initialize read and write position of queue
void CDC_TransmitQueue_Init(CDC_TransmitQueue_TypeDef *queue)
{
  queue->read = 0;
  queue->write = 0;
}

// Determine size, available for write in queue
int CDC_TransmitQueue_WriteSize(CDC_TransmitQueue_TypeDef *queue)
{
  return (queue->read + CDC_TRANSMIT_QUEUE_BUFFER_SIZE - queue->write - 1)
         % CDC_TRANSMIT_QUEUE_BUFFER_SIZE;
}

// Determine size of data, stored in queue
int CDC_TransmitQueue_ReadSize(CDC_TransmitQueue_TypeDef *queue)
{
  return (queue->write + CDC_TRANSMIT_QUEUE_BUFFER_SIZE - queue->read)
         % CDC_TRANSMIT_QUEUE_BUFFER_SIZE;
}

// Write provided data into queue.
void CDC_TransmitQueue_Enqueue(CDC_TransmitQueue_TypeDef *queue,
                               const uint8_t *buffer, uint32_t size)
{
  uint32_t sizeToEnd = CDC_TRANSMIT_QUEUE_BUFFER_SIZE - queue->write;
  if (sizeToEnd > size) {
    memcpy(&queue->buffer[queue->write], &buffer[0], size);
  } else {
    memcpy(&queue->buffer[queue->write], &buffer[0], sizeToEnd);
    memcpy(&queue->buffer[0], &buffer[sizeToEnd], size - sizeToEnd);
  }
  queue->write = (uint16_t)((queue->write + size) %
                            CDC_TRANSMIT_QUEUE_BUFFER_SIZE);
}

// Read flat block from queue biggest as possible, but max QUEUE_MAX_PACKET_SIZE
uint8_t *CDC_TransmitQueue_ReadBlock(CDC_TransmitQueue_TypeDef *queue,
                                     uint16_t *size)
{
  if (queue->write >= queue->read) {
    *size = queue->write - queue->read;
  } else {
    *size = CDC_TRANSMIT_QUEUE_BUFFER_SIZE - queue->read;
  }
  queue->reserved = *size;
  return &queue->buffer[queue->read];
}

void CDC_TransmitQueue_CommitRead(CDC_TransmitQueue_TypeDef *queue)
{
  queue->read = (queue->read + queue->reserved) %
                CDC_TRANSMIT_QUEUE_BUFFER_SIZE;
}

// Initialize read and write position of queue.
void CDC_ReceiveQueue_Init(CDC_ReceiveQueue_TypeDef *queue)
{
  queue->read = 0;
  queue->write = 0;
  queue->length = CDC_RECEIVE_QUEUE_BUFFER_SIZE;
}

// Reserve block in queue and return pointer to it.
uint8_t *CDC_ReceiveQueue_ReserveBlock(CDC_ReceiveQueue_TypeDef *queue)
{
  const uint16_t limit =
    CDC_RECEIVE_QUEUE_BUFFER_SIZE - CDC_MAX_MPS;
  volatile uint16_t read = queue->read;

  if (read <= queue->write) {
    // if write is limited only by buffer size.
    if (queue->write < limit || (queue->write == limit && read > 0)) {
      // if size in the rest of buffer is enough for full packet plus 1 byte
      // or if it tight enough and write position can be set to 0
      return queue->buffer + queue->write;
    } else if (read > CDC_MAX_MPS) {
      // if size in the rest is not enough, but enough size in head
      queue->length = queue->write;
      queue->write = 0;
      return queue->buffer + queue->write;
    }
  } else if (queue->write + CDC_MAX_MPS < read) {
    // write position must be less than read position
    // after reading largest possible packet
    return queue->buffer + queue->write;
  }
  return 0;
}

// Commits block in queue and make it available for reading
void CDC_ReceiveQueue_CommitBlock(CDC_ReceiveQueue_TypeDef *queue,
                                  uint16_t size)
{
  queue->write += size;
  if (queue->write >= queue->length) {
    queue->length = CDC_RECEIVE_QUEUE_BUFFER_SIZE;
  }
  if (queue->write >= CDC_RECEIVE_QUEUE_BUFFER_SIZE) {
    queue->write = 0;
  }
}

// Determine size, available for read
int CDC_ReceiveQueue_ReadSize(CDC_ReceiveQueue_TypeDef *queue)
{
  // reading length after write make guarantee, that length >= write
  // and determined reading size will be smaller or equal than real one.
  volatile uint16_t write = queue->write;
  volatile uint16_t length = queue->length;
  if (write >= queue->read) {
    return write - queue->read;
  }
  return length + write - queue->read;
}

// Read one byte from queue.
int CDC_ReceiveQueue_Dequeue(CDC_ReceiveQueue_TypeDef *queue)
{
  volatile uint16_t write = queue->write;
  volatile uint16_t length = queue->length;
  if (queue->read == length) {
    queue->read = 0;
  }
  if (write == queue->read) {
    return -1;
  }
  uint8_t ch = queue->buffer[queue->read++];
  if (queue->read >= length) {
    queue->read = 0;
  }
  return ch;
}

// Peek byte from queue.
int CDC_ReceiveQueue_Peek(CDC_ReceiveQueue_TypeDef *queue)
{
  volatile uint16_t write = queue->write;
  volatile uint16_t length = queue->length;
  if (queue->read >= length) {
    queue->read = 0;
  }
  if (write == queue->read) {
    return -1;
  }
  return queue->buffer[queue->read];
}

uint16_t CDC_ReceiveQueue_Read(CDC_ReceiveQueue_TypeDef *queue,
                               uint8_t *buffer, uint16_t size)
{
  volatile uint16_t write = queue->write;
  volatile uint16_t length = queue->length;
  uint16_t available;

  if (queue->read >= length) {
    queue->read = 0;
  }
  if (write >= queue->read) {
    available = write - queue->read;
  } else {
    available = length - queue->read;
  }
  if (available < size) {
    size = available;
  }

  memcpy(buffer, &queue->buffer[queue->read], size);
  queue->read = queue->read + size;
  if (queue->read >= length) {
    queue->read = 0;
  }
  return size;
}

bool CDC_ReceiveQueue_ReadUntil(CDC_ReceiveQueue_TypeDef *queue,
                                uint8_t terminator, uint8_t *buffer, uint16_t size, uint16_t *fetched)
{
  volatile uint16_t write = queue->write;
  volatile uint16_t length = queue->length;
  uint16_t available;

  if (queue->read >= length) {
    queue->read = 0;
  }
  if (write >= queue->read) {
    available = write - queue->read;
  } else {
    available = length - queue->read;
  }
  if (available < size) {
    size = available;
  }

  uint8_t *start = &queue->buffer[queue->read];
  for (uint16_t i = 0; i < size; i++) {
    uint8_t ch = start[i];
    if (ch == terminator) {
      queue->read += (uint16_t)(i + 1);
      if (queue->read >= length) {
        queue->read = 0;
      }
      *fetched = i;
      return true;
    } else {
      buffer[i] = ch;
    }
  }

  *fetched = size;
  queue->read += size;
  if (queue->read >= length) {
    queue->read = 0;
  }
  return false;
}

