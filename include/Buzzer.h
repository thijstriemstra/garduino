/*  Copyright (c) 2023, Collab
 *  All rights reserved
*/
/*
  Buzzer.h
*/
#ifndef Buzzer_h
#define Buzzer_h

#include <Arduino.h>
#include <pitches.h>
#include <ArduinoLog.h>

class Buzzer {
  public:
    Buzzer(int pin, int channel = 0);
    void disable();
    void enable(int note = NOTE_C4, int duration = 100);

  private:
    int _buzzerPin;
    int _channel;
};

#endif
