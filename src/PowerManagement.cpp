/*  Copyright (c) 2020-2021, Collab
 *  All rights reserved
*/
/*
  PowerManagement.cpp
*/

#include "PowerManagement.h"

/**
 * Constructor.
 */
PowerManagement::PowerManagement(int wakeupTime) {
    _wakeupTime = wakeupTime;
}

void PowerManagement::init(Method wakeup_callback) {
  _wakeupCallback = wakeup_callback;

  Serial.println(F("***********************************"));

  // print the wakeup reason
  wokeup();

  // configure the wake up sources
  esp_sleep_enable_ext0_wakeup((gpio_num_t) ManualRunButtonPin, 1);

  // wake up esp32 periodically
  esp_sleep_enable_timer_wakeup(_wakeupTime * uS_TO_S_FACTOR);

  Serial.println(F("***********************************"));
}

void PowerManagement::sleep() {
    // put esp32 into deepsleep
    esp_deep_sleep_start();
}

void PowerManagement::wokeup() {
  wakeup_reason = esp_sleep_get_wakeup_cause();

  Serial.print(F("Wakeup reason: "));
  switch (wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0:
      Serial.println(F("external signal using RTC_IO"));
      break;

    case ESP_SLEEP_WAKEUP_EXT1:
      Serial.println(F("external signal using RTC_CNTL"));
      break;

    case ESP_SLEEP_WAKEUP_TIMER:
      Serial.print(F("timer ("));
      Serial.print(_wakeupTime);
      Serial.println(F(" sec)"));
      break;

    case ESP_SLEEP_WAKEUP_TOUCHPAD:
      Serial.println(F("touchpad"));
      break;

    case ESP_SLEEP_WAKEUP_ULP:
      Serial.println(F("ULP program"));
      break;

    default:
      Serial.printf("not caused by deep sleep: %d\n", wakeup_reason);
      break;
  }

  // notify others
  _wakeupCallback.callback();
}
