/*  Copyright (c) 2020-2021, Collab
 *  All rights reserved
*/
/*
  Garduinov3.cpp - Library for monitoring a garden.
*/

#include <Garduinov3.h>

Garduinov3::Garduinov3() {
    // controls
    _controls = new Controls();

    // expander on 2nd I2C bus
    Wire1.setPins(ExpanderSDAPin, ExpanderSCLPin);
    _i2c = new MultiPlexer_TCA9548A(ExpanderAddress);

    // wifi/mqtt
    _iot = new IOT();

    // watering task
    Method wateringReadyCallback;
    wateringReadyCallback.attachCallback(
        makeFunctor((Functor0 *)0, *this, &Garduinov3::onWateringReady));
    Method valveOpenCallback;
    valveOpenCallback.attachCallback(
        makeFunctor((Functor0 *)0, *this, &Garduinov3::onValveOpen));
    Method valveClosedCallback;
    valveClosedCallback.attachCallback(
        makeFunctor((Functor0 *)0, *this, &Garduinov3::onValveClosed));
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
    _display = new SSD1306_OLEDDisplay_Mux(
        _i2c,
        DisplayChannel,
        DisplayAddress,
        DisplayFlipVertical,
        OLEDDISPLAY_GEOMETRY::GEOMETRY_128_32,
        HW_I2C::I2C_TWO
    );

    // sensors
    int publishSchedule = SensorPublishSchedule * 1000;
    _sensors = new Sensors(publishSchedule, _i2c, true, _namespace);
}

