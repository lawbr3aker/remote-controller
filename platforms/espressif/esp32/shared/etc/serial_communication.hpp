#include <stdint.h>

#define id_t int16_t

struct __attribute__((packed)) virtual_digital_pin {
  static const id_t type_id = 1;
  //
  int32_t id, value;
};

#define request_ok 23

enum {
  // modes
    mode_read, mode_write,
  // pins
    pin_digital, pin_pwm
};

template <typename type_content>
struct __attribute__((packed)) virtual_device_request {
  struct type {
    uint8_t base;
    uint8_t on  ;
  } type;
  struct {
    type_content value;
  } content;
  //
     id_t id     = type_content::type_id;
  int16_t result = request_ok           ;
};

struct __attribute__((packed)) virtual_device_response {
  //
  int16_t result;
};

virtual_device_response virtual_device_nwk{0};
virtual_device_response virtual_device_awk{request_ok};