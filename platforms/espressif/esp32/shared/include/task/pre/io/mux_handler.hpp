#pragma once

#include <utility>

#include "vector"

#include "../../../macros.h"
#include "../../../io/pin.h"
#include "../../../io/mux/multi_mux.h"

#define analog_update_off 5

namespace handler {
  type_define(t(handle_mode), enum,
      enumeration(
        on_low   ,
        on_high  ,
        on_change,
        on_value
      )
    )

  namespace io {
    type_declare(t(mux_handler)) {
     public:
      typedef t(int8) (*t(callback))(core::utils::u(pin_controller));

     public:
      member_define(mux_controller, core::utils::u(pin_controller));
      member_define(mux_mode      , handler::t(handle_mode)       );
      member_define(mux_channel   , core::types::io::t(pin)::t(id));
      member_define(callback      , t(callback)                   );

     public:
      t(mux_handler) (
          func_param(controller, decltype(mux_controller)),
          func_param(mode      , decltype(mux_mode)),
          func_param(channel   , decltype(mux_channel)),
          func_param(callback  , decltype(callback))
      ) : mux_controller(controller),
          mux_mode      (mode),
          mux_channel   (channel),
          callback      (callback) {
      }

      func_define(call, t(TaskHandle),
        ) const {
        callback(mux_controller);
//        auto caller = [](void *param_p) {
//          auto const &self = *(t(mux_handler) *) param_p;
//          //
//          if (self.callback(self.mux_controller))
//            return vTaskDelete(nullptr);
//          return vTaskSuspend(nullptr);
//        };
//
//        auto caller_handle = new t(TaskHandle);
//        auto caller_arg    = new t(mux_handler)(*this);
//        if (xTaskCreatePinnedToCore(caller       , "call-callback", 4000,
//                                    caller_arg   , 2,
//                                    caller_handle, 1))
//          return caller_handle;
//        return nullptr;
      }
    };

    type_declare(u(mux_handler)) {
     public:
      member_define(mux_handlers, std::vector<t(mux_handler)> );
      member_define(mux_main    , core::utils::mux::multi_mux   );

     public:
      u(mux_handler) (
          func_param(mux, core::utils::mux::multi_mux)
      ) : mux_main(std::move(mux)) {
      }

      func_define(register_handler, void,
          func_param(handler, t(mux_handler))
      ) {
        mux_handlers.push_back(handler);
      }

      func_define(executor, static void,
          func_param(param_p, void *)
        ) {
        auto &self = *(u(mux_handler) *) param_p;
        //
        self.mux_main.set_channel(0);
        for (;;) {
          for (auto &mux_handler : self.mux_handlers) {
            if (mux_handler.mux_channel == self.mux_main.get_channel()) {
              if (mux_handler.mux_mode == on_value) {
                auto previous = mux_handler.mux_controller.get_value(false),
                     current  = mux_handler.mux_controller.get_value(true );
                //
                if (current - previous > analog_update_off or current - previous < -analog_update_off) {
                    mux_handler.call();
                }
              } else {
                auto previous = mux_handler.mux_controller.get_status(false),
                     current  = mux_handler.mux_controller.get_status(true );
                // check pin stat for type of pin mode
                if ((
                    // state: on high
                    mux_handler.mux_mode == on_high
                      and previous == core::types::io::t(pin)::low
                      and current  == core::types::io::t(pin)::high
                    ) or (
                    // state: on low
                    mux_handler.mux_mode == on_low
                      and previous == core::types::io::t(pin)::high
                      and current  == core::types::io::t(pin)::low
                    )) {
                  // call callback!
                  mux_handler.call();
                }
              }
            }
          }
          // move to next channel
          self.mux_main.set_channel_r();
          // required delay
          delay(3);
        }
      }

      func_define(execute, t(TaskHandle) *,
        ) noexcept {
        auto executor_handle = new t(TaskHandle);
        auto executor_arg    = new u(mux_handler)(*this);
        // create 'executor' task
        if (xTaskCreate(executor       , "gpio-handler-executor", 12000,
                        executor_arg   , 0,
                        executor_handle))
          return executor_handle;
        return nullptr;
      }
    };
  }
}
