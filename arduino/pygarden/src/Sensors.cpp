#include "Sensors.h"

Sensors::Sensors(bool debug) {
  _debug = debug;

  _rain = new YL83_RainSensor(RainSensorPin);
  _temperature = new DS18B20_TemperatureSensors(TemperatureSensorsPin);
  _light = new BH1750_LightSensor(LightSensorSCLPin, LightSensorSDAPin);
  _barometer = new BME280_BarometerSensor(BarometerSCLPin, BarometerSDAPin);
  _soil1 = new FC28_SoilSensor(SoilSensor1Pin);
  _soil2 = new FC28_SoilSensor(SoilSensor2Pin);
}

void Sensors::begin() {
  _barometer->begin();
  _rain->begin();
  _soil1->begin();
  _soil2->begin();
  _temperature->begin();
  _light->begin();
}

void Sensors::publish(const char *base_topic, IOT* iot) {
  char subTopic[80];

  // light
  float lux = measureLight();
  sprintf(subTopic, "%s%s", base_topic, "/inside/light");
  iot->publish(subTopic, lux);

  // barometer
  BME280_Result barometer = readBarometer();

  float temperature = barometer.array[0];
  sprintf(subTopic, "%s%s", base_topic, "/inside/temperature");
  iot->publish(subTopic, temperature);

  float pressure = barometer.array[1];
  sprintf(subTopic, "%s%s", base_topic, "/inside/pressure");
  iot->publish(subTopic, pressure);

  float humidity = barometer.array[2];
  sprintf(subTopic, "%s%s", base_topic, "/inside/humidity");
  iot->publish(subTopic, humidity);

  // rain
  int rain = measureRain();
  sprintf(subTopic, "%s%s", base_topic, "/outside/rain");
  iot->publish(subTopic, rain);

  // soil
  SoilMoistureResult soil = readSoilMoisture();

  int moisture1 = soil.array[0];
  sprintf(subTopic, "%s%s", base_topic, "/inside/soil_left");
  iot->publish(subTopic, moisture1);

  int moisture2 = soil.array[1];
  sprintf(subTopic, "%s%s", base_topic, "/inside/soil_right");
  iot->publish(subTopic, moisture2);

  // temperature
  OutsideTemperatureResult outside = readTemperature();
  
  float outsideTemp = outside.array[0];
  sprintf(subTopic, "%s%s", base_topic, "/outside/temperature");
  iot->publish(subTopic, outsideTemp);

  float waterTemp = outside.array[1];
  sprintf(subTopic, "%s%s", base_topic, "/water/temperature");
  iot->publish(subTopic, waterTemp);
}

float Sensors::measureLight() {
  float lux = _light->read();

  if (_debug) {
    Serial.print("Light: ");
    Serial.print(lux);
    Serial.println(" lx");
  }
  return lux;
}

int Sensors::measureRain() {
  int rainSensorValue = _rain->measurePercentage();

  if (_debug) {
    Serial.print("Rain sensor: ");
    Serial.print(rainSensorValue);
    Serial.println(" % dry");
  }
  return rainSensorValue;
}

BME280_Result Sensors::readBarometer() {
  BME280_Result result = _barometer->readAll();
  float temperature = result.array[0];
  float pressure = result.array[1];
  float humidity = result.array[2];

  if (_debug) {
    Serial.print(F("Temperature: "));
    Serial.print(temperature);
    Serial.println(" ºC");

    Serial.print(F("Pressure: "));
    Serial.print(pressure);

    Serial.print("Humidity = ");
    Serial.print(humidity);
    Serial.println(" %");
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
    Serial.print("Temperature 1: ");
    Serial.print(temperature1);
    Serial.println("ºC");
  
    Serial.print("Temperature 2: ");
    Serial.print(temperature2);
    Serial.println("ºC");
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
    Serial.print("Soil-1 moisture: ");
    Serial.print(moisture1);
    Serial.println(" % dry");

    Serial.print("Soil-2 moisture: ");
    Serial.print(moisture2);
    Serial.println(" % dry");
  }
  return result;
}
