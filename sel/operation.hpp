#pragma once

#include "sel/strongly_ordered.hpp"

#include <concepts>
#include <format>
#include <tuple>

namespace sel {

template <class>
struct operation_interface;

/// deriving from `operation_interface` enables types to model `operation`
template <template <class...> class op, class... Args>
  requires (std::copyable<Args> and ...) and  //
           (strongly_ordered<Args> and ...)
struct operation_interface<op<Args...>>
{
  std::tuple<Args...> args;

  constexpr operation_interface(const Args&... args)
      : args{args...}
  {}

  friend auto
  operator<=>(const operation_interface&, const operation_interface&) = default;
};

/// function that takes one or more arguments
///
/// A type that models `operation` must be `copyable` and must explicitly
/// declare itself as usable as a `term` by inheriting from
/// `operation_interface`.
///
/// Additionally, all valid values of the type must be totally ordered in order
/// to allow reordering of terms in an expression.
template <class T>
concept operation =                          //
    (not std::default_initializable<T>) and  //
    std::copyable<T> and                     //
    strongly_ordered<T> and                  //
    std::derived_from<T, operation_interface<T>>;

}  // namespace sel

template <class Op, class Char>
  requires std::derived_from<Op, ::sel::operation_interface<Op>>
struct ::std::formatter<Op, Char> : std::formatter<typename Op::formatter, Char>
{};
