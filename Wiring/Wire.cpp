/*
  TwoWire.cpp - TWI/I2C library for Wiring & Arduino
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

  Modified 2012 by Todd Krein (todd@krein.org) to implement repeated starts
*/

extern "C"
{
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
}

#include "Wire.h"

void (*TwoWire::user_onRequest)(void);
void (*TwoWire::user_onReceive)(int);

// Preinstantiate Objects //////////////////////////////////////////////////////

TwoWire Wire = TwoWire();
TwoWire Wire1 = TwoWire();

// Constructors ////////////////////////////////////////////////////////////////

TwoWire::TwoWire()
{
}
// Public Methods //////////////////////////////////////////////////////////////

void TwoWire::begin(uint8_t remap, I2C_TypeDef* ptr)
{
  begin((uint8_t)MASTER_ADDRESS, remap, ptr);
}

void TwoWire::begin(uint8_t address, uint8_t remap, I2C_TypeDef* ptr)
{

  _i2c.I2Cx = ptr;
  rxBufferIndex = 0;
  rxBufferLength = 0;
  txBufferIndex = 0;
  txBufferLength = 0;

  transmitting = 0;

  ownAddress = address << 1;

  if (address == MASTER_ADDRESS)
  {
    master = 1;
  }
  else
  {
    master = 0;
  }
  i2c_custom_init(&_i2c, I2C_DEFAULT_SPEED, remap, I2C_ADDMODE_7BIT, ownAddress, master);

  if (master == 0)
  {
    i2c_attachSlaveTxEvent(&_i2c, onRequestService);
    i2c_attachSlaveRxEvent(&_i2c, onReceiveService);
  }
}

void TwoWire::begin(int address, uint8_t remap, I2C_TypeDef* ptr)
{
  begin((uint8_t)address, remap, ptr);
}

void TwoWire::end(void)
{
  i2c_deinit(&_i2c);
}

void TwoWire::setClock(uint32_t frequency)
{
  i2c_setTiming(&_i2c, frequency);
}

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity, uint32_t iaddress, uint8_t isize, uint8_t sendStop)
{
  if (master == true)
  {
    if (isize > 0)
    {
      // send internal address; this mode allows sending a repeated start to access
      // some devices' internal registers. This function is executed by the hardware
      // TWI module on other processors (for example Due's TWI_IADR and TWI_MMR registers)

      beginTransmission(address);

      // the maximum size of internal address is 3 bytes
      if (isize > 3)
      {
        isize = 3;
      }

      // write internal register address - most significant byte first
      while (isize-- > 0)
        write((uint8_t)(iaddress >> (isize * 8)));

      endTransmission(false);
    }

    // clamp to buffer length
    if (quantity > BUFFER_LENGTH)
    {
      quantity = BUFFER_LENGTH;
    }
    // perform blocking read into buffer
    uint8_t read = 0;
    if (I2C_OK == i2c_master_read(&_i2c, address << 1, rxBuffer, quantity))
      read = quantity;

    // set rx buffer iterator vars
    rxBufferIndex = 0;
    rxBufferLength = read;

    return read;
  }

  return 0;
}

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity, uint8_t sendStop)
{
  return requestFrom((uint8_t)address, (uint8_t)quantity, (uint32_t)0, (uint8_t)0, (uint8_t)sendStop);
}

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity)
{
  return requestFrom((uint8_t)address, (uint8_t)quantity, (uint8_t) true);
}

uint8_t TwoWire::requestFrom(int address, int quantity)
{
  return requestFrom((uint8_t)address, (uint8_t)quantity, (uint8_t) true);
}

uint8_t TwoWire::requestFrom(int address, int quantity, int sendStop)
{
  return requestFrom((uint8_t)address, (uint8_t)quantity, (uint8_t)sendStop);
}

void TwoWire::beginTransmission(uint8_t address)
{
  // indicate that we are transmitting
  transmitting = 1;
  // set address of targeted slave
  txAddress = address << 1;
  // reset tx buffer iterator vars
  txBufferIndex = 0;
  txBufferLength = 0;
}

void TwoWire::beginTransmission(int address)
{
  beginTransmission((uint8_t)address);
}

