#include "Arduino.h"
#include "FreeRTOS.h"
#include "task.h"
#include "usb_device.h"
#include "USBSerial.h"
#include "EEPROM.h"


void setup()
{
    Serial1.printf("FreeRTOS Kernel Version:%s\r\n",tskKERNEL_VERSION_NUMBER);
    usb_device_init();
}

char test_str1[10] = {0};
char test_str[] = "EEPROMTEST";

void loop()
{
    if(SerialUSB1.available())
    {
        switch (SerialUSB1.read()) {
            case 'A':
                SerialUSB1.println("started write");
                SerialUSB1.readBytes(test_str, SerialUSB1.available());
                EEPROM.put(0, test_str);
                SerialUSB1.println("writed");
                break;
            case 'B':
                SerialUSB1.println("started read");

                EEPROM.get(0, test_str1);
                SerialUSB1.println(test_str1);
                break;
            default:
                break;
        }
    }

    if(SerialUSB2.available())
    {
        switch (SerialUSB2.read()) {
            case 'A':
                SerialUSB2.println(millis());
                break;
            case 'B':
                SerialUSB2.println(micros());
                break;
            default:
                break;
        }
    }
    vTaskDelay(500);
}
