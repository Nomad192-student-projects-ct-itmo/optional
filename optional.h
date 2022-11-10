#pragma once

#include <new>
#include <utility>
#include <vector>

template <typename T, bool trivial = std::is_trivially_destructible_v<T>>
struct base {
  base() : is_present{false}, dummy(0) {}

  base(T x) : is_present{true}, data{std::move(x)} {}

  template <typename... Args>
  base(Args&&... args) : is_present{true}, data(std::forward<Args>(args)...) {}

  bool is_present;
  union {
    char dummy;
    T data;
  };

  template <typename... Args>
  void emplace(Args&&... args) {
    reset();
    new (&data) T(std::forward<Args>(args)...);
    is_present = true;
  }

  void reset() {
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
  base() : is_present{false}, dummy(0) {}

  base(T x) : is_present{true}, data{std::move(x)} {}

  template <typename... Args>
  base(Args&&... args) : is_present{true}, data(std::forward<Args>(args)...) {}

  bool is_present;
  union {
    char dummy;
    T data;
  };

  template <typename... Args>
  void emplace(Args&&... args) {
    reset();
    new (&data) T(std::forward<Args>(args)...);
    is_present = true;
  }

  void reset() {
    if (is_present) {
      data.~T();
    }
    is_present = false;
  }

  ~base() = default;
};

template <typename T, bool trivial = std::is_trivially_copy_assignable_v<T>>
struct trivial_copy_assign_base : base<T> {
  using base<T>::base;
  trivial_copy_assign_base(const trivial_copy_assign_base& other)
      : base<T>() { ///////////!!!!!!!!!!!!!!!!!!!!!!
    if (other.is_present) {
      std::construct_at(&this->data, other.data);
      this->is_present = true;
    }
  }
  trivial_copy_assign_base(trivial_copy_assign_base&&) = default;
  trivial_copy_assign_base& operator=(const trivial_copy_assign_base& other) {
    if (this->is_present) {
      if (other.is_present) {
        this->data = other.data;
      } else {
        this->data.~T();
        this->is_present = false;
      }
    } else {
      if (other.is_present) {
        std::construct_at(&this->data, other.data);
        this->is_present = true;
      }
    }

    return *this;
  }
  trivial_copy_assign_base&
  operator=(trivial_copy_assign_base&& other) = default;
};

template <typename T>
struct trivial_copy_assign_base<T, true> : base<T> {
  using base<T>::base;
  trivial_copy_assign_base(const trivial_copy_assign_base&) = default;
  trivial_copy_assign_base(trivial_copy_assign_base&&) = default;
  trivial_copy_assign_base&
  operator=(const trivial_copy_assign_base&) = default;
  trivial_copy_assign_base& operator=(trivial_copy_assign_base&&) = default;
};

template <typename T, bool trivial = std::is_trivially_move_assignable_v<T>>
struct trivial_move_assign_base : trivial_copy_assign_base<T> {
  using trivial_copy_assign_base<T>::trivial_copy_assign_base;

  trivial_move_assign_base(const trivial_move_assign_base&) = default;
  trivial_move_assign_base(trivial_move_assign_base&& other) {
    if (other.is_present) {
      std::construct_at(&this->data, std::move(other.data));
      this->is_present = true;
    }
  }

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
        std::construct_at(&this->data, std::move(other.data));
        this->is_present = true;
      }
    }
    return *this;
  }
};

template <typename T>
struct trivial_move_assign_base<T, true> : trivial_copy_assign_base<T> {
  using trivial_copy_assign_base<T>::trivial_copy_assign_base;
  trivial_move_assign_base(const trivial_move_assign_base&) = default;
  trivial_move_assign_base(trivial_move_assign_base&&) = default;
  trivial_move_assign_base&
  operator=(const trivial_move_assign_base&) = default;
  trivial_move_assign_base& operator=(trivial_move_assign_base&&) = default;
};

template <typename T, bool enabled = std::is_copy_assignable_v<T>>
struct copy_assign_base {
  copy_assign_base() = default;
  copy_assign_base(const copy_assign_base&) = delete;
  copy_assign_base(copy_assign_base&&) = default;
  copy_assign_base& operator=(const copy_assign_base&) = delete;
  copy_assign_base& operator=(copy_assign_base&&) = default;
};

