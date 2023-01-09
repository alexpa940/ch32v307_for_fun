/**
******************************************************************************
* @file    twi.h
* @author  WI6LABS
* @version V1.0.0
* @date
* @brief   Header for twi module
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*   1. Redistributions of source code must retain the above copyright notice,
*      this list of conditions and the following disclaimer.
*   2. Redistributions in binary form must reproduce the above copyright notice,
*      this list of conditions and the following disclaimer in the documentation
*      and/or other materials provided with the distribution.
*   3. Neither the name of STMicroelectronics nor the names of its contributors
*      may be used to endorse or promote products derived from this software
*      without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************
*/

#include <stddef.h>
#include "twi.h"
#include "Arduino.h"

/**
* @}
*/

#ifdef __cplusplus
extern "C"
{
#endif
uint32_t registreId = 0;

static i2c_t *I2C_LocalObject1 = NULL;
static i2c_t *I2C_LocalObject2 = NULL;

/**
* @brief  Default init and setup GPIO and I2C peripheral
* @param  obj : pointer to i2c_t structure
* @retval none
*/
void i2c_init(i2c_t *obj)
{
  i2c_custom_init(obj, I2C_DEFAULT_SPEED, 0, I2C_ADDMODE_7BIT, 0x33, 1);
}
/**
* @brief  Initialize and setup GPIO and I2C peripheral
* @param  obj : pointer to i2c_t structure
* @param  timing : one of the i2c_timing_e
* @param  addressingMode : I2C_ADDRESSINGMODE_7BIT or I2C_ADDRESSINGMODE_10BIT
* @param  ownAddress : device address
* @param  master : set to 1 to choose the master mode
* @retval none
*/
uint32_t getRegistre(void)
{
  return registreId;
}

void i2c_custom_init(i2c_t *obj, uint32_t timing,
		  	  	  	   uint8_t remap_s, uint16_t addressingMode, uint16_t ownAddress,
					   uint8_t master)
{

  I2C_InitTypeDef  I2C_InitTSturcture = {0};
  GPIO_InitTypeDef GPIO_InitStructure = {0};
  NVIC_InitTypeDef NVIC_InitStructure;

  if(obj->I2Cx == I2C1)
  {
	  I2C_LocalObject1 = obj;
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

	  if(remap_s == 0)
	  {
		  GPIO_PinRemapConfig(GPIO_Remap_I2C1, DISABLE);
	  } else {
		  GPIO_PinRemapConfig(GPIO_Remap_I2C1, ENABLE);
	  }
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

	  if(remap_s == 0)
	  {
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
	  }
	  else
	  {

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
	  }

  }
  else if (obj->I2Cx == I2C2)
  {
	  I2C_LocalObject2 = obj;

	  if(remap_s == 0)
	  {
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
	  } else {
		  return; //I2C2 cant be remaped
	  }
  }

  I2C_DeInit(obj->I2Cx);

  I2C_InitTSturcture.I2C_ClockSpeed = timing;
  I2C_InitTSturcture.I2C_Mode = I2C_Mode_I2C;
  I2C_InitTSturcture.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitTSturcture.I2C_OwnAddress1 = ownAddress;
  I2C_InitTSturcture.I2C_Ack = I2C_Ack_Enable;
  I2C_InitTSturcture.I2C_AcknowledgedAddress = addressingMode;
  I2C_Init(obj->I2Cx, &I2C_InitTSturcture);


  obj->addressingMode = addressingMode;
  obj->Address = ownAddress;

  if (master == 0)
  {
	/* Configure the I2C event priority */
	if(obj->I2Cx == I2C1)
	{
		NVIC_InitStructure.NVIC_IRQChannel = I2C1_EV_IRQn;
	}
	else if (obj->I2Cx == I2C2)
	{
		NVIC_InitStructure.NVIC_IRQChannel = I2C2_EV_IRQn;
	}

	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//Configure I2C error interrupt to have the higher priority.

	if(obj->I2Cx == I2C1)
	{
		NVIC_InitStructure.NVIC_IRQChannel = I2C1_ER_IRQn;
	}
	else if (obj->I2Cx == I2C2)
	{
		NVIC_InitStructure.NVIC_IRQChannel = I2C2_ER_IRQn;
	}
	NVIC_Init(&NVIC_InitStructure);

	I2C_ITConfig(obj->I2Cx, I2C_IT_EVT, ENABLE); //Part of the STM32 I2C driver
	I2C_ITConfig(obj->I2Cx, I2C_IT_BUF, ENABLE);
	I2C_ITConfig(obj->I2Cx, I2C_IT_ERR, ENABLE); //Part of the STM32 I2C driver
  }
  else
  {
	  I2C_AcknowledgeConfig(obj->I2Cx, ENABLE);
  }
  obj->init_done = 1;
  obj->isMaster = master;
  obj->remap = remap_s;
}

/**
* @brief  Initialize and setup GPIO and I2C peripheral
* @param  obj : pointer to i2c_t structure
* @retval none
*/
void i2c_deinit(i2c_t *obj)
{
  if (obj->I2Cx  == I2C1)
  {

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, DISABLE);
    GPIO_PinRemapConfig(GPIO_Remap_I2C1, DISABLE);
  } else {
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, DISABLE);
  }
  I2C_DeInit(obj->I2Cx);
  obj->init_done = 0;
}
/**
* @brief  Setup transmission speed. I2C must be configured before.
* @param  obj : pointer to i2c_t structure
* @param  frequency : i2c transmission speed
* @retval none
*/
void i2c_setTiming(i2c_t *obj, uint32_t frequency)
{
  if (obj->init_done == 1)
  {
      i2c_custom_init(obj, frequency, obj->remap, obj->addressingMode, obj->Address, obj->isMaster);
  }
}

