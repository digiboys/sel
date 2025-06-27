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
/// tag type denoting the `plus` operation
struct plus
{
  static constexpr auto is_valid_arity = [](std::size_t n) { return n != 0; };
};
}  // namespace op

/// associative and commutative n-ary plus operation
///
template <term... Args>
  requires (op::plus::is_valid_arity(sizeof...(Args)))
struct plus : operation_interface<plus<Args...>>
{
  using op_tag = op::plus;
  using formatter = detail::infix_operation_formatter<"+", Args...>;

  constexpr explicit plus(const Args&... args)
      : operation_interface<plus>{args...}
  {}

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

  template <term_promotable T>
    requires (not detail::is_specialization_of_v<T, plus>)
  [[nodiscard]]
  friend constexpr auto operator+(const plus& x, const T& y)
  {
    return x + plus<term_promoted_type_t<T>>{to_term(y)};
  }

  template <term_promotable T>
    requires (not detail::is_specialization_of_v<T, plus>)
  [[nodiscard]]
  friend constexpr auto operator+(const T& x, const plus& y)
  {
    return plus<term_promoted_type_t<T>>{to_term(x)} + y;
  }
};

template <term_promotable T1, term_promotable T2>
[[nodiscard]]
constexpr auto operator+(const T1& x, const T2& y)
{
  return plus{to_term(x), to_term(y)};
}

}  // namespace sel
