/*  Copyright (c) 2020-2023, Collab
 *  All rights reserved
*/
/*
  Garduino.cpp - Library for monitoring a garden.
*/

#include <Garduino.h>

Garduino::Garduino() {
  // I2C
  Wire1.setPins(I2CSDAPin, I2CSCLPin);

  // I2C multiplexer
  _i2c = new MultiPlexer_TCA9548A(I2CExpanderAddress);

  // IO expander
  _ioExpander = new MultiPlexer_PCF8574(IOExpanderAddress, &Wire1);

  // controls
  _controls = new Controls(_ioExpander);

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
    BuzzerPin,
    _ioExpander,
    _namespace,
    WateringSchedule,
    wateringReadyCallback,
    valveOpenCallback,
    valveClosedCallback
  );

  // system time
  _clock = new SystemClock(&Wire1, NTP_HOST);

  // power management
  _power = new PowerManagement(ManualRunButtonPin, WakeupSchedule);

  // display on I2C multiplexer
  _display = new SSD1306_OLEDDisplay_Mux(
    _i2c,
    DisplayChannel,
    DisplayAddress,
    false
  );

  // display task
  _displayTask = new DisplayTask(_display, _clock);

  // sensors
  _sensors = new Sensors(SensorPublishSchedule, _i2c, true, _namespace);
}

void Garduino::begin() {
  // logging
  Log.setPrefix(printPrefix);
  Log.begin(LOG_LEVEL_VERBOSE, &Serial);
  Log.setShowLevel(false);

  // print version
  Log.info(F("========================" CR));
  Log.info(F("=    %S v%S   =" CR), _namespace, _version);
  Log.info(F("========================" CR));

  // board info
  Log.info(F("Board:\t   %S" CR), ARDUINO_BOARD);
  Log.info(F("Arduino:\t   %d.%d.%d" CR),
    ESP_ARDUINO_VERSION_MAJOR, ESP_ARDUINO_VERSION_MINOR,
    ESP_ARDUINO_VERSION_PATCH
  );
  Log.info(F("ESP-IDF:\t   %S" CR), ESP.getSdkVersion());

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
  failedConnectionCallback.attachCallbackIntArg(
    makeFunctor((Functor1<int> *)0, *this, &Garduino::onConnectionFailed));
  Method publishReadyCallback;
  publishReadyCallback.attachCallback(
    makeFunctor((Functor0 *)0, *this, &Garduino::onPublishReady));
  Method systemWakeupCallback;
  systemWakeupCallback.attachCallback(
    makeFunctor((Functor0 *)0, *this, &Garduino::onSystemWakeup));

  // system time
  _clock->begin();
  Log.info(F("Local time:  %S" CR), _clock->getStartupTime());

  // I2C multiplexer
  _i2c->begin();
  //_i2c->scanAll();

  // expander
  _ioExpander->begin();

  // controls
  _controls->begin(manualBtnCallback, powerBtnCallback);

  // watering task
  _wateringTask->begin();

  // display
  _display->begin();

  // display task
  _displayTask->begin();

  // sensors
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
  _controls->loop();
}

void Garduino::sleep(bool forced) {
  // save total volume added
  _sensors->save();

  // uncomment this to reset stored memory
  //_sensors->reset();

  // iot
  _iot->disconnect();
  delay(300);

  Log.info(CR);
  Log.info(F("******************************" CR));
  String target = "Going";
  if (forced) {
    target = "Forced";
  }
  Log.info(F("**  %S to sleep... Bye.  **" CR), target);
  Log.info(F("******************************" CR));

  // disable leds
  _controls->disableLEDs();
  _wateringTask->disableLEDs();

   // display
  _display->disable();

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
  Log.info(CR);
  Log.info(F("===================" CR));
  Log.info(F("==  Manual mode  ==" CR));
  Log.info(F("===================" CR));
  Log.info(CR);

  // now wait till user presses the manual button again to control the valve,
  // and then eventually presses power button to put device back into deepsleep
}

bool Garduino::isWatering() {
  return _wateringTask->isWatering();
}

