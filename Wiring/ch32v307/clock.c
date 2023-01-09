/**
  ******************************************************************************
  * @file    clock.c
  * @author  WI6LABS
  * @version V1.0.0
  * @date
  * @brief   provide clock services for time purpose
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

#include "ch32v30x.h"
#include "clock.h"


#ifdef __cplusplus
extern "C"
{
#endif

  static volatile uint32_t g_current_ms = 1;

  /**
  * @brief  Function called wto read the current micro second
  * @param  None
  * @retval None
  */
  uint32_t GetCurrentMicro(void)
  {
      uint32_t ms = g_current_ms;
      uint32_t systick_value = SysTick->CNT;
      uint32_t systick_load = SysTick->CMP + 1;
      uint32_t us = ((systick_load - systick_value) * 1000) / systick_load;
      return (ms * 1000 + us);
  }

  /**
  * @brief  Function called wto read the current millisecond
  * @param  None
  * @retval None
  */
  uint32_t GetCurrentMilli(void)
  {
    uint32_t ms;

    ms = g_current_ms;

    return ms;
  }

  /**
  * @brief  This function configures the source of the time base.
  * @param  None
  * @retval None
  */
  void InitDelayTimer(void)
  {
      /*Configuration interrupt priority*/
      NVIC_InitTypeDef NVIC_InitStructure = {0};
      NVIC_InitStructure.NVIC_IRQChannel = SysTicK_IRQn;
      NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//Seeing priority
      NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;//Response priority
      NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//Enable
      NVIC_Init(&NVIC_InitStructure);

      /*Configuration timer*/
      SysTick->CTLR= 0;
      SysTick->SR  = 0;
      SysTick->CNT = 0;
      SysTick->CMP = (SystemCoreClock / 1000)-1; //The latter 1000 represents 1000Hz (that is, 1MS to interrupt once)
      SysTick->CTLR= 0xf;

      NVIC_EnableIRQ(SysTicK_IRQn);
  }

  void noOsSystickHandler()
  {

  }

  void osSystickHandler() __attribute__((weak, alias("noOsSystickHandler")));

  /*********************************************************************
   * @fn      SysTick_Handler
   *
   * @brief   This function handles SysTick interrupt.
   *
   * @return  none
   */
  __attribute__((interrupt("WCH-Interrupt-fast"))) void SysTick_Handler(void)
  {
      SysTick->SR=0;
      g_current_ms++;
      osSystickHandler();

  }

#ifdef __cplusplus
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
