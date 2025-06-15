#pragma once

#include <compare>
#include <concepts>

namespace sel {

template <class T>
concept strongly_ordered =       //
    std::totally_ordered<T> and  //
    requires (T x) {
      { x <=> x } -> std::same_as<std::strong_ordering>;
    };

}  // namespace sel
