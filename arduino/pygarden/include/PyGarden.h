/*
  PyGarden.h - Library for monitoring a garden.
*/
#ifndef PyGarden_h
#define PyGarden_h

#include <Arduino.h>
#include <Functor.h>
#include <Method.h>
#include "time.h"

#include <IOT.h>
#include <LED.h>
#include <Button.h>
#include <Sensors.h>
#include <SingleChannel_Relay.h>

class PyGarden
{
  public:
    PyGarden();
    void begin();
    void loop();
    void startRelay();
    void stopRelay();
    void toggleState();

    // network
    void onConnectionReady();

    // time
    void printLocalTime();

    // callbacks
    void onManualButtonPush();
    void onPowerButtonPush();

    // deepsleep
    void print_wakeup_reason();
    void setupDeepsleep();

  private:
    IOT* _iot;
    LED* _powerLED;
    LED* _manualLED;
    LED* _networkLED;
    Button* _manualBtn;
    Button* _powerBtn;
    Sensors* _sensors;
    SingleChannel_Relay* _water;

    bool started = false;
    const char* ntpServer = "pool.ntp.org";
    const long gmtOffset_sec = 3600;
    const int daylightOffset_sec = 3600;
};

#endif