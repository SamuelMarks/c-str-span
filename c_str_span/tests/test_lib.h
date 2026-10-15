#ifndef C_STR_SPAN_TEST_LIB_H
#define C_STR_SPAN_TEST_LIB_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* clang-format off */
#include <c_str_span_configure.h>

#include <greatest.h>

#include "c_str_span_stdbool.h"

#include <c_str_span.h>
/* clang-format on */

TEST x_lib_should_be(void) {
  uint8_t buf[16];
  az_span dest = az_span_create(buf, sizeof(buf));
  az_span src = AZ_SPAN_FROM_STR("c-str-span");
  az_span rem = az_span_empty();
  az_span s = az_span_empty();
  enum az_result_core rc;

  ASSERT_EQ(0, az_span_size(s));
  ASSERT_EQ((uint8_t *)NULL, az_span_ptr(s));

  rc = az_span_copy(dest, src, &rem);
  ASSERT_EQ(AZ_OK, rc);
  ASSERT_EQ(sizeof(buf) - 10, az_span_size(rem));
  ASSERT(az_span_is_content_equal(az_span_slice(dest, 0, 10), src));

  PASS();
}

/* Suites can group multiple tests with common setup. */
SUITE(lib_suite) { RUN_TEST(x_lib_should_be); }

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !C_STR_SPAN_TEST_LIB_H */