/**
* @brief  Wait for an event with a timeout protection
* @param  obj : I2Cx: where x can be 1 to select the I2C peripheral.
* @param  I2C_EVENT: specifies the event to be checked.
* @retval An ErrorStatus enumeration value (SUCCESS / ERROR)
*/
ErrorStatus i2c_wait_for_event(I2C_TypeDef* I2Cx, uint32_t I2C_Event)
{
    __IO uint32_t timeout = millis();
    ErrorStatus ret;

    ret = I2C_CheckEvent(I2Cx, I2C_Event);
    while (ret == NoREADY)
    {
        if((millis() - timeout > I2C_DELAY_MAX))
        {
            /*!< Clear AF flag */
            I2C_ClearFlag(I2Cx, I2C_FLAG_AF);

            /*!< STOP condition */
            I2C_GenerateSTOP(I2Cx, ENABLE);

            return ret;
        }
        ret = I2C_CheckEvent(I2Cx, I2C_Event);
    }
    return ret;
}



/**
* @brief  Checks if target device is ready for communication
* @param  obj : pointer to i2c_t structure
* @param  devAddr: specifies the address of the device.
* @param  trials : Number of trials.
* @retval status
*/
i2c_status_e i2c_IsDeviceReady(i2c_t *obj, uint8_t dev_address,
                              uint32_t trials)
{
  if (obj->init_done == 1)
  {
    /* Send START condition */
    I2C_GenerateSTART(obj->I2Cx, ENABLE);

    /* Test on EV5 and clear it */
    if(i2c_wait_for_event(obj->I2Cx, I2C_EVENT_MASTER_MODE_SELECT) != READY)
        return I2C_BUSY;

    /* Send slave address + write condition */
    I2C_Send7bitAddress(obj->I2Cx, dev_address, I2C_Direction_Transmitter);

    if(i2c_wait_for_event(obj->I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) != READY){
        return I2C_TIMEOUT;
    } else {
        /*!< Clear AF flag */
        I2C_ClearFlag(obj->I2Cx, I2C_FLAG_AF);

        /*!< STOP condition */
        I2C_GenerateSTOP(obj->I2Cx, ENABLE);

        return I2C_OK;
    }
  }
  else
  {
    return I2C_ERROR;
  }

}


/**
* @brief  Write bytes at a given address
* @param  obj : pointer to i2c_t structure
* @param  dev_address: specifies the address of the device.
* @param  data: pointer to data to be write
* @param  size: number of bytes to be write.
* @retval read status
*/
i2c_status_e i2c_master_write(i2c_t *obj, uint8_t dev_address,
                              uint8_t *data, uint8_t size)
{
  if (size == 0) {
    return i2c_IsDeviceReady(obj, dev_address, 1);
  }

  if (obj->init_done == 1)
  {
    /* Send START condition */
    I2C_GenerateSTART(obj->I2Cx, ENABLE);

    /* Test on EV5 and clear it */
    if(i2c_wait_for_event(obj->I2Cx, I2C_EVENT_MASTER_MODE_SELECT) != READY)
        return I2C_BUSY;

    /* Send slave address + write condition */
    I2C_Send7bitAddress(obj->I2Cx, dev_address, I2C_Direction_Transmitter);

    if(i2c_wait_for_event(obj->I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) != READY)
        return I2C_BUSY;

    /* Send data */
    while (size != 0)
    {
      I2C_SendData(obj->I2Cx, *data++);
      size--;

      if (size > 0)
      {
        if(i2c_wait_for_event(obj->I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING) != READY)
          return I2C_BUSY;
      }
    }

    /* Send STOP condition */
    if(i2c_wait_for_event(obj->I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED) != READY)
        return I2C_BUSY;

    I2C_GenerateSTOP(obj->I2Cx, ENABLE);

    return I2C_OK;
  }
  else
  {
    return I2C_ERROR;
  }

}

