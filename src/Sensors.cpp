/*  Copyright (c) 2020-2021, Collab
 *  All rights reserved
*/
/*
  Sensors.cpp
*/

#include <Sensors.h>

Sensors::Sensors(
  long interval,
  MultiPlexer_TCA9548A* i2c,
  bool debug,
  const char *ns
) {
  _interval = interval;
  _debug = debug;
  _lastPublish = 0;

  //enabled = false;

  SoilSensorsConfig soilCfg;
  soilCfg.sensor1_channel = SoilSensor1Channel;
  soilCfg.sensor1_wet = SoilSensor1Wet;
  soilCfg.sensor1_dry = SoilSensor1Dry;
  soilCfg.sensor2_channel = SoilSensor2Channel;
  soilCfg.sensor2_wet = SoilSensor2Wet;
  soilCfg.sensor2_dry = SoilSensor2Dry;
  soilCfg.sensor3_channel = SoilSensor3Channel;
  soilCfg.sensor3_wet = SoilSensor3Wet;
  soilCfg.sensor3_dry = SoilSensor3Dry;
  soilCfg.sensor4_channel = SoilSensor4Channel;
  soilCfg.sensor4_wet = SoilSensor4Wet;
  soilCfg.sensor4_dry = SoilSensor4Dry;
  soilCfg.sensor5_channel = SoilSensor5Channel;
  soilCfg.sensor5_wet = SoilSensor5Wet;
  soilCfg.sensor5_dry = SoilSensor5Dry;

  _adc = new MultiPlexer_MCP3008(AnalogExpanderCSPin);
  _soil = new SoilSensors(soilCfg, _adc);
  _rain = new YL83_RainSensor_MCP3008(_adc, RainSensorChannel);
  _waterFlow = new WaterFlowMeter(WaterFlowMeterPin, ns);
  _temperature = new DS18B20_TemperatureSensors(TemperatureSensorsPin);
  _light = new BH1750_LightSensor_Mux(i2c, LightSensorChannel, LightSensorAddress);
  _barometer = new BME280_BarometerSensor_Mux(i2c, BarometerChannel);
}

void Sensors::begin() {
  _adc->begin();
  _rain->begin();
  _soil->begin();
  _temperature->begin();
  _barometer->begin();
  _light->begin();
  _waterFlow->begin();
}

void Sensors::reset() {
  // water flow
  _waterFlow->reset();
}

void Sensors::startPublish(IOT* iot, float system_temperature) {
  _iot = iot;
  _sysTemperature = system_temperature;
  _startPublishing = true;
  //enabled = true;
}

bool Sensors::shouldRun(unsigned long time) {
  if (_startPublishing) {

    _startPublishing = false;

    // save the current time
    _lastPublish = (time ? time : millis());

    // continuously publish data
    publish();
  }

  // let default method check for it
  //return Thread::shouldRun(time);
  return false;
}

void Sensors::run() {
  // check if time elapsed since last publish
  if (millis() > _lastPublish + _interval){
    // exceeded time, disable it
    _startPublishing = true;
  }

  // water flow
  _waterFlow->measure(1);

  // run the thread
  //Thread::run();
}

void Sensors::save() {
  // water flow
  _waterFlow->saveHistoric();
}

