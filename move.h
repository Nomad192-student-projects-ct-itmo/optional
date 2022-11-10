#include "copy.h"

template <typename T, bool trivial = std::is_trivially_move_constructible_v<T>>
struct trivial_move_constructable_base : trivial_copy_assign_base<T> {
  using trivial_copy_assign_base<T>::trivial_copy_assign_base;
  constexpr trivial_move_constructable_base(
      const trivial_move_constructable_base&) = default;
  constexpr trivial_move_constructable_base(
      trivial_move_constructable_base&& other) {
    if (other.is_present) {
      new (&this->data) T(std::move(other.data));
      this->is_present = true;
    }
  }
  trivial_move_constructable_base&
  operator=(const trivial_move_constructable_base&) = default;
  trivial_move_constructable_base&
  operator=(trivial_move_constructable_base&& other) = default;
};

template <typename T>
struct trivial_move_constructable_base<T, true> : trivial_copy_assign_base<T> {
  using trivial_copy_assign_base<T>::trivial_copy_assign_base;
};

template <typename T, bool trivial = std::is_trivially_move_constructible_v<T>&&
                          std::is_trivially_move_assignable_v<T>>
struct trivial_move_assign_base : trivial_move_constructable_base<T> {
  using trivial_move_constructable_base<T>::trivial_move_constructable_base;

  constexpr trivial_move_assign_base(const trivial_move_assign_base&) = default;
  constexpr trivial_move_assign_base(trivial_move_assign_base&& other) =
      default;

  trivial_move_assign_base&
  operator=(const trivial_move_assign_base&) = default;
  trivial_move_assign_base& operator=(trivial_move_assign_base&& other) {
    if (this->is_present) {
      if (other.is_present) {
        this->data = std::move(other.data);
      } else {
        this->data.~T();
        this->is_present = false;
      }
    } else {
      if (other.is_present) {
        new (&this->data) T(std::move(other.data));
        this->is_present = true;
      }
    }
    return *this;
  }
};

template <typename T>
struct trivial_move_assign_base<T, true> : trivial_move_constructable_base<T> {
  using trivial_move_constructable_base<T>::trivial_move_constructable_base;
};

template <typename T, bool enabled = std::is_copy_assignable_v<T>&&
                          std::is_copy_constructible_v<T>>
struct copy_assign_base {
  copy_assign_base() = default;
  constexpr copy_assign_base(const copy_assign_base&) = delete;
  constexpr copy_assign_base(copy_assign_base&&) = default;
  copy_assign_base& operator=(const copy_assign_base&) = delete;
  copy_assign_base& operator=(copy_assign_base&&) = default;
};

template <typename T>
struct copy_assign_base<T, true> {};

template <typename T, bool enabled = std::is_move_assignable_v<T>&&
                          std::is_move_constructible_v<T>>
struct move_assign_base {
  move_assign_base() = default;
  constexpr move_assign_base(const move_assign_base&) = default;
  constexpr move_assign_base(move_assign_base&&) = delete;
  move_assign_base& operator=(const move_assign_base&) = default;
  move_assign_base& operator=(move_assign_base&&) = delete;
};

template <typename T>
struct move_assign_base<T, true> {};