#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <string_view>

namespace sel::detail {

// NOLINTBEGIN(modernize-avoid-c-arrays)

/// string-like type with consteval concatenation
///
template <std::size_t N>
struct string_literal
{
  // this member is public so that `string_literal` is structural
  std::array<char, N> chars{};

  consteval string_literal() = default;
  consteval string_literal(const char (&str)[N + 1])
  {
    std::copy_n(std::begin(str), N, chars.begin());
  }

  consteval operator std::string_view() const
  {
    return std::string_view{chars};
  }

  template <std::size_t M>
  friend consteval auto
  operator+(const string_literal& lhs, const string_literal<M>& rhs)
  {
    auto joined = string_literal<N + M>{};

    auto [_, it] = std::ranges::copy(lhs.chars, joined.chars.begin());
    auto _ = std::ranges::copy(rhs.chars, it);

    return joined;
  }

  template <std::size_t M>
  friend consteval auto
  operator+(const char (&lhs)[M], const string_literal& rhs)
  {
    return string_literal<M - 1>{lhs} + rhs;
  }
  template <std::size_t M>
  friend consteval auto
  operator+(const string_literal& lhs, const char (&rhs)[M])
  {
    return lhs + string_literal<M - 1>{rhs};
  }
};

template <std::size_t N>
string_literal(const char (&)[N]) -> string_literal<N - 1>;

// NOLINTEND(modernize-avoid-c-arrays)

}  // namespace sel::detail
