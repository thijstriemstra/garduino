/*
  SoilSensors.cpp - Read multiple Capacitive Soil Moisture Sensors.
*/
#include "SoilSensors.h"

SoilSensors::SoilSensors(
  
  int sensor1_pin,
  int sensor2_pin,
  int sensor3_pin,
  int sensor4_pin,
  int sensor5_pin,
  int sensor6_pin,
  int sensor7_pin,
  int sensor8_pin
) {
  _soil1 = new Capacitive_SoilSensor(sensor1_pin);
  _soil2 = new Capacitive_SoilSensor(sensor2_pin);
  _soil3 = new Capacitive_SoilSensor(sensor3_pin);
  _soil4 = new Capacitive_SoilSensor(sensor4_pin);
  _soil5 = new Capacitive_SoilSensor(sensor5_pin);
  _soil6 = new Capacitive_SoilSensor(sensor6_pin);
  _soil7 = new Capacitive_SoilSensor(sensor7_pin);
  _soil8 = new Capacitive_SoilSensor(sensor8_pin);
}

void SoilSensors::begin() {

}

SoilMoistureResult SoilSensors::readAll(MultiPlexer_74HC4067* mux) {
  SoilMoistureResult result;
  result.array[0] = _soil1->measurePercentage(mux);
  result.array[1] = _soil2->measurePercentage(mux);
  result.array[2] = _soil3->measurePercentage(mux);
  result.array[3] = _soil4->measurePercentage(mux);
  result.array[4] = _soil5->measurePercentage(mux);
  result.array[5] = _soil6->measurePercentage(mux);
  result.array[6] = _soil7->measurePercentage(mux);
  result.array[7] = _soil8->measurePercentage(mux);

  return result;
}
