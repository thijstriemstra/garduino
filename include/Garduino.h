/*  Copyright (c) 2020-2023, Collab
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
#include <Buzzer.h>
#include <Controls.h>
#include <Sensors.h>
#include <SystemClock.h>
#include <DisplayTask.h>
#include <WateringTask.h>
#include <PowerManagement.h>
#include <MultiPlexer_PCF8574.h>
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
    void displayLux();
    void displayHumidity();
    void displayTemperature();
    void displaySoilMoisture();
    void displaySignalStrength();
    void sleep(bool forced = false);
    bool isWatering();

    bool started = false;
    bool connected = false;

  private:
    IOT *_iot;
    Buzzer* _buzzer;
    Sensors* _sensors;
    Controls* _controls;
    SystemClock* _clock;
    PowerManagement* _power;
    DisplayTask* _displayTask;
    MultiPlexer_TCA9548A* _i2c;
    WateringTask* _wateringTask;
    MultiPlexer_PCF8574* _ioExpander;
    SSD1306_OLEDDisplay_Mux* _display;

    bool _manualMode = false;
    bool _showBootScreen = true;
    const int _totalReadings = 16;
    const char *_namespace = "garduino";
    const char *_version = "4.0.0";
    const char *MENU_SOIL = "soil";
    const char *MENU_DEFAULT = "default";
    String _menuMode = MENU_DEFAULT;

    // tasks
    static void displayInfo(void *pvParameter);

    // logging
    static void printPrefix(Print* _logOutput, int logLevel);

    // callbacks
    void onSystemWakeup();
    void onPowerButtonPush();
    void onManualButtonLongPush();
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
