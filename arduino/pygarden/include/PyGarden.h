/*
  PyGarden.h - Library for monitoring a garden.
*/
#ifndef PyGarden_h
#define PyGarden_h

#include <Arduino.h>
#include <Functor.h>
#include <Method.h>
#include <Thread.h>
#include <ThreadController.h>

#include <IOT.h>
#include <LED.h>
#include <Button.h>
#include <Sensors.h>
#include <SystemClock.h>
#include <WateringTask.h>
#include <PowerManagement.h>

class PyGarden
{
  public:
    PyGarden();
    void begin();
    void loop();
    void openValve();
    void closeValve();
    void toggleValve();
    void checkWatering();
    void startManualMode();
    void sleep(bool forced = false);

    bool started = false;
    bool connected = false;

  private:
    IOT* _iot;
    LED* _powerLED;
    LED* _waterLED;
    LED* _manualLED;
    LED* _networkLED;
    Sensors* _sensors;
    Button* _powerBtn;
    Button* _manualBtn;
    SystemClock* _clock;
    PowerManagement* _power;
    WateringTask* _wateringTask;
    ThreadController* _scheduler;

    bool _manualMode = false;
    int _totalReadings = 10;
    const char* _namespace = "pygarden";
    const char* _version = "2.0.0";

    // callbacks
    void onSystemWakeup();
    void onPowerButtonPush();
    void onManualButtonPush();
    void onConnectionReady();
    void onConnectionClosed();
    void onConnectionFailed();
    void onPublishReady();
    void onWateringReady();
};

#endif