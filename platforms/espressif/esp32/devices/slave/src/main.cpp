#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

TaskHandle_t task1_h, task2_h;

void task2(void *) {
  Serial.print(xPortGetCoreID());
  Serial.println(": Called 'void task2()'");
  delay(1000);
  for (;;)
    Serial.println("task2");
}

void task1(void *) {
  Serial.print(xPortGetCoreID());
  Serial.println(": Called 'void task1()'");

  delay(1000);

  Serial.print(xPortGetCoreID());
  Serial.println(": Called 'void task1()'");

  xTaskCreatePinnedToCore(
             task2, /* Task function. */
             "Task2",   /* name of task. */
             1000,     /* Stack size of task */
             NULL,      /* parameter of the task */
             1,         /* priority of the task */
             &task2_h,    /* Task handle to keep track of created task */
             1);

  vTaskSuspend(task1_h);
}

void setup() {
  Serial.begin(115200);
  xTaskCreatePinnedToCore(
             task1, /* Task function. */
             "Task1",   /* name of task. */
             1000,     /* Stack size of task */
             NULL,      /* parameter of the task */
             1,         /* priority of the task */
             &task1_h,    /* Task handle to keep track of created task */
             0);
  delay(500);
}

bool temp = false;
void loop() {
  if (not temp) {
    Serial.print(xPortGetCoreID());
    Serial.println(": Called 'void loop()'");
    delay(1000);
  }
  temp = true;
  Serial.println("loop");
}

/*
#include <Arduino.h>

#include "../../../shared/include/io/pin.h"
#include "../../../shared/include/io/mux/mux_4bit.h"
#include "../../../shared/include/io/mux/multi_mux.h"

//#include "cores/0/io_handler.h"

core::utils::mux::multi_mux<
    core::utils::mux::mux_4bit> *mux_devices;
core::types::pins_set_t<4>      *mux_states ;

void setup() {
  using namespace core::types;
  using namespace core::utils;

  Serial.begin(115200);

  mux_states  = new pins_set_t<4>({pin_t(0, pin_t::output),
                                   pin_t(4, pin_t::output),
                                   pin_t(16, pin_t::output),
                                   pin_t(17, pin_t::output)});
  mux_devices = new mux::multi_mux<
                        core::utils::mux::mux_4bit>(*mux_states);
}
int delay_m = 1000;
void loop() {
  for (int i = 0 ; i  < 13 ; ++i) {
    mux_devices->set_channel(i);
    delay_m -= 20;
    if (delay_m <= 0)
      delay_m = 1000;
    delay(delay_m);
  }
  for (int i = 11 ; i  > -1 ; --i) {
    mux_devices->set_channel(i);
    delay_m -= 20;
    if (delay_m <= 0)
      delay_m = 1000;
    delay(delay_m);
  }
  esp32_gpioMux_t ;
}
*/
