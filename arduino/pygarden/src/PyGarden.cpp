/*
  PyGarden.cpp - Library for monitoring a garden.
*/

#include "Arduino.h"
#include "PyGarden.h"

PyGarden::PyGarden() {
  _iot = new IOT();

  // sensors
  _rain = new YL83_RainSensor(RainSensorPin);
  _soil1 = new FC28_SoilSensor(SoilSensor1Pin, SoilSensor1Dry, SoilSensor1Wet);
  _soil2 = new FC28_SoilSensor(SoilSensor2Pin, SoilSensor2Dry, SoilSensor2Wet);
  _temperature = new DS18B20_TemperatureSensors(TemperatureSensorsPin);
  _light = new BH1750_LightSensor(LightSensorSCLPin, LightSensorSDAPin);
  _water = new SingleChannel_Relay(WaterValvePin);
}

void PyGarden::begin() {
  //_iot->Init();

  // test led
  pinMode(4, OUTPUT);

  _rain->begin();
  _soil1->begin();
  _soil2->begin();
  _temperature->begin();
  _light->begin();
  _water->begin();
}

void PyGarden::loop() {
  //_iot->Run();

  // rain
  measureRain();

  // soil
  int moisture1 = _soil1->measure();
  Serial.print("Soil-1 moisture: ");
  Serial.println(moisture1);
  int moisture2 = _soil2->measure();
  Serial.print("Soil-2 moisture: ");
  Serial.println(moisture2);

  // light
  measureLight();

  // temperature
  float temperature1 = _temperature->getTemperatureByIndex(0);
  Serial.print("Temperature 1: ");
  Serial.print(temperature1);
  Serial.println("ºC");
  float temperature2 = _temperature->getTemperatureByIndex(1);
  Serial.print("Temperature 2: ");
  Serial.print(temperature2);
  Serial.println("ºC");

  // water
  startRelay();

  Serial.println("-----------------------");
}

void PyGarden::measureLight() {
  float lux = _light->read();
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");
}

void PyGarden::measureRain() {
  int rainSensorValue = _rain->measure();
  Serial.print("Rain sensor value: ");
  Serial.println(rainSensorValue);
  //_iot->publish("rain_0", "4095");
}

void PyGarden::startRelay() {
  _water->start();
  Serial.println("Water: flowing");
  delay(5000);

  _water->stop();
  Serial.println("Water: idle");
  delay(5000);
}
