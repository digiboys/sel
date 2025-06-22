#pragma once

#include "sel/detail/string_literal.hpp"

#include <concepts>
#include <cstddef>
#include <format>
#include <tuple>
#include <utility>

namespace sel::detail {

/// formatter for infix operations
/// @tparam delim infix symbol between operands
/// @tparam Args operand types
///
template <string_literal delim, class... Args>
struct infix_operation_formatter
{
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
  const std::tuple<Args...>& args;

  /// implicit constructor from an operation
  ///
  template <class T>
    requires requires (const T& value) {
      { value.args } -> std::same_as<const std::tuple<Args...>&>;
    }
  constexpr infix_operation_formatter(const T& value)
      : args{value.args}
  {}
};

}  // namespace sel::detail

template <auto delim, class... Args, class Char>
struct ::std::
    formatter<sel::detail::infix_operation_formatter<delim, Args...>, Char>
{
  template <bool first>
  static constexpr auto fmt = [] {
    if constexpr (first) {
      return "{}";
    } else {
      return " " + delim + " {}";
    }
  }();

  constexpr auto parse(std::basic_format_parse_context<Char>& ctx)
  {
    return ctx.begin();
  }

  template <class O>
  constexpr auto format(
      ::sel::detail::infix_operation_formatter<delim, Args...> expr,
      std::basic_format_context<O, Char>& ctx
  ) const
  {
    auto out = ctx.out();
    out = std::format_to(out, "(");

    [&]<std::size_t... Is>(std::index_sequence<Is...>) {
      auto _ = {(
          out = std::format_to(out, fmt < Is == 0 >, std::get<Is>(expr.args)),
          true
      )...};
    }(std::index_sequence_for<Args...>{});

    out = std::format_to(out, ")");
    return out;
  }
};
