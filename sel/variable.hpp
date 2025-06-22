#pragma once

#include "sel/contract.hpp"
#include "sel/leaf.hpp"

#include <string>
#include <utility>

namespace sel {

/// symbol that represents an unspecified quantity in an expression
class variable : public leaf_base<variable>
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
  [[nodiscard]]
  constexpr auto value(this Self&& self) -> auto&&
  {
    return std::forward<Self>(self).value_;
  }

  friend auto operator<=>(const variable&, const variable&) = default;
};

}  // namespace sel