void Garduinov3::begin() {
    // print version
    Serial.println(F("\n========================"));
    Serial.print(F("= "));
    Serial.print(_namespace);
    Serial.print(F(" v"));
    Serial.print(_version);
    Serial.println(F(" ="));
    Serial.println(F("========================\n"));

    // board info
    Serial.print(F("Board:\t\t"));
    Serial.println(ARDUINO_BOARD);

    // callbacks
    Method manualBtnCallback;
    manualBtnCallback.attachCallback(
        makeFunctor((Functor0 *)0, *this, &Garduinov3::onManualButtonPush));
    Method powerBtnCallback;
    powerBtnCallback.attachCallback(
        makeFunctor((Functor0 *)0, *this, &Garduinov3::onPowerButtonPush));
    Method connectionReadyCallback;
    connectionReadyCallback.attachCallback(
        makeFunctor((Functor0 *)0, *this, &Garduinov3::onConnectionReady));
    Method disconnectedCallback;
    disconnectedCallback.attachCallback(
        makeFunctor((Functor0 *)0, *this, &Garduinov3::onConnectionClosed));
    Method failedConnectionCallback;
    failedConnectionCallback.attachCallback(
        makeFunctor((Functor0 *)0, *this, &Garduinov3::onConnectionFailed));
    Method publishReadyCallback;
    publishReadyCallback.attachCallback(
        makeFunctor((Functor0 *)0, *this, &Garduinov3::onPublishReady));
    Method systemWakeupCallback;
    systemWakeupCallback.attachCallback(
        makeFunctor((Functor0 *)0, *this, &Garduinov3::onSystemWakeup));

    // i2c
    _i2c->begin();

    // system time
    _clock->begin();

    // controls
    _controls->begin(manualBtnCallback, powerBtnCallback);

    // watering task
    _wateringTask->begin();
    //_scheduler->add(_wateringTask);

    // display
    _display->begin();

    // sensors
    // TODO
    //_scheduler->add(_sensors);
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

void Garduinov3::loop() {
    // scheduler
    // TODO
    //_scheduler.run();

    // controls
    _controls->loop();
}

void Garduinov3::sleep(bool forced) {
    // save total volume added
    _sensors->save();

    // uncomment this to reset stored memory
    //_sensors->reset();

    // iot
    _iot->disconnect();
    delay(300);

    Serial.println();
    Serial.println(F("******************************"));
    Serial.print(F("**  "));
    if (forced) {
        Serial.print(F("Forced"));
    }
    else
    {
        Serial.print(F("Going"));
    }
    Serial.println(F(" to sleep... Bye.  **"));
    Serial.println(F("******************************"));

    // disable power led
    _controls->powerLED->disable();

    // put device to sleep
    _power->sleep();
}

void Garduinov3::openValve() {
    started = true;

    // open valve
    _wateringTask->open();
}

void Garduinov3::closeValve() {
    started = false;

    // close valve
    _wateringTask->close();
}

void Garduinov3::toggleValve() {
    if (started == false) {
        openValve();
    } else {
        closeValve();
    }
}

void Garduinov3::startManualMode() {
    Serial.println();
    Serial.println(F("==================="));
    Serial.println(F("==  Manual mode  =="));
    Serial.println(F("==================="));
    Serial.println();

    // now wait till user presses the manual button again to control the valve,
    // and then eventually presses power button to put device back into deepsleep
}

void Garduinov3::checkWatering() {
    // check if garden needs watering right now
    bool enableValve = _wateringTask->needsWatering(_clock->startupTime);
    Serial.println();
    Serial.println(F("************************************"));
    Serial.print(F("      Watering: "));
    Serial.println(Utils::BoolToString(enableValve));
    Serial.print(F("        Period: "));
    Serial.print(WateringDuration);
    Serial.println(F(" sec"));
    Serial.print(F("Daily schedule: "));
    Serial.print(WateringSchedule);
    Serial.println(F(":00"));
    Serial.print(F("  Current time: "));
    Serial.println(_clock->getStartupTime());
    Serial.print(F("      Last run: "));
    Serial.println(_wateringTask->getLastRunTime());

    Serial.println(F("************************************"));
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

void Garduinov3::onPublishReady() {
    // only shutdown when manual mode is not enabled and system is
    // not watering at the moment
    if (!_manualMode && !_wateringTask->isWatering()) {
        // done, go into deepsleep and wait till woken up by button or timer
        sleep();
    }
}

void Garduinov3::onConnectionClosed() {
    connected = false;
    _controls->networkLED->disable();
}

void Garduinov3::onConnectionFailed() {
    connected = false;
    _controls->networkLED->disable();

    Serial.println();
    Serial.println(F("*** No WiFi connection available! ***"));

    // no connection available to publish sensor data,
    // only check for watering if not in manual mode
    // or watering already
    if (_manualMode) {
        startManualMode();
    } else if (!_manualMode && !_wateringTask->isWatering()) {
        checkWatering();
    }
}

void Garduinov3::onConnectionReady() {
    connected = true;

    // network status LED
    _controls->networkLED->enable();

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

void Garduinov3::onValveOpen() {
    // publish
    _iot->publish("/water/valve", 1);

    // display
    _display->writeBig(F("Open"));
}

void Garduinov3::onValveClosed() {
    // publish
    _iot->publish("/water/valve", 0);

    // display
    _display->writeBig(F("Closed"));
}

void Garduinov3::onWateringReady() {
    Serial.println();
    Serial.println(F("**************************************"));
    Serial.println(F("* Watering finished! Back to sleep.  *"));
    Serial.println(F("**************************************"));

    // wait a while so IOT can complete
    delay(300);

    sleep();
}

void Garduinov3::onSystemWakeup() {
    // enable power led
    _controls->powerLED->enable();

    if (_power->wakeup_reason == ESP_SLEEP_WAKEUP_EXT0) {
        // manual button pressed
        _manualMode = true;

        // enable manual led
        _controls->manualLED->enable();

        // display
        _display->writeBig(F("Manual"));
    } else {
        _manualMode = false;
    }
}

void Garduinov3::onManualButtonPush() {
    toggleValve();
}

void Garduinov3::onPowerButtonPush() {
    // display
    _display->disable();

    // forced to sleep
    sleep(true);
}
