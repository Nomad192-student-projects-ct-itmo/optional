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

template <typename T>
struct base<T, true> {
  constexpr base() {}

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

template <typename T>
struct trivial_move_assign_base<T, true> : trivial_move_constructable_base<T> {
  using trivial_move_constructable_base<T>::trivial_move_constructable_base;
};