void Garduino::checkWatering() {
  // check if garden needs watering right now
  bool enableValve = _wateringTask->needsWatering(_clock->startupTime);

  Log.info(CR);
  Log.info(F("************************************" CR));
  Log.info(F("      Watering: %S" CR), Utils::BoolToString(enableValve));
  Log.info(F("        Period: %d sec" CR), WateringDuration);
  Log.info(F("Daily schedule: %S:00" CR), WateringSchedule);
  Log.info(F("  Current time: %S" CR), _clock->getStartupTime());
  Log.info(F("      Last run: %s" CR), _wateringTask->getLastRunTime().c_str());
  Log.info(F("************************************" CR));
  Log.info(CR);

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

void Garduino::onWateringReady() {
  Log.info(CR);
  Log.info(F("**************************************" CR));
  Log.info(F("* Watering finished! Back to sleep.  *" CR));
  Log.info(F("**************************************" CR));

  // wait a while so IOT can complete
  delay(300);

  // shutdown
  sleep();
}

void Garduino::onPublishReady() {
  // only shutdown when manual mode is not enabled and system is
  // not watering at the moment
  if (!_manualMode && !isWatering()) {
    // done, go into deepsleep and wait till woken up by button
    // or timer
    sleep();
  }
}

void Garduino::onConnectionClosed() {
  connected = false;

  _controls->networkLED->disable();
}

void Garduino::onConnectionFailed(int connection_type) {
  connected = false;

  _controls->networkLED->disable();

  Log.warning(CR);
  Log.info(F("************************************" CR));
  if (connection_type == 0) {
    Log.warning(F("*** No WiFi connection available ***" CR));
  } else {
    Log.warning(F("*** No MQTT connection available ***" CR));
  }
  Log.info(F("************************************" CR));

  // no connection available to publish sensor data,
  // only check for watering if not in manual mode
  // or watering already
  if (_manualMode) {
    startManualMode();

  } else if (!_manualMode && !isWatering()) {
    checkWatering();
  }
}

void Garduino::onConnectionReady() {
  connected = true;

  // network status LED
  _controls->networkLED->enable();

  // enter manual mode (if button was pressed) or check watering
  if (_manualMode) {
    // sync time only in manual mode
    //_clock->sync();

    startManualMode();
  } else {
    checkWatering();
  }

  // publish sensor data
  _sensors->startPublish(_iot, _clock->getStartupTemperature());
}

void Garduino::onValveOpen() {
  // display
  _displayTask->open();
}

void Garduino::onValveClosed() {
  // display
  _displayTask->close();
}

void Garduino::onSystemWakeup() {
  // enable power led
  _controls->powerLED->enable();

  if (_power->wakeup_reason == ESP_SLEEP_WAKEUP_EXT0) {
    // manual button pressed
    _manualMode = true;

    // enable manual led
    _controls->manualLED->enable();

    // show logo
    _displayTask->showLogo();
    delay(1800);

    // start display info task
    xTaskCreatePinnedToCore(
      &Garduino::displayInfo,    /* Task function. */
      "displayInfoTask",         /* String with name of task. */
      2048,                      /* Stack size in words. */
      this,                      /* Parameter passed as input of the task */
      8,                         /* Priority of the task. */
      NULL,                       /* Task handle. */
      1                           /* Core nr */
    );
  } else {
    _manualMode = false;
  }

  _sensors->manualMode = _manualMode;
}

void Garduino::onManualButtonPush() {
  toggleValve();
}

void Garduino::onPowerButtonPush() {
  // force to sleep
  sleep(true);
}

void Garduino::displayInfo(void *pvParameter) {
  for (;;) {
    // obtain the instance pointer
    Garduino* garduino = reinterpret_cast<Garduino*>(pvParameter);
    int pausedMs = 3500;

    // don't overwrite display when watering
    if (!garduino->_wateringTask->isValveOpen()) {
      // display temperature
      garduino->displayTemperature();

      // pause the task
      vTaskDelay(pausedMs / portTICK_PERIOD_MS);
    }

    // don't overwrite display when watering
    if (!garduino->_wateringTask->isValveOpen()) {
      // display humidity
      garduino->displayHumidity();

      // pause the task
      vTaskDelay(pausedMs / portTICK_PERIOD_MS);
    }

    // don't overwrite display when watering
    if (!garduino->_wateringTask->isValveOpen()) {
      // display lux
      garduino->displayLux();

      // pause the task
      vTaskDelay(pausedMs / portTICK_PERIOD_MS);
    }

    // don't overwrite display when watering
    if (!garduino->_wateringTask->isValveOpen()) {
      // display signal strength
      garduino->displaySignalStrength();

      // pause the task
      vTaskDelay(pausedMs / portTICK_PERIOD_MS);
    }

    // don't overwrite display when watering
    if (!garduino->_wateringTask->isValveOpen()) {
      // display time
      garduino->displayTime();

      // pause the task
      vTaskDelay(pausedMs / portTICK_PERIOD_MS);
    }

    // pause the task
    pausedMs = 1000;
    vTaskDelay(pausedMs / portTICK_PERIOD_MS);
  }
}

void Garduino::displayTime() {
  _displayTask->showTime();
}

void Garduino::displayTemperature() {
  BME280_Result tmp = _sensors->readBarometer();

  _displayTask->showTemperature(tmp.temperature);
}

void Garduino::displayHumidity() {
  BME280_Result tmp = _sensors->readBarometer();

  _displayTask->showHumidity(tmp.humidity);
}

void Garduino::displaySignalStrength() {
  int signal_strength = _iot->getSignalStrength();

  _displayTask->showSignalStrength(signal_strength);
}

void Garduino::displayLux() {
  float lux = _sensors->measureLight();

  _displayTask->showLux(lux);
}

void Garduino::printPrefix(Print* _logOutput, int logLevel) {
  const unsigned long MSECS_PER_SEC = 1000;
  const unsigned long SECS_PER_MIN = 60;
  const unsigned long SECS_PER_HOUR = 3600;
  const unsigned long SECS_PER_DAY = 86400;
  const unsigned long msecs = millis();
  const unsigned long secs = msecs / MSECS_PER_SEC;
  const unsigned long MilliSeconds = msecs % MSECS_PER_SEC;
  const unsigned long Seconds = secs % SECS_PER_MIN;
  const unsigned long Minutes = (secs / SECS_PER_MIN) % SECS_PER_MIN;
  const unsigned long Hours = (secs % SECS_PER_DAY) / SECS_PER_HOUR;

  char timestamp[20];
  sprintf(timestamp, "%02d:%02d:%02d.%03d  ", Hours, Minutes, Seconds, MilliSeconds);

  _logOutput->print(timestamp);
}
