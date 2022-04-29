#pragma once

#include <utility>

#include "vector"

#include "../../../macros.h"
#include "../../../io/pin.h"

namespace handler {
  type_define(t(handle_mode), enum,
      enumeration(
        on_low   ,
        on_high  ,
        on_change
      )
    )

  namespace io {
    type_declare(t(gpio_handler)) {
     public:
      typedef t(int8) (*t(callback))(core::utils::u(pin_controller));

     public:
      member_define(gpio_controller, core::utils::u(pin_controller));
      member_define(gpio_mode      ,     handler::t(handle_mode)   );
      member_define(callback       ,              t(callback)      );

     public:
      t(gpio_handler) (
          func_param(controller, decltype(gpio_controller)),
          func_param(mode      , decltype(gpio_mode)),
          func_param(callback  , decltype(callback))
      ) : gpio_controller(std::move(controller)),
          gpio_mode      (mode),
          callback       (callback) {
      }

      func_define(call, t(TaskHandle),
        ) const {
        auto caller = [](void * param_p) {
          auto const &self = *(t(gpio_handler) *) param_p;
          //
          if (self.callback(self.gpio_controller))
            return vTaskDelete(nullptr);
          return vTaskSuspend(nullptr);
        };

        auto caller_handle = new t(TaskHandle);
        auto caller_arg    = new gpio_handler_t(*this);
        if (xTaskCreatePinnedToCore(caller       , "call-callback", 1000,
                                    caller_arg   , 2,
                                    caller_handle, 0))
          return caller_handle;
        return nullptr;
      }
    };

    type_declare(u(gpio_handler)) {
     public:
      member_define(gpio_handlers, std::vector<t(gpio_handler)>);

     public:
      func_define(register_handler, void,
          func_param(handler, t(gpio_handler))
      ) {
        gpio_handlers.push_back(handler);
      }

      [[noreturn]]
      func_define(executor, static void,
          func_param(param_p, void *)
        ) {
        auto &gpio_handlers = *(std::vector<t(gpio_handler)> *) param_p;
        //
        unlimited_loop {
          for (auto &gpio_handler : gpio_handlers) {
            auto previous = gpio_handler.gpio_controller.get_status(false),
                 current  = gpio_handler.gpio_controller.get_status(true );
            // check pin stat for type of pin mode
            if ((
                // state: on high
                gpio_handler.gpio_mode == on_high
                  and previous == core::types::io::t(pin)::low
                  and current  == core::types::io::t(pin)::high
                ) or (
                // state: on low
                gpio_handler.gpio_mode == on_low
                  and previous == core::types::io::t(pin)::high
                  and current  == core::types::io::t(pin)::low
                )) {
              // call callback!
              gpio_handler.call();
            }
          }
        }
      }

      func_define(execute, t(TaskHandle) *,
        ) noexcept {
        auto executor_handle = new t(TaskHandle);
        auto executor_arg    = (void *) &gpio_handlers;
        // create 'executor' task
        if (xTaskCreatePinnedToCore(executor       , "gpio-handler-executor", 10000,
                                    executor_arg   , 2,
                                    executor_handle, 1))
          return executor_handle;
        return nullptr;
      }
    };
  }
}
