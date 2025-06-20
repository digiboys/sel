#pragma once

#include "sel/constant.hpp"
#include "sel/term.hpp"

#include <concepts>
#include <type_traits>

namespace sel {

/// returns a `term`
/// This overload forwards a `term` unchanged
template <class T>
  requires term<std::remove_cvref_t<T>>
[[nodiscard]]
constexpr auto to_term(T&& value) -> T&&
{
  return std::forward<T>(value);
}

/// returns a `term`
/// This overload creates a `constant`
template <class T>
  requires std::signed_integral<T> or std::floating_point<T>
[[nodiscard]]
constexpr auto to_term(const T& value) -> constant<T>
{
  return constant<T>{value};
}

/// a type that can be implicitly promoted to a `term`
///
template <class T>
concept term_promotable =  //
    term<T> or             //
    requires (T a) {
      { to_term(a) };
    };

/// obtain the term type after promotion
/// @{
template <class T, bool = term_promotable<T>>
struct term_promoted_type
{
  using type = decltype(auto{to_term(std::declval<T>())});
};
template <class T>
struct term_promoted_type<T, false>
{};
template <class T>
using term_promoted_type_t = typename term_promoted_type<T>::type;
/// @}

}  // namespace sel
