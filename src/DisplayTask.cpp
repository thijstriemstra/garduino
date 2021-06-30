/*  Copyright (c) 2021, Collab
 *  All rights reserved
*/

#include <DisplayTask.h>

DisplayTask::DisplayTask(SSD1306_OLEDDisplay_Mux* display) {
  _display = display;
}

void DisplayTask::begin() {
}

void DisplayTask::open() {
  _display->writeBig(F("Open"));
}

void DisplayTask::close() {
  _display->writeBig(F("Closed"));
}

void DisplayTask::showTime(DateTime now) {
  char timestamp[5];
  sprintf(timestamp, "%02d:%02d", now.hour(), now.minute());

  _display->writeBig(timestamp);
}

void DisplayTask::showTemperature(float temperature) {
  char buffer[9];
  char tmp[6];
  dtostrf(temperature, 4, 2, tmp);
  sprintf(buffer, "%s °C", tmp);

  _display->writeBig(buffer);
}

void DisplayTask::showInfo() {

}
