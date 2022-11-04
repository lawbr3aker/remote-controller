#include <Arduino.h>

#include <HardwareSerial.h>

#include <io/lora/communication.hpp>
#include <io/pin.h>
#include <serial_communication.hpp>
#include <string/split.hpp>
#include <string/concatenate.hpp>

#include <task/cycle/cycle.hpp>

#include <string>

#define number_update(var, val, min, max) if ((var) + (val) <= (max) and (var) + (val) >= (min)) (var) += (val);
#define string_2int(string) std::strtol(string, (char **) new char, 10)

#define send_digital_stat(device, pin, value)                \
  device->print(utils::string::concatenate::concatenate_all( \
    "digital set ", pin, " ", value, "\r").c_str());
#define send_analog_stat(device, pin, value, sign)           \
  device->print(utils::string::concatenate::concatenate_all( \
    "analog set ", pin, " ", value, " ", sign, "\r").c_str());

// virtual pins
std::vector<virtual_digital_pin> virtual_digital_pins;
std::vector<virtual_digital_pin> virtual_analog_pins ;
// local pins
std::vector<core::utils::pin_controller> local_analog_pins;

core::utils::pin_controller *led_status;
// led cycles
core::task::cycle::cycle_t  *led_status_running      ;
core::task::cycle::cycle_t  *led_status_wait_for_pair;
core::task::cycle::cycle_t  *led_status_device_found ;

core::utils::pin_controller *button_status;
core::utils::pin_controller *button_stop  ;

core::utils::pin_controller *stat_digital;
core::utils::pin_controller *stat_analog ;
core::utils::pin_controller *stat_both   ;

HardwareSerial *communication_device;

SemaphoreHandle_t mutex_1 = nullptr;

void init_cycles();
void init_pair  ();
void eme_off    ();

void setup() {
  mutex_1 = xSemaphoreCreateMutex();
  // initialize: cycles
  init_cycles();

  // initialize: status led
  led_status = new core::utils::pin_controller(core::types::io::pin_t(13, core::types::io::pin_t::output));

  // initialize: status button
  button_status = new core::utils::pin_controller(core::types::io::pin_t(21, core::types::io::pin_t::pull_up));
  // initialize: stop button
  button_stop   = new core::utils::pin_controller(core::types::io::pin_t(25, core::types::io::pin_t::pull_up));

  // initialize: stat outputs
  stat_digital = new core::utils::pin_controller(core::types::io::pin_t(4 , core::types::io::pin_t::output));
  stat_analog  = new core::utils::pin_controller(core::types::io::pin_t(16, core::types::io::pin_t::output));
  stat_both    = new core::utils::pin_controller(core::types::io::pin_t(17, core::types::io::pin_t::output));
  // default set status
  stat_digital->set_status(core::types::io::pin_t::high);
  stat_analog ->set_status(core::types::io::pin_t::high);
  stat_both   ->set_status(core::types::io::pin_t::high);

  // initialize: lora
  if (not core::network::lora::initialize(5, 14, 2)) {
    // status: error
    led_status->set_status(core::types::io::pin_t::high);
    // hang on!
    unlimited_loop{}
  }

  // status: pairing!
//  init_pair();

  // initialize: communication device on serial
  communication_device = &Serial;
  communication_device->begin(115200);

  // initialize: virtual digital pins
  virtual_digital_pins.emplace_back(virtual_digital_pin{2 , 1});
  virtual_digital_pins.emplace_back(virtual_digital_pin{4 , 1});
  virtual_digital_pins.emplace_back(virtual_digital_pin{7 , 1});
  virtual_digital_pins.emplace_back(virtual_digital_pin{8 , 1});
  virtual_digital_pins.emplace_back(virtual_digital_pin{12, 1});
  virtual_digital_pins.emplace_back(virtual_digital_pin{13, 1});
  virtual_digital_pins.emplace_back(virtual_digital_pin{14, 1});
  virtual_digital_pins.emplace_back(virtual_digital_pin{15, 1});
  virtual_digital_pins.emplace_back(virtual_digital_pin{16, 1});
  virtual_digital_pins.emplace_back(virtual_digital_pin{17, 1});
  virtual_digital_pins.emplace_back(virtual_digital_pin{18, 1});
  virtual_digital_pins.emplace_back(virtual_digital_pin{19, 1});
  // initialize: virtual analog pins
  virtual_analog_pins.emplace_back(virtual_digital_pin{3 , 0});
  virtual_analog_pins.emplace_back(virtual_digital_pin{5 , 0});
  virtual_analog_pins.emplace_back(virtual_digital_pin{6 , 0});
  virtual_analog_pins.emplace_back(virtual_digital_pin{9 , 0});
  virtual_analog_pins.emplace_back(virtual_digital_pin{10, 0});
  virtual_analog_pins.emplace_back(virtual_digital_pin{11, 0});
  // initialize: local analog pins
  local_analog_pins.emplace_back(pin_create(0 , output_pwm));
  local_analog_pins.emplace_back(pin_create(12, output_pwm));
  local_analog_pins.emplace_back(pin_create(27, output_pwm));
  local_analog_pins.emplace_back(pin_create(26, output_pwm));
  local_analog_pins.emplace_back(pin_create(33, output_pwm));
  local_analog_pins.emplace_back(pin_create(32, output_pwm));

  // status: initialized
  led_status_running->execute();
  Serial.println("ok");
}

