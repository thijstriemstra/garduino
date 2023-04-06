/*  Copyright (c) 2021-2023, Collab
 *  All rights reserved
*/
/*
  DisplayTask.h
*/
#ifndef DisplayTask_h
#define DisplayTask_h

#include <Arduino.h>
#include <images.h>
#include <RTClib.h>
#include <ArduinoLog.h>
#include <SystemClock.h>
#include <SoilSensors.h>
#include <SSD1306_OLEDDisplay_Mux.h>

class DisplayTask {
  public:
    DisplayTask(
      SSD1306_OLEDDisplay_Mux* display,
      SystemClock* clock
    );

    int currentSoilSensor = 0;
    unsigned long duration;
    bool active = false;
    bool countdownDuration = false;

    void begin();
    void open(bool countdown_duration = false);
    void close();
    void showLogo();
    void showTime();
    void showLux(float lux);
    void showHumidity(float humidity);
    void showTemperature(float temperature);
    void showSignalStrength(int signal_strength);
    void showSoilMoisture(SoilMoistureResult result);
    void showSchedule(String schedule, int duration, bool today_complete);
    void showVersion(String build_date, String build_time, String version_nr);

  private:
    DateTime _openStart;
    bool _counting = false;

    SystemClock* _clock;
    SSD1306_OLEDDisplay_Mux* _display;

    // tasks
    static void countdown(void *pvParameter);
};

#endif
