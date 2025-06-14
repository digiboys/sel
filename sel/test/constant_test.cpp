#include "sel/constant.hpp"
#include "skytest/skytest.hpp"

#include <compare>
#include <cstddef>
#include <format>
#include <limits>
#include <tuple>
#include <type_traits>

namespace test {

template <auto... Values>
struct params_t
{
  template <std::size_t I>
  friend constexpr auto get(params_t) -> auto
  {
    using T = std::remove_cvref_t<decltype(Values...[I])>;
    return std::integral_constant<T, Values... [I]> {};
  }
};

template <auto... Values>
constexpr auto params = params_t<Values...>{};

}  // namespace test

template <auto... Values>
struct std::tuple_size<test::params_t<Values...>>
    : std::integral_constant<std::size_t, sizeof...(Values)>
{};

auto main() -> int
{
  using namespace skytest::literals;
  using namespace skytest;

  "constant value cannot be unordered"_test = [] {
    static constexpr auto nan = []<class T>(T) {
      return std::numeric_limits<T>::quiet_NaN();
    };
    return expect(
        aborts([] { sel::constant{nan(float{})}; }) and
        aborts([] { sel::constant{nan(double{})}; })
    );
  };

  "constants always define a strong ordering"_test *
      test::params<1, 2.0F, 3.0> =  //
      []<class K>(K) {
        auto a = sel::constant{K::value};
        auto b = sel::constant{typename K::value_type{2} * K::value};
        return expect(
            eq(std::strong_ordering::less, (a <=> b)) and     //
            eq(std::strong_ordering::greater, (b <=> a)) and  //
            eq(std::strong_ordering::equal, (b <=> b))
        );
      };

  "constants are equality and less than comparable"_test *
      test::params<1, 2.0F, 3.0> =  //
      []<class K>(K) {
        auto a = sel::constant{K::value};
        auto b = sel::constant{typename K::value_type{2} * K::value};
        return expect(eq(a, a) and lt(a, b));
      };

  "constants with different value types are comparable"_test = [] {
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
