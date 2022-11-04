#include <Arduino.h>
#include <HardwareSerial.h>

#include "../lib/SoftPWM/SoftPWM.h"

#define string String

#define pin_set_mode(...) pinMode(__VA_ARGS__)
//
#define pin_set_ds(...) digitalWrite(__VA_ARGS__)
#define pin_set_as(...)  analogWrite(__VA_ARGS__)

string to_first(string &base, const string& delim);

const int digital_pins[] = {2, 4, 7, 8, 12, 13, 14, 15, 16, 17, 18, 19};

HardwareSerial *communication_device;

void received(string buffer) {
  string command = to_first(buffer, " ");
  //
  if (command == "digital") {
    string sub_command = to_first(buffer, " ");
    //
    if (sub_command == "set") {
      long pin   = to_first(buffer, " ").toInt();
      long value = to_first(buffer, " ").toInt();
      // set status
      pin_set_ds(pin, value);
    }
  } else
  //
  if (command == "analog") {
    string sub_command = to_first(buffer, " ");
    //
    if (sub_command == "set") {
      long pin   = to_first(buffer, " ").toInt();
      long value = to_first(buffer, " ").toInt();
      long sign  = to_first(buffer, " ").toInt();
      // set status
      SoftPWMSetPolarity((char) pin, sign );
      SoftPWMSetPercent ((char) pin, value);
    }
  }
}

void setup() {
  //
  communication_device = &Serial;
  communication_device->begin(115200);
  //
  SoftPWMBegin();
  //
  for (const int &pin : (int []) {2, 4, 7, 8, 12, 13, 14, 15, 16, 17, 18, 19})
    pin_set_mode(pin, OUTPUT),
    pin_set_ds  (pin, 1     );
}

void loop() {
  static string buffer = "";

  char buffer_char = (char) communication_device->read();
  //
  if (buffer_char != -1) {
    if (buffer_char == '\r') {
      received(buffer);
      // append to buffer
      buffer  = "";
    } else
      buffer += buffer_char;
  }
}

string to_first(string &base, const string& delim) {
  string result;
  //
  char *token = strtok(base.begin(), delim.begin());
  if (token != nullptr) {
    result = token;
    base   = base.substring(result.length() + delim.length());
  } else {
    result = base;
    base   = "";
  }
  //
  return result;
}
