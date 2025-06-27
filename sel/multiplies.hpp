#pragma once

#include "sel/detail/infix_operation_formatter.hpp"
#include "sel/detail/is_specialization_of.hpp"
#include "sel/operation.hpp"
#include "sel/term.hpp"
#include "sel/term_promotable.hpp"

#include <cstddef>
#include <tuple>

namespace sel {

namespace op {
/// tag type denoting the `multiplies` operation
struct multiplies
{
  static constexpr auto is_valid_arity = [](std::size_t n) { return n != 0; };
};
}  // namespace op

/// associative and commutative n-ary multiples operation
///
template <term... Args>
  requires (op::multiplies::is_valid_arity(sizeof...(Args)))
struct multiplies : operation_interface<multiplies<Args...>>
{
  using op_tag = op::multiplies;
  using formatter = detail::infix_operation_formatter<"*", Args...>;

  constexpr explicit multiplies(const Args&... args)
      : operation_interface<multiplies>{args...}
  {}

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
    requires (not detail::is_specialization_of_v<T, multiplies>)
  [[nodiscard]]
  friend constexpr auto operator*(const multiplies& x, const T& y)
  {
    return x * multiplies<term_promoted_type_t<T>>{to_term(y)};
  }

  template <term_promotable T>
    requires (not detail::is_specialization_of_v<T, multiplies>)
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
