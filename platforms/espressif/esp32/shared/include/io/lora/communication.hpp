#pragma once

#include <LoRa/LoRa.h>
#include <string>

#include "../../macros.h"
#include "../../io/pin.h"

#include <string/concatenate.hpp>

#define send_message_con(...) core::network::u(lora)::send_message(utils::string::concatenate::concatenate_all(__VA_ARGS__).c_str())

namespace core {
  namespace network {
    namespace u(lora) {
      func_define(initialize, t(int8),
          func_param(ss  , core::types::io::t(pin)::t(id)),
          func_param(rst , core::types::io::t(pin)::t(id)),
          func_param(dio0, core::types::io::t(pin)::t(id))
        ) {
        LoRa.setPins(ss, rst, dio0);
        //
        for (t(uint8) temp_try = 0; temp_try < 30; ++temp_try) {
          if (LoRa.begin(433E6)) {
            LoRa.setSyncWord(0xF3);
            //
            res_succeed
          }
          // otherwise, wait and reconnect to LoRa
          vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
        //
        res_failed // :(
      }

      func_define(send_message, void,
          func_param(message, const char *),
          func_param(tries  , int, =1)
        ) {
        for (int t=0; t < tries; ++t) {
          Serial.println(message);
          LoRa.beginPacket();
          LoRa.print(message);
          LoRa.endPacket();
        }
      }

      func_define(read_message, std::string,
        ) {
        std::string message;

        auto packet_size = LoRa.parsePacket();
        if  (packet_size)
          while (LoRa.available()) {
            message = LoRa.readString().c_str();
          }
        //
        return message;
      }

      func_define(read_message_while, bool,
          func_param(message, const char *),
          func_param(tries  , int, =1)
        ) {
        for (int t=0; t < tries; ++t) {
          if (read_message() == message)
            return true;
        }
        //
        return false;
      }
    }
  }
}