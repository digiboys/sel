#include "sel/constant.hpp"
#include "skytest/skytest.hpp"

#include <compare>
#include <cstddef>
#include <format>
#include <limits>

auto main() -> int
{
  using namespace skytest::literals;
  using namespace skytest;

  "constant value cannot be unordered"_test = [] {
    return expect(
        aborts([] {
          sel::constant{std::numeric_limits<float>::quiet_NaN()};
        }) and
        aborts([] { sel::constant{std::numeric_limits<double>::quiet_NaN()}; })
    );
  };

  "constants always define a strong ordering"_ctest *
      constexpr_params<1, 2.0F, 3.0> =  //
      []<class T>(T value) {
        auto a = sel::constant{value};
        auto b = sel::constant{T{2} * value};
        return expect(
            eq(std::strong_ordering::less, (a <=> b)) and     //
            eq(std::strong_ordering::greater, (b <=> a)) and  //
            eq(std::strong_ordering::equal, (b <=> b))
        );
      };

  "constants are equality and less than comparable"_ctest *
      constexpr_params<1, 2.0F, 3.0> =  //
      []<class T>(T value) {
        auto a = sel::constant{value};
        auto b = sel::constant{T{2} * value};
        return expect(eq(a, a) and lt(a, b));
      };

  "constants with different value types are comparable"_ctest = [] {
    auto a = sel::constant{1};
    auto b = sel::constant{1.0};
    return expect(
        eq(a, b) and  //
        eq(std::strong_ordering::equal, (a <=> b))
    );
  };

  "constant is formattable"_test = [] {
    const auto x = std::format("{}", sel::constant{42});
    const auto y = std::format("{}", sel::constant{3.14});

    return expect(eq("42", x) and eq("3.14", y));
  };
}
