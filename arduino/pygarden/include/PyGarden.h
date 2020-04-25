/*
  PyGarden.h - Library for monitoring a garden.
*/
#ifndef PyGarden_h
#define PyGarden_h

#include "Arduino.h"
#include "IOT.h"
#include <YL83_RainSensor.h>
#include <FC28_SoilSensor.h>
#include <BH1750_LightSensor.h>
#include <SingleChannel_Relay.h>
#include <BME280_BarometerSensor.h>
#include <DS18B20_TemperatureSensors.h>

class PyGarden
{
  public:
    PyGarden();
    void begin();
    void loop();
    void startRelay();
    void measureLight();
    void measureRain();
    void readBarometer();

  private:
    IOT* _iot;
    YL83_RainSensor* _rain;
    FC28_SoilSensor* _soil1;
    FC28_SoilSensor* _soil2;
    BH1750_LightSensor* _light;
    SingleChannel_Relay* _water;
    BME280_BarometerSensor* _barometer;
    DS18B20_TemperatureSensors* _temperature;
};

#endif