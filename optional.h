#pragma once

#include <new>
#include <utility>
#include <vector>

template <typename T>
struct base {
  constexpr base() : is_present{false} {}

  constexpr base(T x) : is_present{true}, data{std::move(x)} {}

  bool is_present;
  union {
    char dummy{};
    T data;
  };

  constexpr void reset()
  {
    if (is_present) {
      data.~T();
    }
    is_present = false;
  }

  constexpr ~base() {
    reset();
  }
};

template <typename T, bool trivial = std::is_trivially_copy_assignable_v<T>>
struct trivial_copy_assign_base : base<T> {
  using base<T>::base;
  constexpr trivial_copy_assign_base(const trivial_copy_assign_base& other) : base<T>()  { ///////////!!!!!!!!!!!!!!!!!!!!!!
    if(other.is_present)
    {
      std::construct_at(&this->data, other.data);
      this->is_present = true;
    }
  }
  constexpr trivial_copy_assign_base(trivial_copy_assign_base&&) = default;
  trivial_copy_assign_base& operator=(const trivial_copy_assign_base& other) {
    if (this->is_present) {
      if(other.is_present)
      {
        this->data = other.data;
      }
      else
      {
        this->data.~T();
        this->is_present = false;
      }
    } else {
      if(other.is_present)
      {
        std::construct_at(&this->data, other.data);
        this->is_present = true;
      }
    }

    return *this;
  }
  trivial_copy_assign_base& operator=(trivial_copy_assign_base&& other) = default;
};

template <typename T>
struct trivial_copy_assign_base<T, true> : base<T> {
  using base<T>::base;
  constexpr trivial_copy_assign_base(const trivial_copy_assign_base&) = default;
  constexpr trivial_copy_assign_base(trivial_copy_assign_base&&) = default;
  trivial_copy_assign_base& operator=(const trivial_copy_assign_base&) = default;
  trivial_copy_assign_base& operator=(trivial_copy_assign_base&&) = default;
};

template <typename T, bool trivial = std::is_trivially_move_assignable_v<T>>
struct trivial_move_assign_base : trivial_copy_assign_base<T> {
  using trivial_copy_assign_base<T>::trivial_copy_assign_base;

  constexpr trivial_move_assign_base(const trivial_move_assign_base&) = default;
  constexpr trivial_move_assign_base(trivial_move_assign_base&& other) {
    if(other.is_present)
    {
      std::construct_at(&this->data, std::move(other.data));
      this->is_present = true;
    }
  }

