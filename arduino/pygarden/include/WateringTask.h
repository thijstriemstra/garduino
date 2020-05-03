/*
  WateringTask.h
*/
#ifndef WateringTask_h
#define WateringTask_h

#include <Arduino.h>
#include <Thread.h>
#include <Method.h>
#include <RtcDS3231.h>
#include <Preferences.h>

#include <IOT.h>
#include <LED.h>
#include <SolenoidValve.h>

#define countof(a) (sizeof(a) / sizeof(a[0]))

class WateringTask: public Thread {
  public:
    WateringTask(
      long duration,
      int valve_pin,
      int led_pin,
      IOT* iot,
      const char* app_namespace,
      String timestamp,
      Method finished_callback
    );
    bool active;
    void run();
    void begin();
    void start();
    void open();
    void close();
    RtcDateTime load();
    void save(RtcDateTime timestamp);
    bool isWatering();
    bool needsWatering(RtcDateTime now);
    bool shouldRun(unsigned long time);
    String getLastRunTime();

  private:
    bool _debug;
    long _duration;
    long _lastRun = 0;
    const char* _namespace;
    IOT* _iot;
    LED* _waterLED;
    String _timestamp;
    Preferences* _prefs;
    Method _finishedCallback;
    SolenoidValve* _waterValve;
};

#endif