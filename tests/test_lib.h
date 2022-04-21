#include <c_str_span_configure.h>

#include <greatest.h>

#undef _Bool
#undef bool
#if __STDC_VERSION__ >= 199901L
#include <stdbool.h>
#else
#include "c_str_span_stdbool.h"
#include "c_str_span_internal.h"
#endif

#include <c_str_span.h>

TEST x_lib_should_be(void) {
  /* TODO: Actually test things */
  az_span s = AZ_SPAN_EMPTY;

  PASS();
}

/* Suites can group multiple tests with common setup. */
SUITE(lib_suite) { RUN_TEST(x_lib_should_be); }
