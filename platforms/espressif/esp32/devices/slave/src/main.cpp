#include <Arduino.h>

#include <task/pre/io/mux_handler.hpp>
#include <io/lora/communication.hpp>

#include <task/cycle/cycle.hpp>

#define m_mux_pin(id) core::types::io::pin_t(id, core::types::io::pin_t::output)
//
#define m_mux_handler(mux, signal, mode, type, channel, callback)      \
 mux->register_handler(handler::io::mux_handler_t(                     \
      core::utils::pin_controller(core::types::io::pin_t(signal, core::types::io::pin_t::mode)), \
      handler::type, channel,                                    \
      [](core::utils::u(pin_controller) controller) -> int8_t callback \
  ));

core::types::controllers_set_t<4> *main_mux_pins;
handler::io::mux_handler          *main_mux;

core::utils::pin_controller *led_status;
// led cycles
core::task::cycle::cycle_t  *led_status_running      ;
core::task::cycle::cycle_t  *led_status_wait_for_pair;
core::task::cycle::cycle_t  *led_status_device_found ;

core::utils::pin_controller *button_status;

void init_cycles();
void init_pair  ();

void setup() {
  Serial.begin(115200);
  Serial.println("salve");
  // initialize: cycles
  init_cycles();

  // initialize: status led
  led_status = new core::utils::pin_controller(core::types::io::pin_t(13, core::types::io::pin_t::output));

  // initialize: button 1
  button_status = new core::utils::pin_controller(core::types::io::pin_t(21, core::types::io::pin_t::pull_up));

  // initialize: lora
  if (not core::network::lora::initialize(5, 14, 2)) {
    // status: error
    led_status->set_status(core::types::io::pin_t::high);
    // hang on!
    unlimited_loop {}
  }

  // status: pairing!
//  init_pair();

  // initialize: mux_handler
  main_mux_pins = new core::types::controllers_set_t<4>({
    m_mux_pin(0 ), // state 0
    m_mux_pin(4 ), // state 1
    m_mux_pin(16), // state 2
    m_mux_pin(17)  // state 3
  });
  main_mux = new handler::io::mux_handler(core::utils::mux::multi_mux(*main_mux_pins));

  using namespace core::network::lora;

  // initialize: button handlers
  m_mux_handler(main_mux, 32, pull_up, on_low, 0 , {send_message("channel toggle 1" ); res_succeed})
  m_mux_handler(main_mux, 32, pull_up, on_low, 1 , {send_message("channel toggle 2" ); res_succeed})
  m_mux_handler(main_mux, 32, pull_up, on_low, 2 , {send_message("channel toggle 3" ); res_succeed})
  m_mux_handler(main_mux, 32, pull_up, on_low, 3 , {send_message("channel toggle 4" ); res_succeed})
  m_mux_handler(main_mux, 32, pull_up, on_low, 4 , {send_message("channel toggle 5" ); res_succeed})
  m_mux_handler(main_mux, 32, pull_up, on_low, 5 , {send_message("channel toggle 6" ); res_succeed})
  m_mux_handler(main_mux, 32, pull_up, on_low, 6 , {send_message("channel toggle 7" ); res_succeed})
  m_mux_handler(main_mux, 32, pull_up, on_low, 7 , {send_message("channel toggle 8" ); res_succeed})
  m_mux_handler(main_mux, 32, pull_up, on_low, 8 , {send_message("channel toggle 9" ); res_succeed})
  m_mux_handler(main_mux, 32, pull_up, on_low, 9 , {send_message("channel toggle 10"); res_succeed})
  m_mux_handler(main_mux, 32, pull_up, on_low, 10, {send_message("channel toggle 11"); res_succeed})
  m_mux_handler(main_mux, 32, pull_up, on_low, 11, {send_message("channel toggle 12"); res_succeed})
  m_mux_handler(main_mux, 32, pull_up, on_low, 12, {send_message("channel toggle 13"); res_succeed})
  m_mux_handler(main_mux, 32, pull_up, on_low, 13, {send_message("channel toggle 14"); res_succeed})
  m_mux_handler(main_mux, 32, pull_up, on_low, 14, {send_message("channel toggle 15"); res_succeed})
  m_mux_handler(main_mux, 32, pull_up, on_low, 15, {send_message("channel toggle 16"); res_succeed})

  // initialize: analog handlers
  m_mux_handler(main_mux, 33, input, on_value, 0  , {send_message_con("analog change 1 " , controller.get_value(false)); res_succeed})
//  m_mux_handler(main_mux, 33, input, on_value, 1  , {send_message_con("analog change 2 " , controller.get_value(false)); res_succeed})
//  m_mux_handler(main_mux, 33, input, on_value, 2  , {send_message_con("analog change 3 " , controller.get_value(false)); res_succeed})
//  m_mux_handler(main_mux, 33, input, on_value, 3  , {send_message_con("analog change 4 " , controller.get_value(false)); res_succeed})
//  m_mux_handler(main_mux, 33, input, on_value, 4  , {send_message_con("analog change 5 " , controller.get_value(false)); res_succeed})
//  m_mux_handler(main_mux, 33, input, on_value, 5  , {send_message_con("analog change 6 " , controller.get_value(false)); res_succeed})
//  m_mux_handler(main_mux, 33, input, on_value, 6  , {send_message_con("analog change 7 " , controller.get_value(false)); res_succeed})
//  m_mux_handler(main_mux, 33, input, on_value, 7  , {send_message_con("analog change 8 " , controller.get_value(false)); res_succeed})
//  m_mux_handler(main_mux, 33, input, on_value, 8  , {send_message_con("analog change 9 " , controller.get_value(false)); res_succeed})
//  m_mux_handler(main_mux, 33, input, on_value, 9  , {send_message_con("analog change 10 ", controller.get_value(false)); res_succeed})
  m_mux_handler(main_mux, 33, input, on_value, 10 , {send_message_con("analog change 11 ", controller.get_value(false)); res_succeed})
//  m_mux_handler(main_mux, 33, input, on_value, 11 , {send_message_con("analog change 12 ", controller.get_value(false)); res_succeed})

  // execute: all mux handlers
  main_mux->execute();

  // status: initialized
  led_status_running->execute();
}

