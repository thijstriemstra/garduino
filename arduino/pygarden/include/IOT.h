/*
  IOT.h
*/
#ifndef IOT_h
#define IOT_h

#include <EEPROM.h>
#include <WiFi.h>
extern "C" {
	#include "freertos/FreeRTOS.h"
	#include "freertos/timers.h"
}
#include <AsyncMqttClient.h>

class IOT
{
  public:
    IOT();
    void begin();
    void publish();
};

#endif