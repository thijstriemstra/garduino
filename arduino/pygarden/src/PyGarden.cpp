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
  _sensors = new Sensors();

  // water
  _water = new SingleChannel_Relay(WaterValvePin, false);

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
  _sensors->begin();

  // water
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

  // sensors
  _sensors->publish(MQTT_BASE_TOPIC, _iot);
}

void PyGarden::onPowerButtonPush() {
  Serial.println("******************************");
  Serial.println("** Going to sleep... Bye.   **");
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
