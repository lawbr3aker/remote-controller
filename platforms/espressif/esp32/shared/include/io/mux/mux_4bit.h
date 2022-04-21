#pragma once

#include <utility>

#include "../pin.h"

namespace core {
  namespace utils {
    namespace mux {
      class u(mux_4bit) {
       private:
        member_define(_channel, core::types::t(pin)::t(id)) = 0;
       public:
        member_define(signal, core::utils::u(pin_controller)          );
        member_define(states, core::utils::t(pin_controllers_set)<4> &);

       public:
        u(mux_4bit)(
            func_param(signal, core::utils::u(pin_controller)          ),
            func_param(states, core::utils::t(pin_controllers_set)<4> &)
        ) : signal(std::move(signal)),
            states(states) {
        }

        func_define(set_channel, core::utils::u(pin_controller),
               func_param(channel, core::types::t(pin)::t(id))
             ) {
          auto current = _channel = channel;
          for (short state_index = 0; state_index < 4; ++state_index) {
            states[state_index]
                .set_status(current%2 ? core::types::t(pin)::high
                                      : core::types::t(pin)::low );
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