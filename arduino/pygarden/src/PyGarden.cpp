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
  _clock = new SystemClock(ClockSCLPin, ClockSDAPin, NTP_HOST);

  // power management
  _power = new PowerManagement(WakeupSchedule);

  // wifi/mqtt
  _iot = new IOT();

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
  Method failedConnectionCallback;
  failedConnectionCallback.attachCallback(
    makeFunctor((Functor0 *)0, *this, &PyGarden::onConnectionFailed));
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

  // watering task
  _wateringTask->begin();
  _scheduler->add(_wateringTask);

  // system time
  _clock->begin();

  // sensors
  _scheduler->add(_sensors);
  _sensors->begin();

  // power management
  _power->init(systemWakeupCallback);

  // connect to wifi/mqtt
  _networkLED->blink();
  _iot->begin(
    _totalReadings,
    connectionReadyCallback,
    disconnectedCallback,
    publishReadyCallback,
    failedConnectionCallback
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

void PyGarden::startManualMode() {
  Serial.println();
  Serial.println("===================");
  Serial.println("==  Manual mode  ==");
  Serial.println("===================");
  Serial.println();

  // open valve
  openValve();

  // now wait till user presses the manual button again to control the valve,
  // and then eventually presses power button to put device back into deepsleep
}

void PyGarden::checkWatering() {
  // check if garden needs watering right now
  bool enableValve = _wateringTask->needsWatering(_clock->startupTime.Hour());
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
  Serial.print("  Current time: ");
  Serial.println(_clock->getStartupTime());

  Serial.println("****************************");
  Serial.println();

  if (enableValve) {
    // start watering
    // will put device back to sleep when done
    _wateringTask->start();
  } else {
    // check if sensor publish is not already done
    if (!connected) {
      // go to sleep, everything is done
      sleep();
    }
  }
}

void PyGarden::onPublishReady() {
  // only shutdown when manual mode is not enabled and system is
  // not watering at the moment
  if (!_manualMode && !_wateringTask->isWatering()) {
    // done, go into deepsleep and wait till woken up by button or timer
    sleep();
  }
}

void PyGarden::onConnectionClosed() {
  connected = false;

  // network connection closed
  _networkLED->disable();
}

void PyGarden::onConnectionFailed() {
  connected = false;

  Serial.println();
  Serial.println("*** No WiFi connection available! ***");
  Serial.println();

  // no connection available to publish sensor data,
  // only check for watering or enter manual mode if
  // button was pressed at startup
  if (_manualMode) {
    startManualMode();
  } else {
    checkWatering();
  }
}

void PyGarden::onConnectionReady() {
  connected = true;

  // network status LED
  // stop blinking
  _networkLED->blink();
  _networkLED->enable();

  // start publishing sensor data
  _sensors->startPublish(_iot);

  // enter manual mode (if button was pressed) or check watering
  if (_manualMode) {
    // sync time only in manual mode
    _clock->sync();

    startManualMode();
  } else {
    checkWatering();
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
