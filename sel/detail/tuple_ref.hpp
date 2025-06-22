#pragma once

#include <cstddef>
#include <functional>
#include <ranges>
#include <tuple>
#include <utility>

namespace sel::detail {

/// create a tuple of references from a random access range
template <std::size_t N>
struct tuple_ref_impl
{
  [[nodiscard]]
  static constexpr auto
  operator()(std::ranges::random_access_range auto& values)
  {
    return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
      return std::make_tuple(std::ref(values[Is])...);
    }(std::make_index_sequence<N>{});
  }
};

template <std::size_t N>
inline constexpr auto tuple_ref = tuple_ref_impl<N>{};

}  // namespace sel::detail
