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

  Serial.println("******************************");

  // print the wakeup reason
  wokeup();

  // configure the wake up sources
  esp_sleep_enable_ext0_wakeup((gpio_num_t) ManualRunButtonPin, 1);

  // wake up esp32 periodically
  esp_sleep_enable_timer_wakeup(_wakeupTime * uS_TO_S_FACTOR);
  Serial.println("Schedule: wake up every " + String(_wakeupTime) +
      " sec");

  Serial.println("******************************");
}

void PowerManagement::sleep() {
    // put esp32 into deepsleep
    esp_deep_sleep_start();
}

void PowerManagement::wokeup() {
  wakeup_reason = esp_sleep_get_wakeup_cause();

  Serial.print("Wakeup cause: ");
  switch (wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0:
      Serial.println("external signal using RTC_IO");
      break;

    case ESP_SLEEP_WAKEUP_EXT1:
      Serial.println("external signal using RTC_CNTL");
      break;

    case ESP_SLEEP_WAKEUP_TIMER:
      Serial.println("timer");
      break;

    case ESP_SLEEP_WAKEUP_TOUCHPAD:
      Serial.println("touchpad");
      break;

    case ESP_SLEEP_WAKEUP_ULP:
      Serial.println("ULP program");
      break;

    default:
      Serial.printf("not caused by deep sleep: %d\n", wakeup_reason);
      break;
  }

  // notify others
  _wakeupCallback.callback();
}
