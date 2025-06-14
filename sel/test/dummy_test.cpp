#include "sel/sel.hpp"
#include "skytest/skytest.hpp"

auto main() -> int
{
  using namespace skytest::literals;
  using ::skytest::eq;
  using ::skytest::expect;

  "dummy test"_test = [] { return expect(eq(42, sel::foo())); };
}