void Sensors::publish() {
  Serial.println(F("MQTT - Publishing sensor data..."));
  Serial.println();

  // INSIDE
  Serial.println(F("Inside"));
  Serial.println(F("------"));
  Serial.println();

  // LIGHT
  float lux = measureLight();
  _iot->publish("/inside/light", lux);

  // BME280
  BME280_Result barometer = readBarometer();
  _iot->publish("/inside/temperature", barometer.temperature);
  _iot->publish("/inside/pressure", barometer.pressure);
  _iot->publish("/inside/humidity", barometer.humidity);

  // SOIL
  SoilMoistureResult soil = readSoilMoisture();
  _iot->publish("/inside/soil_1", soil.sensor1);
  _iot->publish("/inside/soil_2", soil.sensor2);
  _iot->publish("/inside/soil_3", soil.sensor3);
  _iot->publish("/inside/soil_4", soil.sensor4);
  _iot->publish("/inside/soil_5", soil.sensor5);

  // SYSTEM TEMPERATURE
  _iot->publish("/system/temperature", _sysTemperature);
  if (_debug) {
    Serial.print(F("System:\t\t\t"));
    Serial.print(_sysTemperature);
    Serial.println(F(" °C"));
  }

  // OUTSIDE
  Serial.println();
  Serial.println(F("Outside"));
  Serial.println(F("-------"));
  Serial.println();

  // RAIN
  int rain = measureRain();
  _iot->publish("/outside/rain", rain);

  // OUTSIDE TEMPERATURE
  OutsideTemperatureResult outside = readOutsideTemperature();

  float outsideTemp = outside.array[0];
  _iot->publish("/outside/temperature", outsideTemp);

  // WATER
  Serial.println();
  Serial.println(F("Water"));
  Serial.println(F("-------"));
  Serial.println();

  // WATER
  double totalLiters = _waterFlow->getTotalVolume();
  double historicLiters = _waterFlow->getHistoricVolume();
  float waterTemp = outside.array[1];
  _iot->publish("/water/temperature", waterTemp);
  _iot->publish("/water/cycle_volume", totalLiters);
  _iot->publish("/water/historic_volume", historicLiters);
  if (_debug) {
    Serial.print(F("Temperature:\t\t"));
    Serial.print(waterTemp);
    Serial.println(F(" °C"));

    Serial.print(F("Current:\t\t"));
    Serial.print(totalLiters);
    Serial.println(F(" ltr"));

    Serial.print(F("Total:\t\t\t"));
    Serial.print(historicLiters);
    Serial.println(F(" ltr"));
  }

  Serial.println();
  Serial.println(F("**********************************************"));
  Serial.println();
}

float Sensors::measureLight() {
  float lux = _light->read();

  if (_debug) {
    Serial.print(F("Light:\t\t\t"));
    Serial.print(lux);
    Serial.println(F(" lx"));
  }
  return lux;
}

int Sensors::measureRain() {
  int rainSensorValue = _rain->measurePercentage();

  if (_debug) {
    Serial.print(F("Rain:\t\t\t"));
    Serial.print(rainSensorValue);
    Serial.println(F("%"));
  }
  return rainSensorValue;
}

BME280_Result Sensors::readBarometer() {
  BME280_Result result = _barometer->readAll();

  if (_debug) {
    Serial.print(F("Humidity:\t\t"));
    Serial.print(result.humidity);
    Serial.println(F("%"));

    Serial.print(F("Temperature:\t\t"));
    Serial.print(result.temperature);
    Serial.println(F(" °C"));

    Serial.print(F("Pressure:\t\t"));
    Serial.print(result.pressure);
    Serial.println(F(" hPa"));
  }
  return result;
}

OutsideTemperatureResult Sensors::readOutsideTemperature() {
  float temperature1 = _temperature->getTemperatureByIndex(0);
  float temperature2 = _temperature->getTemperatureByIndex(1);

  OutsideTemperatureResult result;
  result.array[0] = temperature1;
  result.array[1] = temperature2;

  if (_debug) {
    Serial.print(F("Temperature:\t\t"));
    Serial.print(temperature1);
    Serial.println(F(" °C"));
  }
  return result;
}

SoilMoistureResult Sensors::readSoilMoisture() {
  SoilMoistureResult result = _soil->readAll();

  if (_debug) {
    Serial.print(F("Soil-1 moisture:\t"));
    Serial.print(result.sensor1);
    Serial.println(F("%"));

    Serial.print(F("Soil-2 moisture:\t"));
    Serial.print(result.sensor2);
    Serial.println(F("%"));

    Serial.print(F("Soil-3 moisture:\t"));
    Serial.print(result.sensor3);
    Serial.println(F("%"));

    Serial.print(F("Soil-4 moisture:\t"));
    Serial.print(result.sensor4);
    Serial.println(F("%"));

    Serial.print(F("Soil-5 moisture:\t"));
    Serial.print(result.sensor5);
    Serial.println(F("%"));
  }

  return result;
}
