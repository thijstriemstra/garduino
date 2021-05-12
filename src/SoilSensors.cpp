/*
  SoilSensors.cpp - Read multiple Capacitive Soil Moisture Sensors.
*/
#include "SoilSensors.h"

SoilSensors::SoilSensors(SoilSensorsConfig cfg, MultiPlexer_MCP3008 *adc) {
  _soil1 = new Capacitive_SoilSensor_MCP3008(cfg.sensor1_pin, cfg.sensor1_wet, cfg.sensor1_dry);
  _soil2 = new Capacitive_SoilSensor_MCP3008(cfg.sensor2_pin, cfg.sensor2_wet, cfg.sensor2_dry);
  _soil3 = new Capacitive_SoilSensor_MCP3008(cfg.sensor3_pin, cfg.sensor3_wet, cfg.sensor3_dry);
  _soil4 = new Capacitive_SoilSensor_MCP3008(cfg.sensor4_pin, cfg.sensor4_wet, cfg.sensor4_dry);
  _soil5 = new Capacitive_SoilSensor_MCP3008(cfg.sensor5_pin, cfg.sensor5_wet, cfg.sensor5_dry);
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
