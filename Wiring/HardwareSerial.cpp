/*
  HardwareSerial.cpp - Hardware serial library for Wiring
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  Modified 23 November 2006 by David A. Mellis
  Modified 28 September 2010 by Mark Sproul
  Modified 14 August 2012 by Alarus
  Modified 3 December 2013 by Matthijs Kooijman
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>
#include "Arduino.h"
#include "HardwareSerial.h"


HardwareSerial Serial1(USART1);
HardwareSerial Serial2(USART2);
HardwareSerial Serial3(USART3);
HardwareSerial Serial4(UART4);
HardwareSerial Serial5(UART5);
HardwareSerial Serial6(UART6);
HardwareSerial Serial7(UART7);
HardwareSerial Serial8(UART8);

// Constructors ////////////////////////////////////////////////////////////////

HardwareSerial::HardwareSerial(USART_TypeDef *peripheral)
{
  _serial.uart = peripheral;
  init();
}

void HardwareSerial::init(void)
{
  _serial.rx_buff = _rx_buffer;
  _serial.rx_head = 0;
  _serial.rx_tail = 0;

  _serial.tx_buff = _tx_buffer;
  _serial.tx_head = 0;
  _serial.tx_tail = 0;
}

// Actual interrupt handlers //////////////////////////////////////////////////////////////

void HardwareSerial::_rx_complete_irq(serial_t* obj)
{
  // No Parity error, read byte and store it in the buffer if there is room
  unsigned char c;

  if (uart_getc(obj, &c) == 0) {

    rx_buffer_index_t i = (unsigned int)(obj->rx_head + 1) % SERIAL_RX_BUFFER_SIZE;

    // if we should be storing the received character into the location
    // just before the tail (meaning that the head would advance to the
    // current location of the tail), we're about to overflow the buffer
    // and so we don't write the character or advance the head.
    if (i != obj->rx_tail) {
      obj->rx_buff[obj->rx_head] = c;
      obj->rx_head = i;
    }
  }
}

void HardwareSerial::_tx_complete_irq(serial_t* obj)
{
    if(obj->tx_head == obj->tx_tail)
    {
      // Buffer empty, so disable interrupts
        USART_ITConfig(obj->uart, USART_IT_TXE, DISABLE);

    }
     else
    {
      // There is more data in the output buffer. Send the next byte
      unsigned char c = obj->tx_buff[obj->tx_tail];
      obj->tx_tail = (obj->tx_tail + 1) % SERIAL_TX_BUFFER_SIZE;

      obj->uart->STATR;
      obj->uart->DATAR = c;

    }
}

// Public Methods //////////////////////////////////////////////////////////////

void HardwareSerial::begin(uint32_t remap, unsigned long baud, byte config)
{
  uint32_t databits = 0;
  _serial.remap_type = remap;
  _serial.baudrate = (uint32_t)baud;

  // Manage databits
  switch(config & 0x07) {
    case 0x02:
      databits = 6;
      break;
    case 0x04:
      databits = 7;
      break;
    case 0x06:
      databits = 8;
      break;
    default:
      databits = 0;
      break;
	}

  if((config & 0x30) == 0x30) {
    _serial.parity = UART_PARITY_ODD;
    databits++;
  } else if((config & 0x20) == 0x20) {
    _serial.parity = UART_PARITY_EVEN;
    databits++;
  } else {
    _serial.parity = UART_PARITY_NONE;
  }

  if((config & 0x08) == 0x08) {
    _serial.stopbits = UART_STOPBITS_2;
  } else {
    _serial.stopbits = UART_STOPBITS_1;
  }

  switch(databits) {
    case 8:
      _serial.databits = UART_WORDLENGTH_8B;
      break;
    case 9:
      _serial.databits = UART_WORDLENGTH_9B;
      break;
    default:
    case 0:
      databits = 0;
      break;
  }

  uart_init(&_serial);
  uart_attach_rx_callback(&_serial, _rx_complete_irq);
  uart_attach_tx_callback(&_serial, _tx_complete_irq);
}

void HardwareSerial::end()
{
  // wait for transmission of outgoing data
  flush();

  uart_deinit(&_serial);

  // clear any received data
  _serial.rx_head = _serial.rx_tail;
}

int HardwareSerial::available(void)
{
  return ((unsigned int)(SERIAL_RX_BUFFER_SIZE + _serial.rx_head - _serial.rx_tail)) % SERIAL_RX_BUFFER_SIZE;
}

int HardwareSerial::peek(void)
{
  if (_serial.rx_head == _serial.rx_tail) {
    return -1;
  } else {
    return _serial.rx_buff[_serial.rx_tail];
  }
}

int HardwareSerial::read(void)
{
  // if the head isn't ahead of the tail, we don't have any characters
  if (_serial.rx_head == _serial.rx_tail) {
    return -1;
  } else {
    unsigned char c = _serial.rx_buff[_serial.rx_tail];
    _serial.rx_tail = (rx_buffer_index_t)(_serial.rx_tail + 1) % SERIAL_RX_BUFFER_SIZE;
    return c;
  }
}

void HardwareSerial::flush()
{
    while(((uint16_t)(SERIAL_TX_BUFFER_SIZE + _serial.tx_head - _serial.tx_tail) % SERIAL_TX_BUFFER_SIZE) != SERIAL_TX_BUFFER_SIZE){};
}

int HardwareSerial::availableForWrite(void)
{
  return (SERIAL_TX_BUFFER_SIZE + _serial.tx_head - _serial.tx_tail) % SERIAL_TX_BUFFER_SIZE;
}

size_t HardwareSerial::write(uint8_t c)
{
    uint32_t i = (_serial.tx_head + 1) % SERIAL_TX_BUFFER_SIZE;

    if (i == _serial.tx_tail)
    {
        return 0;
    }

    _serial.tx_buff[_serial.tx_head] = (uint8_t)c;
    _serial.tx_head = i;

    uart_write(&_serial, c);
    return 1;
}