void loop() {
}

void init_pair() {
  // status: waiting for new connection
  led_status_wait_for_pair->execute();
  //
  bool paired = false;
  // begin for pair
  core::network::lora::send_message("pair request", 2);
  do {
    delay(1);
    if (core::network::lora::read_message_while("pair wait", 4)) {
      while (not core::network::lora::read_message_while("pair accept", 4));
      //
      led_status_device_found ->execute();
      //
      do {
        if (button_status->get_status()==core::types::io::pin_t::low) {
          core::network::lora::send_message("pair accept");
          //
          paired = true;
        }
      } while (not paired);
    }
  } while (not paired);
}

void init_cycles() {
  led_status_running = new core::task::cycle::cycle_t(core::task::cycle::pulse_vec_t({
    core::task::cycle::pulse_t([]() -> int8_t {
      led_status->set_status(core::types::io::pin_t::toggle); res_succeed
    }, {.total=700})
  }));

  led_status_wait_for_pair = new core::task::cycle::cycle_t(core::task::cycle::pulse_vec_t({
    core::task::cycle::pulse_t([]() -> int8_t {
      led_status->set_status(core::types::io::pin_t::toggle); delay(50  ); // on
      led_status->set_status(core::types::io::pin_t::toggle); delay(100 ); // off
      led_status->set_status(core::types::io::pin_t::toggle); delay(100 ); // on
      led_status->set_status(core::types::io::pin_t::toggle); delay(50  ); // off
      led_status->set_status(core::types::io::pin_t::toggle); delay(100 ); // on
      led_status->set_status(core::types::io::pin_t::toggle); delay(50  ); // off
      res_succeed
    }, {.total=700})
  }));

  led_status_device_found = new core::task::cycle::cycle_t(core::task::cycle::pulse_vec_t({
    core::task::cycle::pulse_t([]() -> int8_t {
      led_status->set_status(core::types::io::pin_t::toggle);
      res_succeed
    }, {.total=150})
  }));
}