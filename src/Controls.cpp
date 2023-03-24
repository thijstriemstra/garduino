/*  Copyright (c) 2020-2023, Collab
 *  All rights reserved
*/

#include <Controls.h>

Method _manualBtnCallback;

void btnCallback(Button2& btn) {
  int id = btn.getID();

  if (id == 0) {
    _manualBtnCallback.callback();
  }
}

void longClickCallback(Button2& btn) {
  Serial.print("long click #");
  Serial.print(btn.getLongClickCount());
  Serial.println(" detected");
}

Controls::Controls(MultiPlexer_PCF8574* mcp) {
  _mcp = mcp;

  manualBtn = new Button2();
  manualBtn->setID(0);
  manualBtn->setDebounceTime(50);
  manualBtn->setLongClickTime(800);
  manualBtn->setClickHandler(btnCallback);
  manualBtn->setLongClickDetectedHandler(longClickCallback);

  powerBtn = new Button(PowerButtonPin);
  manualLED = new LED_PCF8574(ManualRunLEDPin, _mcp);
  networkLED = new LED_PCF8574(NetworkLEDPin, _mcp);
  powerLED = new LED_PCF8574(PowerLEDPin, _mcp);
}

void Controls::begin(Method manualBtnCallback, Method powerBtnCallback) {
  _manualBtnCallback = manualBtnCallback;

  manualBtn->begin(ManualRunButtonPin, INPUT, false);
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
