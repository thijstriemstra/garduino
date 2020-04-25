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
  _barometer = new BME280_BarometerSensor(BarometerSCLPin, BarometerSDAPin);
}

void PyGarden::begin() {
  //_iot->Init();

  _barometer->begin();
  _rain->begin();
  _soil1->begin();
  _soil2->begin();
  _temperature->begin();
  _light->begin();
  //_water->begin();
}

void PyGarden::loop() {
  //_iot->Run();

  // barometer
  readBarometer();

  // rain
  measureRain();

  // soil
  readSoilMoisture();

  // light
  measureLight();

  // temperature
  readTemperature();

  // water
  //startRelay();

  Serial.println("-----------------------");

  delay(4000);
}

void PyGarden::measureLight() {
  float lux = _light->read();
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");
}

void PyGarden::measureRain() {
  int rainSensorValue = _rain->measurePercentage();
  Serial.print("Rain sensor: ");
  Serial.print(rainSensorValue);
  Serial.println(" % dry");
  //_iot->publish("rain_0", "4095");
}

void PyGarden::readBarometer() {
  float temperature = _barometer->getTemperature();
  Serial.print(F("Temperature: "));
  Serial.print(temperature);
  Serial.println(" ºC");

  float pressure = _barometer->getPressure();
  Serial.print(F("Pressure: "));
  Serial.print(pressure);
  Serial.println(" hPa");

  float humidity = _barometer->getHumidity();
  Serial.print("Humidity = ");
  Serial.print(humidity);
  Serial.println(" %");

  delay(1000);
}

void PyGarden::readTemperature() {
  float temperature1 = _temperature->getTemperatureByIndex(0);
  Serial.print("Temperature 1: ");
  Serial.print(temperature1);
  Serial.println("ºC");
  float temperature2 = _temperature->getTemperatureByIndex(1);
  Serial.print("Temperature 2: ");
  Serial.print(temperature2);
  Serial.println("ºC");
}

void PyGarden::readSoilMoisture() {
  int moisture1 = _soil1->measurePercentage();
  Serial.print("Soil-1 moisture: ");
  Serial.print(moisture1);
  Serial.println(" % dry");
  int moisture2 = _soil2->measurePercentage();
  Serial.print("Soil-2 moisture: ");
  Serial.print(moisture2);
  Serial.println(" % dry");
}

void PyGarden::startRelay() {
  _water->start();
  Serial.println("Water: flowing");
  delay(5000);

  _water->stop();
  Serial.println("Water: idle");
  delay(5000);
}
