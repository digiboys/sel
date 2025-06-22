#pragma once

#include "sel/constant.hpp"
#include "sel/detail/tuple_ref.hpp"
#include "sel/detail/tuple_transform_reduce.hpp"
#include "sel/leaf.hpp"
#include "sel/multiplies.hpp"
#include "sel/operation.hpp"
#include "sel/plus.hpp"
#include "sel/variable.hpp"

#include <cstddef>
#include <functional>
#include <tuple>
#include <variant>
#include <vector>

namespace sel {

/// type-erased representation of an expression
class tree
{
  /// internal (non-terminal) element describing an operation
  template <template <class...> class op>
  struct node
  {
    template <class... Args>
    using operation_type = op<Args...>;

    std::vector<tree> args;

    template <class... Args>
    constexpr explicit node(const Args&... args)
        : args{tree{args}...}
    {}

    friend auto operator==(const node&, const node&) -> bool = default;
  };

  using node_type = std::variant<
      constant<int>,
      constant<double>,
      variable,
      node<plus>,
      node<multiplies>>;

  node_type value_;

public:
  /// construct a `tree` from a `leaf`
  constexpr explicit tree(const leaf auto& value)
      : value_{value}
  {}

  /// construct a `tree` from an `operation`
  template <template <class...> class op, class... Args>
    requires operation<op<Args...>>
  constexpr explicit tree(const op<Args...>& expr)
      : value_{std::make_from_tuple<node<op>>(expr.args)}
  {}

  /// compare a `tree` with a `leaf`
  [[nodiscard]]
  constexpr auto operator==(const leaf auto& other) const -> bool
  {
    return value_.visit([&other](const auto& value) {
      if constexpr (requires { bool(value == other); }) {
        return value == other;
      } else {
        return false;
      }
    });
  }

  /// compare a `tree` with an `operation`
  template <template <class...> class op, class... Args>
    requires operation<op<Args...>>
  [[nodiscard]]
  constexpr auto operator==(const op<Args...>& other) const -> bool
  {
    if (const auto value = std::get_if<node<op>>(&value_)) {
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
