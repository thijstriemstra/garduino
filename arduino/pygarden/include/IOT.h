/*
  IOT.h
*/
#ifndef IOT_h
#define IOT_h

#include <EEPROM.h>
#include <WiFi.h>
#include <Method.h>
#include <AsyncMqttClient.h>

class IOT
{
  public:
    IOT();
    void begin(
      int totalReadings,
      Method connected_callback,
      Method disconnected_callback,
      Method publishReady_callback,
      Method connectionFailed_callback
    );
    void publish(const char* sub_topic, double value);
    void connect();
    void disconnect();
    void connectToMqtt();

  private:
    uint16_t _lastPacketIdPubSent;
};

#endif