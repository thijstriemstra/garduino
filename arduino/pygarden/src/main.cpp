/*
* Copyright (c) 2020 Collab
* All rights reserved
*/

#include "PyGarden.h"
PyGarden *app = new PyGarden();

void setup() {
  app->begin();
}

void loop() {
  app->loop();
}