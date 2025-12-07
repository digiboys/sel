#pragma once

#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

namespace sel::detail {

/// applies an invocable to each element of a tuple
inline constexpr struct
{
  template <class Tuple>
  [[nodiscard]]
  static constexpr auto operator()(Tuple&& tup, auto transform)
  {
    using tuple_type = std::remove_cvref_t<Tuple>;

    return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
      return std::tuple{transform(std::get<Is>(std::forward<Tuple>(tup)))...};
    }(std::make_index_sequence<std::tuple_size_v<tuple_type>>{});
  }
} tuple_transform_reduce{};

}  // namespace sel::detail
