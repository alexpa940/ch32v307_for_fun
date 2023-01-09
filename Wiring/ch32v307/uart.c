/**
  ******************************************************************************
  * @file    uart.c
  * @author  WI6LABS
  * @version V1.0.0
  * @date    08-10-2018
  * @brief   provide the UART interface
  *
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
#include "uart.h"
#include "Arduino.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif


static USART_TypeDef *uart_handlers[UART_NUM] = {NULL};
static void (*rx_callback[UART_NUM])(serial_t*);
static serial_t *rx_callback_obj[UART_NUM];

static void (*tx_callback[UART_NUM])(serial_t*);
static serial_t *tx_callback_obj[UART_NUM];

/**
* @brief  Function called to initialize the uart interface
* @param  obj : pointer to serial_t structure
* @retval None
*/

void uart_init(serial_t *obj)
{
  static GPIO_InitTypeDef  GPIO_InitStructure;
  static USART_InitTypeDef USART_InitStructure;
  static NVIC_InitTypeDef  NVIC_InitStructure;

  if (obj == NULL)
  {
    return;
  }

  if(obj->uart == NULL) {
    //printf("ERROR: UART pins mismatch\n");
    return;
  }


  if (obj->uart == USART1)
  {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    //TODO: REMAP for UARTs
    if(obj->remap_type == 1)
    {
//        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
//
//        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
//        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//
//        GPIO_Init(GPIOA, &GPIO_InitStructure);
//
//        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
//        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//        GPIO_Init(GPIOA, &GPIO_InitStructure);

    }
    else
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;

        GPIO_Init(GPIOA, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
    }
    obj->index = 0;
  }
  else if(obj->uart == USART2)
  {

  }


  USART_InitStructure.USART_BaudRate = obj->baudrate;
  USART_InitStructure.USART_WordLength = obj->databits;
  USART_InitStructure.USART_StopBits = obj->stopbits;
  USART_InitStructure.USART_Parity = obj->parity;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

  USART_Init(obj->uart, &USART_InitStructure);
  USART_ITConfig(obj->uart, USART_IT_RXNE, ENABLE);
  USART_ITConfig(obj->uart, USART_IT_TXE, ENABLE);

  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  USART_Cmd(obj->uart, ENABLE);

  //Clear status register
  (void)obj->uart->STATR;
  (void)obj->uart->DATAR;


  uart_handlers[obj->index] = obj->uart;
}

/**
* @brief  Function called to deinitialize the uart interface
* @param  serial_id : one of the defined serial interface
* @retval None
*/
void uart_deinit(serial_t *obj)
{
  if(obj == NULL)
    return;
  USART_DeInit(obj->uart);
}

/**
* @brief  write the data on the uart
* @param  obj : pointer to serial_t structure
* @param  data : byte to write
* @retval The number of bytes written
*/
uint8_t uart_write(serial_t *obj, uint8_t data)
{
  if(obj == NULL) {
    return 0;
  }

  USART_ITConfig(obj->uart, USART_IT_TXE, ENABLE); // Enable UART transmission interrupt

//  while(USART_GetFlagStatus(obj->uart, USART_FLAG_TC) == RESET);
//  USART_SendData(obj->uart, data);

  return 1;
}

/**
* @brief  Read receive byte from uart
* @param  obj : pointer to serial_t structure
* @retval last character received
*/
int uart_getc(serial_t *obj, unsigned char* c)
{
  if(obj == NULL) {
    return -1;
  }

  obj->recv = USART_ReceiveData(obj->uart);

  /* In the case that only 7 bits are used, remove the msb (when serial config is SERIAL_7Ex or 7Ox )*/
  if((obj->databits == UART_WORDLENGTH_8B) && (obj->parity != UART_PARITY_NONE))
  {
    obj->recv = (obj->recv & 0x7F);
  }

  *c = (unsigned char)(obj->recv);

  return 0;
}

/**
 * Begin asynchronous RX transfer (enable interrupt for data collecting)
 *
 * @param obj : pointer to serial_t structure
 * @param callback : function call at the end of reception
 * @retval none
 */
void uart_attach_rx_callback(serial_t *obj, void (*callback)(serial_t*))
{
  if(obj == NULL) {
    return;
  }

  rx_callback[obj->index] = callback;
  rx_callback_obj[obj->index] = obj;
}

void uart_attach_tx_callback(serial_t *obj, void (*callback)(serial_t*))
{
  if(obj == NULL) {
    return;
  }

  tx_callback[obj->index] = callback;
  tx_callback_obj[obj->index] = obj;
}

/**
  * @brief  Return index of the serial handler
  * @param  UartHandle pointer on the uart reference
  * @retval index
  */
uint8_t uart_index(USART_TypeDef *huart)
{
  uint8_t i = 0;
  if(huart == NULL) {
    return UART_NUM;
  }

  for(i = 0; i < UART_NUM; i++) {
    if(huart == uart_handlers[i]) {
      break;
    }
  }

  return i;
}

/**
  * @brief  Rx Transfer completed callback
  * @param  UartHandle pointer on the uart reference
  * @retval None
  */
void HAL_UART_RxCpltCallback(USART_TypeDef *huart)
{
  uint8_t index = uart_index(huart);
  uint32_t isrflags   = huart->STATR;
  uint32_t cr1its     = huart->CTLR1;

  if(index < UART_NUM) {
      if (((isrflags & USART_STATR_RXNE) != RESET) && ((cr1its & USART_CTLR1_RXNEIE) != RESET))
          rx_callback[index](rx_callback_obj[index]);

      if (((isrflags & USART_STATR_TXE) != RESET) && ((cr1its & USART_CTLR1_TXEIE) != RESET))
          tx_callback[index](tx_callback_obj[index]);
  }
}

__attribute__((interrupt("WCH-Interrupt-fast"))) void USART1_IRQHandler(void)
{
    HAL_UART_RxCpltCallback(USART1);
}

#ifdef __cplusplus
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
