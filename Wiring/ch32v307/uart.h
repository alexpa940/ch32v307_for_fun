/**
  ******************************************************************************
  * @file    uart.h
  * @author  WI6LABS
  * @version V1.0.0
  * @date    08-10-2018
  * @brief   Header for uart module
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UART_H
#define __UART_H

/* Includes ------------------------------------------------------------------*/
#include "ch32v30x.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define UART_NUM 8

#define UART_PARITY_ODD USART_Parity_Odd
#define UART_PARITY_EVEN USART_Parity_Even
#define UART_PARITY_NONE USART_Parity_No

#define UART_STOPBITS_2 USART_StopBits_2
#define UART_STOPBITS_1 USART_StopBits_1

#define UART_WORDLENGTH_8B USART_WordLength_8b
#define UART_WORDLENGTH_9B USART_WordLength_9b


/* Exported types ------------------------------------------------------------*/
typedef struct serial_s serial_t;

struct serial_s
{
  USART_TypeDef *uart;
  uint8_t index;
  uint8_t recv;
  uint32_t baudrate;
  uint32_t databits;
  uint32_t stopbits;
  uint32_t parity;

  uint8_t *rx_buff;
  volatile uint16_t rx_head;
  uint16_t rx_tail;

  uint8_t *tx_buff;
  volatile uint16_t tx_head;
  uint16_t tx_tail;

  uint32_t remap_type;
};

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void uart_init(serial_t *obj);
void uart_deinit(serial_t *obj);
uint8_t uart_write(serial_t *obj, uint8_t data);
int uart_getc(serial_t *obj, unsigned char* c);
void uart_attach_rx_callback(serial_t *obj, void (*callback)(serial_t*));
void uart_attach_tx_callback(serial_t *obj, void (*callback)(serial_t*));

void HAL_UART_RxCpltCallback(USART_TypeDef *huart);


#ifdef __cplusplus
}
#endif

#endif /* __UART_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
