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

  // water valve
  _waterValve = new SolenoidValve(WaterValvePin);

  // system time
  _clock = new SystemClock(NTP_HOST);

  // power management
  _power = new PowerManagement(WakeupSchedule);

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
  Method disconnectedCallback;
  disconnectedCallback.attachCallback(
    makeFunctor((Functor0 *)0, *this, &PyGarden::onConnectionClosed));
  Method publishReadyCallback;
  publishReadyCallback.attachCallback(
    makeFunctor((Functor0 *)0, *this, &PyGarden::onPublishReady));
  Method systemWakeupCallback;
  systemWakeupCallback.attachCallback(
    makeFunctor((Functor0 *)0, *this, &PyGarden::onSystemWakeup));

  // controls
  _manualBtn->begin(manualBtnCallback);
  _manualLED->begin();
  _powerBtn->begin(powerBtnCallback);
  _powerLED->begin();
  _networkLED->begin();

  // sensors
  _sensors->begin();

  // water valve
  _waterValve->begin();

  // power management
  _power->init(systemWakeupCallback);

  // wifi/mqtt
  _networkLED->blink();
  _iot->begin(
    TotalReadings,
    connectionReadyCallback,
    disconnectedCallback,
    publishReadyCallback
  );
}

void PyGarden::loop() {
  // controls
  _manualBtn->loop();
  _manualLED->loop();
  _powerBtn->loop();
  _powerLED->loop();
  _networkLED->loop();
}

void PyGarden::sleep() {
  Serial.println();
  Serial.println("******************************");
  Serial.println("**  Going to sleep... Bye.  **");
  Serial.println("******************************");

  // disable power led
  _powerLED->disable();

  // put device to sleep
  _power->sleep();
}

void PyGarden::openValve() {
  _waterValve->start();

  _manualLED->enable();

  Serial.println("-----------------------");
  Serial.println("Water: valve open");
}

void PyGarden::closeValve() {
  _waterValve->stop();

  _manualLED->disable();

  Serial.println("-----------------------");
  Serial.println("Water: valve closed");
}

void PyGarden::toggleValve() {
  if (started == false) {
    started = true;

    openValve();
  } else {
    started = false;

    closeValve();
  }
}

void PyGarden::onConnectionClosed() {
  if (_manualMode) {
    Serial.println();
    Serial.println("===================");
    Serial.println("==  Manual mode  ==");
    Serial.println("===================");
    Serial.println();

    // open valve
    toggleValve();

    // now wait till user presses the manual button again to control the valve,
    // and then eventually presses power button to put device back into deepsleep

  } else {
    // XXX: check if water valve needs to be opened before going into
    // deepsleep

    // disable LEDS
    delay(500);
    _networkLED->disable();
    delay(400);
    _powerLED->disable();
    delay(400);

    // done, go into deepsleep and wait till woken up by user or timer
    sleep();
  }
}

void PyGarden::onConnectionReady() {
  // network status LED
  // stop blinking
  _networkLED->blink();
  _networkLED->enable();

  // sync time
  _clock->sync();

  // publish sensor data
  _sensors->publish(MQTT_BASE_TOPIC, _iot);
}

void PyGarden::onPublishReady() {
  Serial.println("----------------------");
  Serial.println("Published sensor data.");
  Serial.println("----------------------");

  // disconnnect iot
  _iot->disconnect();
}

void PyGarden::onSystemWakeup() {
  // enable power led
  _powerLED->enable();

  if (_power->wakeup_reason == ESP_SLEEP_WAKEUP_EXT0) {
    // manual button pressed
    _manualMode = true;
  } else {
    _manualMode = false;
  }

  Serial.print("Manual mode: ");
  Serial.println(_manualMode);
}

void PyGarden::onManualButtonPush() {
  toggleValve();
}

void PyGarden::onPowerButtonPush() {
  sleep();
}