void parse(void *param) {
  std::string data = *(std::string *) param;

  if (xSemaphoreTake(mutex_1, portMAX_DELAY) == pdTRUE) {
    auto command = utils::string::split::to_first(data, " ");
    //
    if (command == "channel") {
      auto sub_command = utils::string::split::to_first(data, " ");
      //
      if (sub_command == "toggle") {
        auto pin  = string_2int(utils::string::split::to_first(data, " ").c_str());
        //
        auto &pin_ref = virtual_digital_pins[pin - 1];
              pin_ref.value = not pin_ref.value;
        //
        if (pin_ref.value) {
          send_digital_stat(communication_device, pin_ref.id, 1)
        } else {
          send_digital_stat(communication_device, pin_ref.id, 0)
        }
      }
    } else if (command == "analog") {
      auto sub_command = utils::string::split::to_first(data, " ");
      //
      if (sub_command == "change") {
        auto pin   = string_2int(utils::string::split::to_first(data, " ").c_str());
        auto value = string_2int(utils::string::split::to_first(data, " ").c_str());
        //
        auto percentage = map(value, 0, 4096, 0, 100);
        //
        if (pin - 1 < virtual_analog_pins.size()) {
          auto &pin_ref = virtual_analog_pins[pin - 1];
                pin_ref.value = value;
          //
          if (value > 200)
            send_analog_stat(communication_device, pin_ref.id, percentage, 0)
          else
            send_analog_stat(communication_device, pin_ref.id, 0, 0)
        } else {
          auto &pin_ref = local_analog_pins[pin - virtual_analog_pins.size() - 1];
          //
          if (value > 200)
            pin_ref.set_value(percentage);
          else
            pin_ref.set_value(0);
        }
      }
    }
    xSemaphoreGive(mutex_1);
  }

  // create current task
  vTaskDelete(nullptr);
}

void loop() {
  if (button_stop->get_status() == core::types::io::pin_t::low) {
    eme_off();
    // pause micro controller
    unlimited_loop;
  }

  auto data = core::network::lora::read_message();
  if  (data.length()) {
    //
    auto parse_handle = new TaskHandle_t;
    auto parse_arg    = new std::string(data);
    if (xTaskCreate(parse       , "parse", 5000,
                    parse_arg   , 2,
                    parse_handle))
      ; // TODO:
  }

  // update stat outputs
  stat_digital->set_status(core::types::io::pin_t::high);
  for (auto &digital_pin: virtual_digital_pins) {
    if (digital_pin.value == 0)
      stat_digital->set_status(core::types::io::pin_t::low);
  }
}

void eme_off() {
  core::task::cycle::cycle_t::end_all();
  //
  for (auto &digital_pin : virtual_digital_pins)
    send_digital_stat(communication_device, digital_pin.id, 1)
  for (auto &analog_pin : virtual_analog_pins)
    send_analog_stat(communication_device, analog_pin.id, 0, 0)
  //
  stat_digital->set_status(core::types::io::pin_t::high);
  stat_analog ->set_status(core::types::io::pin_t::high);
  stat_both   ->set_status(core::types::io::pin_t::high);
  // status: program stopped
  led_status->set_status(core::types::io::pin_t::high);
}

void init_pair() {
  // status: waiting for new connection
  led_status_wait_for_pair->execute();
  //
  bool paired = false;
  // begin for pair
  do {
    if (core::network::lora::read_message_while("pair request", 4)) {
      core::network::lora::send_message("pair wait", 10);
      //
      led_status_device_found ->execute();
      //
      do {
        if (button_status->get_status() == core::types::io::pin_t::low) {
          core::network::lora::send_message("pair accept");
          //
          led_status_wait_for_pair->execute();
          //
          while (not core::network::lora::read_message_while("pair accept", 4));
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