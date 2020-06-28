/*
  SoilSensors.cpp - Read multiple Capacitive Soil Moisture Sensors.
*/
#include "SoilSensors.h"

SoilSensors::SoilSensors(
  int sensor1_pin, int sensor1_wet, int sensor1_dry,
  int sensor2_pin, int sensor2_wet, int sensor2_dry,
  int sensor3_pin, int sensor3_wet, int sensor3_dry,
  int sensor4_pin, int sensor4_wet, int sensor4_dry,
  int sensor5_pin, int sensor5_wet, int sensor5_dry,
  int sensor6_pin, int sensor6_wet, int sensor6_dry,
  int sensor7_pin, int sensor7_wet, int sensor7_dry,
  int sensor8_pin, int sensor8_wet, int sensor8_dry
) {
  _soil1 = new Capacitive_SoilSensor(sensor1_pin, sensor1_wet, sensor1_dry);
  _soil2 = new Capacitive_SoilSensor(sensor2_pin, sensor2_wet, sensor2_dry);
  _soil3 = new Capacitive_SoilSensor(sensor3_pin, sensor3_wet, sensor3_dry);
  _soil4 = new Capacitive_SoilSensor(sensor4_pin, sensor4_wet, sensor4_dry);
  _soil5 = new Capacitive_SoilSensor(sensor5_pin, sensor5_wet, sensor5_dry);
  _soil6 = new Capacitive_SoilSensor(sensor6_pin, sensor6_wet, sensor6_dry);
  _soil7 = new Capacitive_SoilSensor(sensor7_pin, sensor7_wet, sensor7_dry);
  _soil8 = new Capacitive_SoilSensor(sensor8_pin, sensor8_wet, sensor8_dry);
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
