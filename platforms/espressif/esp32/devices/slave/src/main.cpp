#include <Arduino.h>

#include <task/pre/io/handler.hpp>
#include <task/pre/io/mux_handler.hpp>

#define mux_pin(id) core::types::io::pin_t(id, core::types::io::pin_t::output)
#define mux_handler(mux, signal, channel, callback) \
 mux.register_handler(handler::io::mux_handler_t(   \
      core::types::io::pin_t(signal, core::types::io::pin_t::pull_up), \
      handler::on_low, channel,                     \
      [](core::utils::u(pin_controller) controller) -> int8_t callback \
  ));
void test2() {
  Serial.println("called");
}
void test1() {
  Serial.println("calle1");
  test2();
}
void test(void *) {
  for (;;) {
    Serial.println("slalam,");
    vTaskDelay(1000);
    test2();
  }
}

void setup() {
  namespace io = core::types::io;
    Serial.begin(115200);
  Serial.println("initialized");
//  xTaskCreate(test, "test", 1000, nullptr, 1, nullptr);
//  namespace tp_io = core::task::pre::io;
//
  Serial.begin(115200);
  Serial.println("initialized");

  core::types::controllers_set_t<4> mux_pins = {mux_pin(0), mux_pin(4), mux_pin(16), mux_pin(17)};

  handler::io::mux_handler main_mux((core::utils::mux::multi_mux(mux_pins)));

  mux_handler(main_mux, 32, 0,  {Serial.println("Channel 0");return 1;})
  mux_handler(main_mux, 32, 1,  {Serial.println("Channel 1");return 1;})
  mux_handler(main_mux, 32, 2,  {Serial.println("Channel 2");return 1;})
  mux_handler(main_mux, 32, 3,  {Serial.println("Channel 3");return 1;})
  mux_handler(main_mux, 32, 4,  {Serial.println("Channel 4");return 1;})
  mux_handler(main_mux, 32, 5,  {Serial.println("Channel 5");return 1;})
  mux_handler(main_mux, 32, 6,  {Serial.println("Channel 6");return 1;})
  mux_handler(main_mux, 32, 7,  {Serial.println("Channel 7");return 1;})
  mux_handler(main_mux, 32, 8,  {Serial.println("Channel 8");return 1;})
  mux_handler(main_mux, 32, 9,  {Serial.println("Channel 9");return 1;})
  mux_handler(main_mux, 32, 10, {Serial.println("Channel 10");return 1;})
  mux_handler(main_mux, 32, 11, {Serial.println("Channel 11");return 1;})

  main_mux.execute();
//  handler::io::gpio_handler main_handler;
//  main_handler.register_handler(handler::io::gpio_handler_t(
//      core::types::io::pin_t(21, core::types::io::pin_t::pull_up), handler::on_low,
//      [](core::utils::u(pin_controller)) -> int8_t {
//        Serial.println("salam");
//        return 1;
//      }));
//  main_handler.execute();
//  handler::io::mux_handler a(core::types::io::pin_t(21, core::types::io::pin_t::pull_up),
//                              handler::on_low, [](void *) -> void {
//    Serial.println("clicked");
//        vTaskDelete(nullptr);
//  });
//  handler::io::mux_handler::execute();
//
//  auto pin_13_handler = create_callback(tp_io::callback_t, {
//    Serial.print("button clicked! ");
//    vTaskDelete(nullptr);
//  });
//
//  Serial.println("salam");
//  tp_io::pin_handler communication_button(io::pin_t(21, io::pin_t::pull_up),
//                                       tp_io::pin_handler::pin,
//                                       tp_io::pin_handler::on_low,
//                                       pin_13_handler);
//
//  Serial.println("bye");
//  tp_io::pin_handler::handle();
}

void loop() {
}