/*
  SoilSensors.h - Read multiple Capacitive Soil Moisture Sensors.
*/
#ifndef SoilSensors_h
#define SoilSensors_h

#include "Arduino.h"
#include <MultiPlexer_74HC4067.h>
#include <Capacitive_SoilSensor.h>

struct SoilMoistureResult {
    int array[8];
};

class SoilSensors
{
  public:
    SoilSensors(
      
      int sensor1_pin,
      int sensor2_pin,
      int sensor3_pin,
      int sensor4_pin,
      int sensor5_pin,
      int sensor6_pin,
      int sensor7_pin,
      int sensor8_pin
    );
    void begin();
    SoilMoistureResult readAll(MultiPlexer_74HC4067* mux);

  private:
    Capacitive_SoilSensor* _soil1;
    Capacitive_SoilSensor* _soil2;
    Capacitive_SoilSensor* _soil3;
    Capacitive_SoilSensor* _soil4;
    Capacitive_SoilSensor* _soil5;
    Capacitive_SoilSensor* _soil6;
    Capacitive_SoilSensor* _soil7;
    Capacitive_SoilSensor* _soil8;
};

#endif
