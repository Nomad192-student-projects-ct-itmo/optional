#pragma once

#include "enable.h"
#include "trivial.h"

template <typename T>
struct optional : trivial_move_assign_base<T>,
                  copy_assign_base<T>,
                  move_assign_base<T>,
                  copy_construct_base<T>,
                  move_construct_base<T> {

  using trivial_move_assign_base<T>::trivial_move_assign_base;

  constexpr optional() = default;
  constexpr optional(nullopt_t) : trivial_move_assign_base<T>() {};
  constexpr explicit optional(T x)
      : trivial_move_assign_base<T>(in_place, std::move(x)){};

  constexpr optional(const optional&) = default;
  constexpr optional(optional&&) = default;
  optional& operator=(const optional&) = default;
  optional& operator=(optional&&) = default;

  constexpr T& operator*() noexcept {
    return this->data;
  }
  constexpr T const& operator*() const noexcept {
    return this->data;
  }

  constexpr T* operator->() noexcept {
    return &this->data;
  }
  constexpr T const* operator->() const noexcept {
    return &this->data;
  }

  [[nodiscard]] explicit constexpr operator bool() const noexcept {
    return this->has_value();
  }

  optional& operator=(nullopt_t) noexcept {
    this->reset();
    return *this;
  }

  template <typename... Args>
  constexpr void emplace(Args&&... args) {
    this->reset();
    new (&this->data) T(std::forward<Args>(args)...);
    this->is_present = true;
  }

  [[nodiscard]] constexpr bool has_value() const noexcept {
    return this->is_present;
  }

  void swap(optional& other) {
     if (has_value() && other.has_value()) {
       using std::swap;
       swap(this->data, other.data);
     } else if (has_value() && !other.has_value()) {
       other = std::move(*this);
       this->is_present = false;
       this->data.~T();
     } else if (!has_value() && other.has_value()) {
       other.swap2(*this);
     }
   }
};

template <typename T>
constexpr bool operator==(optional<T> const& a, optional<T> const& b) {
  if (bool(a) != bool(b))
    return false;

  if (!bool(a) || !bool(b))
    return true;

  return *a == *b;
}

template <typename T>
constexpr bool operator!=(optional<T> const& a, optional<T> const& b) {
  if (bool(a) != bool(b))
    return true;

  if (!bool(a) || !bool(b))
    return false;

  return *a != *b;
}

template <typename T>
constexpr bool operator<(optional<T> const& a, optional<T> const& b) {
  if (!bool(b))
    return false;

  if (!bool(a))
    return true;

  return *a < *b;
}

template <typename T>
constexpr bool operator<=(optional<T> const& a, optional<T> const& b) {
  if (!bool(a))
    return true;

  if (!bool(b))
    return false;

  return *a <= *b;
}

template <typename T>
constexpr bool operator>(optional<T> const& a, optional<T> const& b) {
  if (!bool(a))
    return false;

  if (!bool(b))
    return true;

  return *a > *b;
}

template <typename T>
constexpr bool operator>=(optional<T> const& a, optional<T> const& b) {
  if (!bool(b))
    return true;

  if (!bool(a))
    return false;

  return *a >= *b;
}
