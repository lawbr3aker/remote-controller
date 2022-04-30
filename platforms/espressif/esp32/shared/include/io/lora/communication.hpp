#pragma once

#include <LoRa/LoRa.h>
#include <string>

#include "../../macros.h"
#include "../../io/pin.h"

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
            succeed
          }
          // otherwise, wait and reconnect to LoRa
          vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
        //
        failed // :(
      }

      func_define(send_message, void,
          func_param(message, const char *)
        ) {
        LoRa.beginPacket();
        LoRa.print(message);
        LoRa.endPacket();
      }
    }
  }
}