#pragma once

#include "sel/leaf.hpp"
#include "sel/operation.hpp"
#include "sel/term.hpp"
#include "sel/tree.hpp"

#include <concepts>
#include <cstddef>
#include <functional>
#include <ranges>
#include <tuple>

namespace sel {

template <class... Ts>
struct overloads : Ts...
{
  using Ts::operator()...;
};

/// apply a transformation to a `tree`
/// @param expr `tree` to transform
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
  [[nodiscard]]
  constexpr auto
  operator()(this const auto& self, const tree& expr, auto func) -> tree
  {
    if (func.pred(expr)) {
      return func.transform(expr);
    }

    return expr.node().visit(overloads(
        [](const leaf auto& node) -> tree { return node; },
        [transform = std::views::transform(std::bind_back(self, func))]<
            class OpNode>(const OpNode& op) -> tree {
          return {
              std::in_place_type<typename OpNode::op_tag_type>,
              std::from_range,
              op.args | transform
          };
        }
    ));
  }
} transform{};

template <class Pred, class Transform>
struct conditional_transformer
{
  Pred pred;
  Transform transform_;

  [[nodiscard]]
  constexpr auto transform(const tree& expr) const -> tree
  {
    if constexpr (std::convertible_to<Transform, tree>) {
      return transform_;
    } else {
      return transform_(expr);
    }
  }
};

template <class T>
struct equal_to
{
  T value;

  template <class U>
  [[nodiscard]]
  constexpr auto then(const U& transform_) const
  {
    return conditional_transformer{
        // TODO __cpp_lib_bind_back >= 202306L
        std::bind_back(std::equal_to{}, value),  //
        transform_
    };
  }
};

}  // namespace sel
