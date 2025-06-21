#pragma once

#include "sel/operation.hpp"
#include "sel/strongly_ordered.hpp"

#include <concepts>

namespace sel {

/// deriving from `term_base` enables types to model `term`
struct term_base
{
  friend auto operator<=>(const term_base&, const term_base&) = default;
};

/// hook for types to declare usability as a term
template <class T>
constexpr bool enable_as_term =  //
    std::derived_from<T, term_base> or operation<T>;

/// a symbol that represents a quantity in an expression
///
/// A type that models `term` must be `copyable` and must explicitly declare
/// itself as usable as a `term` by specializing `enable_as_term` or inheriting
/// from `term_base`.
///
/// Additionally, all valid values of the type must be totally ordered in order
/// to allow reordering of terms in an expression.
template <class T>
concept term =               //
    std::copyable<T> and     //
    strongly_ordered<T> and  //
    enable_as_term<T>;

}  // namespace sel
