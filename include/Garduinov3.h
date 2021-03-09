/*
  Garduinov3.h - Library for monitoring a garden.
*/
#ifndef Garduinov3_h
#define Garduinov3_h

#include <Arduino.h>
#include <Functor.h>
#include <Method.h>

#include <IOT.h>
#include <Controls.h>
#include <Sensors.h>
#include <SystemClock.h>
#include <WateringTask.h>
#include <PowerManagement.h>
#include <SSD1306_OLEDDisplay.h>
#include <MultiPlexer_MCP3008.h>
#include <MultiPlexer_TCA9548A.h>

class Garduinov3 {
  public:
      Garduinov3();
      void begin();
      void loop();
      void openValve();
      void closeValve();
      void toggleValve();
      void checkWatering();
      void startManualMode();
      void sleep(bool forced = false);

      bool started = false;
      bool connected = false;

  private:
      IOT *_iot;
      Sensors *_sensors;
      Controls *_controls;
      SystemClock *_clock;
      PowerManagement *_power;
      MultiPlexer_MCP3008 *_adc;
      MultiPlexer_TCA9548A *_ic2;
      WateringTask *_wateringTask;
      SSD1306_OLEDDisplay *_display;

      bool _manualMode = false;
      int _totalReadings = 15;
      const char *_namespace = "garduino";
      const char *_version = "3.0.0";

      // callbacks
      void onSystemWakeup();
      void onPowerButtonPush();
      void onManualButtonPush();
      void onConnectionReady();
      void onConnectionClosed();
      void onConnectionFailed();
      void onPublishReady();
      void onWateringReady();
      void onValveOpen();
      void onValveClosed();
};

#endif
