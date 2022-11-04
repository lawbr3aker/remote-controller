#pragma once

#include <vector>

#include "../pin.h"

namespace core {
  namespace utils {
    namespace mux {
      class u(multi_mux) {
       public:
        member_define(signals, std::vector<core::utils::u(pin_controller)>);
       public:
        member_define(_channel, core::types::io::t(pin)::t(id)        , = -1);
        member_define(_states , core::types::    t(controllers_set)<4>,     );

       public:
        u(multi_mux)(
            func_param(states, core::types::t(controllers_set)<4>)
        ) : _states(states) {
        }

        func_define(add_mux, void,
              core::utils::u(pin_controller) signal
            ) {
          signals.push_back(signal);
        }

        func_define(get_channel, core::types::io::t(pin)::t(id),
            ) const {
          return _channel;
        }

        func_define(set_channel, void,
              func_param(channel, core::types::io::t(pin)::t(id))
            ) {
          _channel = channel;
          for (short state_index = 0; state_index < 4; ++state_index) {
            _states[state_index]
                .set_status(channel%2 ? core::types::io::t(pin)::high
                                      : core::types::io::t(pin)::low );
            channel /= 2;
          }
        }

        func_define(set_channel_r, void,
            ) {
          return set_channel(_channel >= 15 ? 0 : _channel + 1);
        }
      };
    }
  }
}