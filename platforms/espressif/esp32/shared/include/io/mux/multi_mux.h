#pragma once

#include "../pin.h"

#define max_multi_mux_devices 2

namespace core {
  namespace utils {
    namespace mux {
      type_template(typename mux_type)
      class u(multi_mux) {
       private:
        member_define(_devices, mux_type *, [max_multi_mux_devices]);
      };
    }
  }
}