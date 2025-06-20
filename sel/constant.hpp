#pragma once

#include "sel/contract.hpp"
#include "sel/strongly_ordered.hpp"
#include "sel/term.hpp"

#include <compare>
#include <concepts>
#include <format>
#include <type_traits>

namespace sel {

/// symbol that represents a specified quantity in an expression
template <class T>
  requires std::signed_integral<T> or std::floating_point<T>
class constant : public term_base
{
  T value_{};

public:
  constexpr explicit constant(T value)
      : value_{[&] {
          if constexpr (not strongly_ordered<T>) {
            SEL_PRECONDITION(value == value);
          }
          return std::move(value);
        }()}
  {}

  template <class Self>
  [[nodiscard]]
  constexpr auto value(this Self&& self) -> auto&&
  {
    return std::forward<Self>(self).value_;
  }

  [[nodiscard]]
  friend auto
  operator==(const constant& x, const constant& y) -> bool = default;

  template <class U>
    requires std::equality_comparable_with<T, U>
  [[nodiscard]]
  friend constexpr auto
  operator==(const constant& x, const constant<U>& y) -> bool
  {
    return x.value() == y.value();
  }

  [[nodiscard]]
  friend constexpr auto
  operator<=>(const constant& x, const constant& y) -> std::strong_ordering
  {
    const auto cmp = x.value() <=> y.value();
    if constexpr (strongly_ordered<T>) {
      return cmp;
    } else {
      if (cmp < 0) {
        return std::strong_ordering::less;
      }
      if (cmp > 0) {
        return std::strong_ordering::greater;
      }
      return std::strong_ordering::equal;
    }
  }

  template <class U>
    requires std::three_way_comparable_with<T, U> and
             // NOLINTNEXTLINE(modernize-type-traits)
             requires { typename std::common_type<T, U>::type; }
  [[nodiscard]]
  friend constexpr auto
  operator<=>(const constant& x, const constant<U>& y) -> std::strong_ordering
  {
    using common_constant = constant<std::common_type_t<T, U>>;
    return common_constant(x.value()) <=> common_constant(y.value());
  }
};

}  // namespace sel

template <class T, class Char>
struct ::std::formatter<::sel::constant<T>, Char> : std::formatter<T, Char>
{
  template <class O>
  constexpr auto format(
      const ::sel::constant<T>& x, std::basic_format_context<O, Char>& ctx
  ) const
  {
    return std::format_to(ctx.out(), "{}", x.value());
  }
};
