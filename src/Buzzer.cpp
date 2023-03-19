/*  Copyright (c) 2023, Collab
 *  All rights reserved
 */

#include <Buzzer.h>

Buzzer::Buzzer(int pin) {
  _buzzerPin = pin;

  disable();
}

void Buzzer::enable() {
  int melody[] = {
    NOTE_C4
  };

  int noteDurations[] = {
    4
  };

  for (int thisNote = 0; thisNote < 1; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(_buzzerPin, melody[thisNote], noteDuration);

    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(_buzzerPin);
  }

  disable();
}

void Buzzer::disable() {
  // disable buzzer to prevent noise and heat
  // see https://forum.arduino.cc/t/solved-problem-with-arduino-buzzer-module/496242/10
  digitalWrite(_buzzerPin, HIGH);
}
