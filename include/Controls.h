/*  Copyright (c) 2021-2023, Collab
 *  All rights reserved
*/
#ifndef Controls_h
#define Controls_h

#include <Arduino.h>
#include <Method.h>
#include <Button.h>
#include <LED_PCF8574.h>
#include <MultiPlexer_PCF8574.h>

class Controls
{
  public:
    Controls(MultiPlexer_PCF8574* mcp);
    void begin(Method manualBtnCallback, Method powerBtnCallback);
    void loop();
    void disableLEDs();

    LED_PCF8574 *powerLED;
    LED_PCF8574 *manualLED;
    LED_PCF8574 *networkLED;
    Button *powerBtn;
    Button *manualBtn;

  private:
    MultiPlexer_PCF8574* _mcp;
};

#endif
