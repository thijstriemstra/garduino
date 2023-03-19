/*  Copyright (c) 2020-2023, Collab
 *  All rights reserved
*/
/*
  WateringTask.h
*/
#ifndef WateringTask_h
#define WateringTask_h

#include <Arduino.h>
#include <Method.h>
#include <ArduinoLog.h>
#include <RTClib.h>
#include <Preferences.h>
#include <LED_PCF8574.h>
#include <Utils.h>
#include <SolenoidValve.h>
#include <Buzzer.h>
#include <MultiPlexer_PCF8574.h>

class WateringTask {
  public:
    WateringTask(
      long task_duration,
      int valve_pin,
      int led_pin,
      int buzzer_pin,
      MultiPlexer_PCF8574* mcp,
      const char* app_namespace,
      String timestamp,
      Method finished_callback,
      Method valveOpen_callback,
      Method valveClosed_callback
    );

    long duration;
    bool active = false;

    void run();
    void begin();
    void start();
    void open();
    void close();
    void disableLEDs();
    DateTime load();
    void save(DateTime timestamp);
    bool isValveOpen();
    bool isWatering();
    bool needsWatering(DateTime now);
    String getLastRunTime();

  private:
    bool _debug;
    const char* _namespace;
    LED_PCF8574* _waterLED;
    String _timestamp;
    Buzzer* _buzzer;
    Preferences* _prefs;
    MultiPlexer_PCF8574* _mcp;
    SolenoidValve* _waterValve;

    static void setupTask(void *pvParameter);

    // key name for nvram storage (limited to 15 chars)
    const char* TIMESTAMP = "timestamp";

    // callbacks
    Method _finishedCallback;
    Method _valveOpenCallback;
    Method _valveClosedCallback;
};

#endif