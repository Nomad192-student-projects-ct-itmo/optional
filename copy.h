#pragma once

#include "base.h"

template <typename T, bool trivial = std::is_trivially_copy_constructible_v<T>>
struct trivial_copy_constructable_base : base<T> {
  using base<T>::base;
  constexpr trivial_copy_constructable_base(
      const trivial_copy_constructable_base& other)
      : base<T>() { ///???
    if (other.is_present) {
      new (&this->data) T(other.data);
      this->is_present = true;
    }
  }
  constexpr trivial_copy_constructable_base(trivial_copy_constructable_base&&) =
      default;
};

template <typename T>
struct trivial_copy_constructable_base<T, true> : base<T> {
  using base<T>::base;
};

template <typename T, bool trivial = std::is_trivially_copy_constructible_v<T>&&
                          std::is_trivially_copy_assignable_v<T>>
struct trivial_copy_assign_base : trivial_copy_constructable_base<T> {
  using trivial_copy_constructable_base<T>::trivial_copy_constructable_base;
  constexpr trivial_copy_assign_base(const trivial_copy_assign_base& other) =
      default;
  trivial_copy_assign_base& operator=(const trivial_copy_assign_base& other) {
    if (this->is_present) {
      if (other.is_present) {
        this->data = other.data;
      } else {
        this->reset();
      }
    } else {
      if (other.is_present) {
        new (&this->data) T(other.data);
        this->is_present = true;
      }
    }

    return *this;
  }
};

template <typename T>
struct trivial_copy_assign_base<T, true> : trivial_copy_constructable_base<T> {
  using trivial_copy_constructable_base<T>::trivial_copy_constructable_base;
};
