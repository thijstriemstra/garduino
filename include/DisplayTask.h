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

    long duration;
    bool active = false;

    void begin();
    void open();
    void close();
    void showLogo();
    void showTime();
    void showLux(float lux);
    void showHumidity(float humidity);
    void showTemperature(float temperature);
    void showSignalStrength(int signal_strength);
    void showSoilMoisture(SoilMoistureResult result);

  private:
    DateTime _openStart;
    bool _counting = false;
    int _currentSoilSensor = 0;

    SystemClock* _clock;
    SSD1306_OLEDDisplay_Mux* _display;

    // tasks
    static void countdown(void *pvParameter);
};

#endif
