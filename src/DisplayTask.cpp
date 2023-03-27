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
  _display->setTextAlignment(TEXT_ALIGN_LEFT);

  _display->writeTiny(build_date, 0, 0);
  _display->writeSmall("v" + version_nr, 70, 12, false);

  _display->setTextAlignment(TEXT_ALIGN_CENTER);
}

void DisplayTask::showTime() {
  DateTime now = _clock->now();
  char timestamp[5];
  sprintf(timestamp, "%02d:%02d", now.hour(), now.minute());

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

  _display->writeBig(buffer, 70);

  _display->drawImage(2, 5,
    temperature_three_quarters_width,
    temperature_three_quarters_height,
    temperature_three_quarters_bits,
    false
  );
}

void DisplayTask::showHumidity(float humidity) {
  char buffer[9];
  char tmp[6];
  dtostrf(humidity, 4, 0, tmp);
  sprintf(buffer, "%s%%", tmp);

  _display->writeBig(buffer);

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
  } else {
    char buffer[8];
    char tmp[6];
    dtostrf(signal_strength, 4, 0, tmp);
    sprintf(buffer, "%s dBm", tmp);

    _display->writeSmall(buffer, 70, 6);
  }

  _display->drawImage(0, 7,
    wifi_width,
    wifi_height,
    wifi_bits,
    false
  );
}

void DisplayTask::showSchedule(String schedule, int duration, bool today_complete) {
  _display->setTextAlignment(TEXT_ALIGN_LEFT);
  _display->writeTiny(schedule, 38, 0);
  _display->writeTiny(String(duration) + " sec", 38, 16, false);
  _display->setTextAlignment(TEXT_ALIGN_CENTER);

  _display->drawImage(0, 4,
    calendar_days_width,
    calendar_days_height,
    calendar_days_bits,
    false
  );

  if (today_complete) {
    _display->drawImage(100, 10,
      circle_check_width,
      circle_check_height,
      circle_check_bits,
      false
    );
  } else {
    _display->drawImage(100, 10,
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
    _display->writeSmall(buffer, 74, 6);

    _display->drawImage(0, 4,
      sun_width,
      sun_height,
      sun_bits,
      false
    );
  } else {
    if (lux < 50) {
      _display->writeBig(buffer, 62);

      _display->drawImage(0, 7,
        moon_width,
        moon_height,
        moon_bits,
        false
      );
    } else {
      _display->writeBig(buffer, 74);

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
  int moisture = 0;
  switch (currentSoilSensor) {
    case 0:
      moisture = result.sensor1;
      break;
    case 1:
      moisture = result.sensor2;
      break;
    case 2:
      moisture = result.sensor3;
      break;
    case 3:
      moisture = result.sensor4;
      break;
    case 4:
      moisture = result.sensor5;
      break;
    case 5:
      moisture = result.sensor6;
      break;
    case 6:
      moisture = result.sensor7;
      break;
    case 7:
      moisture = result.sensor8;
      break;
  }

  char buffer[9];
  char tmp[7];
  dtostrf(moisture, 4, 0, tmp);
  sprintf(buffer, "%s", tmp);

  _display->writeSmall(String(currentSoilSensor + 1), 38, 11);

  _display->setTextAlignment(TEXT_ALIGN_LEFT);
  _display->writeBig(buffer, 60, 0, false);
  _display->setTextAlignment(TEXT_ALIGN_CENTER);

  _display->drawImage(0, 4,
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
