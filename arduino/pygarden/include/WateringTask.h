/*
  WateringTask.h
*/
#ifndef WateringTask_h
#define WateringTask_h

#include <Arduino.h>
#include "nvs_flash.h"
#include <Thread.h>
#include <Method.h>
#include <RtcDS3231.h>
#include <Preferences.h>
#include <SolenoidValve.h>

#define countof(a) (sizeof(a) / sizeof(a[0]))

class WateringTask: public Thread {
  public:
    WateringTask(
      long interval,
      int valve_pin,
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
    long _interval;
    long _lastRun;
    String _timestamp;
    Preferences* _prefs;
    SolenoidValve* _waterValve;
    Method _finishedCallback;
    const char* _namespace = "pygarden";
};

#endif