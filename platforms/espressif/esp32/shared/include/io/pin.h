#pragma once

#include <array>

#include <Arduino.h>
#include "../macros.h"

namespace core {
  namespace types {
    class t(pin) {
     public:
      type_define(t(id)    , unsigned short,                 )
      type_define(t(mode)  ,      enum     , {input , output})
      type_define(t(status),      enum     , { low  ,  high })

     public:
      member_define(id  , const t(id)  );
      member_define(mode, const t(mode));

     public:
      t(pin)(
          t(id)   id  ,
          t(mode) mode = input
      ) : id  (id)  ,
          mode(mode)
          {
      }
    };
  }

  namespace utils {
    class u(pin_controller) {
     public:
      member_define(pin, const types::t(pin));

     public:
      u(pin_controller)(
          types::t(pin) pin
      ) : pin(pin)
          {
      }

      func_define(f(get_status), types::t(pin)::t(status),
          ) const {
        return digitalRead(pin.id) ? types::t(pin)::high : types::t(pin)::low;
      };

      func_define(f(set_status), void,
              types::t(pin)::t(status) status
          ) const {
        digitalWrite(pin.id, status);
      }
    };

    type_template(t(size) contorllers_count)
    type_alias   (t(pin_controllers_set), std::array<u(pin_controller), contorllers_count>)
  }
}
