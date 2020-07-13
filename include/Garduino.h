/*
  Garduino.h - Library for monitoring a garden.
*/
#ifndef Garduino_h
#define Garduino_h

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
#include <SSD1306_OLEDDisplay.h>

class Garduino
{
  public:
    Garduino();
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
    LED* _manualLED;
    LED* _networkLED;
    Sensors* _sensors;
    Button* _powerBtn;
    Button* _manualBtn;
    SystemClock* _clock;
    PowerManagement* _power;
    WateringTask* _wateringTask;
    ThreadController* _scheduler;
    SSD1306_OLEDDisplay* _display;

    bool _manualMode = false;
    int _totalReadings = 15;
    const char* _namespace = "garduino";
    const char* _version = "2.3.0";

    // callbacks
    void onSystemWakeup();
    void onPowerButtonPush();
    void onManualButtonPush();
    void onConnectionReady();
    void onConnectionClosed();
    void onConnectionFailed();
    void onPublishReady();
    void onWateringReady();
    void onValveOpen();
    void onValveClosed();
};

#endif