/*  Copyright (c) 2020-2023, Collab
 *  All rights reserved
*/
/*
  SoilSensors.h - Read multiple Capacitive Soil Moisture Sensors.
*/
#ifndef SoilSensors_h
#define SoilSensors_h

#include <Arduino.h>
#include <MultiPlexer_MCP3008.h>
#include <Capacitive_SoilSensor_MCP3008.h>

struct SoilMoistureResult {
  int sensor1;
  int sensor2;
  int sensor3;
  int sensor4;
  int sensor5;
  int sensor6;
  int sensor7;
  int sensor8;
  int totalSensors = 8;
};

struct SoilSensorsConfig {
  int sensor1_channel;
  int sensor1_wet;
  int sensor1_dry;
  int sensor2_channel;
  int sensor2_wet;
  int sensor2_dry;
  int sensor3_channel;
  int sensor3_wet;
  int sensor3_dry;
  int sensor4_channel;
  int sensor4_wet;
  int sensor4_dry;
  int sensor5_channel;
  int sensor5_wet;
  int sensor5_dry;
  int sensor6_channel;
  int sensor6_wet;
  int sensor6_dry;
  int sensor7_channel;
  int sensor7_wet;
  int sensor7_dry;
  int sensor8_channel;
  int sensor8_wet;
  int sensor8_dry;
};

class SoilSensors {
  public:
    SoilSensors(SoilSensorsConfig cfg, MultiPlexer_MCP3008* adc);
    SoilMoistureResult readAll();
    SoilMoistureResult readAllPercentage();
    void begin();

  private:
    Capacitive_SoilSensor_MCP3008* _soil1;
    Capacitive_SoilSensor_MCP3008* _soil2;
    Capacitive_SoilSensor_MCP3008* _soil3;
    Capacitive_SoilSensor_MCP3008* _soil4;
    Capacitive_SoilSensor_MCP3008* _soil5;
    Capacitive_SoilSensor_MCP3008* _soil6;
    Capacitive_SoilSensor_MCP3008* _soil7;
    Capacitive_SoilSensor_MCP3008* _soil8;
};

#endif