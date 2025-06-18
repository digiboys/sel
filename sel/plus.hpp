#pragma once

#include "sel/term.hpp"

#include <cstddef>
#include <format>
#include <string_view>
#include <tuple>
#include <utility>

namespace sel {

/// associative and commutative n-ary plus operation
///
template <term... Args>
  requires (sizeof...(Args) != 0)
struct plus
{
  std::tuple<Args...> args{};

  constexpr plus(const Args&... args)
      : args{args...}
  {}

  friend auto operator<=>(const plus&, const plus&) = default;

  template <term... Ts>
  [[nodiscard]]
  friend constexpr auto operator+(const plus& x, const plus<Ts...>& y)
  {
    // TODO(replace)
    // https://en.cppreference.com/w/cpp/compiler_support/26.html#cpp_structured_bindings_202411L
    return std::make_from_tuple<plus<Args..., Ts...>>(
        std::tuple_cat(x.args, y.args)
    );
  }

  template <term T>
  [[nodiscard]]
  friend constexpr auto operator+(const plus& x, const T& y)
  {
    return x + plus<T>{y};
  }

  template <term T>
  [[nodiscard]]
  friend constexpr auto operator+(const T& x, const plus& y)
  {
    return plus<T>{x} + y;
  }
};

template <term T1, term T2>
[[nodiscard]]
constexpr auto operator+(const T1& x, const T2& y)
{
  return plus<T1, T2>{x, y};
}

}  // namespace sel

template <class... Args, class Char>
struct ::std::formatter<::sel::plus<Args...>, Char>
    : std::formatter<std::string_view, Char>
{
  template <class O>
  constexpr auto format(
      const ::sel::plus<Args...>& expr, std::basic_format_context<O, Char>& ctx
  ) const
  {
    auto out = ctx.out();
    out = std::format_to(out, "(");

    [&]<std::size_t... Is>(std::index_sequence<Is...>) {
      auto fmt = [](bool first) { return first ? "{}" : " + {}"; };

      auto _ = {(
          out = std::format_to(out, fmt(Is == 0), std::get<Is>(expr.args)), 0
      )...};
    }(std::index_sequence_for<Args...>{});

    out = std::format_to(out, ")");
    return out;
  }
};
