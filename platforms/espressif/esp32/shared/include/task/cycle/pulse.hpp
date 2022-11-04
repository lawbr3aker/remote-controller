#pragma once

#include <vector>

#include "../../macros.h"

namespace core {
  namespace task {
    type_alias(t(millisecond), unsigned long long)

    typedef t(int8) (*t(callback))();

    namespace cycle {
      struct t(period) {
        member_define(total, t(millisecond));
      };

      class t(pulse) {
       public:
        member_define(callback, t(callback));
        member_define(period  , t(period)  );

       public:
        t(pulse) (
            t(callback) callback       ,
            t(period)   callback_period
        ) : callback(callback       ),
            period  (callback_period) {
        }
      };

      type_alias(t(pulse_vec), std::vector<t(pulse)>)
    }
  }
}

/* TODO: Archive
 *
 * core::task
#define create_callback(callback_name, callback_body)     \
  struct callback_name : public core::task::t(callback) { \
    inline static int callback() noexcept                 \
      callback_body                                       \
  }
struct t(callback) {
  inline static int callback() noexcept {
    return 1;
  }
};

 *
 * core::macro
namespace core {
  namespace macro {
    template<class t, t v>
    class template_container {
     public:
                 using type       = t;
      static constexpr type value = v;
    };
  }
}

 *
 * core::task::cycle
func_define(execute, TaskHandle_t *,
    ) {
         auto executor_handle = new TaskHandle_t;
  static auto executor        = [](void *callback_p) {
    if (((callback_t *) callback_p)->callback())
      vTaskDelete(nullptr);
    vTaskSuspend(nullptr);
  };

  if (xTaskCreatePinnedToCore(executor, "pulse", 1000, &callback, 2, executor_handle, 1))
    return executor_handle;
  return nullptr;
}
 */
