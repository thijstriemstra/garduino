/*  Copyright (c) 2020-2023, Collab
 *  All rights reserved
*/

#include <Controls.h>

Controls::Controls(MultiPlexer_PCF8574* mcp) {
  _mcp = mcp;

  manualBtn = new Button(ManualRunButtonPin);
  manualLED = new LED_PCF8574(ManualRunLEDPin, _mcp);
  networkLED = new LED_PCF8574(NetworkLEDPin, _mcp);
  powerBtn = new Button(PowerButtonPin);
  powerLED = new LED_PCF8574(PowerLEDPin, _mcp);
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
  powerBtn->loop();

  // enable loop for led because it can blink
  networkLED->loop();
}

void Controls::disableLEDs() {
  manualLED->disable();
  powerLED->disable();

  networkLED->blink = false;
  networkLED->disable();
}