template <typename T>
struct copy_assign_base<T, true> {
  copy_assign_base() = default;
  copy_assign_base(const copy_assign_base&) = default;
  copy_assign_base(copy_assign_base&&) = default;
  copy_assign_base& operator=(const copy_assign_base&) = default;
  copy_assign_base& operator=(copy_assign_base&&) = default;
};

template <typename T, bool enabled = std::is_move_assignable_v<T>>
struct move_assign_base {
  move_assign_base() = default;
  move_assign_base(const move_assign_base&) = default;
  move_assign_base(move_assign_base&&) = delete;
  move_assign_base& operator=(const move_assign_base&) = default;
  move_assign_base& operator=(move_assign_base&&) = delete;
};

template <typename T>
struct move_assign_base<T, true> {
  move_assign_base() = default;
  move_assign_base(const move_assign_base&) = default;
  move_assign_base(move_assign_base&&) = default;
  move_assign_base& operator=(const move_assign_base&) = default;
  move_assign_base& operator=(move_assign_base&&) = default;
};

struct nullopt_t {};
nullopt_t nullopt{}; /// inline?
struct in_place_t {};
in_place_t in_place{}; /// inline?

template <typename T>
struct optional : trivial_move_assign_base<T>,
                  copy_assign_base<T>,
                  move_assign_base<T> {
  using trivial_move_assign_base<T>::trivial_move_assign_base;
  // using trivial_move_assign_base<T>::trivial_copy_assign_base;

  //  using copy_assign_base<T>::copy_assign_base;
  //  using move_assign_base<T>::move_assign_base;

  optional() = default;
  optional(nullopt_t) {}

  template <typename... Args>
  optional(in_place_t, Args&&... args)
      : trivial_move_assign_base<T>(std::forward<Args>(args)...) {}
  optional(const optional&) = default;
  optional(optional&&) = default;
  optional& operator=(const optional&) = default;
  optional& operator=(optional&&) = default;

  T& operator*() noexcept {
    return this->data;
  }
  T const& operator*() const noexcept {
    return this->data;
  }

  T* operator->() noexcept {
    return &this->data;
  }
  T const* operator->() const noexcept {
    return &this->data;
  }

  [[nodiscard]] bool has_value() const noexcept {
    return this->is_present;
  }

  [[nodiscard]] explicit operator bool() const noexcept {
    return has_value();
  }

  optional& operator=(nullopt_t) noexcept {
    this->reset();
    return *this;
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
bool operator==(optional<T> const& a, optional<T> const& b) {
  if (bool(a) != bool(b))
    return false;

  if (!bool(a) || !bool(b))
    return true;

  return *a == *b;
}

template <typename T>
bool operator!=(optional<T> const& a, optional<T> const& b) {
  if (bool(a) != bool(b))
    return true;

  if (!bool(a) || !bool(b))
    return false;

  return *a != *b;
}

template <typename T>
bool operator<(optional<T> const& a, optional<T> const& b) {
  if (!bool(b))
    return false;

  if (!bool(a))
    return true;

  return *a < *b;
}

template <typename T>
bool operator<=(optional<T> const& a, optional<T> const& b) {
  if (!bool(a))
    return true;

  if (!bool(b))
    return false;

  return *a <= *b;
}

template <typename T>
bool operator>(optional<T> const& a, optional<T> const& b) {
  if (!bool(a))
    return false;

  if (!bool(b))
    return true;

  return *a > *b;
}

template <typename T>
bool operator>=(optional<T> const& a, optional<T> const& b) {
  if (!bool(b))
    return true;

  if (!bool(a))
    return false;

  return *a >= *b;
}

/*
struct nullopt_t;

struct in_place_t;

template <typename T>
class optional {
public:
   optional() noexcept;
  constexpr optional(nullopt_t) noexcept;

  constexpr optional(optional const&);
  constexpr optional(optional&&);

  optional& operator=(optional const&);
  optional& operator=(optional&&);

   optional(T value);

  template <typename... Args>
  explicit constexpr optional(in_place_t, Args&&... args);

  optional& operator=(nullopt_t) noexcept;

  constexpr explicit operator bool() const noexcept;

  constexpr T& operator*() noexcept;
  constexpr T const& operator*() const noexcept;

  constexpr T* operator->() noexcept;
  constexpr T const* operator->() const noexcept;

  template <typename... Args>
  void emplace(Args&&... args);

  void reset();
};
*/