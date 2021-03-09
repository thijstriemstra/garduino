/*
  WateringTask.cpp
*/

#include <WateringTask.h>

WateringTask::WateringTask(
  long duration,
  int valve_pin,
  int led_pin,
  const char* app_namespace,
  String timestamp,
  Method finished_callback,
  Method valveOpen_callback,
  Method valveClosed_callback
) {
  _duration = duration;
  _namespace = app_namespace;
  _timestamp = timestamp;
  _finishedCallback = finished_callback;
  _valveOpenCallback = valveOpen_callback;
  _valveClosedCallback = valveClosed_callback;

  active = false;
  //enabled = false;

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
    //enabled = true;
    active = true;
}

void WateringTask::open() {
  Serial.println("-----------------------");
  Serial.println("Water: valve open");

  // notify others
  _valveOpenCallback.callback();

  // enable LED
  _waterLED->enable();

  // open valve
  _waterValve->start();
}

void WateringTask::close() {
  Serial.println("-----------------------");
  Serial.println("Water: valve closed");

  // notify others
  _valveClosedCallback.callback();

  // disable LED
  _waterLED->disable();

  // close valve
  _waterValve->stop();
}

bool WateringTask::isWatering() {
    //return enabled;
    return false;
}

bool WateringTask::needsWatering(DateTime now) {
  /*
  String targetHour = getValue(_timestamp, ':', 0);
  String targetMinute = getValue(_timestamp, ':', 1);
  int currentHour = now.Hour();
  int currentMinute = now.Minute();

  // currently in part of hour for watering
  if (currentHour == targetHour.toInt() && currentMinute >= targetMinute.toInt()) {
    // compare timestamp, check if it's not today
    DateTime timestamp = load();
    if (timestamp.Day() != now.Day()) {
      // timestamp is not from today, overwrite timestamp
      // with current time and start watering
      save(now);
      return true;
    }
  }
  */
  return false;
}

bool WateringTask::shouldRun(unsigned long time) {
  if (active) {
    active = false;

    // save the current time
    _lastRun = (time ? time : millis());

    Serial.println();
    Serial.print("Started watering for ");
    Serial.print(_duration);
    Serial.println(" seconds!");
    Serial.println("---------------------------------------");
    Serial.println();

    // open valve for x seconds
    open();
  }

  // let default method check for it
  //return Thread::shouldRun(time);
  return false;
}

void WateringTask::run() {
  // check if time elapsed since last publish
  if (millis() > _lastRun + (_duration * 1000)) {
    // done watering
    active = false;
    //enabled = false;

    // close valve
    close();

    // notify others
    _finishedCallback.callback();
  }

  // run the thread
  //Thread::run();
}

void WateringTask::save(DateTime timestamp) {
  // Initialize NVS
  //esp_err_t err = nvs_flash_init();
  _prefs->begin(_namespace, false);

  // store the timestamp
  //_prefs->putUInt("timestamp", timestamp.TotalSeconds());

  // close the preferences
  _prefs->end();
}

DateTime WateringTask::load() {
  _prefs->begin(_namespace, true);

  // get the value, if the key does not exist,
  // return a default value of 0
  // note: key name is limited to 15 chars
  unsigned int timestamp = _prefs->getUInt("timestamp", 0);

  // close the preferences
  _prefs->end();

  return DateTime(timestamp);
}

String WateringTask::getLastRunTime() {

  //DateTime lastRun = load();
  char datestring[20];
  /*
  snprintf_P(datestring,
    countof(datestring),
    PSTR("%04u/%02u/%02u %02u:%02u:%02u"),
    lastRun.Year(),
    lastRun.Month(),
    lastRun.Day(),
    lastRun.Hour(),
    lastRun.Minute(),
    lastRun.Second()
  );
  */
  return datestring;
}
