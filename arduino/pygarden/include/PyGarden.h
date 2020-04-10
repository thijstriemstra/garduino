/*
  PyGarden.h - Library for monitoring a garden.
*/
#ifndef PyGarden_h
#define PyGarden_h

#include "Arduino.h"

#define VERSION "1.0.0"

class PyGarden
{
  public:
    PyGarden();
    void begin();
    void loop();
};

#endif