/**
* @brief  read bytes in master mode at a given address
* @param  obj : pointer to i2c_t structure
* @param  dev_address: specifies the address of the device.
* @param  data: pointer to data to be read
* @param  size: number of bytes to be read.
* @retval read status
*/
i2c_status_e i2c_master_read(i2c_t *obj, uint8_t dev_address, uint8_t *data, uint8_t size)
{
  uint8_t tmpSize = size;
  uint8_t idx = 0;
  if (obj->init_done == 1)
  {
    /* Enable ACK */
    I2C_AcknowledgeConfig(obj->I2Cx, ENABLE);
    /* Send START condition */
    I2C_GenerateSTART(obj->I2Cx, ENABLE);

    if(i2c_wait_for_event(obj->I2Cx, I2C_EVENT_MASTER_MODE_SELECT) != READY)
        return I2C_BUSY;

    /* Send slave address + read condition */
    I2C_Send7bitAddress(obj->I2Cx, dev_address, I2C_Direction_Receiver);

    if(i2c_wait_for_event(obj->I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) != READY)
        return I2C_BUSY;

    /* Send data */
    while (size != 0)
    {
      if (tmpSize == 1)
      {
        /* Disable ACK */
        I2C_AcknowledgeConfig(obj->I2Cx, DISABLE);

        /* Send STOP condition */
        I2C_GenerateSTOP(obj->I2Cx, ENABLE);

        tmpSize = 0;
      }
      else if (tmpSize == 2)
      {
        if(i2c_wait_for_event(obj->I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) != READY)
            return I2C_BUSY;

        /* Disable ACK */
        I2C_AcknowledgeConfig(obj->I2Cx, DISABLE);

        while (I2C_GetFlagStatus(obj->I2Cx, I2C_FLAG_BTF) == RESET);

        /* Send STOP condition */
        I2C_GenerateSTOP(obj->I2Cx, ENABLE);

        while (size != 0)
        {
          data[idx++] = I2C_ReceiveData(obj->I2Cx);
          size--;
        }

        tmpSize = 0;
      }
      else if ((tmpSize > 2) && (size == 3))
      {
        while (I2C_GetFlagStatus(obj->I2Cx, I2C_FLAG_BTF) == RESET);

        /* Disable ACK */
        I2C_AcknowledgeConfig(obj->I2Cx, DISABLE);

        data[idx++] = I2C_ReceiveData(obj->I2Cx);
        size--;

        /* Send STOP condition */
        I2C_GenerateSTOP(obj->I2Cx, ENABLE);

        data[idx++] = I2C_ReceiveData(obj->I2Cx);
        size--;
        tmpSize = 0;
      }

      /* Read data received */
      if (I2C_GetFlagStatus(obj->I2Cx, I2C_FLAG_RXNE) == SET)
      {
        data[idx++] = I2C_ReceiveData(obj->I2Cx);
        size--;
      }
    }
  }
  else
  {
    return I2C_ERROR;
  }

  return I2C_OK;

}
/**
* @brief  Write bytes to master
* @param  obj : instance to use
* @param  data: pointer to data to be write
* @param  size: number of bytes to be write.
* @retval none
*/
void i2c_slave_write_IT(i2c_t *obj, uint8_t *data, uint8_t size)
{
  uint8_t i = 0;
  if (obj->init_done == 1)
  {
    // Check the communication status
    for (i = 0; (i < size) && (i < I2C_TX_RX_BUFFER_SIZE); i++)
    {
      obj->i2cTxRxBuffer[i] = *(data + i);
      obj->i2cTxRxBufferSize++;
    }
  }
}
/** @brief  sets function called before a slave read operation
* @param  obj : pointer to i2c_t structure
* @param  function: callback function to use
* @retval None
*/
void i2c_attachSlaveRxEvent(i2c_t *obj, void (*function)(uint8_t *, int))
{
  if ((obj == NULL) || (function == NULL))
    return;

  if (obj->init_done == 1)
  {
    obj->i2c_onSlaveReceive = function;
  }
}

