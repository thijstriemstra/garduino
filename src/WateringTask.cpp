/*  Copyright (c) 2020-2021, Collab
 *  All rights reserved
*/
/*
  WateringTask.cpp
*/

#include <WateringTask.h>

WateringTask::WateringTask(
  long task_duration,
  int valve_pin,
  int led_pin,
  const char* app_namespace,
  String timestamp,
  Method finished_callback,
  Method valveOpen_callback,
  Method valveClosed_callback
) {
  duration = task_duration;

  _namespace = app_namespace;
  _timestamp = timestamp;
  _finishedCallback = finished_callback;
  _valveOpenCallback = valveOpen_callback;
  _valveClosedCallback = valveClosed_callback;

  // preferences storage
  _prefs = new Preferences();

  // indication LED
  _waterLED = new LED(led_pin);

  // water valve
  _waterValve = new SolenoidValve(valve_pin);
}

void WateringTask::begin() {
  // indication LED
  _waterLED->begin();

  // water valve
  _waterValve->begin();
}

void WateringTask::start() {
  active = true;

  // start task on core 1, otherwise it will corrupt the OLED display
  // see https://github.com/ThingPulse/esp8266-oled-ssd1306/issues/326
  xTaskCreatePinnedToCore(
    &WateringTask::setupTask,  // function that should be called
    "wateringTask",            // name of the task (for debugging)
    2048,                      // stack size (bytes)
    this,                      // passing instance pointer as function param
    1,                         // task priority
    NULL,                      // task handle
    1                          // core to run the task on (0 or 1)
  );
}

void WateringTask::setupTask(void *pvParameter) {
  // obtain the instance pointer
  WateringTask* task = reinterpret_cast<WateringTask*>(pvParameter);

  // delay start of task
  vTaskDelay(1000 / portTICK_PERIOD_MS);

  // run forest, run
  task->run();

  // pause the task and leave pump open
  vTaskDelay(task->duration * 1000 / portTICK_PERIOD_MS);

  // done watering
  task->active = false;

  // close valve
  task->close();

  // notify others
  task->_finishedCallback.callback();

  // cleanup task
  vTaskDelete(NULL);
}

void WateringTask::run() {
  Log.info(CR);
  Log.info(F("**************************************" CR));
  Log.info(F("* Started watering for %l seconds!   *" CR), duration);
  Log.info(F("**************************************" CR));
  Log.info(CR);

  // open valve for x seconds
  open();
}

void WateringTask::open() {
  Log.info(F("-----------------------" CR));
  Log.info(F("Water: valve open" CR));

  // notify others
  _valveOpenCallback.callback();

  // enable LED
  _waterLED->enable();

  // open valve
  _waterValve->start();
}

void WateringTask::close() {
  Log.info(F("-----------------------" CR));
  Log.info(F("Water: valve closed" CR));

  // notify others
  _valveClosedCallback.callback();

  // disable LED
  _waterLED->disable();

  // close valve
  _waterValve->stop();
}

bool WateringTask::isWatering() {
  return active;
}

bool WateringTask::isValveOpen() {
  return _waterValve->active;
}

bool WateringTask::needsWatering(DateTime now) {
  long targetHour = Utils::splitHourString(_timestamp, ':', 0);
  long targetMinute = Utils::splitHourString(_timestamp, ':', 1);
  int currentHour = now.hour();
  int currentMinute = now.minute();

  // currently in part of hour for watering
  if (currentHour == targetHour && currentMinute >= targetMinute) {
    // compare timestamp, check if it's not today
    DateTime timestamp = load();
    if (timestamp.day() != now.day()) {
      // timestamp is not from today, overwrite timestamp
      // with current time and start watering
      save(now);
      return true;
    }
  }

  return false;
}

void WateringTask::save(DateTime timestamp) {
  _prefs->begin(_namespace, false);

  // store the timestamp
  _prefs->putUInt(TIMESTAMP, timestamp.secondstime());

  // close the preferences
  _prefs->end();
}

DateTime WateringTask::load() {
  _prefs->begin(_namespace, true);

  // get the value, if the key does not exist,
  // return a default value of 0
  unsigned int timestamp = _prefs->getUInt(TIMESTAMP, 0);

  // close the preferences
  _prefs->end();

  return DateTime(SECONDS_FROM_1970_TO_2000 + timestamp);
}

String WateringTask::getLastRunTime() {
  DateTime lastRun = load();

  return lastRun.timestamp(DateTime::TIMESTAMP_DATE) + " " +
    lastRun.timestamp(DateTime::TIMESTAMP_TIME);
}
