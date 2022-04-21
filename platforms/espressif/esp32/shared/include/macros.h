#pragma once

#define t(name) name##_t
#define u(name) name
#define f(name) name

#define type_name(name)   name##_t
#define type_rename(name) using type_name(name) = name;

#define type_template(...) template<__VA_ARGS__>
#define type_define(name, type, ...) typedef type __VA_ARGS__ name;
#define type_alias(name, ...) using name = __VA_ARGS__;

#define func_define(name, type, ...) type name (__VA_ARGS__)
#define func_param(name, type, ...) type name __VA_ARGS__

#define member_define(member, type, ...) type member __VA_ARGS__