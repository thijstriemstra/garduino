/*  Copyright (c) 2020-2021, Collab
 *  All rights reserved
*/
/*
  Garduino.h - Library for monitoring a garden.
*/
#ifndef Garduino_h
#define Garduino_h

#include <Arduino.h>
#include <Functor.h>
#include <Method.h>
#include <ArduinoLog.h>

#include <IOT.h>
#include <Utils.h>
#include <Controls.h>
#include <Sensors.h>
#include <SystemClock.h>
#include <DisplayTask.h>
#include <WateringTask.h>
#include <PowerManagement.h>
#include <MultiPlexer_TCA9548A.h>
#include <SSD1306_OLEDDisplay_Mux.h>

class Garduino {
  public:
    Garduino();
    void begin();
    void loop();
    void openValve();
    void closeValve();
    void toggleValve();
    void checkWatering();
    void startManualMode();
    void displayTime();
    void displayTemperature();
    void sleep(bool forced = false);
    bool isWatering();

    bool started = false;
    bool connected = false;

  private:
    IOT *_iot;
    Sensors* _sensors;
    Controls* _controls;
    SystemClock* _clock;
    PowerManagement* _power;
    DisplayTask* _displayTask;
    MultiPlexer_TCA9548A* _i2c;
    WateringTask* _wateringTask;
    SSD1306_OLEDDisplay_Mux* _display;

    bool _manualMode = false;
    int _totalReadings = 15;
    const char *_namespace = "garduino";
    const char *_version = "3.3.0";

    // tasks
    static void displayInfo(void *pvParameter);

    // logging
    static void printPrefix(Print* _logOutput, int logLevel);

    // callbacks
    void onSystemWakeup();
    void onPowerButtonPush();
    void onManualButtonPush();
    void onConnectionReady();
    void onConnectionClosed();
    void onConnectionFailed(int connection_type);
    void onPublishReady();
    void onWateringReady();
    void onValveOpen();
    void onValveClosed();
};

#endif
