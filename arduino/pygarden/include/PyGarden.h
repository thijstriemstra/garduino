/*
  PyGarden.h - Library for monitoring a garden.
*/
#ifndef PyGarden_h
#define PyGarden_h

#include <Arduino.h>
#include <Functor.h>
#include <Method.h>

#include <IOT.h>
#include <LED.h>
#include <Button.h>
#include <Sensors.h>
#include <SolenoidValve.h>
#include <SystemClock.h>

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
    SystemClock* _clock;
    SolenoidValve* _waterValve;

    bool started = false;
};

#endif