#include "Sensors.h"

Sensors::Sensors(bool debug) {
  _debug = debug;

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

void Sensors::publish(const char *base_topic, IOT* iot) {
  char subTopic[80];

  Serial.println("Publishing sensor data...");
  Serial.println();

  Serial.println("Inside");
  Serial.println("------");
  Serial.println();

  // LIGHT
  float lux = measureLight();
  sprintf(subTopic, "%s%s", base_topic, "/inside/light");
  iot->publish(subTopic, lux);

  // BAROMETER
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

  // SOIL
  SoilMoistureResult soil = readSoilMoisture();

  int moisture1 = soil.array[0];
  sprintf(subTopic, "%s%s", base_topic, "/inside/soil_left");
  iot->publish(subTopic, moisture1);

  int moisture2 = soil.array[1];
  sprintf(subTopic, "%s%s", base_topic, "/inside/soil_right");
  iot->publish(subTopic, moisture2);

  Serial.println();
  Serial.println("Outside");
  Serial.println("-------");
  Serial.println();

  // RAIN
  int rain = measureRain();
  sprintf(subTopic, "%s%s", base_topic, "/outside/rain");
  iot->publish(subTopic, rain);

  // TEMPERATURE
  OutsideTemperatureResult outside = readTemperature();
  
  float outsideTemp = outside.array[0];
  sprintf(subTopic, "%s%s", base_topic, "/outside/temperature");
  iot->publish(subTopic, outsideTemp);

  float waterTemp = outside.array[1];
  sprintf(subTopic, "%s%s", base_topic, "/water/temperature");
  iot->publish(subTopic, waterTemp);

  Serial.println();
}

float Sensors::measureLight() {
  float lux = _light->read();

  if (_debug) {
    Serial.print("Light inside:\t\t");
    Serial.print(lux);
    Serial.println(" lx");
  }
  return lux;
}

int Sensors::measureRain() {
  int rainSensorValue = _rain->measurePercentage();

  if (_debug) {
    Serial.print("Rain outside:\t\t");
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
