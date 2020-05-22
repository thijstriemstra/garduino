/*
  Sensors.h
*/
#ifndef Sensors_h
#define Sensors_h

#include <Arduino.h>
#include <Thread.h>

#include <IOT.h>
#include <WaterFlowMeter.h>
#include <YL83_RainSensor.h>
#include <FC28_SoilSensor.h>
#include <BH1750_LightSensor.h>
#include <BME280_BarometerSensor.h>
#include <DS18B20_TemperatureSensors.h>

struct OutsideTemperatureResult {
    float array[2];
};

class Sensors: public Thread {
  public:
    Sensors(long interval, bool debug = true, const char * ns = "pygarden");
    void begin();

    void run();
    void save();
    void reset();
    void publish();
    void startPublish(IOT* iot, float system_temperature);
    bool shouldRun(unsigned long time);
    float measureLight();
    int measureRain();
    BME280_Result readBarometer();
    OutsideTemperatureResult readTemperature();
    SoilMoistureResult readSoilMoisture();

  private:
    bool _debug;
    long _interval;
    long _lastPublish;
    bool _startPublishing = false;
    float _sysTemperature = 0;

    IOT* _iot;
    YL83_RainSensor* _rain;
    FC28_SoilSensor* _soil1;
    FC28_SoilSensor* _soil2;
    WaterFlowMeter* _waterFlow;
    BH1750_LightSensor* _light;
    BME280_BarometerSensor* _barometer;
    DS18B20_TemperatureSensors* _temperature;
};

#endif