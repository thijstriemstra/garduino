/*
  WateringTask.h
*/
#ifndef WateringTask_h
#define WateringTask_h

#include <Arduino.h>
#include <Method.h>
#include <RTClib.h>
#include <Preferences.h>

#include <LED.h>
#include <SolenoidValve.h>

class WateringTask {
  public:
    WateringTask(
      long duration,
      int valve_pin,
      int led_pin,
      const char* app_namespace,
      String timestamp,
      Method finished_callback,
      Method valveOpen_callback,
      Method valveClosed_callback
    );
    bool active;
    void run();
    void begin();
    void start();
    void open();
    void close();
    DateTime load();
    void save(DateTime timestamp);
    bool isWatering();
    bool needsWatering(DateTime now);
    bool shouldRun(unsigned long time);
    String getLastRunTime();

  private:
    bool _debug;
    long _duration;
    long _lastRun = 0;
    const char* _namespace;
    LED* _waterLED;
    String _timestamp;
    Preferences* _prefs;
    SolenoidValve* _waterValve;

    // callbacks
    Method _finishedCallback;
    Method _valveOpenCallback;
    Method _valveClosedCallback;
};

#endif