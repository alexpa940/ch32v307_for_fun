/*
  Copyright (c) 2011 Arduino.  All right reserved.
  Copyright (c) 2013 by Paul Stoffregen <paul@pjrc.com> (delayMicroseconds)

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _WIRING_TIME_H_
#define _WIRING_TIME_H_

#include "clock.h"

#ifdef __cplusplus
extern "C"
{
#endif
  /**
 * \brief Returns the number of milliseconds since the Arduino board began running the current program.
 *
 * This number will overflow (go back to zero), after approximately 50 days.
 *
 * \return Number of milliseconds since the program started (uint32_t)
 */
  extern uint32_t millis(void);

  /**
 * \brief Returns the number of microseconds since the Arduino board began running the current program.
 *
 * This number will overflow (go back to zero), after approximately 70 minutes. On 16 MHz Arduino boards
 * (e.g. Duemilanove and Nano), this function has a resolution of four microseconds (i.e. the value returned is
 * always a multiple of four). On 8 MHz Arduino boards (e.g. the LilyPad), this function has a resolution
 * of eight microseconds.
 *
 * \note There are 1,000 microseconds in a millisecond and 1,000,000 microseconds in a second.
 */
  extern uint32_t micros(void);

  /**
 * \brief Pauses the program for the amount of time (in miliseconds) specified as parameter.
 * (There are 1000 milliseconds in a second.)
 *
 * \param dwMs the number of milliseconds to pause (uint32_t)
 */
  extern void delay(uint32_t dwMs);

/**
 * \brief Pauses the program for the amount of time (in microseconds) specified as parameter.
 *
 * \param dwUs the number of microseconds to pause (uint32_t)
 */
//#ifndef __CSMC__
//  static inline void delayMicroseconds(uint32_t) __attribute__((always_inline, unused));
//#endif
//  static inline void delayMicroseconds(uint32_t usec)
//  {
//    uint32_t start = GetCurrentMicro();
//
//    while ((start + usec) > GetCurrentMicro())
//      ;
//  }
  static inline void delayMicroseconds(uint32_t us) {
    if(us==0)
      return;
    uint32_t start = micros();
    do {
        ((void)0);
    } while (micros() - start < us);
//    uint32_t n = us * (SystemCoreClock / 1000000) / 3;
//    g:
//    n -=1;
//    if(n != 0)
//        goto g;
  }
#ifdef __cplusplus
}
#endif

#endif /* _WIRING_TIME_H_ */
