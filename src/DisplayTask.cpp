/*  Copyright (c) 2021-2023, Collab
 *  All rights reserved
*/

#include <DisplayTask.h>

DisplayTask::DisplayTask(
  SSD1306_OLEDDisplay_Mux* display,
  SystemClock* clock
) {
  _display = display;
  _clock = clock;
}

void DisplayTask::begin() {
}

void DisplayTask::open(bool countdown_duration) {
  _openStart = _clock->now();
  _counting = true;
  countdownDuration = countdown_duration;

  // start countdown task
  xTaskCreatePinnedToCore(
    &DisplayTask::countdown,   /* Task function. */
    "countdownTask",           /* String with name of task. */
    2048,                      /* Stack size in words. */
    this,                      /* Parameter passed as input of the task */
    10,                        /* Priority of the task. */
    NULL,                      /* Task handle. */
    1                          /* Core nr */
  );
}

void DisplayTask::close() {
  _counting = false;

  //_display->writeBig(F("Closed"));
}

void DisplayTask::showLogo() {
  _display->drawImage(0, 0,
    logo_width,
    logo_height,
    logo_bits,
    false
  );
}

void DisplayTask::showVersion(
  String build_date,
  String build_time,
  String version_nr
) {
  // text
  _display->writeTiny("v" + version_nr, 64, 0);
  _display->writeTiny(build_date, 64, 16, false);
}

void DisplayTask::showTime() {
  DateTime now = _clock->now();
  char timestamp[5];
  sprintf(timestamp, "%02d:%02d", now.hour(), now.minute());

  // text
  _display->writeBig(timestamp, 78);

  _display->drawImage(0, 6,
    clock_width,
    clock_height,
    clock_bits,
    false
  );
}

void DisplayTask::showTemperature(float temperature) {
  char buffer[9];
  char tmp[6];
  dtostrf(temperature, 4, 1, tmp);
  sprintf(buffer, "%s°", tmp);

  // text
  _display->writeBig(buffer, 76, 2);

  // icon
  _display->drawImage(0, 3,
    greenhouse_width,
    greenhouse_height,
    greenhouse_bits,
    false
  );
}

void DisplayTask::showHumidity(float humidity) {
  char buffer[9];
  char tmp[6];
  dtostrf(humidity, 4, 0, tmp);
  sprintf(buffer, "%s%%", tmp);

  // text
  _display->writeBig(buffer);

  // icon
  _display->drawImage(0, 4,
    droplet_width,
    droplet_height,
    droplet_bits,
    false
  );
}

void DisplayTask::showSignalStrength(int signal_strength) {
  if (signal_strength == 0) {
    _display->writeBig("None", 72);
    signal_strength = -90;

  } else {
    char buffer[8];
    char tmp[6];
    dtostrf(signal_strength, 4, 0, tmp);
    sprintf(buffer, "%s dBm", tmp);

    _display->writeSmall(buffer, 70, 6);
  }

  // icon
  if (signal_strength <= -90) {
    // bad signal strength
    _display->drawImage(0, 5,
      wifi_strength_alert_width,
      wifi_strength_alert_height,
      wifi_strength_alert_bits,
      false
    );
  } else if (signal_strength <= -80) {
    // unreliable signal strength
    _display->drawImage(0, 5,
      wifi_strength_1_width,
      wifi_strength_1_height,
      wifi_strength_1_bits,
      false
    );
  } else if (signal_strength <= -70) {
    // not a strong signal strength
    _display->drawImage(0, 5,
      wifi_strength_2_width,
      wifi_strength_2_height,
      wifi_strength_2_bits,
      false
    );
  } else if (signal_strength <= -67) {
    // reliable signal strength
    _display->drawImage(0, 5,
      wifi_strength_3_width,
      wifi_strength_3_height,
      wifi_strength_3_bits,
      false
    );
  } else {
    // good signal strength
    _display->drawImage(0, 5,
      wifi_strength_4_width,
      wifi_strength_4_height,
      wifi_strength_4_bits,
      false
    );
  }
}

void DisplayTask::showSchedule(String schedule, int duration, bool today_complete) {
  // text
  _display->setTextAlignment(TEXT_ALIGN_LEFT);
  _display->writeTiny(schedule, 38, 0);
  _display->writeTiny(String(duration) + " sec", 38, 16, false);
  _display->setTextAlignment(TEXT_ALIGN_CENTER);

  // icon
  _display->drawImage(0, 4,
    calendar_days_width,
    calendar_days_height,
    calendar_days_bits,
    false
  );

  // status icon
  if (today_complete) {
    // completed icon
    _display->drawImage(102, 12,
      circle_check_width,
      circle_check_height,
      circle_check_bits,
      false
    );
  } else {
    // uncompleted icon
    _display->drawImage(102, 12,
      circle_width,
      circle_height,
      circle_bits,
      false
    );
  }
}

