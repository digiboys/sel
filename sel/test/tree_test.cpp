#include "sel/constant.hpp"
#include "sel/multiplies.hpp"
#include "sel/plus.hpp"
#include "sel/tree.hpp"
#include "sel/variable.hpp"
#include "skytest/skytest.hpp"

#include <ranges>
#include <type_traits>
#include <utility>

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

  "tree constructible from elements"_ctest = [] {
    return expect(
        eq(x, sel::tree{std::in_place_type<sel::variable>, "x"}) and          //
        eq(a, sel::tree{std::in_place_type<sel::constant<int>>, 1}) and       //
        eq(b, sel::tree{std::in_place_type<sel::constant<double>>, 2.0}) and  //
        eq(x + a, sel::tree{std::in_place_type<sel::op::plus>, x, a}) and     //
        eq(a * x + b,
           sel::tree{std::in_place_type<sel::op::plus>, a * x, b}) and  //
        eq(a + a + a,
           sel::tree{
               std::in_place_type<sel::op::plus>,
               std::from_range,
               std::views::repeat(a, 3)
           })
    );
  };

  "tree op construction requires valid number of args"_ctest = [] {
    return expect(
        not pred(
            std::is_constructible<
                sel::tree,
                std::in_place_type_t<sel::op::plus>>{}
        ) and
        not pred(
            std::is_constructible<
                sel::tree,
                std::in_place_type_t<sel::op::multiplies>>{}
        )
    );
  };

  "trees are comparable"_ctest = [] {
    return expect(
        eq(sel::tree{x + a}, sel::tree{x + a}) and      //
        ne(sel::tree{x + a + b}, sel::tree{x + a}) and  //
        ne(sel::tree{x + b}, sel::tree{x + a}) and      //
        ne(sel::tree{x * a}, sel::tree{x + a}) and      //
        eq(sel::tree{a * x + b}, sel::tree{a * x + b})  //
    );
  };
}
