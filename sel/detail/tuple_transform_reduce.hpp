#pragma once

#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

namespace sel::detail {

/// applies an invocable to each element of two tuples, then reduces
inline constexpr struct
{
  template <class Tuple1, class Tuple2, class T>
    requires (
        std::tuple_size_v<std::remove_cvref_t<Tuple1>> ==
        std::tuple_size_v<std::remove_cvref_t<Tuple2>>
    )
  [[nodiscard]]
  static constexpr auto
  operator()(Tuple1&& tup1, Tuple2&& tup2, T init, auto reduce, auto transform)
      -> T
  {
    return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
      auto _ = {(
          init = reduce(
              std::move(init),  //
              transform(
                  std::get<Is>(std::forward<Tuple1>(tup1)),
                  std::get<Is>(std::forward<Tuple2>(tup2))
              )
          ),
          true
      )...};

      return init;
    }(std::make_index_sequence<
               std::tuple_size_v<std::remove_cvref_t<Tuple1>>>{});
  }
} tuple_transform_reduce{};

}  // namespace sel::detail
