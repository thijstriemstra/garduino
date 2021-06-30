/*  Copyright (c) 2021, Collab
 *  All rights reserved
*/
/*
  DisplayTask.h
*/
#ifndef DisplayTask_h
#define DisplayTask_h

#include <Arduino.h>
#include <RTClib.h>
#include <SSD1306_OLEDDisplay_Mux.h>

class DisplayTask {
  public:
    DisplayTask(
      SSD1306_OLEDDisplay_Mux* display
    );

    long duration;
    bool active = false;

    void begin();
    void open();
    void close();
    void showInfo();
    void showTime(DateTime now);
    void showTemperature(float temperature);

  private:
    SSD1306_OLEDDisplay_Mux* _display;
};

#endif
