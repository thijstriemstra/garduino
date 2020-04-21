
/*
  IOT.h
*/
#ifndef IOT_h
#define IOT_h

#include "WiFi.h"
#include "ArduinoJson.h"
#include <sys/time.h>
extern "C" {
    #include "freertos/FreeRTOS.h"
    #include "freertos/timers.h"
}
#include "AsyncMqttClient.h"
#include "IotWebConf.h"

// general string buffer size
#define STR_LEN 64
// configuration string buffer size
#define CONFIG_LEN 32

#define TAG "PyGarden"

class IOT
{
  public:
    IOT();
    void Init();
    void Run();
    void publish(const char *subtopic, const char *value, boolean retained = false);

  private:
    bool _clientsConfigured = false;
};

#endif