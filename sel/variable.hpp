#pragma once

#include "sel/contract.hpp"

#include <format>
#include <string>
#include <string_view>
#include <utility>

namespace sel {

/// symbol that represents an unspecified quantity in an expression
class variable
{
  std::string value_{};

public:
  constexpr variable(std::string value)
      : value_{[&] {
          SEL_PRECONDITION(not value.empty());
          return std::move(value);
        }()}
  {}

  template <class Self>
  constexpr auto value(this Self&& self) -> auto&&
  {
    return std::forward<Self>(self).value_;
  }

  friend auto operator<=>(const variable&, const variable&) = default;
};

}  // namespace sel

template <class Char>
struct ::std::formatter<::sel::variable, Char>
    : std::formatter<std::string_view, Char>
{
  template <class O>
  constexpr auto format(
      const ::sel::variable& x, std::basic_format_context<O, Char>& ctx
  ) const
  {
    return std::format_to(ctx.out(), "{}", x.value());
  }
};
