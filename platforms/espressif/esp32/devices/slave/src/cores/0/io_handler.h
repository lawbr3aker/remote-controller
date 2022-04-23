#pragma once

#include <io/pin.h>
#include <io/mux/multi_mux.h>
#include <io/mux/mux_4bit.h>

core::utils::mux::multi_mux<
    core::utils::mux::mux_4bit> *mux_devices;
core::types::pins_set_t<4>      *mux_states ;

void init_io() {
  mux_states  = new core::types::pins_set_t<4>({1, 2, 3, 4});
  mux_devices = new core::utils::mux::multi_mux<
                        core::utils::mux::mux_4bit>(*mux_states);
}

void io_pins_reader() {
  mux_devices->set_channel_r();
}