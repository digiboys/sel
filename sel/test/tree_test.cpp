#include "sel/constant.hpp"
#include "sel/tree.hpp"
#include "sel/variable.hpp"
#include "skytest/skytest.hpp"

auto main() -> int
{
  using namespace skytest::literals;
  using namespace skytest;

  static constexpr auto x = sel::variable{"x"};
  static constexpr auto y = sel::variable{"y"};

  static constexpr auto a = sel::constant{1};
  static constexpr auto b = sel::constant{2};

  "tree constructible from leaf"_ctest = [] {
    return expect(
        eq(x, sel::tree{x}) and  //
        ne(y, sel::tree{x}) and  //
        ne(a, sel::tree{x}) and  //
        eq(a, sel::tree{a}) and  //
        ne(b, sel::tree{a}) and  //
        ne(x, sel::tree{a})      //
    );
  };

  "tree constructible from operation"_ctest = [] {
    return expect(
        eq(x + a, sel::tree{x + a}) and      //
        ne(x * a, sel::tree{x + a}) and      //
        eq(a * x + b, sel::tree{a * x + b})  //
    );
  };

  "trees are comparable"_ctest = [] {
    return expect(
        eq(sel::tree{x + a}, sel::tree{x + a}) and      //
        ne(sel::tree{x + b}, sel::tree{x + a}) and      //
        ne(sel::tree{x * a}, sel::tree{x + a}) and      //
        eq(sel::tree{a * x + b}, sel::tree{a * x + b})  //
    );
  };
}
