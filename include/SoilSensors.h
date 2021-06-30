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
  int totalSensors = 5;
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
};

class SoilSensors {
  public:
    SoilSensors(SoilSensorsConfig cfg, MultiPlexer_MCP3008* adc);
    SoilMoistureResult readAll();
    void begin();

  private:
    Capacitive_SoilSensor_MCP3008* _soil1;
    Capacitive_SoilSensor_MCP3008* _soil2;
    Capacitive_SoilSensor_MCP3008* _soil3;
    Capacitive_SoilSensor_MCP3008* _soil4;
    Capacitive_SoilSensor_MCP3008* _soil5;
};

#endif