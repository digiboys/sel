#pragma once

#include "sel/strongly_ordered.hpp"

#include <concepts>
#include <format>

namespace sel {

// deriving from `leaf_base` allows the type model `leaf`
template <class Derived>
class leaf_base
{
  friend Derived;
  leaf_base() = default;

  friend auto operator<=>(const leaf_base&, const leaf_base&) = default;
};

/// terminal element in an expression tree
///
/// A type that models `leaf` must be `copyable` and must explicitly declare
/// itself as usable as a `term` by inheriting from `leaf_base`.
///
/// Additionally, all valid values of the type must be totally ordered in order
/// to allow reordering of terms in an expression.
template <class T>
concept leaf =                         //
    requires (T x) { x.value(); } and  //
    std::copyable<T> and               //
    strongly_ordered<T> and            //
    std::derived_from<T, leaf_base<T>>;

}  // namespace sel

template <class L, class Char>
  requires std::derived_from<L, ::sel::leaf_base<L>>
struct std::formatter<L, Char>
{
  constexpr auto parse(std::basic_format_parse_context<Char>& ctx)
  {
    return ctx.begin();
  }

  template <class O>
  constexpr auto
  format(const L& x, std::basic_format_context<O, Char>& ctx) const
  {
    return std::format_to(ctx.out(), "{}", x.value());
  }
};
