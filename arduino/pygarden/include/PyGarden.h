/*
  PyGarden.h - Library for monitoring a garden.
*/
#ifndef PyGarden_h
#define PyGarden_h

#include <Arduino.h>
#include <Functor.h>

#include <IOT.h>
#include <LED.h>
#include <Button.h>
#include <Method.h>
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
    void stopRelay();
    void toggleState();

    // sensors
    void measureLight();
    void measureRain();
    void readBarometer();
    void readTemperature();
    void readSoilMoisture();

    // callbacks
    void onManualButtonPush();
    void onPowerButtonPush();

    // deepsleep
    void print_wakeup_reason();
    void setupDeepsleep();

  private:
    IOT* _iot;
    LED* _manualLED;
    LED* _powerLED;
    Button* _manualBtn;
    Button* _powerBtn;
    YL83_RainSensor* _rain;
    FC28_SoilSensor* _soil1;
    FC28_SoilSensor* _soil2;
    BH1750_LightSensor* _light;
    SingleChannel_Relay* _water;
    BME280_BarometerSensor* _barometer;
    DS18B20_TemperatureSensors* _temperature;

    bool started = false;
};

#endif