  trivial_move_assign_base& operator=(const trivial_move_assign_base&) = default;
  trivial_move_assign_base& operator=(trivial_move_assign_base&& other) {
    if (this->is_present) {
      if(other.is_present)
      {
        this->data = std::move(other.data);
      }
      else
      {
        this->data.~T();
        this->is_present = false;
      }
    } else {
      if(other.is_present)
      {
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
  constexpr trivial_move_assign_base(const trivial_move_assign_base&) = default;
  constexpr trivial_move_assign_base(trivial_move_assign_base&&) = default;
  trivial_move_assign_base& operator=(const trivial_move_assign_base&) = default;
  trivial_move_assign_base& operator=(trivial_move_assign_base&&) = default;
};

template <typename T, bool enabled = std::is_copy_assignable_v<T>>
struct copy_assign_base {
  copy_assign_base() = default;
  constexpr copy_assign_base(const copy_assign_base&) = delete;
  constexpr copy_assign_base(copy_assign_base&&) = default;
  copy_assign_base& operator=(const copy_assign_base&) = delete;
  copy_assign_base& operator=(copy_assign_base&&) = default;
};

template <typename T>
struct copy_assign_base<T, true> {
  copy_assign_base() = default;
  constexpr copy_assign_base(const copy_assign_base&) = default;
  constexpr copy_assign_base(copy_assign_base&&) = default;
  copy_assign_base& operator=(const copy_assign_base&) = default;
  copy_assign_base& operator=(copy_assign_base&&) = default;
};

template <typename T, bool enabled = std::is_move_assignable_v<T>>
struct move_assign_base {
  move_assign_base() = default;
  constexpr move_assign_base(const move_assign_base&) = default;
  constexpr move_assign_base(move_assign_base&&) = delete;
  move_assign_base& operator=(const move_assign_base&) = default;
  move_assign_base& operator=(move_assign_base&&) = delete;
};

template <typename T>
struct move_assign_base<T, true> {
  move_assign_base() = default;
  constexpr move_assign_base(const move_assign_base&) = default;
  constexpr move_assign_base(move_assign_base&&) = default;
  move_assign_base& operator=(const move_assign_base&) = default;
  move_assign_base& operator=(move_assign_base&&) = default;
};

template <typename T>
struct optional : trivial_move_assign_base<T>, copy_assign_base<T>, move_assign_base<T>
{
  using trivial_move_assign_base<T>::trivial_move_assign_base;
  //using trivial_move_assign_base<T>::trivial_copy_assign_base;

//  using copy_assign_base<T>::copy_assign_base;
//  using move_assign_base<T>::move_assign_base;

  constexpr optional() = default;
  constexpr optional(const optional&) = default;
  constexpr optional(optional&&) = default;
  constexpr optional& operator=(const optional&) = default;
  constexpr optional& operator=(optional&&) = default;

  constexpr T& operator*() noexcept
  {
    return this->data;
  }
  constexpr T const& operator*() const noexcept
  {
    return this->data;
  }

  constexpr T* operator->() noexcept
  {
    return &this->data;
  }
  constexpr T const* operator->() const noexcept
  {
    return &this->data;
  }

  [[nodiscard]] constexpr bool has_value() const noexcept {
    return this->is_present;
  }

  [[nodiscard]] constexpr explicit operator bool() const noexcept
  {
    return has_value();
  }

//  void swap(optional& other) {
//    if (has_value() && other.has_value()) {
//      using std::swap;
//      swap(this->data, other.data);
//    } else if (has_value() && !other.has_value()) {
//      other = std::move(*this);
//      this->is_present = false;
//      this->data.~T();
//    } else if (!has_value() && other.has_value()) {
//      other.swap2(*this);
//    }
//  }
};

/*#pragma once

#include <new>
#include <utility>
#include <vector>

template <typename T>
struct base {
  constexpr base() : is_present{false}, dummy{0} {}

  constexpr base(T x) : is_present{true}, data{x} {}

  bool is_present{false};
  union {
    char dummy;
    T data;
  };

  constexpr ~base() {
    reset();
  }

  void reset()
  {
    if (is_present) {
      data.~T();
    }
    is_present = false;
  }
};

template <typename T, bool trivial = std::is_trivially_copy_assignable_v<T>>
struct trivial_copy_assign_base : base<T> {
  using base<T>::base;
  trivial_copy_assign_base(const trivial_copy_assign_base& other) {
    if(other.is_present)
    {
      std::construct_at(&this->data, other.data);
      this->is_present = true;
    }
  }
  trivial_copy_assign_base& operator=(const trivial_copy_assign_base& other) {
    if (this->is_present) {
      if(other.is_present)
      {
        this->data = other.data;
      }
      else
      {
        this->data.~T();
        this->is_present = false;
      }
    } else {
      if(other.is_present)
      {
        std::construct_at(&this->data, other.data);
        this->is_present = true;
      }
    }
  }
};

template <typename T>
struct trivial_copy_assign_base<T, true> : base<T> {
  using base<T>::base;
  trivial_copy_assign_base&
  operator=(const trivial_copy_assign_base&) = default;
};

template <typename T, bool enabled = std::is_copy_assignable_v<T>>
struct copy_assign_base {
  copy_assign_base() = default;
  copy_assign_base& operator=(const copy_assign_base&) = delete;
};

template <typename T>
struct copy_assign_base<T, true> {
  copy_assign_base() = default;
  copy_assign_base& operator=(const copy_assign_base&) = default;
};

template <typename T>
struct optional : trivial_copy_assign_base<T>, copy_assign_base<T> {
  using trivial_copy_assign_base<T>::trivial_copy_assign_base;
  using copy_assign_base<T>::copy_assign_base;

  constexpr optional& operator=(const optional&) = default;

  constexpr T& operator*() noexcept
  {
    return this->data;
  }
  constexpr T const& operator*() const noexcept
  {
    return this->data;
  }

  constexpr T* operator->() noexcept
  {
    return &this->data;
  }
  constexpr T const* operator->() const noexcept
  {
    return &this->data;
  }

  [[nodiscard]] bool has_value() const noexcept {
    return this->is_present;
  }

  [[nodiscard]] constexpr explicit operator bool() const noexcept
  {
    return has_value();
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
*/














/*
struct nullopt_t;

struct in_place_t;

template <typename T>
class optional {
public:
  constexpr optional() noexcept;
  constexpr optional(nullopt_t) noexcept;

  constexpr optional(optional const&);
  constexpr optional(optional&&);

  optional& operator=(optional const&);
  optional& operator=(optional&&);

  constexpr optional(T value);

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

template <typename T>
constexpr bool operator==(optional<T> const& a, optional<T> const& b);

template <typename T>
constexpr bool operator!=(optional<T> const& a, optional<T> const& b);

template <typename T>
constexpr bool operator<(optional<T> const& a, optional<T> const& b);

template <typename T>
constexpr bool operator<=(optional<T> const& a, optional<T> const& b);

template <typename T>
constexpr bool operator>(optional<T> const& a, optional<T> const& b);

template <typename T>
constexpr bool operator>=(optional<T> const& a, optional<T> const& b);*/