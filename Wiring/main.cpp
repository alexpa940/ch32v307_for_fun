/*
  main.cpp - Main loop for Arduino sketches
  Copyright (c) 2005-2013 Arduino Team.  All right reserved.

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
*/

#define ARDUINO_MAIN
#include "Arduino.h"
#include "FreeRTOS.h"
#include "task.h"
/* Global define */
#define TASK1_TASK_PRIO     4
#define TASK1_STK_SIZE      2048

/* Global Variable */
TaskHandle_t Task1Task_Handler;

extern "C" {

    void __libc_init_array(void);

    __attribute__((weak)) void _init(void)
    {
    }
}


/*
 * \brief Main entry point of Arduino application
 */
void main_loop(void* par)
{
  setup();

  for (;;) {
    loop();
  }

  return;
}

int main(void)
{
    Serial1.begin(0, 115200, SERIAL_8N1);
    xTaskCreate((TaskFunction_t )main_loop,
                        (const char*    )"MainLoop",
                        (uint16_t       )TASK1_STK_SIZE,
                        (void*          )NULL,
                        (UBaseType_t    )TASK1_TASK_PRIO,
                        (TaskHandle_t*  )&Task1Task_Handler);
    vTaskStartScheduler();

    while(1);
    return 0;

}

// Force init to be called *first*, i.e. before static object allocation.
// Otherwise, statically allocated objects that need HAL may fail.
extern "C" void premain()
{
  init();
  InitDelayTimer();
}

