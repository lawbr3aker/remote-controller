#pragma once

#include <freertos/task.h>

#include <vector>
#include <freertos/timers.h>

#include "../../macros.h"
#include "pulse.hpp"

namespace core {
  namespace task {
    namespace cycle {
      class t(cycle) {
       public:
        member_define(pulses, cycle::t(pulse_vec) &);

       public:
        t(cycle) (
            func_param(pulses, cycle::t(pulse_vec) &)
        ) : pulses(pulses) {
        }

        func_define(executor, static void,
              func_param(pulses_p, void *)
            ) {
          auto const &pulses = *(t(pulse_vec) *) pulses_p;
          //
          for (;;) {
            for (t(pulse) pulse : pulses) {
              if (not pulse.callback.callback())
                return vTaskDelete(nullptr);
              //
              if (pulse.period.total)
                vTaskDelay(pulse.period.total);
            }
          }
        }

        func_define(execute, TaskHandle_t *,
            ) {
          auto executor_handle = new t(TaskHandle)        ;
          auto executor_arg    = new t(pulse_vec )(pulses);

          if (xTaskCreatePinnedToCore(executor       , "pulse", 10000,
                                      executor_arg   , 2,
                                      executor_handle,
                                      1))
            return executor_handle;
          return nullptr;
        }
      };
    }
  }
}