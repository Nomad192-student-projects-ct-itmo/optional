#pragma once

#include <new>
#include <utility>

struct nullopt_t {};
inline constexpr nullopt_t nullopt{};

struct in_place_t {};
inline constexpr in_place_t in_place{};

template <typename T, bool trivial = std::is_trivially_destructible_v<T>>
struct base {
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
    if (is_present) {
      data.~T();
    }
    is_present = false;
  }

  ~base() {
    reset();
  }
};

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
        this->reset();
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

template <typename T, bool enabled = std::is_copy_constructible_v<T>>
struct copy_construct_base {
  copy_construct_base() = default;
  constexpr copy_construct_base(const copy_construct_base&) = delete;
  constexpr copy_construct_base(copy_construct_base&&) = default;
  copy_construct_base& operator=(const copy_construct_base&) = default;
  copy_construct_base& operator=(copy_construct_base&&) = default;
};

template <typename T, bool enabled = std::is_move_constructible_v<T>>
struct move_construct_base {
  move_construct_base() = default;
  constexpr move_construct_base(const move_construct_base&) = default;
  constexpr move_construct_base(move_construct_base&&) = delete;
  move_construct_base& operator=(const move_construct_base&) = default;
  move_construct_base& operator=(move_construct_base&&) = default;
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

template <typename T, bool enabled = std::is_move_assignable_v<T>&&
                          std::is_move_constructible_v<T>>
struct move_assign_base {
  move_assign_base() = default;
  constexpr move_assign_base(const move_assign_base&) = default;
  constexpr move_assign_base(move_assign_base&&) = delete;
  move_assign_base& operator=(const move_assign_base&) = default;
  move_assign_base& operator=(move_assign_base&&) = delete;
};
