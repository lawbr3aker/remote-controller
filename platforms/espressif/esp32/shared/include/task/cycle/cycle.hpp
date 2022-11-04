#pragma once

#include <freertos/task.h>

#include <utility>
#include <vector>
#include <freertos/timers.h>

#include "../../macros.h"
#include "pulse.hpp"

namespace core {
  namespace task {
    namespace cycle {
      class t(cycle) {
       public:
        member_define(all_cycles, static std::vector<t(cycle) *>);

        member_define(pulses, cycle::t(pulse_vec));
        member_define(task  , TaskHandle_t *, = nullptr);

       public:
        t(cycle) (
            func_param(pulses, cycle::t(pulse_vec))
        ) : pulses(std::move(pulses)) {
          all_cycles.push_back(this);
        }

        func_define(end_all, static void,
            ) {
          for (auto &cycle: all_cycles)
            cycle->end();
        }

        func_define(executor, static void,
              func_param(pulses_p, void *)
            ) {
          auto const &pulses = *(t(pulse_vec) *) pulses_p;
          //
          for (;;) {
            for (t(pulse) pulse : pulses) {
              if (not pulse.callback())
                return vTaskDelete(nullptr);
              //
              if (pulse.period.total)
                vTaskDelay(pulse.period.total);
            }
          }
        }

        func_define(execute, TaskHandle_t *,
            ) {
          end_all();
          //
          auto executor_handle = new t(TaskHandle)        ;
          auto executor_arg    = new t(pulse_vec )(pulses);
          if (xTaskCreatePinnedToCore(executor       , "pulse", 10000,
                                      executor_arg   , 2,
                                      executor_handle,
                                      1))
            return task = executor_handle;
          return task = nullptr;
        }

        func_define(end, void,
            ) {
          if (task)
            vTaskDelete(*task);
          task = nullptr;
        }
      };
      //
      std::vector<t(cycle) *> t(cycle)::all_cycles;
    }
  }
}