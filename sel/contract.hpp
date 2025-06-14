#pragma once

// TODO(replace)
// https://en.cppreference.com/w/cpp/experimental/feature_test.html#cpp_contracts

#if defined(NDEBUG)
#undef NDEBUG
#include <cassert>
#define NDEBUG
#else
#include <cassert>
#endif

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define SEL_PRECONDITION(...) assert(__VA_ARGS__)
