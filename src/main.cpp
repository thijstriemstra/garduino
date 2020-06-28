/*
* Copyright (c) 2020 Collab
* All rights reserved
*/

#include "PyGarden.h"
#define SERIAL_BAUD_RATE 115200

PyGarden *app = new PyGarden();

void setup() {
  // start serial connection
  Serial.begin(SERIAL_BAUD_RATE);

  app->begin();
}

void loop() {
  app->loop();
}
