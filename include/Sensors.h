/*  Copyright (c) 2020-2021, Collab
 *  All rights reserved
*/
/*
  Sensors.h
*/
#ifndef Sensors_h
#define Sensors_h

#include <Arduino.h>

#include <IOT.h>
#include <SoilSensors.h>
#include <WaterFlowMeter.h>
#include <MultiPlexer_MCP3008.h>
#include <MultiPlexer_TCA9548A.h>
#include <BH1750_LightSensor_Mux.h>
#include <YL83_RainSensor_MCP3008.h>
#include <BME280_BarometerSensor_Mux.h>
#include <DS18B20_TemperatureSensors.h>

struct OutsideTemperatureResult {
    float air;
    float water;
};

class Sensors {
  public:
    Sensors(long interval, MultiPlexer_TCA9548A* i2c, bool debug = true, const char *ns = "garduino");
    void begin();
    void run();
    void save();
    void reset();
    void publish();
    void startPublish(IOT* iot, float system_temperature);
    float measureLight();
    int measureRain();
    BME280_Result readBarometer();
    OutsideTemperatureResult readOutsideTemperature();
    SoilMoistureResult readSoilMoisture();

  private:
    bool _debug;
    long _interval;
    float _sysTemperature = 0;

    static void setupTask(void *pvParameter);

    IOT* _iot;
    SoilSensors* _soil;
    MultiPlexer_MCP3008* _adc;
    WaterFlowMeter* _waterFlow;
    BH1750_LightSensor_Mux* _light;
    YL83_RainSensor_MCP3008* _rain;
    BME280_BarometerSensor_Mux* _barometer;
    DS18B20_TemperatureSensors* _temperature;
};

#endif