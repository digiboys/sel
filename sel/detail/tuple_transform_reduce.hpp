#pragma once

#include <cstddef>
#include <tuple>
#include <utility>

namespace sel::detail {

/// applies an invocable to each element of two tuples, then reduces
inline constexpr struct
{
  template <class... T1, class... T2, class T>
    requires (sizeof...(T1) == sizeof...(T2))
  [[nodiscard]]
  static constexpr auto operator()(
      const std::tuple<T1...>& tup1,
      const std::tuple<T2...>& tup2,
      T init,
      auto reduce,
      auto transform
  ) -> T
  {
    return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
      auto _ = {(
          init = reduce(
              std::move(init),  //
              transform(std::get<Is>(tup1), std::get<Is>(tup2))
          ),
          true
      )...};

      return init;
    }(std::index_sequence_for<T1...>{});
  }
} tuple_transform_reduce{};

}  // namespace sel::detail
