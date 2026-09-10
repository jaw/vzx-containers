/*
  SPDX-FileCopyrightText: 2026 Vovoid Media Technologies AB
  Author: Jonatan Wallmander <jonatan@vovoid.com>

  SPDX-License-Identifier: MIT
*/
#pragma once

#include <cstdio>

inline int num_errors = 0;
inline int num_successful = 0;
inline int total_number_of_test_cases = 0;
inline bool test_suite_initialized = false;

inline void initialize_test_suite()
{
  #ifndef VZX_ASTREE
  printf("** VZX Test: Begin Test Suite\n");
  #endif
  test_suite_initialized = true;
}

#ifdef VZX_ASTREE

#define test_assert(a) __ASTREE_assert((a))

#else

#define test_assert(a) [&](){\
  if (!test_suite_initialized) \
{ \
  printf("VZX Test: Test suite not initialized!"); \
  exit(1000);\
} \
total_number_of_test_cases++; if (!(a)) { num_errors++; printf("** VZX Test: Assert failed at %hs:%d\n", __FILE__, __LINE__); } else {num_successful++;}}()

#endif

#define test_fail() printf("** VZX Test: test_fail() called in %hs on line %d\n", __FILE__, __LINE__)

#ifdef VZX_ASTREE
#define test_complete
#else
#define test_complete \
  if (num_errors) \
  { \
    printf("** VZX Test: %d test assertions have FAILED out of %d\n", num_errors, total_number_of_test_cases); \
    return num_errors; \
  } else { \
    printf("** VZX Test: All %d test assertions are OK\n", total_number_of_test_cases); \
    return 0; \
  }
#endif
