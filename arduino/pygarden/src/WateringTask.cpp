/*
  WateringTask.cpp
*/

#include "WateringTask.h"

RTC_DATA_ATTR bool wateredToday = false;

String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

WateringTask::WateringTask(long interval, int valve_pin, String timestamp, Method finished_callback): Thread() {
  _interval = interval;
  _timestamp = timestamp;
  _lastRun = 0;
  _finishedCallback = finished_callback;

  active = false;
  enabled = false;

  // water valve
  _waterValve = new SolenoidValve(valve_pin);
}

void WateringTask::begin() {
  // water valve
  _waterValve->begin();
}

void WateringTask::start() {
    enabled = true;
    active = true;
}

void WateringTask::open() {
  // open valve
  _waterValve->start();
}

void WateringTask::close() {
  // close valve
  _waterValve->stop();
}

bool WateringTask::needsWatering() {
  String targetHour = getValue(_timestamp, ':', 0);
  String targetMinute = getValue(_timestamp, ':', 1);

  // reset flag at midnight
  if (hour() == 0) {
    if (wateredToday == true) {
      wateredToday = false;
    }
  }

  // needs watering today
  if (wateredToday == false) {
    if (hour() == targetHour.toInt()) {
      // set flag to prevent watering multiple times this hour
      wateredToday = true;
      return true;
    }
  }
  return false;
}

bool WateringTask::shouldRun(unsigned long time) {
  if (active) {
    active = false;

    // save the current time
    _lastRun = (time ? time : millis());

    Serial.println();
    Serial.println("Started watering!");
    Serial.println("---------------------------------------");
    Serial.println();

    // open valve for x seconds
    open();
  }

  // let default method check for it
  return Thread::shouldRun(time);
}

void WateringTask::run() {
  // check if time elapsed since last publish
  if (millis() > _lastRun + _interval){
    // exceeded time, disable it
    active = false;
    enabled = false;

    // close valve
    close();

    // notify others
    _finishedCallback.callback();
  }

  // run the thread
  Thread::run();
}
