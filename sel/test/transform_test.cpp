#include "sel/constant.hpp"
#include "sel/multiplies.hpp"
#include "sel/operation.hpp"
#include "sel/plus.hpp"
#include "sel/transform.hpp"
#include "sel/variable.hpp"
#include "skytest/skytest.hpp"

#include <cstddef>
#include <utility>

struct replace_multiplies
{
  template <class... Args>
  constexpr auto
  operator()(this const auto& self, const sel::multiplies<Args...>& expr)
  {
    auto parts = sel::detail::transform(expr.args, [&](const auto& subexpr) {
      return sel::transform(subexpr, self);
    });

    return [&parts]<std::size_t... Is>(std::index_sequence<Is...>) {
      return sel::plus{std::get<Is>(parts)...};
    }(std::index_sequence_for<Args...>{});

    // auto [...parts] =
    //     expr.args  //
    //     | sel::transform([&](const auto& subexpr) {
    //         return subexpr | sel::transform(self)
    //       });

    // return sel::plus{parts...};
  }

  template <class T>
    requires (not sel::operation<T>)
  constexpr auto operator()(const T& value) const -> T
  {
    return value;
  }
};

auto main() -> int
{
  using namespace skytest::literals;
  using namespace skytest;

  static constexpr auto x = sel::variable{"x"};
  static constexpr auto y = sel::variable{"y"};

  static constexpr auto a = sel::constant{1};
  static constexpr auto b = sel::constant{2};

  "replaces any term"_ctest = [] {
    constexpr auto replace_with = [](const auto& c) {
      return [c](const auto&) { return c; };
    };

    return expect(
        eq(a, sel::transform(a + b, replace_with(a))) and  //
        eq(b, sel::transform(a * b, replace_with(b))) and  //
        eq(a, sel::transform(b, replace_with(a)))
    );
  };

  "replaces variables with constants"_ctest = [] {
    return expect(eq(
        sel::plus{a, b},
        sel::transform(sel::plus{x, y}, [](const sel::variable& v) {
          return v == x ? a : b;
        })
    ));
  };

  "replaces subexpressions"_ctest = [] {
    static constexpr auto f = sel::multiplies{x, x};
    static constexpr auto g = sel::multiplies{x, y};
    return expect(eq(
        sel::plus{a, a, g},
        sel::transform(sel::plus{f, f, g}, [](const decltype(f)& expr) {
          if (expr == f) {
            return a;
          } else {
            return expr;  // FIXME, different return type
          }
        })
    ));
  };

  "replaces operation types"_ctest = [] {
    return expect(eq(
        sel::plus{sel::plus{sel::plus{sel::constant{0}}}},
        sel::transform(
            sel::multiplies{sel::plus{sel::multiplies{sel::constant{0}}}},
            replace_multiplies{}
        )
    ));
  };

  "expands an expression"_ctest = [] {
    static constexpr auto f = sel::multiplies{x, x};

    return expect(eq(
        sel::plus{a, f},
        sel::transform(
            sel::plus{a, b},  //
            [](const decltype(b)& k) {
              if (k == b) {
                return f;
              } else {
                return k;
              }
            }
        )
    ));
  };
}