//
//	Originally, 'endTransmission' was an f(void) function.
//	It has been modified to take one parameter indicating
//	whether or not a STOP should be performed on the bus.
//	Calling endTransmission(false) allows a sketch to
//	perform a repeated start.
//
//	WARNING: Nothing in the library keeps track of whether
//	the bus tenure has been properly ended with a STOP. It
//	is very possible to leave the bus in a hung state if
//	no call to endTransmission(true) is made. Some I2C
//	devices will behave oddly if they do not see a STOP.
//
uint8_t TwoWire::endTransmission(uint8_t sendStop)
{
  int8_t ret = 4;

  if (master == true)
  {
    // transmit buffer (blocking)
    switch (i2c_master_write(&_i2c, txAddress, txBuffer, txBufferLength))
    {
    case I2C_OK:
      ret = 0;
      break;
    case I2C_TIMEOUT:
      ret = 1;
      break;
    default:
      ret = 4;
      break;
    }

    // reset tx buffer iterator vars
    txBufferIndex = 0;
    txBufferLength = 0;
    // indicate that we are done transmitting
    transmitting = 0;
  }

  return ret;
}

//	This provides backwards compatibility with the original
//	definition, and expected behaviour, of endTransmission
//
uint8_t TwoWire::endTransmission(void)
{
  return endTransmission(true);
}

// must be called in:
// slave tx event callback
// or after beginTransmission(address)
size_t TwoWire::write(uint8_t data)
{
  if (transmitting)
  {
    // in master transmitter mode
    // don't bother if buffer is full
    if (txBufferLength >= BUFFER_LENGTH)
    {
      return 0;
    }
    // put byte in tx buffer
    txBuffer[txBufferIndex] = data;
    ++txBufferIndex;
    // update amount in buffer
    txBufferLength = txBufferIndex;
  }
  else
  {
    // in slave send mode
    // reply to master
    i2c_slave_write_IT(&_i2c, &data, 1);
  }
  return 1;
}

// must be called in:
// slave tx event callback
// or after beginTransmission(address)
size_t TwoWire::write(const uint8_t *data, size_t quantity)
{
  if (transmitting)
  {
    // in master transmitter mode
    for (size_t i = 0; i < quantity; ++i)
    {
      write(data[i]);
    }
  }
  else
  {
    // in slave send mode
    // reply to master
    i2c_slave_write_IT(&_i2c, (uint8_t *)data, quantity);
  }
  return quantity;
}

// must be called in:
// slave rx event callback
// or after requestFrom(address, numBytes)
int TwoWire::available(void)
{
  return rxBufferLength - rxBufferIndex;
}

// must be called in:
// slave rx event callback
// or after requestFrom(address, numBytes)
int TwoWire::read(void)
{
  int value = -1;

  // get each successive byte on each call
  if (rxBufferIndex < rxBufferLength)
  {
    value = rxBuffer[rxBufferIndex];
    ++rxBufferIndex;

    /* Commented as not I think it is not useful
     * but kept to show that it is possible to
     * reset rx buffer when no more data available */
    /*if(rxBufferIndex == rxBufferLength) {
      resetRxBuffer();
    }*/
  }

  return value;
}

// must be called in:
// slave rx event callback
// or after requestFrom(address, numBytes)
int TwoWire::peek(void)
{
  int value = -1;

  if (rxBufferIndex < rxBufferLength)
  {
    value = rxBuffer[rxBufferIndex];
  }

  return value;
}

void TwoWire::flush(void)
{
  rxBufferIndex = 0;
  rxBufferLength = 0;
  txBufferIndex = 0;
  txBufferLength = 0;
}

// behind the scenes function that is called when data is received
void TwoWire::onReceiveService(uint8_t *inBytes, int numBytes)
{
  TwoWire *ptr;
  ptr = &Wire;

  // don't bother if user hasn't registered a callback
  if (!ptr->user_onReceive)
  {
    return;
  }

  // don't bother if rx buffer is in use by a master requestFrom() op
  // i know this drops data, but it allows for slight stupidity
  // meaning, they may not have read all the master requestFrom() data yet
  if (ptr->rxBufferIndex < ptr->rxBufferLength)
  {
    return;
  }
  // copy twi rx buffer into local read buffer
  // this enables new reads to happen in parallel
  for (uint8_t i = 0; i < numBytes; ++i)
  {
    ptr->rxBuffer[i] = inBytes[i];
  }
  // set rx iterator vars
  ptr->rxBufferIndex = 0;
  ptr->rxBufferLength = numBytes;
  // alert user program
  ptr->user_onReceive(numBytes);
}

void TwoWire::onRequestService(void)
{
  TwoWire *ptr;
  ptr = &Wire;

  // don't bother if user hasn't registered a callback
  if (!ptr->user_onRequest)
  {
    return;
  }

  // reset tx buffer iterator vars
  // !!! this will kill any pending pre-master sendTo() activity
  ptr->txBufferIndex = 0;
  ptr->txBufferLength = 0;
  // alert user program
  ptr->user_onRequest();
}

// sets function called on slave write
void TwoWire::onReceive(void (*function)(int))
{
  user_onReceive = function;
}

// sets function called on slave read
void TwoWire::onRequest(void (*function)(void))
{
  user_onRequest = function;
}


