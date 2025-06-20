#pragma once

#include <type_traits>

namespace sel::detail {

// https://wg21.link/p2098

template <class T, template <class...> class primary>
struct is_specialization_of : std::false_type
{};

template <template <class...> class primary, class... Args>
struct is_specialization_of<primary<Args...>, primary> : std::true_type
{};

template <class T, template <class...> class primary>
inline constexpr bool is_specialization_of_v =
    is_specialization_of<T, primary>::value;

}  // namespace sel::detail
