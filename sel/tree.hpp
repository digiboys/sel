#pragma once

#include "sel/constant.hpp"
#include "sel/contract.hpp"
#include "sel/detail/tuple_ref.hpp"
#include "sel/detail/tuple_transform_reduce.hpp"
#include "sel/leaf.hpp"
#include "sel/multiplies.hpp"
#include "sel/operation.hpp"
#include "sel/plus.hpp"
#include "sel/term.hpp"
#include "sel/variable.hpp"

#include <concepts>
#include <cstddef>
#include <functional>
#include <ranges>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

namespace sel {

/// type-erased representation of an expression
class tree
{
  /// internal (non-terminal) element describing an operation
  template <class OpTag>
  struct op_node
  {
    std::vector<tree> args;

    template <class... Args>
      requires (
          (std::same_as<tree, std::remove_cvref_t<Args>> or
           term<std::remove_cvref_t<Args>>) and
          ...
      )
    constexpr explicit op_node(Args&&... args)
        : args{tree{std::forward<Args>(args)}...}
    {}

    constexpr op_node(std::from_range_t, std::ranges::sized_range auto& values)
    {
      SEL_PRECONDITION(OpTag::is_valid_arity(std::ranges::size(values)));
      args.reserve(std::ranges::size(values));

      for (auto&& value : values) {
        args.emplace_back(std::forward<decltype(value)>(value));
      }
    }

    friend auto operator==(const op_node&, const op_node&) -> bool = default;
  };

  using node_type = std::variant<
      constant<int>,
      constant<double>,
      variable,
      op_node<op::plus>,
      op_node<op::multiplies>>;

  node_type value_;

public:
  /// construct from a `leaf`
  template <class T>
    requires leaf<std::remove_cvref_t<T>>
  constexpr explicit tree(T&& value)
      : value_{std::forward<T>(value)}
  {}

  /// construct from an `operation`
  template <class Op>
    requires (not std::same_as<tree, std::remove_cvref_t<Op>>) and
             operation<std::remove_cvref_t<Op>>
  constexpr explicit tree(Op&& expr)
      : value_{std::make_from_tuple<op_node<typename std::remove_cvref_t<
            Op>::op_tag>>(std::forward<Op>(expr).args)}
  {}

  /// construct from elements
  /// @{
  template <leaf L, class T>
    requires std::constructible_from<L, T>
  constexpr tree(std::in_place_type_t<L>, T&& value)
      : value_{std::in_place_type<L>, std::forward<T>(value)}
  {}

  template <class OpTag, class... Args>
    requires (OpTag::is_valid_arity(sizeof...(Args)))
  constexpr tree(std::in_place_type_t<OpTag>, Args&&... args)
      : value_{std::in_place_type<op_node<OpTag>>, std::forward<Args>(args)...}
  {}

  template <class OpTag>
  constexpr tree(
      std::in_place_type_t<OpTag>,
      std::from_range_t,
      std::ranges::sized_range auto&& args
  )
      : value_{std::in_place_type<op_node<OpTag>>, std::from_range, args}
  {}
  /// @}

  /// compare a `tree` with a `leaf`
  [[nodiscard]]
  constexpr auto operator==(const leaf auto& other) const -> bool
  {
    return std::visit(
        [&other](const auto& value) -> bool {
          if constexpr (requires { bool(value == other); }) {
            return value == other;
          } else {
            return false;
          }
        },
        value_
    );
  }

  /// compare a `tree` with an `operation`
  template <template <class...> class op, class... Args>
    requires operation<op<Args...>>
  [[nodiscard]]
  constexpr auto operator==(const op<Args...>& other) const -> bool
  {
    if (const auto value =
            std::get_if<op_node<typename op<Args...>::op_tag>>(&value_)) {
      if (value->args.size() == sizeof...(Args)) {
        return detail::tuple_transform_reduce(
            detail::tuple_ref<sizeof...(Args)>(value->args),
            other.args,
            true,
            std::logical_and{},
            std::equal_to{}
        );
      }
    }
    return false;
  }

  /// compare two `tree`s
  friend auto operator==(const tree&, const tree&) -> bool = default;
};

}  // namespace sel
