#pragma once

#include <new>
#include <utility>

struct nullopt_t {};
inline const nullopt_t nullopt{}; /// inline? cppreference

struct in_place_t {};
inline const in_place_t in_place{}; /// inline? cppreference

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

  template <typename... Args>
  constexpr void emplace(Args&&... args) {
    reset();
    new (&data) T(std::forward<Args>(args)...);
    is_present = true;
  }

  [[nodiscard]] constexpr bool has_value() const noexcept {
    return this->is_present;
  }

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
  constexpr base(nullopt_t) {}

  template <typename... Args>
  constexpr explicit base(in_place_t, Args&&... args)
      : is_present{true}, data(std::forward<Args>(args)...) {}

  bool is_present{false};
  union {
    char dummy{};
    T data;
  };

  template <typename... Args>
  constexpr void emplace(Args&&... args) {
    reset();
    new (&data) T(std::forward<Args>(args)...);
    is_present = true;
  }

  [[nodiscard]] constexpr bool has_value() const noexcept {
    return this->is_present;
  }

  constexpr void reset() {
    is_present = false;
  }
};
