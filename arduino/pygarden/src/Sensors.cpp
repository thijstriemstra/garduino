#include "Sensors.h"

Sensors::Sensors(long interval, bool debug): Thread() {
  _interval = interval;
  _debug = debug;
  _lastPublish = 0;

  enabled = true;

  _rain = new YL83_RainSensor(RainSensorPin);
  _soil1 = new FC28_SoilSensor(SoilSensor1Pin);
  _soil2 = new FC28_SoilSensor(SoilSensor2Pin);
  _temperature = new DS18B20_TemperatureSensors(TemperatureSensorsPin);
  _light = new BH1750_LightSensor(LightSensorSCLPin, LightSensorSDAPin);
  _barometer = new BME280_BarometerSensor(BarometerSCLPin, BarometerSDAPin);
}

void Sensors::begin() {
  _barometer->begin();
  _rain->begin();
  _soil1->begin();
  _soil2->begin();
  _temperature->begin();
  _light->begin();
}

void Sensors::startPublish(IOT* iot) {
  _iot = iot;
  _startPublishing = true;
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

  // run the thread
  Thread::run();
}

void Sensors::publish() {
  Serial.print("Publishing sensor data every ");
  Serial.print(_interval / 1000);
  Serial.println(" seconds...");
  Serial.println();

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
  int moisture1 = soil.array[0];
  _iot->publish("/inside/soil_left", moisture1);

  int moisture2 = soil.array[1];
  _iot->publish("/inside/soil_right", moisture2);

  Serial.println();
  Serial.println("Outside");
  Serial.println("-------");
  Serial.println();

  // RAIN
  int rain = measureRain();
  _iot->publish("/outside/rain", rain);

  // TEMPERATURE
  OutsideTemperatureResult outside = readTemperature();
  
  float outsideTemp = outside.array[0];
  _iot->publish("/outside/temperature", outsideTemp);

  float waterTemp = outside.array[1];
  _iot->publish("/water/temperature", waterTemp);

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
    Serial.println("% dry");
  }
  return rainSensorValue;
}

BME280_Result Sensors::readBarometer() {
  BME280_Result result = _barometer->readAll();
  float temperature = result.array[0];
  float pressure = result.array[1];
  float humidity = result.array[2];

  if (_debug) {
    Serial.print(F("Temperature inside:\t"));
    Serial.print(temperature);
    Serial.println(" °C");

    Serial.print(F("Pressure inside:\t"));
    Serial.println(pressure);

    Serial.print("Humidity inside:\t");
    Serial.print(humidity);
    Serial.println("%");
  }
  return result;
}

OutsideTemperatureResult Sensors::readTemperature() {
  float temperature1 = _temperature->getTemperatureByIndex(0);
  float temperature2 = _temperature->getTemperatureByIndex(1);

  OutsideTemperatureResult result;
  result.array[0] = temperature1;
  result.array[1] = temperature2;

  if (_debug) {
    Serial.print("Temperature outside:\t");
    Serial.print(temperature1);
    Serial.println(" °C");
  
    Serial.print("Temperature water:\t");
    Serial.print(temperature2);
    Serial.println(" °C");
  }
  return result;
}

SoilMoistureResult Sensors::readSoilMoisture() {
  int moisture1 = _soil1->measurePercentage();
  int moisture2 = _soil2->measurePercentage();
  
  SoilMoistureResult result;
  result.array[0] = moisture1;
  result.array[1] = moisture2;

  if (_debug) {
    Serial.print("Soil-1 moisture:\t");
    Serial.print(moisture1);
    Serial.println("% dry");

    Serial.print("Soil-2 moisture:\t");
    Serial.print(moisture2);
    Serial.println("% dry");
  }
  return result;
}
