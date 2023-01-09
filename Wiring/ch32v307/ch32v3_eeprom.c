/**
  ******************************************************************************
  * @file    stm32_eeprom.c
  * @brief   Provides emulated eeprom from flash
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2016-2021, STMicroelectronics
  * All rights reserved.
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

#include "ch32v3_eeprom.h"
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

static uint8_t eeprom_buffer[E2END + 1] __attribute__((aligned(2))) = {0};
/**
  * @brief  Function reads a byte from emulated eeprom (flash)
  * @param  pos : address to read
  * @retval byte : data read from eeprom
  */
uint8_t eeprom_read_byte(const uint32_t pos)
{
  eeprom_buffer_fill();
  return eeprom_buffered_read_byte(pos);
}

/**
  * @brief  Function writes a byte to emulated eeprom (flash)
  * @param  pos : address to write
  * @param  value : value to write
  * @retval none
  */
void eeprom_write_byte(uint32_t pos, uint8_t value)
{
  eeprom_buffered_write_byte(pos, value);
  eeprom_buffer_flush();
}


/**
  * @brief  Function reads a byte from the eeprom buffer
  * @param  pos : address to read
  * @retval byte : data read from eeprom
  */
uint8_t eeprom_buffered_read_byte(const uint32_t pos)
{
  return eeprom_buffer[pos];
}

/**
  * @brief  Function writes a byte to the eeprom buffer
  * @param  pos : address to write
  * @param  value : value to write
  * @retval none
  */
void eeprom_buffered_write_byte(uint32_t pos, uint8_t value)
{
  eeprom_buffer[pos] = value;
}

/**
  * @brief  This function copies the data from flash into the buffer
  * @param  none
  * @retval none
  */
void eeprom_buffer_fill(void)
{
  memcpy(eeprom_buffer, (uint8_t *)(PAGE_WRITE_START_ADDR), E2END + 1);
}

/**
  * @brief  This function writes the buffer content into the flash
  * @param  none
  * @retval none
  */
void eeprom_buffer_flush(void)
{
    static uint32_t NbrOfPage;
    static uint32_t Address;
    static uint16_t word;
    static uint32_t prev_hclk_state;
    prev_hclk_state = RCC->CFGR0 | (uint32_t)RCC_HPRE_DIV2;

    RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV2;
    __disable_irq();
    FLASH_Unlock();
    NbrOfPage = (PAGE_WRITE_END_ADDR - PAGE_WRITE_START_ADDR) / FLASH_PAGE_SIZE;

    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_WRPRTERR);

    for(uint32_t i = 0; i < NbrOfPage; i++)
    {
        FLASH_ErasePage(PAGE_WRITE_START_ADDR + (FLASH_PAGE_SIZE * i)); //Erase 4KB
    }

    Address = PAGE_WRITE_START_ADDR;
    FLASH->CTLR |= CR_PG_Set;
    while(Address < PAGE_WRITE_START_ADDR + E2END + 1)
    {
        memcpy(&word, &eeprom_buffer[Address - PAGE_WRITE_START_ADDR], 2);
        *(__IO uint16_t *)Address = word;
        FLASH_WaitForLastOperation(ProgramTimeout);
        Address = Address + 2;
    }

    FLASH->CTLR &= CR_PG_Reset;
    FLASH_Lock();

    //return hclk state
    RCC->CFGR0 |= prev_hclk_state;
    __enable_irq();
}

#ifdef __cplusplus
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
