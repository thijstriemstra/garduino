/*  Copyright (c) 2020-2021, Collab
 *  All rights reserved
*/
/*
  Sensors.cpp
*/

#include <Sensors.h>

Sensors::Sensors(
  long interval_period,
  MultiPlexer_TCA9548A* i2c,
  bool debug,
  const char *ns
) {
  interval = interval_period;
  _debug = debug;

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

  // start task on core 1, otherwise it will corrupt the OLED display
  // see https://github.com/ThingPulse/esp8266-oled-ssd1306/issues/326
  xTaskCreatePinnedToCore(
    &Sensors::setupTask,      // function that should be called
    "sensorsTask",            // name of the task (for debugging)
    2048,                     // stack size (bytes)
    this,                     // passing instance pointer as function param
    5,                        // task priority
    NULL,                     // task handle
    1                         // core to run the task on (0 or 1)
  );
}

void Sensors::setupTask(void *pvParameter) {
  for (;;) {
    // obtain the instance pointer
    Sensors* sensors = reinterpret_cast<Sensors*>(pvParameter);

    // run forest, run
    sensors->run();

    // pause the task
    vTaskDelay((sensors->interval * 1000) / portTICK_PERIOD_MS);

    // previous publish didn't finish yet unfortunately but this
    // can safely be ignored
    if (!sensors->manualMode && !sensors->_iot->publishReady()) {
      // exit and shutdown (will be ignored if watering)
      sensors->_iot->exit();
    }
  }
}

void Sensors::run() {
  // publish data
  publish();

  // water flow
  _waterFlow->measure(1);
}

void Sensors::wait() {
  // delay task
  vTaskDelay(500 / portTICK_PERIOD_MS);
}

void Sensors::save() {
  // water flow
  _waterFlow->saveHistoric();
}

void Sensors::publish() {
  // no connection
  if (!_iot->connected()) {
    Log.warning(F("MQTT - No connection, cannot publish" CR));
    Log.warning(CR);

    if (!manualMode) {
      // exit and shutdown (will be ignored if watering)
      _iot->exit();
      return;
    }
  }

  Log.info(F("MQTT - Publishing sensor data..." CR));
  Log.info(CR);

  // INSIDE
  Log.info(F("Inside" CR));
  Log.info(F("------" CR));
  Log.info(CR);

  // LIGHT
  float lux = measureLight();
  _iot->publish("/inside/light", lux);

  // BME280
  BME280_Result barometer = readBarometer(_debug);
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
    Log.info(F("System:\t\t%F °C" CR), _sysTemperature);
  }

  // OUTSIDE
  Log.info(CR);
  Log.info(F("Outside" CR));
  Log.info(F("-------" CR));
  Log.info(CR);

  // RAIN
  int rain = measureRain();
  _iot->publish("/outside/rain", rain);

  // OUTSIDE TEMPERATURE
  OutsideTemperatureResult outside = readOutsideTemperature();

  float outsideTemp = outside.air;
  _iot->publish("/outside/temperature", outsideTemp);

  // WATER
  Log.info(CR);
  Log.info(F("Water" CR));
  Log.info(F("-------" CR));
  Log.info(CR);

  double totalLiters = _waterFlow->getTotalVolume();
  double historicLiters = _waterFlow->getHistoricVolume();
  float waterTemp = outside.water;
  _iot->publish("/water/temperature", waterTemp);
  _iot->publish("/water/cycle_volume", totalLiters);
  _iot->publish("/water/historic_volume", historicLiters);
  if (_debug) {
    Log.info(F("Temperature:\t%F °C" CR), waterTemp);
    Log.info(F("Current:\t\t%D ltr" CR), totalLiters);
    Log.info(F("Total:\t\t%D ltr" CR), historicLiters);
  }

  Log.info(CR);
  Log.info(F("**********************************************" CR));
  Log.info(CR);
}

float Sensors::measureLight() {
  float lux = _light->read();

  if (_debug) {
    Log.info(F("Light:\t\t%F lx" CR), lux);
  }
  return lux;
}

int Sensors::measureRain() {
  int rainSensorValue = _rain->measurePercentage();

  if (_debug) {
    Log.info(F("Rain:\t\t%D%%" CR), rainSensorValue);
  }
  return rainSensorValue;
}

BME280_Result Sensors::readBarometer(bool debug) {
  BME280_Result result = _barometer->readAll();

  if (debug) {
    Log.info(F("Humidity:\t\t%F%%" CR), result.humidity);
    Log.info(F("Temperature:\t%F °C" CR), result.temperature);
    Log.info(F("Pressure:\t\t%F hPa" CR), result.pressure);
  }
  return result;
}

OutsideTemperatureResult Sensors::readOutsideTemperature() {
  OutsideTemperatureResult result;
  result.air = _temperature->getTemperatureByIndex(0);
  result.water = _temperature->getTemperatureByIndex(1);

  if (_debug) {
    Log.info(F("Temperature:\t%F °C" CR), result.air);
  }
  return result;
}

SoilMoistureResult Sensors::readSoilMoisture() {
  SoilMoistureResult result = _soil->readAll();

  if (_debug) {
    Log.info(F("Soil-1 moisture:\t%d%%" CR), result.sensor1);
    Log.info(F("Soil-2 moisture:\t%d%%" CR), result.sensor2);
    Log.info(F("Soil-3 moisture:\t%d%%" CR), result.sensor3);
    Log.info(F("Soil-4 moisture:\t%d%%" CR), result.sensor4);
    Log.info(F("Soil-5 moisture:\t%d%%" CR), result.sensor5);
  }

  return result;
}
