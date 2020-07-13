/*
  SoilSensors.cpp - Read multiple Capacitive Soil Moisture Sensors.
*/
#include "SoilSensors.h"

SoilSensors::SoilSensors(
  int sensor1_pin, int sensor1_wet, int sensor1_dry,
  int sensor2_pin, int sensor2_wet, int sensor2_dry,
  int sensor3_pin, int sensor3_wet, int sensor3_dry,
  int sensor4_pin, int sensor4_wet, int sensor4_dry,
  int sensor5_pin, int sensor5_wet, int sensor5_dry
) {
  _soil1 = new Capacitive_SoilSensor(sensor1_pin, sensor1_wet, sensor1_dry);
  _soil2 = new Capacitive_SoilSensor(sensor2_pin, sensor2_wet, sensor2_dry);
  _soil3 = new Capacitive_SoilSensor(sensor3_pin, sensor3_wet, sensor3_dry);
  _soil4 = new Capacitive_SoilSensor(sensor4_pin, sensor4_wet, sensor4_dry);
  _soil5 = new Capacitive_SoilSensor(sensor5_pin, sensor5_wet, sensor5_dry);
}

void SoilSensors::begin() {
}

SoilMoistureResult SoilSensors::readAll() {
  SoilMoistureResult result;
  result.array[0] = _soil1->measurePercentage();
  result.array[1] = _soil2->measurePercentage();
  result.array[2] = _soil3->measurePercentage();
  result.array[3] = _soil4->measurePercentage();
  result.array[4] = _soil5->measurePercentage();

  return result;
}
