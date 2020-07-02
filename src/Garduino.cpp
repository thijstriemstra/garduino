/*
  Garduino.cpp - Library for monitoring a garden.
*/

#include <Garduino.h>

Garduino::Garduino() {
  // scheduler
  _scheduler = new ThreadController();

  // controls
  _manualBtn = new Button(ManualRunButtonPin);
  _manualLED = new LED(ManualRunLEDPin);
  _networkLED = new LED(NetworkLEDPin);
  _powerBtn = new Button(PowerButtonPin);
  _powerLED = new LED(PowerLEDPin);

  // wifi/mqtt
  _iot = new IOT();

  // watering task
  Method wateringReadyCallback;
  wateringReadyCallback.attachCallback(
    makeFunctor((Functor0 *)0, *this, &Garduino::onWateringReady));
  Method valveOpenCallback;
  valveOpenCallback.attachCallback(
    makeFunctor((Functor0 *)0, *this, &Garduino::onValveOpen));
  Method valveClosedCallback;
  valveClosedCallback.attachCallback(
    makeFunctor((Functor0 *)0, *this, &Garduino::onValveClosed));
  _wateringTask = new WateringTask(
    WateringDuration,
    WaterValvePin,
    WateringIndicationLEDPin,
    _namespace,
    WateringSchedule,
    wateringReadyCallback,
    valveOpenCallback,
    valveClosedCallback
  );

  // system time
  _clock = new SystemClock(ClockSCLPin, ClockSDAPin, NTP_HOST);

  // power management
  _power = new PowerManagement(WakeupSchedule);

  // display
  _display = new SSD1306_OLEDDisplay(DisplaySDAPin, DisplaySCLPin);

  // sensors
  int publishSchedule = SensorPublishSchedule * 1000;
  _sensors = new Sensors(publishSchedule, _namespace);
}

void Garduino::begin() {
  // print version
  Serial.print(_namespace);
  Serial.print(" ");
  Serial.println(_version);

  // callbacks
  Method manualBtnCallback;
  manualBtnCallback.attachCallback(
    makeFunctor((Functor0 *)0, *this, &Garduino::onManualButtonPush));
  Method powerBtnCallback;
  powerBtnCallback.attachCallback(
    makeFunctor((Functor0 *)0, *this, &Garduino::onPowerButtonPush));
  Method connectionReadyCallback;
  connectionReadyCallback.attachCallback(
    makeFunctor((Functor0 *)0, *this, &Garduino::onConnectionReady));
  Method disconnectedCallback;
  disconnectedCallback.attachCallback(
    makeFunctor((Functor0 *)0, *this, &Garduino::onConnectionClosed));
  Method failedConnectionCallback;
  failedConnectionCallback.attachCallback(
    makeFunctor((Functor0 *)0, *this, &Garduino::onConnectionFailed));
  Method publishReadyCallback;
  publishReadyCallback.attachCallback(
    makeFunctor((Functor0 *)0, *this, &Garduino::onPublishReady));
  Method systemWakeupCallback;
  systemWakeupCallback.attachCallback(
    makeFunctor((Functor0 *)0, *this, &Garduino::onSystemWakeup));

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

  // display
  _display->begin();

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

void Garduino::loop() {
  // controls
  _manualBtn->loop();
  _manualLED->loop();
  _powerBtn->loop();
  _powerLED->loop();
  _networkLED->loop();

  // scheduler
  _scheduler->run();
}

void Garduino::sleep(bool forced) {
  // save total volume added
  _sensors->save();

  // uncomment this to reset stored memory
  //_sensors->reset();

  // iot
  _iot->disconnect();
  delay(300);

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

void Garduino::openValve() {
  started = true;

  // open valve
  _wateringTask->open();
}

void Garduino::closeValve() {
  started = false;

  // close valve
  _wateringTask->close();
}

void Garduino::toggleValve() {
  if (started == false) {
    openValve();
  } else {
    closeValve();
  }
}

void Garduino::startManualMode() {
  Serial.println();
  Serial.println("===================");
  Serial.println("==  Manual mode  ==");
  Serial.println("===================");
  Serial.println();

  // now wait till user presses the manual button again to control the valve,
  // and then eventually presses power button to put device back into deepsleep
}

void Garduino::checkWatering() {
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

void Garduino::onPublishReady() {
  // only shutdown when manual mode is not enabled and system is
  // not watering at the moment
  if (!_manualMode && !_wateringTask->isWatering()) {
    // done, go into deepsleep and wait till woken up by button or timer
    sleep();
  }
}

void Garduino::onConnectionClosed() {
  connected = false;
  _networkLED->disable();
}

void Garduino::onConnectionFailed() {
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

void Garduino::onConnectionReady() {
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

void Garduino::onValveOpen() {
  // publish
  _iot->publish("/water/valve", 1);

  // display
  _display->writeBig("Open");
}

void Garduino::onValveClosed() {
  // publish
  _iot->publish("/water/valve", 0);

  // display
  _display->writeBig("Closed");
}

void Garduino::onWateringReady() {
  Serial.println();
  Serial.println("**************************************");
  Serial.println("* Watering finished! Back to sleep.  *");
  Serial.println("**************************************");

  // wait a while so IOT can complete
  delay(300);

  sleep();
}

void Garduino::onSystemWakeup() {
  // enable power led
  _powerLED->enable();

  if (_power->wakeup_reason == ESP_SLEEP_WAKEUP_EXT0) {
    // manual button pressed
    _manualMode = true;

    // enable manual led
    _manualLED->enable();

    // display
    _display->writeBig("Manual");
  } else {
    _manualMode = false;
  }
}

void Garduino::onManualButtonPush() {
  toggleValve();
}

void Garduino::onPowerButtonPush() {
  // display
  _display->disable();

  // forced to sleep
  sleep(true);
}
