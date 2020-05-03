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
  _waterLED = new LED(WateringIndicationLEDPin);

  // scheduler
  _scheduler = new ThreadController();

  // wifi/mqtt
  _iot = new IOT();

  // watering task
  Method wateringReadyCallback;
  wateringReadyCallback.attachCallback(
    makeFunctor((Functor0 *)0, *this, &PyGarden::onWateringReady));
  _wateringTask = new WateringTask(
    WateringDuration,
    WaterValvePin,
    _iot,
    _namespace,
    WateringSchedule,
    wateringReadyCallback
  );

  // system time
  _clock = new SystemClock(ClockSCLPin, ClockSDAPin, NTP_HOST);

  // power management
  _power = new PowerManagement(WakeupSchedule);

  // sensors
  int publishSchedule = SensorPublishSchedule * 1000;
  _sensors = new Sensors(publishSchedule);
}

void PyGarden::begin() {
  // print version
  Serial.print(_namespace);
  Serial.print(" ");
  Serial.println(_version);

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
  _waterLED->begin();

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
  _waterLED->loop();

  // scheduler
  _scheduler->run();
}

void PyGarden::sleep(bool forced) {
  Serial.println();
  Serial.println("******************************");
  Serial.print("**  ");
  if (forced) {
    Serial.print("Forced");
  } else {
    Serial.print("Going");
  }
  Serial.println(" to sleep... Bye.  **");
  Serial.println("******************************");

  // disable power led
  _powerLED->disable();

  // put device to sleep
  _power->sleep();
}

void PyGarden::openValve() {
  started = true;

  // indication LED
  _waterLED->enable();

  // open valve
  _wateringTask->open();

  Serial.println("-----------------------");
  Serial.println("Water: valve open");
}

void PyGarden::closeValve() {
  started = false;

  // indication LED
  _waterLED->disable();

  // close valve
  _wateringTask->close();

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

  // now wait till user presses the manual button again to control the valve,
  // and then eventually presses power button to put device back into deepsleep
}

void PyGarden::checkWatering() {
  // check if garden needs watering right now
  bool enableValve = _wateringTask->needsWatering(_clock->startupTime);
  Serial.println();
  Serial.println("************************************");
  Serial.print("      Watering: ");
  if (enableValve) {
    Serial.println("Yes");
  } else {
    Serial.println("No");
  }
  Serial.print("        Period: ");
  Serial.print(WateringDuration);
  Serial.println(" sec");
  Serial.print("Daily schedule: ");
  Serial.print(WateringSchedule);
  Serial.println(":00");
  Serial.print("  Current time: ");
  Serial.println(_clock->getStartupTime());
  Serial.print("      Last run: ");
  Serial.println(_wateringTask->getLastRunTime());

  Serial.println("************************************");
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
  _networkLED->disable();
}

void PyGarden::onConnectionFailed() {
  connected = false;
  _networkLED->disable();

  Serial.println();
  Serial.println("*** No WiFi connection available! ***");

  // no connection available to publish sensor data,
  // only check for watering if not in manual mode
  // or watering already
  if (_manualMode) {
    startManualMode();
  } else if (!_manualMode && !_wateringTask->isWatering()) {
    checkWatering();
  }
}

void PyGarden::onConnectionReady() {
  connected = true;

  // network status LED
  _networkLED->enable();

  // publish sensor data
  _sensors->startPublish(_iot, _clock->getStartupTemperature());

  // enter manual mode (if button was pressed) or check watering
  if (_manualMode) {
    // sync time only in manual mode
    //_clock->sync();

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

    // enable manual led
    _manualLED->enable();
  } else {
    _manualMode = false;
  }
}

void PyGarden::onManualButtonPush() {
  toggleValve();
}

void PyGarden::onPowerButtonPush() {
  // forced to sleep
  sleep(true);
}
