#pragma once

#include "sel/leaf.hpp"
#include "sel/operation.hpp"

namespace sel {

/// a symbol that represents a quantity in an expression
///
/// A type that models `term` must model `operation` or `leaf`.
template <class T>
concept term = operation<T> or leaf<T>;

}  // namespace sel
