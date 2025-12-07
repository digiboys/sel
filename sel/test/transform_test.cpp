#include "sel/constant.hpp"
#include "sel/multiplies.hpp"
#include "sel/operation.hpp"
#include "sel/plus.hpp"
#include "sel/transform.hpp"
#include "sel/tree.hpp"
#include "sel/variable.hpp"
#include "skytest/skytest.hpp"

#include <cstddef>
#include <utility>

auto main() -> int
{
  using namespace skytest::literals;
  using namespace skytest;

  static constexpr auto x = sel::variable{"x"};
  static constexpr auto y = sel::variable{"y"};

  static constexpr auto a = sel::constant{1};
  static constexpr auto b = sel::constant{2};

  "replaces variables with constants"_ctest = [] {
    return expect(
        eq(a + b,
           sel::transform(
               sel::tree{x + y},
               sel::equal_to{x + y}.then([](const auto&) { return a + b; })
           )) and  //
        eq(a + b,
           sel::transform(sel::tree{x + y}, sel::equal_to{x + y}.then(a + b)))
    );
  };

  "replaces subexpressions"_ctest = [] {
    static constexpr auto f = sel::multiplies{x, x};
    static constexpr auto g = sel::multiplies{x, y};
    return expect(eq(
        a + a + g,
        sel::transform(
            f + f + g,  //
            sel::equal_to{f}.then(a)
        )
    ));
  };

  //  "replaces operation types"_ctest = [] {
  //    return expect(eq(
  //        sel::plus{sel::plus{sel::plus{sel::constant{0}}}},
  //        sel::transform(
  //            sel::multiplies{sel::plus{sel::multiplies{sel::constant{0}}}},
  //            {//
  //             &sel::tree::is_node<sel::plus>,
  //             [](this const auto& self, const sel::tree::node<sel::plus>&
  //             node) {
  //               return sel::tree
  //               {
  //                 sel::node_tag<sel::plus>,
  //                 std::from_range,
  //                 node.args | std::views::transform([](const ::sel::tree&
  //                 subtree) {
  //                        return sel::transform(subtree,
  //                        {&sel::tree::is_node<sel::plus>, self});
  //                 })
  //               };
  //             }
  //            }
  //        )
  //    ));
  //  };

  "expands an expression"_ctest = [] {
    static constexpr auto f = x * x;

    return expect(eq(
        a + f,
        sel::transform(
            a + b,  //
            sel::equal_to{b}.then(f)
        )
    ));
  };
}
