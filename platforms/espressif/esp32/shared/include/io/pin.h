#pragma once

#include <array>

#include <Arduino.h>
#include "../macros.h"

#define pin_set_mode(...) pinMode(__VA_ARGS__)

#define pin_set_ds(...) digitalWrite(__VA_ARGS__)
#define pin_get_ds(...) digitalRead (__VA_ARGS__)

namespace core {
  namespace types {
    class t(pin) {
     public:
      type_define(t(id)    , unsigned short,                         )
      type_define(t(mode)  ,      enum     , {input , output}        )
      type_define(t(status),      enum     , { low  ,  high , toggle, none})

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

  namespace utils {
    class u(pin_controller) {
     private:
      member_define(_status, types::t(pin)::t(status), = types::pin_t::none);
     public:
      member_define(pin, const types::t(pin));

     public:
      u(pin_controller)(
          types::t(pin) pin
      ) : pin(pin)
          {
      }

      func_define(f(get_status), types::t(pin)::t(status),
          ) {
        return (_status = pin_get_ds(pin.id) ? types::t(pin)::high : types::t(pin)::low);
      };

      func_define(f(set_status), types::t(pin)::t(status),
            types::t(pin)::t(status) status
          ) {
        if (pin.mode == types::t(pin)::output) {
          pin_set_mode(pin.id, OUTPUT);
          if (status != types::t(pin)::none) {
            if (status == types::t(pin)::toggle)
              pin_set_ds(pin.id, _status == types::t(pin)::high ? LOW : HIGH);
            else
              pin_set_ds(pin.id,  status == types::t(pin)::high ? HIGH : LOW);
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
    type_alias   (t(pins_set), std::array<types::t(pin), pins>)

    type_template(t(size) controllers)
    type_alias   (t(controllers_set), std::array<utils::u(pin_controller) *, controllers>)
  }
}