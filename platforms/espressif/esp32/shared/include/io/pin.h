#pragma once

#include <array>

#include <Arduino.h>
#include "../macros.h"

#define pin_set_mode(...) pinMode(__VA_ARGS__)

#define pin_set_ds(...) digitalWrite(__VA_ARGS__)
#define pin_get_ds(...) digitalRead (__VA_ARGS__)

namespace core {
  namespace types {
    namespace io {
      class t(pin) {
       public:
        type_define(t(id)    , t(uint8),)
        type_define(t(mode)  ,  enum   , {input=1, output=2, pull_down=4, pull_up=8})
        type_define(t(status),  enum   , {low , high , toggle , none})
  
       public:
        member_define(id  , const t(id)  );
        member_define(mode, const t(mode));
  
       public:
        t(pin)(
            t(id)   id  ,
            t(mode) mode
        ) : id  (id)  ,
            mode(mode)
            {
        }
      };
    }
  }

  namespace utils {
    class u(pin_controller) {
     private:
      member_define(_status, types::io::t(pin)::t(status), = types::io::t(pin)::none);
     public:
      member_define(pin, const types::io::t(pin));

     public:
      u(pin_controller)(
          types::io::t(pin) pin
      ) : pin(pin)
          {
        if (pin.mode == types::io::t(pin)::input)
          pin_set_mode(pin.id, INPUT);
        else if (pin.mode == types::io::t(pin)::output)
          pin_set_mode(pin.id, OUTPUT);
        else if (pin.mode == types::io::t(pin)::pull_down)
          pin_set_mode(pin.id, INPUT_PULLDOWN);
        else if (pin.mode == types::io::t(pin)::pull_up)
          pin_set_mode(pin.id, INPUT_PULLUP);
      }

      func_define(f(get_status), types::io::t(pin)::t(status),
              func_param(update, bool, = true)
          ) {
        return update
               ? _status = pin_get_ds(pin.id)
                 ? types::io::t(pin)::high
                 : types::io::t(pin)::low
               : _status;
      };

      func_define(f(set_status), types::io::t(pin)::t(status),
            types::io::t(pin)::t(status) status
          ) {
        if (pin.mode == types::io::t(pin)::output) {
          pin_set_mode(pin.id, OUTPUT);
          if (status != types::io::t(pin)::none) {
            if (status == types::io::t(pin)::toggle)
              pin_set_ds(pin.id, _status == types::io::t(pin)::high ? LOW : HIGH);
            else
              pin_set_ds(pin.id,  status == types::io::t(pin)::high ? HIGH : LOW);
            //
            _status = status;
          }
        }
        //
        return _status;
      }
    };
  }
}

// Aliases
namespace core {
  namespace types {
    type_template(t(size) pins)
    type_alias   (t(pins_set), std::array<types::io::t(pin), pins>)

    type_template(t(size) controllers)
    type_alias   (t(controllers_set), std::array<utils::u(pin_controller), controllers>)
  }
}