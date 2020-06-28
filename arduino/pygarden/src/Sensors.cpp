/*
  Sensors.cpp
*/

#include "Sensors.h"

Sensors::Sensors(long interval, bool debug, const char * ns): Thread() {
  _interval = interval;
  _debug = debug;
  _lastPublish = 0;

  enabled = false;

  _mux = new MultiPlexer_74HC4067(
    MultiPlexerSignalPin,
    MultiPlexerS0Pin,
    MultiPlexerS1Pin,
    MultiPlexerS2Pin,
    MultiPlexerS3Pin
  );

  _soil = new SoilSensors(
    SoilSensor1Pin,
    SoilSensor2Pin,
    SoilSensor3Pin,
    SoilSensor4Pin,
    SoilSensor5Pin,
    SoilSensor6Pin,
    SoilSensor7Pin,
    SoilSensor8Pin
  );
  _rain = new YL83_RainSensor(RainSensorPin);
  _temperature = new DS18B20_TemperatureSensors(TemperatureSensorsPin);
  _light = new BH1750_LightSensor(LightSensorSCLPin, LightSensorSDAPin);
  _barometer = new BME280_BarometerSensor(BarometerSCLPin, BarometerSDAPin);
  _waterFlow = new WaterFlowMeter(WaterFlowMeterPin, ns);
}

void Sensors::begin() {
  _barometer->begin();
  _rain->begin();
  _soil->begin();
  _temperature->begin();
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
  enabled = true;
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
  return Thread::shouldRun(time);
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
  Thread::run();
}

void Sensors::save() {
  // water flow
  _waterFlow->saveHistoric();
}

void Sensors::publish() {
  Serial.println("MQTT - Publishing sensor data...");
  Serial.println();

  // INSIDE
  Serial.println("Inside");
  Serial.println("------");
  Serial.println();

  // LIGHT
  float lux = measureLight();
  _iot->publish("/inside/light", lux);

  // BME280
  BME280_Result barometer = readBarometer();
  float temperature_inside = barometer.array[0];
  _iot->publish("/inside/temperature", temperature_inside);

  float pressure_inside = barometer.array[1];
  _iot->publish("/inside/pressure", pressure_inside);

  float humidity_inside = barometer.array[2];
  _iot->publish("/inside/humidity", humidity_inside);

  // SOIL
  SoilMoistureResult soil = readSoilMoisture();
  _iot->publish("/inside/soil_1", soil.array[0]);
  _iot->publish("/inside/soil_2", soil.array[1]);
  _iot->publish("/inside/soil_3", soil.array[2]);
  _iot->publish("/inside/soil_4", soil.array[3]);
  _iot->publish("/inside/soil_5", soil.array[4]);
  _iot->publish("/inside/soil_6", soil.array[5]);
  _iot->publish("/inside/soil_7", soil.array[6]);
  _iot->publish("/inside/soil_8", soil.array[7]);

  // SYSTEM TEMPERATURE
  _iot->publish("/system/temperature", _sysTemperature);
  if (_debug) {
    Serial.print("System:\t\t\t");
    Serial.print(_sysTemperature);
    Serial.println(" °C");
  }

  // OUTSIDE
  Serial.println();
  Serial.println("Outside");
  Serial.println("-------");
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
  Serial.println("Water");
  Serial.println("-------");
  Serial.println();

  // WATER
  double totalLiters = _waterFlow->getTotalVolume();
  double historicLiters = _waterFlow->getHistoricVolume();
  float waterTemp = outside.array[1];
  _iot->publish("/water/temperature", waterTemp);
  _iot->publish("/water/cycle_volume", totalLiters);
  _iot->publish("/water/historic_volume", historicLiters);
  if (_debug) {
    Serial.print("Temperature:\t\t");
    Serial.print(waterTemp);
    Serial.println(" °C");

    Serial.print("Current:\t\t");
    Serial.print(totalLiters);
    Serial.println(" ltr");

    Serial.print("Total:\t\t\t");
    Serial.print(historicLiters);
    Serial.println(" ltr");
  }

  Serial.println();
  Serial.println("**********************************************");
  Serial.println();
}

float Sensors::measureLight() {
  float lux = _light->read();

  if (_debug) {
    Serial.print("Light:\t\t\t");
    Serial.print(lux);
    Serial.println(" lx");
  }
  return lux;
}

int Sensors::measureRain() {
  int rainSensorValue = _rain->measurePercentage();

  if (_debug) {
    Serial.print("Rain:\t\t\t");
    Serial.print(rainSensorValue);
    Serial.println("%");
  }
  return rainSensorValue;
}

BME280_Result Sensors::readBarometer() {
  BME280_Result result = _barometer->readAll();
  float temperature = result.array[0];
  float pressure = result.array[1];
  float humidity = result.array[2];

  if (_debug) {
    Serial.print("Humidity:\t\t");
    Serial.print(humidity);
    Serial.println("%");

    Serial.print(F("Temperature:\t\t"));
    Serial.print(temperature);
    Serial.println(" °C");

    Serial.print(F("Pressure:\t\t"));
    Serial.print(pressure);
    Serial.println(" hPa");
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
    Serial.print("Temperature:\t\t");
    Serial.print(temperature1);
    Serial.println(" °C");
  }
  return result;
}

SoilMoistureResult Sensors::readSoilMoisture() {
  SoilMoistureResult result = _soil->readAll(_mux);
  int sensorCount = sizeof(result.array) / sizeof(int);

  if (_debug) {
    for (int index = 0; index < sensorCount; index++) {
      Serial.print("Soil-");
      Serial.print(index);
      Serial.print(" moisture:\t");
      Serial.print(result.array[index]);
      Serial.println("%");
    }
  }

  return result;
}
