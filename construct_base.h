#pragma once

#include <new>
#include <utility>

template <typename T, bool enabled = std::is_copy_constructible_v<T>>
struct copy_construct_base {
  copy_construct_base() = default;
  constexpr copy_construct_base(const copy_construct_base&) = delete;
  constexpr copy_construct_base(copy_construct_base&&) = default;
  copy_construct_base& operator=(const copy_construct_base&) = default;
  copy_construct_base& operator=(copy_construct_base&&) = default;
};

template <typename T>
struct copy_construct_base<T, true> {};

template <typename T, bool enabled = std::is_move_constructible_v<T>>
struct move_construct_base {
  move_construct_base() = default;
  constexpr move_construct_base(const move_construct_base&) = default;
  constexpr move_construct_base(move_construct_base&&) = delete;
  move_construct_base& operator=(const move_construct_base&) = default;
  move_construct_base& operator=(move_construct_base&&) = default;
};

template <typename T>
struct move_construct_base<T, true> {};
