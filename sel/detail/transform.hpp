#pragma once

#include <concepts>
#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

namespace sel::detail {

/// apply a transformation to a `std::tuple`
/// @param values `std::tuple` to transform
/// @param func transformation function
///
/// Applies transformation function `func` to each element in `values`,
///
/// @return `std::tuple` of the transformed elements
///
inline constexpr struct
{
  template <class... Args, class F>
    requires (std::regular_invocable<F&, const Args&> and ...)
  [[nodiscard]]
  static constexpr auto operator()(const std::tuple<Args...>& values, F func)
      -> std::tuple<std::invoke_result_t<F&, const Args&>...>
  {
    return [&values, &func]<std::size_t... Is>(std::index_sequence<Is...>) {
      return std::tuple{std::invoke(func, std::get<Is>(values))...};
    }(std::index_sequence_for<Args...>{});
  }
} transform{};

}  // namespace sel::detail
