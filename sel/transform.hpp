#pragma once

#include "sel/detail/transform.hpp"
#include "sel/operation.hpp"
#include "sel/term.hpp"

#include <cstddef>
#include <tuple>

namespace sel {

/// apply a transformation to a `term`
/// @param expr `term` to transform
/// @param func transformation function
/// @return copy of `expr` with `func` applied to it
///
/// Applies transformation function `func` to `term`.
///
/// If `func(expr)` is a valid expression, returns `func(expr)`; otherwise if
/// `expr` is an `operation`,returns `operation{func(args)...}` where `args...`
/// is a pack introduced from `expr.args`.
///
inline constexpr struct
{
  template <term T, class F>
  [[nodiscard]]
  constexpr auto operator()(this const auto& self, const T& expr, F func)
  {
    if constexpr (requires {
                    { func(expr) } -> term;
                  }) {
      return func(expr);
    } else if constexpr (operation<T>) {
      return std::apply(
          T::construct,
          detail::transform(
              expr.args,  //
              [&self, &func](const term auto& subexpr) -> term auto {
                return self(subexpr, func);
              }
          )
      );
    }
  }
} transform{};

}  // namespace sel
