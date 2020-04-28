/*
  PyGarden.cpp - Library for monitoring a garden.
*/

#include <PyGarden.h>

PyGarden::PyGarden() {
  // controls
  _manualBtn = new Button(ManualRunButtonPin);
  _manualLED = new LED(ManualRunLEDPin);
  _networkLED = new LED(NetworkLEDPin);
  _powerBtn = new Button(PowerButtonPin);
  _powerLED = new LED(PowerLEDPin);

  // sensors
  _rain = new YL83_RainSensor(RainSensorPin);
  _soil1 = new FC28_SoilSensor(SoilSensor1Pin, SoilSensor1Dry, SoilSensor1Wet);
  _soil2 = new FC28_SoilSensor(SoilSensor2Pin, SoilSensor2Dry, SoilSensor2Wet);
  _temperature = new DS18B20_TemperatureSensors(TemperatureSensorsPin);
  _light = new BH1750_LightSensor(LightSensorSCLPin, LightSensorSDAPin);
  _water = new SingleChannel_Relay(WaterValvePin, false);
  _barometer = new BME280_BarometerSensor(BarometerSCLPin, BarometerSDAPin);

  // wifi/mqtt
  _iot = new IOT();
}

void PyGarden::begin() {
  // callbacks
  Method manualBtnCallback;
  manualBtnCallback.attachCallback(
    makeFunctor((Functor0 *)0, *this, &PyGarden::onManualButtonPush));
  Method powerBtnCallback;
  powerBtnCallback.attachCallback(
    makeFunctor((Functor0 *)0, *this, &PyGarden::onPowerButtonPush));
  Method connectionReadyCallback;
  connectionReadyCallback.attachCallback(
    makeFunctor((Functor0 *)0, *this, &PyGarden::onConnectionReady));

  // controls
  _manualBtn->begin(manualBtnCallback);
  _manualLED->begin();
  _powerBtn->begin(powerBtnCallback);
  _powerLED->begin();
  _networkLED->begin();

  // sensors
  _barometer->begin();
  _rain->begin();
  _soil1->begin();
  _soil2->begin();
  _temperature->begin();
  _light->begin();
  _water->begin();

  // deepsleep
  setupDeepsleep();

  // wifi/mqtt
  _networkLED->blink();
  _iot->begin(connectionReadyCallback);
}

void PyGarden::loop() {
  // controls
  _manualBtn->loop();
  _manualLED->loop();
  _powerBtn->loop();
  _powerLED->loop();
  _networkLED->loop();
}

void PyGarden::measureLight() {
  float lux = _light->read();
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");

  // publish
  _iot->publish("emon/kleine_kas/inside/light", lux);
}

void PyGarden::measureRain() {
  int rainSensorValue = _rain->measurePercentage();
  Serial.print("Rain sensor: ");
  Serial.print(rainSensorValue);
  Serial.println(" % dry");

  // publish
  _iot->publish("emon/kleine_kas/outside/rain", rainSensorValue);
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

  // publish
  _iot->publish("emon/kleine_kas/inside/temperature", temperature);
  _iot->publish("emon/kleine_kas/inside/pressure", pressure);
  _iot->publish("emon/kleine_kas/inside/humidity", humidity);
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

  // publish
  _iot->publish("emon/kleine_kas/outside/temperature", temperature1);
  _iot->publish("emon/kleine_kas/water/temperature", temperature2);
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

  // publish
  _iot->publish("emon/kleine_kas/soil/left", moisture1);
  _iot->publish("emon/kleine_kas/soil/right", moisture2);
}

void PyGarden::startRelay() {
  _water->start();

  _manualLED->enable();

  Serial.println("-----------------------");
  Serial.println("Water: valve open");
}

void PyGarden::stopRelay() {
  _water->stop();

  _manualLED->disable();

  Serial.println("Water: valve closed");
}

void PyGarden::onManualButtonPush() {
  toggleState();
}

void PyGarden::toggleState() {
  if (started == false) {
    started = true;

    // water
    startRelay();
  } else {
    started = false;

    // water
    stopRelay();
  }
  Serial.println("-----------------------");
}

void PyGarden::onConnectionReady() {
  // network status LED
  // stop blinking
  _networkLED->blink();
  _networkLED->enable();

  // time
  Serial.println("Syncing time...");
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  // log sensor data
  Serial.println("Publishing sensor data...");

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
}

void PyGarden::onPowerButtonPush() {
  Serial.println("******************************");
  Serial.println("Going to sleep... Bye.");
  Serial.println("******************************");

  // disable power led
  _powerLED->disable();

  // put esp32 to sleep
  esp_deep_sleep_start();
}

void PyGarden::setupDeepsleep() {
  // Print the wakeup reason for ESP32
  print_wakeup_reason();

  // configure the wake up source
  // 1 = High, 0 = Low
  esp_sleep_enable_ext0_wakeup((gpio_num_t) ManualRunButtonPin, 1);
}

/*
Method to print the reason by which ESP32
has been awaken from sleep
*/
void PyGarden::print_wakeup_reason() {
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0:
      Serial.println("Wakeup caused by external signal using RTC_IO");

      // powered on using manual button, start manual mode right away
      // toggleState();
      break;

    case ESP_SLEEP_WAKEUP_EXT1:
      Serial.println("Wakeup caused by external signal using RTC_CNTL");
      break;

    case ESP_SLEEP_WAKEUP_TIMER:
      Serial.println("Wakeup caused by timer");
      break;

    case ESP_SLEEP_WAKEUP_TOUCHPAD:
      Serial.println("Wakeup caused by touchpad");
      break;

    case ESP_SLEEP_WAKEUP_ULP:
      Serial.println("Wakeup caused by ULP program");
      break;

    default:
      Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason);
      break;
  }

  // enable power led
  _powerLED->enable();
}

void PyGarden::printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.println("==============================");
}