void DisplayTask::showLux(float lux) {
  char buffer[8];
  char tmp[6];
  dtostrf(lux, 4, 0, tmp);
  sprintf(buffer, "%s lx", tmp);

  if (lux > 999) {
    // text
    _display->writeSmall(buffer, 74, 6);

    // icon
    _display->drawImage(0, 4,
      sun_width,
      sun_height,
      sun_bits,
      false
    );
  } else {
    if (lux < 50) {
      // text
      _display->writeBig(buffer, 62);

      // icon
      _display->drawImage(2, 7,
        moon_width,
        moon_height,
        moon_bits,
        false
      );
    } else {
      // text
      _display->writeBig(buffer, 74);

      // icon
      _display->drawImage(0, 7,
        cloud_width,
        cloud_height,
        cloud_bits,
        false
      );
    }
  }
}

void DisplayTask::showSoilMoisture(SoilMoistureResult result) {
  int icon_width;
  int icon_height;
  int moisture = 0;
  static unsigned char *icon_bits;

  switch (currentSoilSensor) {
    case 0:
      moisture = result.sensor1;
      icon_width = numeric_1_circle_width;
      icon_height = numeric_1_circle_height;
      icon_bits = numeric_1_circle_bits;
      break;
    case 1:
      moisture = result.sensor2;
      icon_width = numeric_2_circle_width;
      icon_height = numeric_2_circle_height;
      icon_bits = numeric_2_circle_bits;
      break;
    case 2:
      moisture = result.sensor3;
      icon_width = numeric_3_circle_width;
      icon_height = numeric_3_circle_height;
      icon_bits = numeric_3_circle_bits;
      break;
    case 3:
      moisture = result.sensor4;
      icon_width = numeric_4_circle_width;
      icon_height = numeric_4_circle_height;
      icon_bits = numeric_4_circle_bits;
      break;
    case 4:
      moisture = result.sensor5;
      icon_width = numeric_5_circle_width;
      icon_height = numeric_5_circle_height;
      icon_bits = numeric_5_circle_bits;
      break;
    case 5:
      moisture = result.sensor6;
      icon_width = numeric_6_circle_width;
      icon_height = numeric_6_circle_height;
      icon_bits = numeric_6_circle_bits;
      break;
    case 6:
      moisture = result.sensor7;
      icon_width = numeric_7_circle_width;
      icon_height = numeric_7_circle_height;
      icon_bits = numeric_7_circle_bits;
      break;
    case 7:
      moisture = result.sensor8;
      icon_width = numeric_8_circle_width;
      icon_height = numeric_8_circle_height;
      icon_bits = numeric_8_circle_bits;
      break;
  }

  char buffer[9];
  char tmp[7];
  dtostrf(moisture, 4, 0, tmp);
  sprintf(buffer, "%s", tmp);

  // icon
  _display->drawImage(24, 13,
    icon_width,
    icon_height,
    icon_bits
  );

  // text
  _display->setTextAlignment(TEXT_ALIGN_LEFT);
  _display->writeBig(buffer, 61, 3, false);
  _display->setTextAlignment(TEXT_ALIGN_CENTER);

  // icon
  _display->drawImage(0, 6,
    soil_width,
    soil_height,
    soil_bits,
    false
  );
}

void DisplayTask::countdown(void *pvParameter) {
  for (;;) {
    // obtain the instance pointer
    DisplayTask* task = reinterpret_cast<DisplayTask*>(pvParameter);

    if (task->_counting) {
      TimeSpan time;
      char timestamp[8];
      if (task->countdownDuration) {
        // show time left (countdown)
        time = (task->_openStart + TimeSpan(WateringDuration)) - task->_clock->now();
      } else {
        // show elapsed time (countup)
        time = task->_clock->now() - task->_openStart;
      }

      sprintf(timestamp, "%02d:%02d", time.minutes(), time.seconds());
      task->_display->writeBig(timestamp, 76);

      task->_display->drawImage(0, 6,
        shower_width,
        shower_height,
        shower_bits,
        false
      );

      // pause the task
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    } else {
      vTaskDelete(NULL);
    }
  }
}
