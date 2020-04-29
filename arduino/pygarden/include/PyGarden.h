/*
  PyGarden.h - Library for monitoring a garden.
*/
#ifndef PyGarden_h
#define PyGarden_h

#include <Arduino.h>

#include <Thread.h>
#include <Functor.h>
#include <Method.h>
#include <TimeLib.h>

#include <IOT.h>
#include <LED.h>
#include <Button.h>
#include <Sensors.h>
#include <SystemClock.h>
#include <SolenoidValve.h>
#include <PowerManagement.h>

class PyGarden
{
  public:
    PyGarden();
    void begin();
    void loop();
    void sleep();
    void openValve();
    void closeValve();
    void toggleValve();

  private:
    IOT* _iot;
    LED* _powerLED;
    LED* _manualLED;
    LED* _networkLED;
    Button* _manualBtn;
    Button* _powerBtn;
    Sensors* _sensors;
    SystemClock* _clock;
    PowerManagement* _power;
    SolenoidValve* _waterValve;
    Thread* _wateringTask;

    bool started = false;
    bool _manualMode = false;
    bool needsWatering(String timestamp);

    // callbacks
    void onSystemWakeup();
    void onPowerButtonPush();
    void onManualButtonPush();
    void onConnectionReady();
    void onConnectionClosed();
    void onPublishReady();
};

#endif