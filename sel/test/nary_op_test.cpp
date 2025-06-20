#include "sel/constant.hpp"
#include "sel/multiplies.hpp"
#include "sel/plus.hpp"
#include "sel/variable.hpp"
#include "skytest/skytest.hpp"

#include <format>
#include <functional>
#include <tuple>
#include <type_traits>

namespace test {

template <template <class...> class nary_op>
struct construct
{
  template <class... Ts>
  constexpr auto operator()() const -> nary_op<Ts...>
  {
    return {};
  }
};

}  // namespace test

auto main() -> int
{
  using namespace skytest::literals;
  using namespace skytest;

  static constexpr auto x = sel::variable{"x"};
  static constexpr auto y = sel::variable{"y"};
  static constexpr auto z = sel::variable{"z"};

  static constexpr auto a = sel::constant{1};
  static constexpr auto b = sel::constant{2};

  "nary op cannot have zero arguments"_ctest = [] {
    return expect(
        not pred(std::is_invocable<test::construct<sel::plus>>{})() and
        not pred(std::is_invocable<test::construct<sel::multiplies>>{})()
    );
  };

  "nary op suite"_test *
      std::tuple{std::plus{}, std::multiplies{}} = []<class T>(T) {
    static constexpr auto info = [] {
      if constexpr (std::is_same_v<T, std::plus<>>) {
        return std::tuple{
            "+", [](const auto&... args) { return sel::plus{args...}; }
        };
      }
      if constexpr (std::is_same_v<T, std::multiplies<>>) {
        return std::tuple{
            "*", [](const auto&... args) { return sel::multiplies{args...}; }
        };
      }
    }();

    static constexpr auto glyph = std::get<0>(info);
    static constexpr auto nary_op = std::get<1>(info);
    static constexpr auto op = T{};

    "expression can have a single argument"_ctest * types<T> = [](auto) {
      auto _ = nary_op(x);
      auto _ = nary_op(a);
      return expect(true);
    };

    "nary op can be used with two arguments"_ctest * types<T> = [](auto) {
      return expect(
          eq(nary_op(x, x), op(x, x)) and  //
          eq(nary_op(x, y), op(x, y)) and  //
          eq(nary_op(y, x), op(y, x)) and  //
          eq(nary_op(a, a), op(a, a)) and  //
          eq(nary_op(a, b), op(a, b)) and  //
          eq(nary_op(a, x), op(a, x))
      );
    };

    "nary op can implicitly convert a signed integer or floating point type to `constant`"_ctest *
        types<T> = [](auto) {
      return expect(
          eq(nary_op(sel::constant{3}, x), op(3, x)) and            //
          eq(nary_op(x, sel::constant{3.0}), op(x, 3.0)) and        //
          eq(nary_op(x, y, sel::constant{0}), op(op(x, y), 0)) and  //
          eq(nary_op(x, y, sel::constant{op(1, 2)}), op(op(x, y), op(1, 2)))
      );
    };

    "nary op can be used with more than 2 arguments"_ctest *
        types<T> = [](auto) {
      return expect(
          eq(nary_op(x, y, z), op(x, op(y, z))) and  //
          eq(nary_op(x, y, z), op(op(x, y), z)) and  //
          eq(nary_op(x, y, y, z), op(op(x, y), op(y, z)))
      );
    };

    "nary op is formattable"_test * types<T> = [](auto) {
      return expect(
          eq("(x)", std::format("{}", nary_op(x))) and  //
          eq(std::format("(x {} y)", glyph),
             std::format("{}", nary_op(x, y))) and  //
          eq(std::format("(x {0} y {0} z)", glyph),
             std::format("{}", nary_op(x, y, z)))
      );
    };

    return expect(true);
  };
}
