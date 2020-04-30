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

  // scheduler
  _scheduler = new ThreadController();

  // watering task
  Method wateringReadyCallback;
  wateringReadyCallback.attachCallback(
    makeFunctor((Functor0 *)0, *this, &PyGarden::onWateringReady));
  int duration = WateringDuration * 1000;
  _wateringTask = new WateringTask(
    duration,
    WaterValvePin,
    WateringSchedule,
    wateringReadyCallback
  );

  // system time
  _clock = new SystemClock(NTP_HOST);

  // power management
  _power = new PowerManagement(WakeupSchedule);

  // wifi/mqtt
  _iot = new IOT(MQTT_BASE_TOPIC);

  // sensors
  int publishSchedule = SensorPublishSchedule * 1000;
  _sensors = new Sensors(publishSchedule);
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
  Method systemWakeupCallback;
  systemWakeupCallback.attachCallback(
    makeFunctor((Functor0 *)0, *this, &PyGarden::onSystemWakeup));

  // controls
  _manualBtn->begin(manualBtnCallback);
  _manualLED->begin();
  _powerBtn->begin(powerBtnCallback);
  _powerLED->begin();
  _networkLED->begin();

  // watering task
  _wateringTask->begin();
  _scheduler->add(_wateringTask);

  // sensors
  _scheduler->add(_sensors);
  _sensors->begin();

  // power management
  _power->init(systemWakeupCallback);

  // connect to wifi/mqtt
  _networkLED->blink();
  _iot->begin(
    connectionReadyCallback,
    disconnectedCallback
  );
}

void PyGarden::loop() {
  // controls
  _manualBtn->loop();
  _manualLED->loop();
  _powerBtn->loop();
  _powerLED->loop();
  _networkLED->loop();

  // scheduler
  _scheduler->run();
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
  started = true;

  // open valve
  _wateringTask->open();

  // enable led
  _manualLED->enable();

  Serial.println("-----------------------");
  Serial.println("Water: valve open");
}

void PyGarden::closeValve() {
  started = false;

  // close valve
  _wateringTask->close();

  // disable led
  _manualLED->disable();

  Serial.println("-----------------------");
  Serial.println("Water: valve closed");
}

void PyGarden::toggleValve() {
  if (started == false) {
    openValve();
  } else {
    closeValve();
  }
}

void PyGarden::onConnectionClosed() {
  // network connection closed
  _networkLED->disable();
}

void PyGarden::onConnectionReady() {
  // network status LED
  // stop blinking
  _networkLED->blink();
  _networkLED->enable();

  // sync time
  _clock->sync();

  // start publishing sensor data
  _sensors->startPublish(_iot);

  if (_manualMode) {
    Serial.println();
    Serial.println("===================");
    Serial.println("==  Manual mode  ==");
    Serial.println("===================");
    Serial.println();

    // open valve
    openValve();

    // now wait till user presses the manual button again to control the valve,
    // and then eventually presses power button to put device back into deepsleep

  } else {
    // check if garden needs watering right now
    bool enableValve = _wateringTask->needsWatering();
    Serial.println();
    Serial.println("****************************");
    Serial.print("   Time for watering: ");
    if (enableValve) {
      Serial.println("Yes");
    } else {
      Serial.println("No");
    }
    Serial.print("      Schedule: ");
    Serial.println(WateringSchedule);
    Serial.print("        Period: ");
    Serial.print(WateringDuration);
    Serial.println(" sec");
    Serial.println("****************************");
    Serial.println();

    if (enableValve) {
      // start watering
      _wateringTask->start();
    } else {
      // disable LEDS
      _powerLED->disable();

      // XXX: done, go into deepsleep and wait till woken up by button or timer
      //sleep();
    }
  }
}

void PyGarden::onWateringReady() {
  Serial.println();
  Serial.println("**************************************");
  Serial.println("* Watering finished! Back to sleep.  *");
  Serial.println("**************************************");

  sleep();
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
