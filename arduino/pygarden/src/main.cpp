/*
* Copyright (c) 2020 Collab
* All rights reserved
*/

#include "PyGarden.h"

PyGarden *app = new PyGarden();

void setup() {
  // start serial connection
  Serial.begin(115200);

  app->begin();
}

void loop() {
  app->loop();
}
