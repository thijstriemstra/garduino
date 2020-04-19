/*
  PyGarden.h - Library for monitoring a garden.
*/
#ifndef PyGarden_h
#define PyGarden_h

#include "Arduino.h"
#include <YL83_RainSensor.h>
#include <FC28_SoilSensor.h>
#include <BH1750_LightSensor.h>
#include <SingleChannel_Relay.h>
#include <HCSR04_DistanceSensor.h>
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
    BH1750_LightSensor* _light;
    SingleChannel_Relay* _water;
    HCSR04_DistanceSensor* _distance;
    DS18B20_TemperatureSensors* _temperature;
};

#endif