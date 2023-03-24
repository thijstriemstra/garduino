/*  Copyright (c) 2023, Collab
 *  All rights reserved
 */

#include <Buzzer.h>

Buzzer::Buzzer(int pin, int channel) {
  _buzzerPin = pin;
  _channel = channel;

  setToneChannel(channel);

  disable();
}

void Buzzer::enable(int note, int duration) {
  tone(_buzzerPin, note, duration);

  disable();
}

void Buzzer::disable() {
  // disable buzzer to prevent noise and heat
  // see https://forum.arduino.cc/t/solved-problem-with-arduino-buzzer-module/496242/10
  digitalWrite(_buzzerPin, HIGH);
}
