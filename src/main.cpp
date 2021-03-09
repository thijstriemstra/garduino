/*
* Copyright (c) 2020-2021 Collab
* All rights reserved
*/

#include "Garduinov3.h"
#define SERIAL_BAUD_RATE 115200

const int led1 = 22;

TaskHandle_t *task1Handle = NULL;
TaskHandle_t *task2Handle = NULL;

//Garduinov3 *app = new Garduinov3();

void toggleLED(void *parameter) {
  // infinite loop
  for (;;) {
    // Turn the LED on
    digitalWrite(led1, HIGH);

    // Pause the task
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    // Turn the LED off
    digitalWrite(led1, LOW);

    // Pause the task again
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    Serial.print(millis());
    Serial.print(" - toggleLED is running on core: ");
    Serial.println(xPortGetCoreID());
  }
}

void task2(void *parameter) {
  // infinite loop
  for (;;)
  {
    Serial.print(millis());
    Serial.print(" - task2 is running on core: ");
    Serial.println(xPortGetCoreID());

    // Pause the task again
    vTaskDelay(4000 / portTICK_PERIOD_MS);
  }
}

void setup() {
  // start serial connection
  Serial.begin(SERIAL_BAUD_RATE);

  //app->begin();
  pinMode(led1, OUTPUT);

  xTaskCreatePinnedToCore(
      toggleLED,    // Function that should be called
      "Toggle LED", // Name of the task (for debugging)
      1000,         // Stack size (bytes)
      NULL,         // Parameter to pass
      1,            // Task priority
      task1Handle,  // Task handle
      0             // Core you want to run the task on (0 or 1)
  );

  xTaskCreatePinnedToCore(
      task2,        // Function that should be called
      "Task 2", // Name of the task (for debugging)
      1000,         // Stack size (bytes)
      NULL,         // Parameter to pass
      1,            // Task priority
      task2Handle,  // Task handle
      1             // Core you want to run the task on (0 or 1)
  );
}

void loop() {
  //app->loop();
}
