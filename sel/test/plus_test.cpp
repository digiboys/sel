#include "sel/plus.hpp"
#include "sel/variable.hpp"
#include "skytest/skytest.hpp"

#include <format>

auto main() -> int
{
  using namespace skytest::literals;
  using namespace skytest;

  static constexpr auto x = sel::variable{"x"};
  static constexpr auto y = sel::variable{"y"};
  static constexpr auto z = sel::variable{"z"};

  "plus expression can have a single argument"_ctest = [] {
    const auto _ = sel::plus{x};
    return expect(true);
  };

  "binary + returns a plus expression"_ctest = [] {
    return expect(
        eq(sel::plus{x, x}, x + x) and  //
        eq(sel::plus{x, y}, x + y) and  //
        eq(sel::plus{y, x}, y + x)
    );
  };

  "+ can be used with more than 2 arguments"_ctest = [] {
    return expect(
        eq(sel::plus{x, y, z}, x + y + z) and             //
        eq(sel::plus{x, y, z}, x + (y + z)) and           //
        eq(sel::plus{x, y, z}, (x + y) + z) and           //
        eq(sel::plus{x, y, y, z}, (x + y) + (y + z)) and  //
        eq(sel::plus{x, y, y, z}, x + y + y + z)
    );
  };

  "plus is formattable"_test = [] {
    return expect(
        eq("(x)", std::format("{}", sel::plus{x})) and         //
        eq("(x + y)", std::format("{}", sel::plus{x, y})) and  //
        eq("(x + y + z)", std::format("{}", sel::plus{x, y, z}))
    );
  };
}
