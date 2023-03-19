/*  Copyright (c) 2020-2023, Collab
 *  All rights reserved
*/
/*
  SoilSensors.cpp - Read multiple Capacitive Soil Moisture Sensors.
*/
#include "SoilSensors.h"

SoilSensors::SoilSensors(SoilSensorsConfig cfg, MultiPlexer_MCP3008 *adc) {
  _soil1 = new Capacitive_SoilSensor_MCP3008(adc, cfg.sensor1_channel, cfg.sensor1_wet, cfg.sensor1_dry);
  _soil2 = new Capacitive_SoilSensor_MCP3008(adc, cfg.sensor2_channel, cfg.sensor2_wet, cfg.sensor2_dry);
  _soil3 = new Capacitive_SoilSensor_MCP3008(adc, cfg.sensor3_channel, cfg.sensor3_wet, cfg.sensor3_dry);
  _soil4 = new Capacitive_SoilSensor_MCP3008(adc, cfg.sensor4_channel, cfg.sensor4_wet, cfg.sensor4_dry);
  _soil5 = new Capacitive_SoilSensor_MCP3008(adc, cfg.sensor5_channel, cfg.sensor5_wet, cfg.sensor5_dry);
  _soil6 = new Capacitive_SoilSensor_MCP3008(adc, cfg.sensor6_channel, cfg.sensor6_wet, cfg.sensor6_dry);
  _soil7 = new Capacitive_SoilSensor_MCP3008(adc, cfg.sensor7_channel, cfg.sensor7_wet, cfg.sensor7_dry);
  _soil8 = new Capacitive_SoilSensor_MCP3008(adc, cfg.sensor8_channel, cfg.sensor8_wet, cfg.sensor8_dry);
}

void SoilSensors::begin() {
}

SoilMoistureResult SoilSensors::readAll() {
  SoilMoistureResult result;
  result.sensor1 = _soil1->measurePercentage();
  result.sensor2 = _soil2->measurePercentage();
  result.sensor3 = _soil3->measurePercentage();
  result.sensor4 = _soil4->measurePercentage();
  result.sensor5 = _soil5->measurePercentage();
  result.sensor6 = _soil6->measurePercentage();
  result.sensor7 = _soil7->measurePercentage();
  result.sensor8 = _soil8->measurePercentage();

  return result;
}
