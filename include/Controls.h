/*  Copyright (c) 2021, Collab
 *  All rights reserved
*/
#ifndef Controls_h
#define Controls_h

#include <Arduino.h>
#include <Method.h>
#include <LED.h>
#include <Button.h>

class Controls
{
    public:
        Controls();
        void begin(Method manualBtnCallback, Method powerBtnCallback);
        void loop();

        LED *powerLED;
        LED *manualLED;
        LED *networkLED;
        Button *powerBtn;
        Button *manualBtn;
};

#endif
