#pragma once

#include "sel/term.hpp"
#include "sel/term_promotable.hpp"

#include <cstddef>
#include <format>
#include <string_view>
#include <tuple>
#include <utility>

namespace sel {

/// associative and commutative n-ary multiples operation
///
template <term... Args>
  requires (sizeof...(Args) != 0)
struct multiplies
{
  std::tuple<Args...> args{};

  constexpr multiplies(const Args&... args)
      : args{args...}
  {}

  friend auto operator<=>(const multiplies&, const multiplies&) = default;

  template <term... Ts>
  [[nodiscard]]
  friend constexpr auto
  operator*(const multiplies& x, const multiplies<Ts...>& y)
  {
    // TODO(replace)
    // https://en.cppreference.com/w/cpp/compiler_support/26.html#cpp_structured_bindings_202411L
    return std::make_from_tuple<multiplies<Args..., Ts...>>(
        std::tuple_cat(x.args, y.args)
    );
  }

  template <term_promotable T>
  [[nodiscard]]
  friend constexpr auto operator*(const multiplies& x, const T& y)
  {
    return x * multiplies<term_promoted_type_t<T>>{to_term(y)};
  }

  template <term_promotable T>
  [[nodiscard]]
  friend constexpr auto operator*(const T& x, const multiplies& y)
  {
    return multiplies<term_promoted_type_t<T>>{to_term(x)} * y;
  }
};

template <term_promotable T1, term_promotable T2>
[[nodiscard]]
constexpr auto operator*(const T1& x, const T2& y)
{
  return multiplies{to_term(x), to_term(y)};
}

}  // namespace sel

template <class... Args, class Char>
struct ::std::formatter<::sel::multiplies<Args...>, Char>
    : std::formatter<std::string_view, Char>
{
  template <class O>
  constexpr auto format(
      const ::sel::multiplies<Args...>& expr,
      std::basic_format_context<O, Char>& ctx
  ) const
  {
    auto out = ctx.out();
    out = std::format_to(out, "(");

    [&]<std::size_t... Is>(std::index_sequence<Is...>) {
      auto fmt = [](bool first) { return first ? "{}" : " * {}"; };

      auto _ = {(
          out = std::format_to(out, fmt(Is == 0), std::get<Is>(expr.args)), 0
      )...};
    }(std::index_sequence_for<Args...>{});

    out = std::format_to(out, ")");
    return out;
  }
};
