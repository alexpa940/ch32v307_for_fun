/********************************** (C) COPYRIGHT *******************************
* File Name          : system_ch32v30x.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : CH32V30x Device Peripheral Access Layer System Header File.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/
#ifndef __SYSTEM_CH32V30x_H 
#define __SYSTEM_CH32V30x_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include <stdint.h>
#include "ch32v30x.h"

extern volatile uint32_t SystemCoreClock;          /* System Clock Frequency (Core Clock) */

/* System_Exported_Functions */  
extern void SystemInit(void);
extern void SystemCoreClockUpdate(void);

extern void SetSysClockToHSE( void );
extern void SetSysClockTo48_HSE( void );
extern void SetSysClockTo56_HSE( void );
extern void SetSysClockTo72_HSE( void );
extern void SetSysClockTo96_HSE( void );
extern void SetSysClockTo120_HSE( void );
extern void SetSysClockTo144_HSE( void );
extern void SetSysClockTo48_HSI( void );
extern void SetSysClockTo56_HSI( void );
extern void SetSysClockTo72_HSI( void );
extern void SetSysClockTo96_HSI( void );
extern void SetSysClockTo120_HSI( void );
extern void SetSysClockTo144_HSI( void );

#ifdef __cplusplus
}
#endif

#endif /*__CH32V30x_SYSTEM_H */



