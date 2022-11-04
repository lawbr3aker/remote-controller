#pragma once

#include <array>

#include <Arduino.h>
#include <AnalogWrite/analogWrite.h>

#include "../macros.h"

#define pin_set_mode(...) pinMode(__VA_ARGS__)

#define pin_set_ds(...) digitalWrite(__VA_ARGS__)
#define pin_get_ds(...) digitalRead (__VA_ARGS__)
#define pin_get_as(...) analogRead  (__VA_ARGS__)

#define pin_create(id, mode) core::types::io::t(pin)(id, core::types::io::t(pin)::mode)

#define default_pwm_resolution 8
#define default_pwm_frequency  5000

namespace core {
  namespace types {
    namespace io {
      class t(pin) {
       public:
        type_define(t(id)    , t(uint8),)
        type_define(t(value) , t(int32),)
        type_define(t(mode)  ,  enum   , {input, pull_down, pull_up, output, output_pwm})
        type_define(t(status),  enum   , {low , high , toggle , none})
  
       public:
        member_define(id  , t(id)  );
        member_define(mode, t(mode));
        member_define(pwm , int, = -1); // TODO: type
  
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
      member_define(_pwm_count, static int);

     private:
      member_define(_status, types::io::t(pin)::t(status), = types::io::t(pin)::none);
      member_define(_value , types::io::t(pin)::t(value) , = 0);
     public:
      member_define(pin, types::io::t(pin));

     public:
      u(pin_controller)(
          types::io::t(pin) pin
      ) : pin(pin)
          {
        set_mode(pin.mode, true);
      }

      func_define(set_mode, void,
              func_param(mode , types::io::t(pin)::t(mode)),
              func_param(force, bool, = false)
          ) {
        if (force or pin.mode != mode) {
          if (not force and pin.mode == types::io::t(pin)::output_pwm)
            --_pwm_count;
          //
          pin.mode = mode;
          if (mode == types::io::t(pin)::input)
            ; // pin_set_mode(pin.id, INPUT);
          else if (mode == types::io::t(pin)::output)
            pin_set_mode(pin.id, OUTPUT);
          else if (mode == types::io::t(pin)::pull_down)
            pin_set_mode(pin.id, INPUT_PULLDOWN);
          else if (mode == types::io::t(pin)::pull_up)
            pin_set_mode(pin.id, INPUT_PULLUP);
          else if (mode == types::io::t(pin)::output_pwm) {
            pin.pwm = _pwm_count;
            set_pwm(default_pwm_frequency, default_pwm_resolution);
            ++_pwm_count;
          }
        }
      }

      func_define(set_pwm, void,
              func_param(frequency , int),
              func_param(resolution, int)
          ) {
        if (pin.mode == types::io::t(pin)::output_pwm) {
          ledcSetup    (pin.pwm, frequency, resolution);
          ledcAttachPin(pin.id , pin.pwm);
        }
      }

      func_define(f(get_value), types::io::t(pin)::t(value),
              func_param(update, bool, = true)
          ) {
        set_mode(pin.mode);
        //
        return update
               ? _value = pin_get_as(pin.id)
               : _value;
      };

      func_define(f(set_value), types::io::t(pin)::t(value),
              func_param(value, types::io::t(pin)::t(value))
          ) {
        if (pin.mode == types::io::t(pin)::output_pwm) {
          _value = map(value, 0, 100, 0, 255);
          //
          ledcWrite(pin.pwm, _value);
        }
        //
        return _value;
      };

      func_define(f(get_status), types::io::t(pin)::t(status),
              func_param(update, bool, = true)
          ) {
        set_mode(pin.mode);
        //
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
          if (status != types::io::t(pin)::none) {
            if (status == types::io::t(pin)::toggle)
              _status = _status == types::io::t(pin)::high ? types::io::t(pin)::low : types::io::t(pin)::high;
            else
              _status =  status;
            //
            pin_set_ds(pin.id, _status == types::io::t(pin)::high ? HIGH : LOW);
          }
        }
        //
        return _status;
      }
    };
    //
    int pin_controller::_pwm_count = 0;
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