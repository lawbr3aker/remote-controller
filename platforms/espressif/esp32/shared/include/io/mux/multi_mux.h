#pragma once

#include <vector>

#include "../pin.h"

namespace core {
  namespace utils {
    namespace mux {
      type_template(typename mux_type)
      class u(multi_mux) {
       public:
        member_define(devices, std::vector<mux_type>);
       private:
        member_define(_channel, core::types::t(pin)::t(id)   , = 0);
        member_define(_states , core::types::t(pins_set)<4> &,    );

       public:
        u(multi_mux)(
            func_param(states, core::types::t(pins_set)<4> &)
        ) : _states(states) {
        }

        func_define(add_mux, mux_type,
              core::utils::u(pin_controller) signal
            ) {
                   mux_type device(signal, _states);
          devices.push_back(device);

          return device;
        }

        func_define(set_channel, core::utils::u(pin_controller),
              func_param(channel, core::types::t(pin)::t(id))
            ) {
          auto current = _channel = channel;
          for (short state_index = 0; state_index < 4; ++state_index) {
//            _states[state_index]
//                .set_status(current%2 ? core::types::t(pin)::high
//                                      : core::types::t(pin)::low );
            current /= 2;
          }
        }

        func_define(set_channel_r, core::utils::u(pin_controller),
            ) {
          return set_channel(_channel >= 15 ? 0 : _channel + 1);
        }
      };
    }
  }
}