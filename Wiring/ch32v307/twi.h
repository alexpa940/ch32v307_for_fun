/**
  ******************************************************************************
  * @file    twi.h
  * @author  WI6LABS
  * @version V1.0.0
  * @date
  * @brief   Header for twi module
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TWI_H__
#define __TWI_H__

/* Includes ------------------------------------------------------------------*/
#include "ch32v30x.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* Exported macro ------------------------------------------------------------*/
#define I2C_ADDMODE_7BIT I2C_AcknowledgedAddress_7bit

/* Exported types ------------------------------------------------------------*/
/* I2C Tx/Rx buffer size */
#define I2C_TX_RX_BUFFER_SIZE 32

  typedef struct i2c_s i2c_t;

  struct i2c_s
  {
    /*  The 1st 2 members I2CName i2c
     *  and I2C_HandleTypeDef handle should
     *  be kept as the first members of this struct
     *  to have get_i2c_obj() function work as expected
     */
    I2C_TypeDef* I2Cx;
    uint8_t init_done;
    uint16_t Address;
    uint8_t isMaster;
    void (*i2c_onSlaveReceive)(uint8_t *, int);
    void (*i2c_onSlaveTransmit)(void);
    uint8_t i2cTxRxBuffer[I2C_TX_RX_BUFFER_SIZE];
    uint8_t i2cTxRxBufferSize;
    uint8_t txCounter;
    uint8_t remap;
    uint16_t addressingMode;
  };

  ///@brief I2C state
  typedef enum
  {
    I2C_OK = 0,
    I2C_TIMEOUT = 1,
    I2C_ERROR = 2,
    I2C_BUSY = 3
  } i2c_status_e;

#define I2C_DEFAULT_SPEED ((uint32_t)100000)

#define I2C_DELAY_MAX ((uint32_t)1)

  /* Exported constants --------------------------------------------------------*/
  /* Exported functions ------------------------------------------------------- */
  void i2c_init(i2c_t *obj);
  void i2c_custom_init(i2c_t *obj, uint32_t timing,
		  	  	  	   uint8_t remap_s, uint16_t addressingMode, uint16_t ownAddress,
                       uint8_t master);

  void i2c_deinit(i2c_t *obj);
  void i2c_setTiming(i2c_t *obj, uint32_t frequency);

  i2c_status_e i2c_master_write(i2c_t *obj, uint8_t dev_address, uint8_t *data, uint8_t size);
  i2c_status_e i2c_master_read(i2c_t *obj, uint8_t dev_address, uint8_t *data, uint8_t size);

  void i2c_slave_write_IT(i2c_t *obj, uint8_t *data, uint8_t size);

  void i2c_attachSlaveRxEvent(i2c_t *obj, void (*function)(uint8_t *, int));
  void i2c_attachSlaveTxEvent(i2c_t *obj, void (*function)(void));

  void I2C_slaveITCallback(i2c_t *obj);
  i2c_status_e i2c_IsDeviceReady(i2c_t *obj, uint8_t dev_address,
                                uint32_t trials);
  uint32_t getRegistre(void);
#ifdef __cplusplus
}
#endif

#endif /* __TWI_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
