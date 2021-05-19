/*
* Copyright (c) 2020-2021 Collab
* All rights reserved
*/

#include "Garduino.h"
#define SERIAL_BAUD_RATE 115200

Garduino *app = new Garduino();

void setup() {
  // start serial connection
  Serial.begin(SERIAL_BAUD_RATE);

  app->begin();
}

void loop() {
  app->loop();
}
