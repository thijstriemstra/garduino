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
    IOT(const char *base_topic);
    void begin(Method connected_callback, Method disconnected_callback);
    void publish(const char* sub_topic, double value);
    void connect();
    void disconnect();
    void connectToMqtt();

  private:
    const char* _baseTopic;
    uint16_t _lastPacketIdPubSent;
};

#endif