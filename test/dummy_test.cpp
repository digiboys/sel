#include "skytest/skytest.hpp"

auto main() -> int
{
  using namespace skytest::literals;
  using ::skytest::expect;

  "dummy test"_ctest = [] { return expect(true); };
}
