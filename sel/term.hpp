#pragma once

#include "sel/strongly_ordered.hpp"

#include <concepts>

namespace sel {

/// hook for types to declare usability as a term
template <class T>
constexpr bool enable_as_term = false;

/// a symbol that represents a quantity in an expression
///
/// A type that models `term` must be `copyable` and must explicitly declare
/// itself as usable as a `term` by specializing `enable_as_term`.
///
/// Additionally, all valid values of the type must be totally ordered in order
/// to allow reordering of terms in an expression.
template <class T>
concept term =               //
    std::copyable<T> and     //
    strongly_ordered<T> and  //
    enable_as_term<T>;

}  // namespace sel
