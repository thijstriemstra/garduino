/*
  PyGarden.h - Library for monitoring a garden.
*/
#ifndef PyGarden_h
#define PyGarden_h

#include "Arduino.h"
#include <YL83_RainSensor.h>
#include <FC28_SoilSensor.h>
#include <DS18B20_TemperatureSensors.h>

#define VERSION "1.0.0"

class PyGarden
{
  public:
    PyGarden();
    void begin();
    void loop();

  private:
    YL83_RainSensor* _rain;
    FC28_SoilSensor* _soil1;
    FC28_SoilSensor* _soil2;
    DS18B20_TemperatureSensors* _temperature;
};

#endif