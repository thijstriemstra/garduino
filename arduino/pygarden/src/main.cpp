/*
* Copyright (c) 2020 Collab
* All rights reserved


#include "PyGarden.h"
PyGarden *app = new PyGarden();

void setup() {
  app->begin();
}

void loop() {
  app->loop();
}
*/

#include "Arduino.h"

const int relay = WaterValvePin;

void setup() {
  Serial.begin(115200);
  pinMode(relay, OUTPUT);
}

void loop() {
  // Normally Open configuration, send LOW signal to let current flow
  // (if you're usong Normally Closed configuration send HIGH signal)
  digitalWrite(relay, LOW);
  Serial.println("Current Flowing");
  delay(5000); 
  
  // Normally Open configuration, send HIGH signal stop current flow
  // (if you're usong Normally Closed configuration send LOW signal)
  digitalWrite(relay, HIGH);
  Serial.println("Current not Flowing");
  delay(5000);
}