/*
* Copyright (c) 2020-2021 Collab
* All rights reserved
*/

#include "Garduinov3.h"
#define SERIAL_BAUD_RATE 115200

Garduinov3 *app = new Garduinov3();

void setup() {
  // start serial connection
  Serial.begin(SERIAL_BAUD_RATE);

  app->begin();
}

void loop() {
  app->loop();
}
