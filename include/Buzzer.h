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
    Buzzer(int pin);
    void enable();
    void disable();

  private:
    int _buzzerPin;
};

#endif
