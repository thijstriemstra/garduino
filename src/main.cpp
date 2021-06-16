/*
* Copyright (c) 2020-2021 Collab
* All rights reserved
*/

#include "Garduino.h"

// *************************************************************************
//  Uncomment line below to fully disable logging, and reduce project size
// ************************************************************************
//#define DISABLE_LOGGING

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
