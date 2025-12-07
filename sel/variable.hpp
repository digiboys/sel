#pragma once

#include "sel/contract.hpp"
#include "sel/leaf.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <string_view>

namespace sel {

/// symbol that represents an unspecified quantity in an expression
class variable : public leaf_base<variable>
{
  static constexpr auto max_variable_size = 10;
  std::array<char, max_variable_size> value_{};
  std::uint8_t size_{};

public:
  constexpr variable(std::string_view value)
  {
    SEL_PRECONDITION(not value.empty());
    SEL_PRECONDITION(value.size() < max_variable_size);

    std::ranges::copy(value, value_.begin());
    size_ = static_cast<std::uint8_t>(value.size());
  }

  [[nodiscard]]
  constexpr auto value() const -> std::string_view
  {
    return {value_.data(), size_};
  }

  friend auto operator<=>(const variable&, const variable&) = default;
};

}  // namespace sel