/** @brief  sets function called before a slave write operation
* @param  obj : pointer to i2c_t structure
* @param  function: callback function to use
* @retval None
*/
void i2c_attachSlaveTxEvent(i2c_t *obj, void (*function)(void))
{
  if ((obj == NULL) || (function == NULL))
    return;

  if (obj->init_done == 1)
  {
    obj->i2c_onSlaveTransmit = function;
  }
}

/**
* @brief  Slave printf callback.
* @param  none
* @retval None
*/
void I2C_slaveITCallback(i2c_t *obj)
{
  __IO uint32_t Event = 0;
  /* Read SR2 register to get I2C error */
  if ((obj->I2Cx->STAR2) != 0)
  {
    obj->I2Cx->STAR2 = 0; //
    I2C_ClearFlag(obj->I2Cx, I2C_FLAG_OVR | I2C_FLAG_BERR);
  }

  Event = I2C_GetLastEvent(obj->I2Cx);
  switch (Event)
  {
    /******* Slave transmitter ******/
    /* check on EV1 */
  case I2C_EVENT_SLAVE_TRANSMITTER_ADDRESS_MATCHED:
    obj->i2c_onSlaveTransmit();
    obj->txCounter = 0;
    break;

    /* check on EV3 */
  case I2C_EVENT_SLAVE_BYTE_TRANSMITTING:
    /* Transmit data */
    I2C_SendData(obj->I2Cx, obj->i2cTxRxBuffer[obj->txCounter++]);
    break;

    /* check on EV3-2 */
  case I2C_EVENT_SLAVE_ACK_FAILURE:
    /* Clear Ack failure flag */
    I2C_ClearFlag(obj->I2Cx, I2C_FLAG_AF);
    break;
    /******* Slave receiver **********/
    /* check on EV1*/
  case I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED:
    I2C_CheckEvent(obj->I2Cx, I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED);
    obj->i2cTxRxBufferSize = 0;
    break;

    /* Check on EV2*/
  case I2C_EVENT_SLAVE_BYTE_RECEIVED:
    if (obj->i2cTxRxBufferSize < I2C_TX_RX_BUFFER_SIZE)
    {
      obj->i2cTxRxBuffer[obj->i2cTxRxBufferSize++] = I2C_ReceiveData(obj->I2Cx);
    }
    else
    {
      I2C_ReceiveData(obj->I2Cx);
    }
    break;
  default:
    break;
  }

  /* Check on EV4 */
  /* Do not check event EV4 in switch above because flag is never "SUCCESS" */
  if (I2C_GetFlagStatus(obj->I2Cx, I2C_FLAG_STOPF) == SET)
  {
    I2C_AcknowledgeConfig(obj->I2Cx, ENABLE);
    if (obj->i2cTxRxBufferSize < I2C_TX_RX_BUFFER_SIZE)
    {
      obj->i2cTxRxBuffer[obj->i2cTxRxBufferSize++] = I2C_ReceiveData(obj->I2Cx);
    }
    else
    {

      I2C_ReceiveData(obj->I2Cx);
    }
    obj->i2c_onSlaveReceive(
        obj->i2cTxRxBuffer,
        obj->i2cTxRxBufferSize);
  }
}

/**
* @}
*/

void I2C1_EV_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void I2C2_EV_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void I2C1_ER_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void I2C2_ER_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));


void I2C1_EV_IRQHandler(void)
{
	I2C_slaveITCallback(I2C_LocalObject1);
}

void I2C2_EV_IRQHandler(void)
{
	I2C_slaveITCallback(I2C_LocalObject2);
}

void I2C1_ER_IRQHandler(void)
{
  if (I2C_GetITStatus(I2C1, I2C_IT_AF))
  {
    I2C_ClearITPendingBit(I2C1, I2C_IT_AF);
  }
}

void I2C2_ER_IRQHandler(void)
{
  if (I2C_GetITStatus(I2C2, I2C_IT_AF))
  {
    I2C_ClearITPendingBit(I2C2, I2C_IT_AF);
  }
}

#ifdef __cplusplus
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
