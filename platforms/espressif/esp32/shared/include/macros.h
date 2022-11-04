#pragma once

#define t(name) name##_t
#define u(name) name
#define f(name) name

#define type_declare(name) class name
#define type_template(...) template<__VA_ARGS__>
#define type_define(name, type, ...) typedef type __VA_ARGS__ name;
#define type_alias(name, ...) using name = __VA_ARGS__;

#define func_define(name, type, ...) type name (__VA_ARGS__)
#define func_param(name, type, ...) type name __VA_ARGS__

#define member_define(member, type, ...) type member __VA_ARGS__

#define create_callback(callback_master, callback_body, ...)    \
  callback_master {               \
    .callback = [__VA_ARGS__](void *) -> void callback_body \
  }

#define enumeration(...) {__VA_ARGS__}

#define unlimited_loop for (;;)
#define res_succeed return 1;
#define res_failed  return 0;