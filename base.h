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
