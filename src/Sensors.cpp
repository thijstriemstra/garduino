/*  Copyright (c) 2020-2023, Collab
 *  All rights reserved
*/
/*
  Sensors.cpp
*/

#include <Sensors.h>

Sensors::Sensors(
  long interval_period,
  MultiPlexer_TCA9548A* i2c,
  TwoWire* wire,
  bool debug,
  const char *ns
) {
  interval = interval_period;
  _wire = wire;
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
  soilCfg.sensor6_channel = SoilSensor6Channel;
  soilCfg.sensor6_wet = SoilSensor6Wet;
  soilCfg.sensor6_dry = SoilSensor6Dry;
  soilCfg.sensor7_channel = SoilSensor7Channel;
  soilCfg.sensor7_wet = SoilSensor7Wet;
  soilCfg.sensor7_dry = SoilSensor7Dry;
  soilCfg.sensor8_channel = SoilSensor8Channel;
  soilCfg.sensor8_wet = SoilSensor8Wet;
  soilCfg.sensor8_dry = SoilSensor8Dry;

  _adc = new MultiPlexer_MCP3008(AnalogExpanderCSPin);
  _soil = new SoilSensors(soilCfg, _adc);
  _temperature = new DS18B20_TemperatureSensors(TemperatureSensorsPin);
  _light = new TSL2591_LightSensor_Mux(i2c, _wire, LightSensorChannel, LightSensorAddress);
  _barometer = new BME280_BarometerSensor_Mux(i2c, _wire, BarometerChannel);
}

void Sensors::begin() {
  _adc->begin();
  _soil->begin();
  _temperature->begin();

  _light->begin();
  if (!_light->working) {
    Log.warning(F("Error initialising TSL2591 sensor!" CR));
  }

  _barometer->begin();
  if (!_barometer->working) {
    Log.warning(F("Error initialising BME280 sensor!" CR));
  }
}

void Sensors::reset() {
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
}

void Sensors::wait() {
  // delay task
  vTaskDelay(500 / portTICK_PERIOD_MS);
}

void Sensors::save() {
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
  float lux = measureLight(_debug);
  _iot->publish("/inside/light", lux);

  // BME280
  BME280_Result barometer = readBarometer(_debug);
  _iot->publish("/inside/temperature", barometer.temperature);
  _iot->publish("/inside/pressure", barometer.pressure);
  _iot->publish("/inside/humidity", barometer.humidity);

  // SOIL
  SoilMoistureResult soil = readSoilMoisture(_debug, true);
  _iot->publish("/inside/soil_1", soil.sensor1);
  _iot->publish("/inside/soil_2", soil.sensor2);
  _iot->publish("/inside/soil_3", soil.sensor3);
  _iot->publish("/inside/soil_4", soil.sensor4);
  _iot->publish("/inside/soil_5", soil.sensor5);
  _iot->publish("/inside/soil_6", soil.sensor6);
  _iot->publish("/inside/soil_7", soil.sensor7);
  _iot->publish("/inside/soil_8", soil.sensor8);

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

  // OUTSIDE TEMPERATURE
  OutsideTemperatureResult outside = readOutsideTemperature();

  float outsideTemp = outside.air;
  _iot->publish("/outside/temperature", outsideTemp);

  // WATER
  Log.info(CR);
  Log.info(F("Water" CR));
  Log.info(F("-----" CR));
  Log.info(CR);

  float waterTemp = outside.water;
  _iot->publish("/water/temperature", waterTemp);
  if (_debug) {
    Log.info(F("Temperature:\t%F °C" CR), waterTemp);
  }

  // WIFI
  Log.info(CR);
  Log.info(F("WIFI" CR));
  Log.info(F("----" CR));
  Log.info(CR);

  int8_t signalStrength = _iot->getSignalStrength();
  _iot->publish("/wifi/rssi", signalStrength);
  if (_debug) {
    Log.info(F("RSSI:\t\t%d dBm" CR), signalStrength);
  }

  Log.info(CR);
  Log.info(F("**********************************************" CR));
  Log.info(CR);
}

float Sensors::measureLight(bool debug) {
  float lux = _light->read();

  if (debug) {
    Log.info(F("Light:\t\t%F lx" CR), lux);
  }
  return lux;
}

BME280_Result Sensors::readBarometer(bool debug) {
  BME280_Result result;
  if (_barometer->working) {
    result = _barometer->readAll();
  } else {
    // bme280 not working, use fake values
    result.altitude = 0;
    result.humidity = 0;
    result.pressure = 0;
    result.temperature = 0;
  }

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

SoilMoistureResult Sensors::readSoilMoisture(bool debug, bool percentage) {
  SoilMoistureResult result;
  if (!percentage) {
    result = _soil->readAll();
  } else {
    result = _soil->readAllPercentage();
  }

  if (debug) {
    if (percentage) {
      Log.info(F("Soil-1 wet:\t%d%%" CR), result.sensor1);
      Log.info(F("Soil-2 wet:\t%d%%" CR), result.sensor2);
      Log.info(F("Soil-3 wet:\t%d%%" CR), result.sensor3);
      Log.info(F("Soil-4 wet:\t%d%%" CR), result.sensor4);
      Log.info(F("Soil-5 wet:\t%d%%" CR), result.sensor5);
      Log.info(F("Soil-6 wet:\t%d%%" CR), result.sensor6);
      Log.info(F("Soil-7 wet:\t%d%%" CR), result.sensor7);
      Log.info(F("Soil-8 wet:\t%d%%" CR), result.sensor8);
    } else {
      Log.info(F("Soil-1 wet:\t%d" CR), result.sensor1);
      Log.info(F("Soil-2 wet:\t%d" CR), result.sensor2);
      Log.info(F("Soil-3 wet:\t%d" CR), result.sensor3);
      Log.info(F("Soil-4 wet:\t%d" CR), result.sensor4);
      Log.info(F("Soil-5 wet:\t%d" CR), result.sensor5);
      Log.info(F("Soil-6 wet:\t%d" CR), result.sensor6);
      Log.info(F("Soil-7 wet:\t%d" CR), result.sensor7);
      Log.info(F("Soil-8 wet:\t%d" CR), result.sensor8);
    }
  }

  return result;
}
