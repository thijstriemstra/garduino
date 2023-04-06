/*  Copyright (c) 2021-2023, Collab
 *  All rights reserved
*/
#ifndef Controls_h
#define Controls_h

#include <Arduino.h>
#include <Method.h>
#include <AceButton.h>
#include <LED_PCF8574.h>
#include <MultiPlexer_PCF8574.h>

using namespace ace_button;

class Controls
{
  public:
    Controls(MultiPlexer_PCF8574* mcp);
    void begin(
      Method manualBtnCallback,
      Method powerBtnCallback,
      Method longBtnCallback
    );
    void loop();
    void disableLEDs();

    AceButton *powerBtn;
    AceButton *manualBtn;
    LED_PCF8574 *powerLED;
    LED_PCF8574 *manualLED;
    LED_PCF8574 *networkLED;

  private:
    MultiPlexer_PCF8574* _mcp;
};

#endif
