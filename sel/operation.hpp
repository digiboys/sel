#pragma once

#include "sel/strongly_ordered.hpp"

#include <concepts>
#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

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

  /// construct from arguments
  constexpr operation_interface(const Args&... args)
      : args{args...}
  {}

  /// construct the derived operation with different arguments
  static constexpr auto construct =
      []<class... Ts>(const Ts&... args) -> op<Ts...> { return {args...}; };

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

template <template <class...> class op, class... Args>
  requires std::
      derived_from<op<Args...>, ::sel::operation_interface<op<Args...>>>
    struct std::tuple_size<op<Args...>>
    : std::integral_constant<std::size_t, sizeof...(Args)>
{};
