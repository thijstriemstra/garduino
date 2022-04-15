/*  Copyright (c) 2020-2022, Collab
 *  All rights reserved
*/

#include <Controls.h>

Controls::Controls() {
  manualBtn = new Button(ManualRunButtonPin);
  manualLED = new LED(ManualRunLEDPin);
  networkLED = new LED(NetworkLEDPin);
  powerBtn = new Button(PowerButtonPin);
  powerLED = new LED(PowerLEDPin);
}

void Controls::begin(Method manualBtnCallback, Method powerBtnCallback) {
  manualBtn->begin(manualBtnCallback);
  manualLED->begin();
  powerBtn->begin(powerBtnCallback);
  powerLED->begin();
  networkLED->begin();
}

void Controls::loop() {
  manualBtn->loop();
  manualLED->loop();
  powerBtn->loop();
  powerLED->loop();
  networkLED->loop();
}
