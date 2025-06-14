#pragma once

#include <concepts>

namespace sel {

/// a symbol that represents a quantity in an expression
template <class T>
concept term = std::same_as<T, class variable>;

}  // namespace sel
