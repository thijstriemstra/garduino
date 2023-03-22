/*  Copyright (c) 2020-2023, Collab
 *  All rights reserved
*/
/*
  PowerManagement.cpp
*/

#include "PowerManagement.h"

/**
 * Constructor.
 */
PowerManagement::PowerManagement(int wakeupPin, int wakeupTime) {
  _wakeupTime = wakeupTime;
  _wakeupPin = wakeupPin;
}

void PowerManagement::init(Method wakeup_callback) {
  _wakeupCallback = wakeup_callback;

  Log.info(F("*******************************************" CR));

  // print the wakeup reason
  wokeup();

  // configure the wake up sources
  esp_sleep_enable_ext0_wakeup((gpio_num_t) _wakeupPin, 1);

  // wake up esp32 periodically
  esp_sleep_enable_timer_wakeup(_wakeupTime * uS_TO_S_FACTOR);

  Log.info(F("*******************************************" CR));
}

void PowerManagement::sleep() {
  // put esp32 into deepsleep
  esp_deep_sleep_start();
}

void PowerManagement::wokeup() {
  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT0:
      Log.info(F("Wakeup reason: external signal using RTC_IO" CR));
      break;

    case ESP_SLEEP_WAKEUP_EXT1:
      Log.info(F("Wakeup reason: external signal using RTC_CNTL" CR));
      break;

    case ESP_SLEEP_WAKEUP_TIMER:
      Log.info(F("Wakeup reason: timer (%d sec)" CR), _wakeupTime);
      break;

    case ESP_SLEEP_WAKEUP_TOUCHPAD:
      Log.info(F("Wakeup reason: touchpad" CR));
      break;

    case ESP_SLEEP_WAKEUP_ULP:
      Log.info(F("Wakeup reason: ULP program" CR));
      break;

    default:
      if (wakeup_reason == 0) {
        Log.info(F("Wakeup reason: USB Serial" CR));
      } else {
        Log.info(F("Wakeup reason: not caused by deep sleep: %d" CR),
          wakeup_reason
        );
      }
      break;
  }

  // notify others
  _wakeupCallback.callback();
}
