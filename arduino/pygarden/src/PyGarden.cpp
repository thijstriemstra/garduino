/*
  PyGarden.cpp - Library for monitoring a garden.
*/

#include "Arduino.h"
#include "PyGarden.h"

PyGarden::PyGarden() {
  /*
  _led = new LED(2, LOW);
  _btn = new Button(3);
  _switch = new Switch(4);
  _display = new TM1637_LCDDisplay(5, 6);
  _temperature = new DS18B20_TemperatureSensors(12);
  _distance = new HCSR04_DistanceSensor(10, 11);
  */
}

void PyGarden::begin() {
  // start serial connection
  Serial.begin(9600);

}

void PyGarden::loop() {
}
