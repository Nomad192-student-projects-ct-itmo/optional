#pragma once

#include <new>
#include <utility>

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
