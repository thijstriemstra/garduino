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
#include <Method.h>

class IOT
{
  public:
    IOT();
    void begin(int totalReadings, Method connected_callback, Method publishReady_callback);
    void publish(const char* topic, double value);
    void connectToMqtt();
    void disconnectMqtt();

  private:
    uint16_t _lastPacketIdPubSent;
};

#endif