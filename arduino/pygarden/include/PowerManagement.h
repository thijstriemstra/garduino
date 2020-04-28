/*
   PowerManagement.h
*/
#ifndef PowerManagement_h
#define PowerManagement_h

#include "Arduino.h"
#include <Method.h>

/* Conversion factor for micro seconds to seconds */
#define uS_TO_S_FACTOR 1000000

class PowerManagement
{
  public:
    PowerManagement(int wakeupTime = 20);
    void init(Method wakeup_callback);
    void sleep();
    void wokeup();

  private:
    Method _wakeupCallback;
    int _wakeupTime;
};

#endif