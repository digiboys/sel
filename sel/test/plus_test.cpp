#include "sel/constant.hpp"
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

  static constexpr auto a = sel::constant{1};
  static constexpr auto b = sel::constant{2};

  "plus expression can have a single argument"_ctest = [] {
    auto _ = sel::plus{x};
    auto _ = sel::plus{a};
    return expect(true);
  };

  "binary + returns a plus expression"_ctest = [] {
    return expect(
        eq(sel::plus{x, x}, x + x) and  //
        eq(sel::plus{x, y}, x + y) and  //
        eq(sel::plus{y, x}, y + x) and  //
        eq(sel::plus{a, a}, a + a) and  //
        eq(sel::plus{a, b}, a + b) and  //
        eq(sel::plus{a, x}, a + x)
    );
  };

  "binary + can implicit convert a signed integer or floating point type to `constant`"_ctest =
      [] {
        return expect(
            eq(sel::plus{sel::constant{3}, x}, 3 + x) and           //
            eq(sel::plus{x, sel::constant{3.0}}, x + 3.0) and       //
            eq(sel::plus{x, y, sel::constant{0}}, (x + y) + 0) and  //
            eq(sel::plus{x, y, sel::constant{3}}, (x + y) + (1 + 2))
        );
      };

  "+ can be used with more than 2 arguments"_ctest = [] {
    return expect(
        eq(sel::plus{x, y, z}, x + y + z) and             //
        eq(sel::plus{x, y, z}, x + (y + z)) and           //
        eq(sel::plus{x, y, z}, (x + y) + z) and           //
        eq(sel::plus{x, y, y, z}, (x + y) + (y + z)) and  //
        eq(sel::plus{x, y, y, z}, x + y + y + z) and      //
        eq(sel::plus{a, x, b, y}, a + x + b + y)
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
