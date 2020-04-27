/*
  PyGarden.cpp - Library for monitoring a garden.
*/

#include <PyGarden.h>

PyGarden::PyGarden() {
  // controls
  _manualBtn = new Button(ManualRunButtonPin, INPUT);
  _manualLED = new LED(ManualRunLEDPin);
  _resetBtn = new Button(WIFI_AP_PIN, INPUT);
  _wifiLED = new LED(WIFI_STATUS_PIN);

  // sensors
  _rain = new YL83_RainSensor(RainSensorPin);
  _soil1 = new FC28_SoilSensor(SoilSensor1Pin, SoilSensor1Dry, SoilSensor1Wet);
  _soil2 = new FC28_SoilSensor(SoilSensor2Pin, SoilSensor2Dry, SoilSensor2Wet);
  _temperature = new DS18B20_TemperatureSensors(TemperatureSensorsPin);
  _light = new BH1750_LightSensor(LightSensorSCLPin, LightSensorSDAPin);
  _water = new SingleChannel_Relay(WaterValvePin, false);
  _barometer = new BME280_BarometerSensor(BarometerSCLPin, BarometerSDAPin);

  // wifi + mqtt
  _iot = new IOT();
}

void PyGarden::begin() {
  // callbacks
  Method manualBtnCallback;
  manualBtnCallback.attachCallback(
    makeFunctor((Functor0 *)0, *this, &PyGarden::onManualButtonPush));
  Method resetBtnCallback;
  resetBtnCallback.attachCallback(
    makeFunctor((Functor0 *)0, *this, &PyGarden::onResetButtonPush));

  // controls
  _manualBtn->begin(manualBtnCallback);
  _resetBtn->begin(resetBtnCallback);
  _wifiLED->begin();
  _manualLED->begin();

  // sensors
  _barometer->begin();
  _rain->begin();
  _soil1->begin();
  _soil2->begin();
  _temperature->begin();
  _light->begin();
  _water->begin();

  // wifi and mqtt
  //_iot->begin();
}

void PyGarden::loop() {
  // controls
  _manualBtn->loop();
  _resetBtn->loop();
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

  _manualLED->enable();

  Serial.println("Water: flowing");
}

void PyGarden::stopRelay() {
  _water->stop();

  _manualLED->disable();

  Serial.println("Water: idle");
}

void PyGarden::onManualButtonPush() {
  if (started == false) {
    started = true;

    // water
    startRelay();

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
  } else {
    started = false;

    // water
    stopRelay();
  }
  Serial.println("-----------------------");
}

void PyGarden::onResetButtonPush() {
  Serial.println("Reset button pushed.");
}
