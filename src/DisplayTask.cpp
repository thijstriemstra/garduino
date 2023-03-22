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

void DisplayTask::open() {
  _openStart = _clock->now();
  _counting = true;

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

void DisplayTask::showTime() {
  DateTime now = _clock->now();
  char timestamp[5];
  sprintf(timestamp, "%02d:%02d", now.hour(), now.minute());

  _display->writeBig(timestamp, 78);

  _display->drawImage(0, 5,
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

  _display->drawImage(2, 2,
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

  _display->drawImage(0, 2,
    droplet_width,
    droplet_height,
    droplet_bits,
    false
  );
}

void DisplayTask::countdown(void *pvParameter) {
  for (;;) {
    // obtain the instance pointer
    DisplayTask* task = reinterpret_cast<DisplayTask*>(pvParameter);

    if (task->_counting) {
      TimeSpan elapsed = task->_clock->now() - task->_openStart;
      char timestamp[8];
      sprintf(timestamp, "%02d:%02d", elapsed.minutes(), elapsed.seconds());

      task->_display->writeBig(timestamp, 72);

      task->_display->drawImage(0, 4,
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
