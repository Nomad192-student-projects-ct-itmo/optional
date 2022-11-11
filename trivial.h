#pragma once

#include "enable.h"

template <typename T>
struct base<T, true> {
  constexpr base() {}
  constexpr base(nullopt_t) {}

  template <typename... Args>
  constexpr explicit base(in_place_t, Args&&... args)
      : is_present{true}, data(std::forward<Args>(args)...) {}

  bool is_present{false};
  union {
    char dummy{};
    T data;
  };

  constexpr void reset() {
    is_present = false;
  }
};

template <typename T>
struct trivial_copy_constructable_base<T, true> : base<T> {
  using base<T>::base;
};

template <typename T>
struct trivial_copy_assign_base<T, true> : trivial_copy_constructable_base<T> {
  using trivial_copy_constructable_base<T>::trivial_copy_constructable_base;
};

template <typename T>
struct trivial_move_constructable_base<T, true> : trivial_copy_assign_base<T> {
  using trivial_copy_assign_base<T>::trivial_copy_assign_base;
};

template <typename T>
struct trivial_move_assign_base<T, true> : trivial_move_constructable_base<T> {
  using trivial_move_constructable_base<T>::trivial_move_constructable_base;
};

template <typename T>
struct copy_construct_base<T, true> {};

template <typename T>
struct move_construct_base<T, true> {};

template <typename T>
struct copy_assign_base<T, true> {};

template <typename T>
struct move_assign_base<T, true> {};
