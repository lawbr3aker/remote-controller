#include <Arduino.h>

#include <task/pre/io/handler.hpp>
#include <task/pre/io/mux_handler.hpp>
#include <io/lora/communication.hpp>

#define m_mux_pin(id) core::types::io::pin_t(id, core::types::io::pin_t::output)
#define m_mux_handler(mux, signal, channel, callback) \
 mux->register_handler(handler::io::mux_handler_t(   \
      core::types::io::pin_t(signal, core::types::io::pin_t::pull_up), \
      handler::on_low, channel,                     \
      [](core::utils::u(pin_controller) controller) -> int8_t callback \
  ));

core::types::controllers_set_t<4> *main_mux_pins;
handler::io::mux_handler          *main_mux;

void setup() {
  namespace io = core::types::io;

  Serial.begin(115200);
  Serial.println("initialized");

  if (not core::network::lora::initialize(5, 14, 2)) {
    Serial.println("Failed!");
    unlimited_loop{};
  }

  main_mux_pins = new core::types::controllers_set_t<4>({
    m_mux_pin(0 ), // state 0
    m_mux_pin(4 ), // state 1
    m_mux_pin(16), // state 2
    m_mux_pin(17)  // state 3
  });
  main_mux = new handler::io::mux_handler(core::utils::mux::multi_mux(*main_mux_pins));

  m_mux_handler(main_mux, 32, 0 , {Serial.println("Channel 0");return 1;})
  m_mux_handler(main_mux, 32, 1 , {Serial.println("Channel 1");return 1;})
  m_mux_handler(main_mux, 32, 2 , {Serial.println("Channel 2");return 1;})
  m_mux_handler(main_mux, 32, 3 , {core::network::lora::send_message("toggle dig+pin 3" ); succeed})
  m_mux_handler(main_mux, 32, 4 , {core::network::lora::send_message("toggle dig+pin 4" ); succeed})
  m_mux_handler(main_mux, 32, 5 , {core::network::lora::send_message("toggle dig+pin 5" ); succeed})
  m_mux_handler(main_mux, 32, 6 , {core::network::lora::send_message("toggle dig+pin 6" ); succeed})
  m_mux_handler(main_mux, 32, 7 , {core::network::lora::send_message("toggle dig+pin 7" ); succeed})
  m_mux_handler(main_mux, 32, 8 , {core::network::lora::send_message("toggle dig+pin 8" ); succeed})
  m_mux_handler(main_mux, 32, 9 , {core::network::lora::send_message("toggle dig+pin 9" ); succeed})
  m_mux_handler(main_mux, 32, 10, {core::network::lora::send_message("toggle dig+pin 10"); succeed})
  m_mux_handler(main_mux, 32, 11, {core::network::lora::send_message("toggle dig+pin 11"); succeed})

  main_mux->execute();
}

void loop() {
}