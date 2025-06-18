#include "sel/variable.hpp"
#include "skytest/skytest.hpp"

#include <format>

auto main() -> int
{
  using namespace skytest::literals;
  using namespace skytest;

  "variable cannot be empty"_test = [] {
    return expect(aborts([] { sel::variable{""}; }));
  };

  "variable is formattable"_test = [] {
    const auto x = std::format("{}", sel::variable{"x"});
    const auto y = std::format("{}", sel::variable{"y"});

    return expect(eq("x", x) and eq("y", y));
  };
}
