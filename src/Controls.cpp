/*  Copyright (c) 2020-2023, Collab
 *  All rights reserved
*/

#include <Controls.h>

Method _manualBtnCallback;
Method _powerBtnCallback;
Method _longBtnCallback;
ButtonConfig _powerBtnConfig;
ButtonConfig _manualBtnConfig;

void handleEvent(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  uint8_t btnId = button->getId();

  if (btnId == 0) {
    switch (eventType) {
      case AceButton::kEventPressed:
        _powerBtnCallback.callback();
        break;
    }

  } else if (btnId == 1) {
    switch (eventType) {
      // interpret a Released event as a Pressed event, to distinguish it
      // from a LongPressed event
      case AceButton::kEventReleased:
        _manualBtnCallback.callback();
        break;

      case AceButton::kEventLongPressed:
        _longBtnCallback.callback();
        break;
    }
  }
}

Controls::Controls(MultiPlexer_PCF8574* mcp) {
  _mcp = mcp;

  powerBtn = new AceButton(&_powerBtnConfig, PowerButtonPin, LOW, 0);
  manualBtn = new AceButton(&_manualBtnConfig, ManualRunButtonPin, LOW, 1);

  powerLED = new LED_PCF8574(PowerLEDPin, _mcp);
  networkLED = new LED_PCF8574(NetworkLEDPin, _mcp);
  manualLED = new LED_PCF8574(ManualRunLEDPin, _mcp);
}

void Controls::begin(
  Method manualBtnCallback,
  Method powerBtnCallback,
  Method longBtnCallback
) {
  _manualBtnCallback = manualBtnCallback;
  _powerBtnCallback = powerBtnCallback;
  _longBtnCallback = longBtnCallback;

  // initialize buttons
  pinMode(PowerButtonPin, INPUT);
  pinMode(ManualRunButtonPin, INPUT);

  // configure buttons
  _manualBtnConfig.setLongPressDelay(800);
  _manualBtnConfig.setEventHandler(handleEvent);
  _manualBtnConfig.setFeature(ButtonConfig::kFeatureLongPress);
  _manualBtnConfig.setFeature(ButtonConfig::kFeatureSuppressAfterLongPress);
  _powerBtnConfig.setEventHandler(handleEvent);

  // configure leds
  manualLED->begin();
  powerLED->begin();
  networkLED->begin();
}

void Controls::loop() {
  manualBtn->check();
  powerBtn->check();

  // enable loop for led because it can blink
  networkLED->loop();
}

void Controls::disableLEDs() {
  manualLED->disable();
  powerLED->disable();
  networkLED->disable();
}
