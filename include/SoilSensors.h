/*
  SoilSensors.h - Read multiple Capacitive Soil Moisture Sensors.
*/
#ifndef SoilSensors_h
#define SoilSensors_h

#include "Arduino.h"
#include <Capacitive_SoilSensor.h>

struct SoilMoistureResult {
    int array[5];
};

class SoilSensors
{
  public:
    SoilSensors(
      int sensor1_pin, int sensor1_wet, int sensor1_dry,
      int sensor2_pin, int sensor2_wet, int sensor2_dry,
      int sensor3_pin, int sensor3_wet, int sensor3_dry,
      int sensor4_pin, int sensor4_wet, int sensor4_dry,
      int sensor5_pin, int sensor5_wet, int sensor5_dry
    );
    SoilMoistureResult readAll();
    void begin();

  private:
    Capacitive_SoilSensor* _soil1;
    Capacitive_SoilSensor* _soil2;
    Capacitive_SoilSensor* _soil3;
    Capacitive_SoilSensor* _soil4;
    Capacitive_SoilSensor* _soil5;
};

#endif