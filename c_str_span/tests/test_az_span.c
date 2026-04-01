/* Copyright (c) Microsoft Corporation. All rights reserved.
 * SPDX-License-Identifier: MIT */

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

/* clang-format off */
#include "c_str_precondition_internal.h"

#include "c_str_span.h"
#include "c_str_span_private.h"

#include "c_str_span_internal.h"
#include "c_str_span_printf.h"

#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
#ifdef _WIN64
#define FAIL_SIZE_T_VAL (-1)
#else
#define FAIL_SIZE_T_VAL UINT_MAX
#endif /* _WIN64 */
#else
#define FAIL_SIZE_T_VAL (-1)
#endif /* defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__) */

#ifdef __cplusplus
#include <climits>
#include <cmath>
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#else
#if defined(_MSC_VER) && _MSC_VER < 1600
#include "c_str_span_stdbool.h"
#include "c_str_span_stdint.h"
#else
#include <stdint.h>
#endif /* defined(_MSC_VER) && _MSC_VER < 1600 */
#include <limits.h>
#include <math.h>
#include <stdarg.h>
#include <stddef.h>
#endif /* ! __cplusplus */

#include <greatest.h>
#include <stdio.h>

#include "test_az_span.h"
/* clang-format on */

#define TEST_EXPECT_SUCCESS(exp) ASSERT(az_result_succeeded(exp))

TEST test_az_span_getters(void) {

  {
    uint8_t example[] = "example";
    az_span span = AZ_SPAN_FROM_BUFFER(example);
    ASSERT_EQ(8, az_span_size(span));
    ASSERT_EQ((uint8_t *)&example, az_span_ptr(span));
  }
  PASS();
}

TEST az_single_char_ascii_lower_test(void) {
  uint8_t i;

  for (i = 0; i <= SCHAR_MAX; ++i) {
    uint8_t buffer[1];
    buffer[0] = i;
    {
      az_span span = AZ_SPAN_FROM_BUFFER(buffer);

      /* Comparison to itself should return true for all values in the range. */
      ASSERT(az_span_is_content_equal_ignoring_case(span, span));

      /* For ASCII letters, verify that comparing upper and lower case return
       * true. */
      if (i >= 'A' && i <= 'Z') {
        uint8_t lower[1];
        lower[0] = (uint8_t)(i + 32);
        {
          az_span lowerSpan = AZ_SPAN_FROM_BUFFER(lower);
          ASSERT(az_span_is_content_equal_ignoring_case(span, lowerSpan));
          ASSERT(az_span_is_content_equal_ignoring_case(lowerSpan, span));
        }
      } else if (i >= 'a' && i <= 'z') {
        uint8_t upper[1];
        upper[0] = (uint8_t)(i - 32);
        {
          az_span upperSpan = AZ_SPAN_FROM_BUFFER(upper);
          ASSERT(az_span_is_content_equal_ignoring_case(span, upperSpan));
          ASSERT(az_span_is_content_equal_ignoring_case(upperSpan, span));
        }
      } else {
        uint8_t j;
        /* Make sure that no other comparison returns true. */
        for (j = 0; j <= SCHAR_MAX; ++j) {
          uint8_t other[1];
          other[0] = j;
          {
            az_span otherSpan = AZ_SPAN_FROM_BUFFER(other);

            if (i == j) {
              ASSERT(az_span_is_content_equal_ignoring_case(span, otherSpan));
            } else {
              ASSERT(
                  !(az_span_is_content_equal_ignoring_case(span, otherSpan)));
            }
          }
        }
      }
    }
  }
  PASS();
}

TEST az_span_to_lower_test(void) {
  az_span a = AZ_SPAN_FROM_STR("one");
  az_span b = AZ_SPAN_FROM_STR("One");
  az_span c = AZ_SPAN_FROM_STR("ones");
  az_span d = AZ_SPAN_FROM_STR("ona");
  ASSERT(az_span_is_content_equal_ignoring_case(a, b));
  ASSERT(!(az_span_is_content_equal_ignoring_case(a, c)));
  ASSERT(!(az_span_is_content_equal_ignoring_case(a, d)));

  PASS();
}

TEST test_az_span_is_content_equal(void) {
  az_span a = AZ_SPAN_FROM_STR("one");
  az_span b = AZ_SPAN_FROM_STR("One");
  az_span c = AZ_SPAN_FROM_STR("one1");
  az_span d = AZ_SPAN_FROM_STR("done"); /* d contains a */

  ASSERT(!(az_span_is_content_equal(a, b)));
  ASSERT(!(az_span_is_content_equal(b, a)));
  ASSERT(!(az_span_is_content_equal(a, c)));
  ASSERT(!(az_span_is_content_equal(c, a)));
  ASSERT(!(az_span_is_content_equal(a, d)));
  ASSERT(!(az_span_is_content_equal(d, a)));

  ASSERT(az_span_is_content_equal(a, AZ_SPAN_FROM_STR("one")));
  ASSERT(az_span_is_content_equal(a, a));

  /* Comparing subsets */
  ASSERT(az_span_is_content_equal(a, az_span_slice_to_end(d, 1)));
  ASSERT(az_span_is_content_equal(az_span_slice_to_end(d, 1), a));

  /* Comparing empty to non-empty */
  ASSERT(!(az_span_is_content_equal(a, az_span_empty())));
  ASSERT(!(az_span_is_content_equal(az_span_empty(), a)));

  /* Empty spans are equal */
  ASSERT(az_span_is_content_equal(az_span_empty(), az_span_empty()));

  ASSERT(az_span_is_content_equal(az_span_slice_to_end(a, 3), az_span_empty()));
  ASSERT(az_span_is_content_equal(az_span_slice_to_end(a, 3),
                                  az_span_slice_to_end(b, 3)));

  ASSERT(az_span_is_content_equal(AZ_SPAN_FROM_STR(""), AZ_SPAN_FROM_STR("")));
  ASSERT(az_span_is_content_equal(AZ_SPAN_FROM_STR(""), az_span_empty()));
  ASSERT(az_span_is_content_equal(AZ_SPAN_FROM_STR(""),
                                  az_span_slice_to_end(a, 3)));

  PASS();
}

#define az_SPAN_ATOX_RETURN_ERRORS_HELPER_EXCLUDE_DOUBLE(source)               \
  do {                                                                         \
    uint32_t ui32 = 0;                                                         \
    int32_t i32 = 0;                                                           \
    uint64_t ui64 = 0;                                                         \
    int64_t i64 = 0;                                                           \
    ASSERT(az_span_atou32(source, &ui32) == AZ_ERROR_UNEXPECTED_CHAR);         \
    ASSERT(az_span_atoi32(source, &i32) == AZ_ERROR_UNEXPECTED_CHAR);          \
    ASSERT(az_span_atou64(source, &ui64) == AZ_ERROR_UNEXPECTED_CHAR);         \
    ASSERT(az_span_atoi64(source, &i64) == AZ_ERROR_UNEXPECTED_CHAR);          \
  } while (0)

#define AZ_SPAN_ATOX_RETURN_ERRORS_HELPER(source)                              \
  do {                                                                         \
    uint32_t ui32 = 0;                                                         \
    int32_t i32 = 0;                                                           \
    uint64_t ui64 = 0;                                                         \
    int64_t i64 = 0;                                                           \
    double decimal = 0;                                                        \
    ASSERT(az_span_atou32(source, &ui32) == AZ_ERROR_UNEXPECTED_CHAR);         \
    ASSERT(az_span_atoi32(source, &i32) == AZ_ERROR_UNEXPECTED_CHAR);          \
    ASSERT(az_span_atou64(source, &ui64) == AZ_ERROR_UNEXPECTED_CHAR);         \
    ASSERT(az_span_atoi64(source, &i64) == AZ_ERROR_UNEXPECTED_CHAR);          \
    ASSERT(az_span_atod(source, &decimal) == AZ_ERROR_UNEXPECTED_CHAR);        \
  } while (0)

TEST az_span_atox_return_errors(void) {

  AZ_SPAN_ATOX_RETURN_ERRORS_HELPER(AZ_SPAN_FROM_STR("test"));
  AZ_SPAN_ATOX_RETURN_ERRORS_HELPER(AZ_SPAN_FROM_STR(" "));
  AZ_SPAN_ATOX_RETURN_ERRORS_HELPER(AZ_SPAN_FROM_STR(" 1"));
  AZ_SPAN_ATOX_RETURN_ERRORS_HELPER(AZ_SPAN_FROM_STR("-"));
  AZ_SPAN_ATOX_RETURN_ERRORS_HELPER(AZ_SPAN_FROM_STR("+"));
  AZ_SPAN_ATOX_RETURN_ERRORS_HELPER(AZ_SPAN_FROM_STR("--1"));
  AZ_SPAN_ATOX_RETURN_ERRORS_HELPER(AZ_SPAN_FROM_STR("++1"));
  AZ_SPAN_ATOX_RETURN_ERRORS_HELPER(AZ_SPAN_FROM_STR("-+"));
  AZ_SPAN_ATOX_RETURN_ERRORS_HELPER(AZ_SPAN_FROM_STR("+-"));
  AZ_SPAN_ATOX_RETURN_ERRORS_HELPER(AZ_SPAN_FROM_STR("-0+"));
  AZ_SPAN_ATOX_RETURN_ERRORS_HELPER(AZ_SPAN_FROM_STR("0-"));
  AZ_SPAN_ATOX_RETURN_ERRORS_HELPER(AZ_SPAN_FROM_STR("+0-"));
  AZ_SPAN_ATOX_RETURN_ERRORS_HELPER(AZ_SPAN_FROM_STR("1-"));
  AZ_SPAN_ATOX_RETURN_ERRORS_HELPER(AZ_SPAN_FROM_STR("123a"));
  AZ_SPAN_ATOX_RETURN_ERRORS_HELPER(AZ_SPAN_FROM_STR("123,"));
  AZ_SPAN_ATOX_RETURN_ERRORS_HELPER(AZ_SPAN_FROM_STR("123 "));
  AZ_SPAN_ATOX_RETURN_ERRORS_HELPER(AZ_SPAN_FROM_STR("--123"));
  AZ_SPAN_ATOX_RETURN_ERRORS_HELPER(AZ_SPAN_FROM_STR("-+123"));
  AZ_SPAN_ATOX_RETURN_ERRORS_HELPER(AZ_SPAN_FROM_STR("+-123"));
  AZ_SPAN_ATOX_RETURN_ERRORS_HELPER(AZ_SPAN_FROM_STR("  -1-"));
  AZ_SPAN_ATOX_RETURN_ERRORS_HELPER(AZ_SPAN_FROM_STR("- INFINITY"));
  AZ_SPAN_ATOX_RETURN_ERRORS_HELPER(AZ_SPAN_FROM_STR("- 0"));
  AZ_SPAN_ATOX_RETURN_ERRORS_HELPER(AZ_SPAN_FROM_STR("+ 1"));
  AZ_SPAN_ATOX_RETURN_ERRORS_HELPER(AZ_SPAN_FROM_STR("1.-e3"));
  AZ_SPAN_ATOX_RETURN_ERRORS_HELPER(AZ_SPAN_FROM_STR("1.-e/3"));
  az_SPAN_ATOX_RETURN_ERRORS_HELPER_EXCLUDE_DOUBLE(AZ_SPAN_FROM_STR("1.23"));
  az_SPAN_ATOX_RETURN_ERRORS_HELPER_EXCLUDE_DOUBLE(AZ_SPAN_FROM_STR("-1.23"));
  az_SPAN_ATOX_RETURN_ERRORS_HELPER_EXCLUDE_DOUBLE(AZ_SPAN_FROM_STR("11e2"));
  az_SPAN_ATOX_RETURN_ERRORS_HELPER_EXCLUDE_DOUBLE(AZ_SPAN_FROM_STR("-1.1e+2"));
  az_SPAN_ATOX_RETURN_ERRORS_HELPER_EXCLUDE_DOUBLE(AZ_SPAN_FROM_STR("1.23e3"));
  az_SPAN_ATOX_RETURN_ERRORS_HELPER_EXCLUDE_DOUBLE(
      AZ_SPAN_FROM_STR("99999999999999999999"));
  az_SPAN_ATOX_RETURN_ERRORS_HELPER_EXCLUDE_DOUBLE(
      AZ_SPAN_FROM_STR("999999999999999999999"));
  az_SPAN_ATOX_RETURN_ERRORS_HELPER_EXCLUDE_DOUBLE(
      AZ_SPAN_FROM_STR("18446744073709551616"));
  az_SPAN_ATOX_RETURN_ERRORS_HELPER_EXCLUDE_DOUBLE(
      AZ_SPAN_FROM_STR("-18446744073709551616"));
  PASS();
}

TEST az_span_atou32_test(void) {
  uint32_t value = 0;

  ASSERT_EQ(AZ_OK, az_span_atou32(AZ_SPAN_FROM_STR("0"), &value));
  ASSERT_EQ(0, value);
  ASSERT_EQ(AZ_OK, az_span_atou32(AZ_SPAN_FROM_STR("1024"), &value));
  ASSERT_EQ(1024, value);
  ASSERT_EQ(AZ_OK, az_span_atou32(AZ_SPAN_FROM_STR("+1024"), &value));
  ASSERT_EQ(1024, value);
  ASSERT_EQ(AZ_OK, az_span_atou32(AZ_SPAN_FROM_STR("001024"), &value));
  ASSERT_EQ(1024, value);
  ASSERT_EQ(AZ_OK, az_span_atou32(AZ_SPAN_FROM_STR("2147483647"), &value));
  ASSERT_EQ(2147483647, value);
  ASSERT_EQ(AZ_OK, az_span_atou32(AZ_SPAN_FROM_STR("4294967295"), &value));
  ASSERT_EQ(4294967295U, value);
  ASSERT_EQ(AZ_OK, az_span_atou32(AZ_SPAN_FROM_STR("00004294967295"), &value));
  ASSERT_EQ(4294967295U, value);

  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atou32(AZ_SPAN_FROM_STR("-123"), &value));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atou32(AZ_SPAN_FROM_STR("42949672950"), &value));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atou32(AZ_SPAN_FROM_STR("-2147483648"), &value));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atou32(AZ_SPAN_FROM_STR("-4294967295"), &value));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atou32(AZ_SPAN_FROM_STR("-4294967296"), &value));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atou32(AZ_SPAN_FROM_STR("4294967296"), &value));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atou32(AZ_SPAN_FROM_STR("42949672950"), &value));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atou32(AZ_SPAN_FROM_STR("9223372036854775807"), &value));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atou32(AZ_SPAN_FROM_STR("9223372036854775808"), &value));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atou32(AZ_SPAN_FROM_STR("18446744073709551615"), &value));
  PASS();
}

TEST az_span_atoi32_test(void) {
  int32_t value = 0;

  ASSERT_EQ(AZ_OK, az_span_atoi32(AZ_SPAN_FROM_STR("0"), &value));
  ASSERT_EQ(0, value);
  ASSERT_EQ(AZ_OK, az_span_atoi32(AZ_SPAN_FROM_STR("1024"), &value));
  ASSERT_EQ(1024, value);
  ASSERT_EQ(AZ_OK, az_span_atoi32(AZ_SPAN_FROM_STR("+1024"), &value));
  ASSERT_EQ(1024, value);
  ASSERT_EQ(AZ_OK, az_span_atoi32(AZ_SPAN_FROM_STR("-1024"), &value));
  ASSERT_EQ(-1024, value);
  ASSERT_EQ(AZ_OK, az_span_atoi32(AZ_SPAN_FROM_STR("001024"), &value));
  ASSERT_EQ(1024, value);
  ASSERT_EQ(AZ_OK, az_span_atoi32(AZ_SPAN_FROM_STR("2147483647"), &value));
  ASSERT_EQ(2147483647, value);
  ASSERT_EQ(AZ_OK, az_span_atoi32(AZ_SPAN_FROM_STR("-2147483648"), &value));
  ASSERT_EQ(-2147483647 - 1, value);
  ASSERT_EQ(AZ_OK, az_span_atoi32(AZ_SPAN_FROM_STR("-00002147483648"), &value));
  ASSERT_EQ(-2147483647 - 1, value);

  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atoi32(AZ_SPAN_FROM_STR("21474836470"), &value));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atoi32(AZ_SPAN_FROM_STR("2147483648"), &value));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atoi32(AZ_SPAN_FROM_STR("-2147483649"), &value));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atoi32(AZ_SPAN_FROM_STR("-4294967295"), &value));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atoi32(AZ_SPAN_FROM_STR("-4294967296"), &value));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atoi32(AZ_SPAN_FROM_STR("9223372036854775807"), &value));
  PASS();
}

TEST az_span_atou64_test(void) {
  uint64_t value = 0;

  ASSERT_EQ(AZ_OK, az_span_atou64(AZ_SPAN_FROM_STR("0"), &value));
  ASSERT_EQ(0, value);
  ASSERT_EQ(AZ_OK, az_span_atou64(AZ_SPAN_FROM_STR("1024"), &value));
  ASSERT_EQ(1024, value);
  ASSERT_EQ(AZ_OK, az_span_atou64(AZ_SPAN_FROM_STR("+1024"), &value));
  ASSERT_EQ(1024, value);
  ASSERT_EQ(AZ_OK, az_span_atou64(AZ_SPAN_FROM_STR("001024"), &value));
  ASSERT_EQ(1024, value);
  ASSERT_EQ(AZ_OK, az_span_atou64(AZ_SPAN_FROM_STR("2147483647"), &value));
  ASSERT_EQ(2147483647, value);
  ASSERT_EQ(AZ_OK, az_span_atou64(AZ_SPAN_FROM_STR("4294967295"), &value));
  ASSERT_EQ(4294967295U, value);
  ASSERT_EQ(AZ_OK,
            az_span_atou64(AZ_SPAN_FROM_STR("9223372036854775807"), &value));
  ASSERT_EQ(9223372036854775807UL, value);
  ASSERT_EQ(AZ_OK,
            az_span_atou64(AZ_SPAN_FROM_STR("18446744073709551615"), &value));
  ASSERT_EQ(18446744073709551615UL, value);
  ASSERT_EQ(AZ_OK, az_span_atou64(AZ_SPAN_FROM_STR("000018446744073709551615"),
                                  &value));
  ASSERT_EQ(18446744073709551615UL, value);

  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atou64(AZ_SPAN_FROM_STR("-123"), &value));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atou64(AZ_SPAN_FROM_STR("184467440737095516150"), &value));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atou64(AZ_SPAN_FROM_STR("18446744073709551616"), &value));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atou64(AZ_SPAN_FROM_STR("-9223372036854775809"), &value));
  PASS();
}

TEST az_span_atoi64_test(void) {
  int64_t value = 0;

  ASSERT_EQ(AZ_OK, az_span_atoi64(AZ_SPAN_FROM_STR("0"), &value));
  ASSERT_EQ(0, value);
  ASSERT_EQ(AZ_OK, az_span_atoi64(AZ_SPAN_FROM_STR("1024"), &value));
  ASSERT_EQ(1024, value);
  ASSERT_EQ(AZ_OK, az_span_atoi64(AZ_SPAN_FROM_STR("+1024"), &value));
  ASSERT_EQ(1024, value);
  ASSERT_EQ(AZ_OK, az_span_atoi64(AZ_SPAN_FROM_STR("-1024"), &value));
  ASSERT_EQ(-1024, value);
  ASSERT_EQ(AZ_OK, az_span_atoi64(AZ_SPAN_FROM_STR("001024"), &value));
  ASSERT_EQ(1024, value);
  ASSERT_EQ(AZ_OK, az_span_atoi64(AZ_SPAN_FROM_STR("2147483647"), &value));
  ASSERT_EQ(2147483647, value);
  ASSERT_EQ(AZ_OK, az_span_atoi64(AZ_SPAN_FROM_STR("-2147483648"), &value));
  ASSERT_EQ(-2147483647 - 1, value);
  ASSERT_EQ(AZ_OK, az_span_atoi64(AZ_SPAN_FROM_STR("4294967295"), &value));
  ASSERT_EQ(4294967295U, value);
  ASSERT_EQ(AZ_OK, az_span_atoi64(AZ_SPAN_FROM_STR("-4294967296"), &value));
  ASSERT_EQ(-4294967296, value);
  ASSERT_EQ(AZ_OK,
            az_span_atoi64(AZ_SPAN_FROM_STR("9223372036854775807"), &value));
  ASSERT_EQ(9223372036854775807L, value);
  ASSERT_EQ(AZ_OK,
            az_span_atoi64(AZ_SPAN_FROM_STR("-9223372036854775808"), &value));
  ASSERT_EQ(-9223372036854775807L - 1, value);
  ASSERT_EQ(AZ_OK, az_span_atoi64(AZ_SPAN_FROM_STR("-00009223372036854775808"),
                                  &value));
  ASSERT_EQ(-9223372036854775807L - 1, value);

  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atoi64(AZ_SPAN_FROM_STR("92233720368547758070"), &value));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atoi64(AZ_SPAN_FROM_STR("12233720368547758070"), &value));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atoi64(AZ_SPAN_FROM_STR("9223372036854775808"), &value));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atoi64(AZ_SPAN_FROM_STR("18446744073709551615"), &value));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atoi64(AZ_SPAN_FROM_STR("18446744073709551616"), &value));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atoi64(AZ_SPAN_FROM_STR("-9223372036854775809"), &value));
  PASS();
}

#define TEST_AZ_ISFINITE_HELPER(source, expected)                              \
  do {                                                                         \
    double decimal = 0.0;                                                      \
    memcpy(&decimal, &(source), sizeof(decimal));                              \
    ASSERT_EQ(expected, _az_isfinite(decimal));                                \
  } while (0)

TEST test_az_isfinite(void) {
  uint64_t source = 0;

  TEST_AZ_ISFINITE_HELPER(source, true);
  source = 1;
  TEST_AZ_ISFINITE_HELPER(source, true);
  source = 0x6FFFFFFFFFFFFFFF;
  TEST_AZ_ISFINITE_HELPER(source, true);
  source = 0x7FEFFFFFFFFFFFFF;
  TEST_AZ_ISFINITE_HELPER(source, true);

  source = 0x7FF0000000000000; /* +inf */
  TEST_AZ_ISFINITE_HELPER(source, false);
  source = 0x7FF0000000000001; /* nan */
  TEST_AZ_ISFINITE_HELPER(source, false);
  source = 0x7FF7FFFFFFFFFFFF; /* nan */
  TEST_AZ_ISFINITE_HELPER(source, false);
  source = 0x7FF8000000000000; /* nan */
  TEST_AZ_ISFINITE_HELPER(source, false);
  source = 0x7FFFFFFFFFFFFFFF; /* nan */
  TEST_AZ_ISFINITE_HELPER(source, false);

  source = 0x8000000000000000;
  TEST_AZ_ISFINITE_HELPER(source, true);
  source = 0xFFEFFFFFFFFFFFFF;
  TEST_AZ_ISFINITE_HELPER(source, true);

  source = 0xFFF0000000000000; /* -inf */
  TEST_AZ_ISFINITE_HELPER(source, false);
  source = 0xFFF7FFFFFFFFFFFF; /* nan */
  TEST_AZ_ISFINITE_HELPER(source, false);
  source = 0xFFF8000000000000; /* nan */
  TEST_AZ_ISFINITE_HELPER(source, false);
  source = 0xFFFFFFFFFFFFFFFF; /* nan */
  TEST_AZ_ISFINITE_HELPER(source, false);

#if !defined(_MSC_VER) || _MSC_VER >= 1600
  source = 0xFFFFFFFFFFFFFFFF + 1;
  TEST_AZ_ISFINITE_HELPER(source, true);
#endif /* !defined(_MSC_VER) || _MSC_VER >= 1600 */
  PASS();
}

/* Disable warning for float comparisons, for this particular test */
/* error : comparing floating point with == or != is unsafe[-Werror = float -
 * equal] */
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
#endif /* __GNUC__ */

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"
#endif /* __clang__ */

TEST az_span_atod_test(void) {
  double value = 0;

  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("0"), &value));
  ASSERT(value == 0);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("1024"), &value));
  ASSERT(value == 1024);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("+1024"), &value));
  ASSERT(value == 1024);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("001024"), &value));
  ASSERT(value == 1024);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("-1024"), &value));
  ASSERT(value == -1024);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("2147483647"), &value));
  ASSERT(value == 2147483647);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("-2147483648"), &value));
  ASSERT(value == -2147483647 - 1);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("4294967295"), &value));
  ASSERT(value == 4294967295U);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("-4294967296"), &value));
  ASSERT(value == -4294967296);
  ASSERT_EQ(AZ_OK,
            az_span_atod(AZ_SPAN_FROM_STR("9223372036854775807"), &value));
  ASSERT(value == (double)LLONG_MAX);
  ASSERT_EQ(AZ_OK,
            az_span_atod(AZ_SPAN_FROM_STR("-9223372036854775808"), &value));
  ASSERT(value == (double)LLONG_MIN);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("1.23e3"), &value));
  ASSERT(value == 1.23e3);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("+001.23e3"), &value));
  ASSERT(value == 1.23e3);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("1.23"), &value));
  ASSERT(value == 1.23);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("-123.456e-78"), &value));
  ASSERT(value == -123.456e-78);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("123.456e+78"), &value));
  ASSERT(value == 123.456e+78);

  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("0"), &value));
  ASSERT(value == 0);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("-0"), &value));
  ASSERT(value == 0);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("0.0"), &value));
  ASSERT(value == 0);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("1"), &value));
  ASSERT(value == 1);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("1."), &value));
  ASSERT(value == 1);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("1.e3"), &value));
  ASSERT(value == 1000);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("-1"), &value));
  ASSERT(value == -1.0);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("1.0"), &value));
  ASSERT(value == 1);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("-1.0"), &value));
  ASSERT(value == -1.0);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("12345"), &value));
  ASSERT(value == 12345);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("-12345"), &value));
  ASSERT(value == -12345);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("123.123"), &value));
  ASSERT(value == 123.123);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("123.1230"), &value));
  ASSERT(value == 123.1230);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("123.0100"), &value));
  ASSERT(value == 123.0100);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("123.01"), &value));
  ASSERT(value == 123.01);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("123.001"), &value));
  ASSERT(value == 123.001);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("0.000000000000001"), &value));
  ASSERT(value == 0.000000000000001);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("1.0000000001"), &value));
  ASSERT(value == 1.0000000001);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("-1.0000000001"), &value));
  ASSERT(value == -1.0000000001);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("100.001"), &value));
  ASSERT(value == 100.001);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("100.00100"), &value));
  ASSERT(value == 100.00100);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("00100.001"), &value));
  ASSERT(value == 00100.001);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("00100.00100"), &value));
  ASSERT(value == 00100.00100);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("0.001"), &value));
  ASSERT(value == 0.001);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("0.0012"), &value));
  ASSERT(value == 0.0012);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("1.2e4"), &value));
  ASSERT(value == 1.2e4);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("1.2e-4"), &value));
  ASSERT(value == 1.2e-4);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("1.2e+4"), &value));
  ASSERT(value == 1.2e+4);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("-1.2e4"), &value));
  ASSERT(value == -1.2e4);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("-1.2e-4"), &value));
  ASSERT(value == -1.2e-4);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("0.0001"), &value));
  ASSERT(value == 0.0001);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("0.00102"), &value));
  ASSERT(value == 0.00102);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("0.34567"), &value));
  ASSERT(value == .34567);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("+0.34567"), &value));
  ASSERT(value == .34567);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("-0.34567"), &value));
  ASSERT(value == -.34567);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("9876.54321"), &value));
  ASSERT(value == 9876.54321);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("-9876.54321"), &value));
  ASSERT(value == -9876.54321);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("987654.321"), &value));
  ASSERT(value == 987654.321);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("-987654.321"), &value));
  ASSERT(value == -987654.321);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("987654.0000321"), &value));
  ASSERT(value == 987654.0000321);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("2147483647"), &value));
  ASSERT(value == 2147483647);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("2147483648"), &value));
  ASSERT(value == 2147483648U);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("-2147483648"), &value));
  ASSERT(value == -2147483647 - 1);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("4503599627370496"), &value));
  ASSERT(value == 4503599627370496);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("9007199254740991"), &value));
  ASSERT(value == 9007199254740991);
  ASSERT_EQ(AZ_OK,
            az_span_atod(AZ_SPAN_FROM_STR("4503599627370496.2"), &value));
  ASSERT(value == 4503599627370496.2);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("1e15"), &value));
  ASSERT(value == 1e15);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("-1e15"), &value));
  ASSERT(value == -1e15);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("1.8e10"), &value));
  ASSERT(value == 1.8e10);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("-1.8e10"), &value));
  ASSERT(value == -1.8e10);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("1e-15"), &value));
  ASSERT(value == 1e-15);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("1e-10"), &value));
  ASSERT(value == 1e-10);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("1e-5"), &value));
  ASSERT(value == 1e-5);
  ASSERT_EQ(AZ_OK,
            az_span_atod(AZ_SPAN_FROM_STR("0.1234567890123456"), &value));
  ASSERT(value == 0.1234567890123456);
  ASSERT_EQ(AZ_OK,
            az_span_atod(AZ_SPAN_FROM_STR("123456789012345.123456789012340000"),
                         &value));
  ASSERT(value == 123456789012345.123456789012340000);
  ASSERT_EQ(AZ_OK,
            az_span_atod(AZ_SPAN_FROM_STR("1000000000000.123456789012340000"),
                         &value));
  ASSERT(value == 1000000000000.123456789012340000);
  ASSERT_EQ(AZ_OK, az_span_atod(
                       AZ_SPAN_FROM_STR("123456789012345.1234567890123400001"),
                       &value));
  ASSERT(value == 123456789012345.1234567890123400001);
  ASSERT_EQ(AZ_OK,
            az_span_atod(AZ_SPAN_FROM_STR("1000000000000.1234567890123400001"),
                         &value));
  ASSERT(value == 1000000000000.1234567890123400001);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("12345.123e-15"), &value));
  ASSERT(value == 12345.123e-15);
  ASSERT_EQ(AZ_OK,
            az_span_atod(AZ_SPAN_FROM_STR("12345.12300000010e5"), &value));
  ASSERT(value == 12345.12300000010e5);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("1e-300"), &value));
  ASSERT(value == 1e-300);

  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("9007199254740992"), &value));
  ASSERT(value == 9007199254740992);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("9007199254740993"), &value));
  ASSERT(value == 9007199254740993);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("45035996273704961"), &value));
  ASSERT(value == 45035996273704961);
  ASSERT_EQ(AZ_OK,
            az_span_atod(AZ_SPAN_FROM_STR("9223372036854775806"), &value));
  ASSERT(value == 9223372036854775806);
  ASSERT_EQ(AZ_OK,
            az_span_atod(AZ_SPAN_FROM_STR("-9223372036854775806"), &value));
  ASSERT(value == -9223372036854775806);
  ASSERT_EQ(AZ_OK,
            az_span_atod(AZ_SPAN_FROM_STR("1844674407370955100"), &value));
  ASSERT(value == 1844674407370955100);
  ASSERT_EQ(AZ_OK,
            az_span_atod(AZ_SPAN_FROM_STR("1.844674407370955e+19"), &value));
  ASSERT(value == 1.844674407370955e+19);
  ASSERT_EQ(AZ_OK,
            az_span_atod(AZ_SPAN_FROM_STR("1.8446744073709551e+19"), &value));
  ASSERT(value == 1.8446744073709551e+19);
  ASSERT_EQ(AZ_OK,
            az_span_atod(AZ_SPAN_FROM_STR("1.8446744073709552e+19"), &value));
  ASSERT(value == 1.8446744073709552e+19);
  ASSERT_EQ(AZ_OK,
            az_span_atod(AZ_SPAN_FROM_STR("18446744073709551615"), &value));
  ASSERT(value == 18446744073709551615UL);
  ASSERT_EQ(AZ_OK,
            az_span_atod(
                AZ_SPAN_FROM_STR("18446744073709551615.18446744073709551615"),
                &value));
  ASSERT(value == 18446744073709551615.18446744073709551615);
  ASSERT_EQ(AZ_OK,
            az_span_atod(AZ_SPAN_FROM_STR(
                             "+000018446744073709551615.18446744073709551615"),
                         &value));
  ASSERT(value == 18446744073709551615.18446744073709551615);
  ASSERT_EQ(AZ_OK,
            az_span_atod(AZ_SPAN_FROM_STR(
                             "-000018446744073709551615.18446744073709551615"),
                         &value));
  ASSERT(value == -18446744073709551615.18446744073709551615);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("1e16"), &value));
  ASSERT(value == 1e16);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("12345.123e15"), &value));
  ASSERT(value == 12345.123e15);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("-12345.123e15"), &value));
  ASSERT(value == -12345.123e15);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("1e300"), &value));
  ASSERT(value == 1e300);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("-1e300"), &value));
  ASSERT(value == -1e300);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("1.7e308"), &value));
  ASSERT(value == 1.7e308);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("-1.7e308"), &value));
  ASSERT(value == -1.7e308);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("2.22507e-308"), &value));
  ASSERT(value == 2.22507e-308);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("-2.22507e-308"), &value));
  ASSERT(value == -2.22507e-308);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("4.94e-325"), &value));
  ASSERT(value == 0);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("1e-400"), &value));
  ASSERT(value == 0);
  ASSERT_EQ(AZ_OK, az_span_atod(AZ_SPAN_FROM_STR("-1e-400"), &value));
  ASSERT(value == 0);
  PASS();
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif /* __GNUC__ */

#ifdef __clang__
#pragma clang diagnostic pop
#endif /* __clang__ */

TEST az_span_atod_non_finite_not_allowed(void) {
  double value = 0;

  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atod(AZ_SPAN_FROM_STR("nan"), &value));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atod(AZ_SPAN_FROM_STR("-nan"), &value));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atod(AZ_SPAN_FROM_STR("+nan"), &value));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atod(AZ_SPAN_FROM_STR("inf"), &value));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atod(AZ_SPAN_FROM_STR("+inf"), &value));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atod(AZ_SPAN_FROM_STR("-inf"), &value));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atod(AZ_SPAN_FROM_STR("NAN"), &value));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atod(AZ_SPAN_FROM_STR("INF"), &value));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atod(AZ_SPAN_FROM_STR("+INF"), &value));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atod(AZ_SPAN_FROM_STR("-INF"), &value));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atod(AZ_SPAN_FROM_STR("INFINITY"), &value));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atod(AZ_SPAN_FROM_STR("+INFINITY"), &value));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atod(AZ_SPAN_FROM_STR("-INFINITY"), &value));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atod(AZ_SPAN_FROM_STR("1.8e308"), &value));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atod(AZ_SPAN_FROM_STR("-1.8e308"), &value));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atod(AZ_SPAN_FROM_STR("1.8e309"), &value));
  PASS();
}

TEST az_span_ato_number_whitespace_or_invalid_not_allowed(void) {
  int32_t value_i32 = 0;
  uint32_t value_u32 = 0;
  int64_t value_i64 = 0;
  uint64_t value_u64 = 0;
  double value_d = 0;

  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atoi32(AZ_SPAN_FROM_STR("   123"), &value_i32));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atou32(AZ_SPAN_FROM_STR("   123"), &value_u32));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atoi64(AZ_SPAN_FROM_STR("   123"), &value_i64));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atou64(AZ_SPAN_FROM_STR("   123"), &value_u64));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atod(AZ_SPAN_FROM_STR("   123"), &value_d));

  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atoi32(AZ_SPAN_FROM_STR("\n123"), &value_i32));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atou32(AZ_SPAN_FROM_STR("\n123"), &value_u32));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atoi64(AZ_SPAN_FROM_STR("\n123"), &value_i64));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atou64(AZ_SPAN_FROM_STR("\n123"), &value_u64));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atod(AZ_SPAN_FROM_STR("\n123"), &value_d));

  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atoi32(AZ_SPAN_FROM_STR("a123"), &value_i32));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atou32(AZ_SPAN_FROM_STR("a123"), &value_u32));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atoi64(AZ_SPAN_FROM_STR("a123"), &value_i64));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atou64(AZ_SPAN_FROM_STR("a123"), &value_u64));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atod(AZ_SPAN_FROM_STR("a123"), &value_d));

  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atoi32(AZ_SPAN_FROM_STR("- 123"), &value_i32));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atou32(AZ_SPAN_FROM_STR("- 123"), &value_u32));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atoi64(AZ_SPAN_FROM_STR("- 123"), &value_i64));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atou64(AZ_SPAN_FROM_STR("- 123"), &value_u64));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atod(AZ_SPAN_FROM_STR("- 123"), &value_d));

  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atoi32(AZ_SPAN_FROM_STR("-\n123"), &value_i32));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atou32(AZ_SPAN_FROM_STR("-\n123"), &value_u32));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atoi64(AZ_SPAN_FROM_STR("-\n123"), &value_i64));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atou64(AZ_SPAN_FROM_STR("-\n123"), &value_u64));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atod(AZ_SPAN_FROM_STR("-\n123"), &value_d));
  PASS();
}

TEST az_span_ato_number_no_out_of_bounds_reads(void) {
  int32_t value_i32 = 0;
  double value_d = 0;

  az_span source = AZ_SPAN_FROM_STR("   123456");

  /* Makes sure we only read and parse up to the character '3', since that is
   * the last character */
  /* within the span slice */
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atoi32(az_span_slice(source, 0, 6), &value_i32));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atod(az_span_slice(source, 0, 6), &value_d));

  ASSERT_EQ(AZ_OK, az_span_atoi32(az_span_slice(source, 3, 6), &value_i32));
  ASSERT_EQ(123, value_i32);
  ASSERT_EQ(AZ_OK, az_span_atod(az_span_slice(source, 3, 6), &value_d));
  ASSERT_EQ(123, value_d);

  source = AZ_SPAN_FROM_STR("   123A");
  /* Makes sure we only read and parse up to the character '3', since that is
   * the last character */
  /* within the span slice */
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atoi32(az_span_slice(source, 0, 6), &value_i32));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atod(az_span_slice(source, 0, 6), &value_d));

  ASSERT_EQ(AZ_OK, az_span_atoi32(az_span_slice(source, 3, 6), &value_i32));
  ASSERT_EQ(123, value_i32);
  ASSERT_EQ(AZ_OK, az_span_atod(az_span_slice(source, 3, 6), &value_d));
  ASSERT_EQ(123, value_d);

  source = AZ_SPAN_FROM_STR("   123.");
  /* Makes sure we only read and parse up to the character '3', since that is
   * the last character */
  /* within the span slice */
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atoi32(az_span_slice(source, 0, 6), &value_i32));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atod(az_span_slice(source, 0, 6), &value_d));

  ASSERT_EQ(AZ_OK, az_span_atoi32(az_span_slice(source, 3, 6), &value_i32));
  ASSERT_EQ(123, value_i32);
  ASSERT_EQ(AZ_OK, az_span_atod(az_span_slice(source, 3, 6), &value_d));
  ASSERT_EQ(123, value_d);

  source = AZ_SPAN_FROM_STR("   123-");
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atoi32(az_span_slice(source, 0, 6), &value_i32));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atod(az_span_slice(source, 0, 6), &value_d));

  ASSERT_EQ(AZ_OK, az_span_atoi32(az_span_slice(source, 3, 6), &value_i32));
  ASSERT_EQ(123, value_i32);
  ASSERT_EQ(AZ_OK, az_span_atod(az_span_slice(source, 3, 6), &value_d));
  ASSERT_EQ(123, value_d);

  source = AZ_SPAN_FROM_STR("   12-4");
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atoi32(az_span_slice(source, 0, 6), &value_i32));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atod(az_span_slice(source, 0, 6), &value_d));

  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atoi32(az_span_slice(source, 3, 6), &value_i32));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atod(az_span_slice(source, 3, 6), &value_d));

  source = AZ_SPAN_FROM_STR("n1");
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR, az_span_atoi32(source, &value_i32));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR, az_span_atod(source, &value_d));
  PASS();
}

TEST az_span_to_str_test(void) {
  az_span sample = AZ_SPAN_FROM_STR("hello World!");
  char str[20];

  az_span_to_str(str, 20, sample);
  ASSERT_STR_EQ("hello World!", str);
  PASS();
}

TEST az_span_find_beginning_success(void) {
  az_span span = AZ_SPAN_FROM_STR("abcdefgabcdefg");
  az_span target = AZ_SPAN_FROM_STR("abc");

  ASSERT_EQ(0, az_span_find(span, target));
  PASS();
}

TEST az_span_find_middle_success(void) {
  az_span span = AZ_SPAN_FROM_STR("abcdefgabcdefg");
  az_span target = AZ_SPAN_FROM_STR("gab");

  ASSERT_EQ(6, az_span_find(span, target));
  PASS();
}

TEST az_span_find_end_success(void) {
  az_span span = AZ_SPAN_FROM_STR("abcdefgabcdefgh");
  az_span target = AZ_SPAN_FROM_STR("efgh");

  ASSERT_EQ(11, az_span_find(span, target));
  PASS();
}

TEST az_span_find_source_target_identical_success(void) {
  az_span span = AZ_SPAN_FROM_STR("abcdefgabcdefg");
  az_span target = AZ_SPAN_FROM_STR("abcdefgabcdefg");

  ASSERT_EQ(0, az_span_find(span, target));
  PASS();
}

TEST az_span_find_not_found_fail(void) {
  az_span span = AZ_SPAN_FROM_STR("abcdefgabcdefg");
  az_span target = AZ_SPAN_FROM_STR("abd");

  ASSERT_EQ(FAIL_SIZE_T_VAL, az_span_find(span, target));
  PASS();
}

TEST az_span_find_error_cases_fail(void) {
  az_span span = AZ_SPAN_FROM_STR("abcdefgabcdefg");
  az_span target = AZ_SPAN_FROM_STR("abd");

  ASSERT_EQ(0, az_span_find(az_span_empty(), az_span_empty()));
  ASSERT_EQ(0, az_span_find(span, az_span_empty()));
  ASSERT_EQ(FAIL_SIZE_T_VAL, az_span_find(az_span_empty(), target));
  PASS();
}

TEST az_span_find_target_longer_than_source_fails(void) {
  az_span span = AZ_SPAN_FROM_STR("aa");
  az_span target = AZ_SPAN_FROM_STR("aaa");

  ASSERT_EQ(FAIL_SIZE_T_VAL, az_span_find(span, target));
  PASS();
}

TEST az_span_find_target_overlap_continuation_of_source_fails(void) {
  az_span span = AZ_SPAN_FROM_STR("abcd");
  az_span target = AZ_SPAN_FROM_STR("cde");

  ASSERT_EQ(FAIL_SIZE_T_VAL, az_span_find(span, target));
  PASS();
}

TEST az_span_find_target_more_chars_than_prefix_of_source_fails(void) {
  az_span span = AZ_SPAN_FROM_STR("abcd");
  az_span target = AZ_SPAN_FROM_STR("zab");

  ASSERT_EQ(FAIL_SIZE_T_VAL, az_span_find(span, target));
  PASS();
}

TEST az_span_find_overlapping_target_success(void) {
  az_span span = AZ_SPAN_FROM_STR("abcdefghij");
  az_span target = az_span_slice(span, 6, 9);

  ASSERT_EQ(6, az_span_find(span, target));
  PASS();
}

TEST az_span_find_embedded_NULLs_success(void) {
  az_span span = AZ_SPAN_FROM_STR("abcd\0\0fghij");
  az_span target = AZ_SPAN_FROM_STR("\0\0");

  ASSERT_EQ(4, az_span_find(span, target));
  PASS();
}

TEST az_span_find_capacity_checks_success(void) {
  uint8_t *buffer = (uint8_t *)"aaaa";

  ASSERT_EQ(0,
            az_span_find(az_span_create(buffer, 2), az_span_create(buffer, 2)));
  ASSERT_EQ(0,
            az_span_find(az_span_create(buffer, 2), az_span_create(buffer, 0)));
  ASSERT_EQ(0,
            az_span_find(az_span_create(buffer, 0), az_span_create(buffer, 0)));

  ASSERT_EQ(0, az_span_find(az_span_create(buffer, 2),
                            az_span_create(buffer + 1, 2)));
  ASSERT_EQ(0, az_span_find(az_span_create(buffer + 1, 2),
                            az_span_create(buffer, 2)));
  ASSERT_EQ(0, az_span_find(az_span_create(buffer + 1, 2),
                            az_span_create(buffer + 1, 2)));
  ASSERT_EQ(0, az_span_find(az_span_create(buffer, 2),
                            az_span_create(buffer + 2, 2)));
  ASSERT_EQ(0, az_span_find(az_span_create(buffer + 2, 2),
                            az_span_create(buffer, 2)));
  PASS();
}

TEST az_span_find_overlapping_checks_success(void) {
  az_span span = AZ_SPAN_FROM_STR("abcdefghij");
  az_span source = az_span_slice(span, 1, 4);
  az_span target = az_span_slice(span, 6, 9);
  ASSERT_EQ(FAIL_SIZE_T_VAL, az_span_find(source, target));
  ASSERT_EQ(FAIL_SIZE_T_VAL, az_span_find(source, az_span_slice(span, 1, 5)));
  ASSERT_EQ(1, az_span_find(source, az_span_slice(span, 2, 4)));

  PASS();
}

TEST az_span_i64toa_test(void) {
  uint8_t buffer[100];
  az_span b_span = AZ_SPAN_FROM_BUFFER(buffer);
  az_span remainder;
  size_t size_before_write = az_span_size(b_span);
  int64_t number = 123;
  az_span number_str = AZ_SPAN_FROM_STR("123");
  uint64_t reverse;

  ASSERT_EQ(AZ_OK, az_span_i64toa(b_span, number, &remainder));
  ASSERT_EQ(az_span_size(b_span), size_before_write);
  /* remainder should be size minus number of digits (3) */
  ASSERT_EQ(size_before_write - 3, az_span_size(remainder));

  /* create az_span for written data */
  b_span = az_span_create(az_span_ptr(b_span),
                          az_span_size(b_span) - az_span_size(remainder));

  ASSERT(az_span_is_content_equal(b_span, number_str));

  /* convert back */
  reverse = 0;
  ASSERT_EQ(AZ_OK, az_span_atou64(b_span, &reverse));
  ASSERT_EQ(number, reverse);

  PASS();
}

TEST az_span_i64toa_negative_number_test(void) {
  uint8_t buffer[100];
  az_span b_span = AZ_SPAN_FROM_BUFFER(buffer);
  az_span remainder;
  size_t size_before_write = az_span_size(b_span);
  int64_t number = -123;
  az_span number_str = AZ_SPAN_FROM_STR("-123");
  int64_t reverse;

  ASSERT_EQ(AZ_OK, az_span_i64toa(b_span, number, &remainder));
  ASSERT_EQ(az_span_size(b_span), size_before_write);
  /* remainder should be size minus number of digits (4) */
  ASSERT_EQ(size_before_write - 4, az_span_size(remainder));

  /* create az_span for written data */
  b_span = az_span_create(az_span_ptr(b_span),
                          az_span_size(b_span) - az_span_size(remainder));

  ASSERT(az_span_is_content_equal(b_span, number_str));

  reverse = 0;
  ASSERT_EQ(AZ_OK, az_span_atoi64(b_span, &reverse));
  ASSERT_EQ(number, reverse);
  PASS();
}

TEST az_span_slice_to_end_test(void) {
  uint8_t raw_buffer[20];
  az_span buffer = AZ_SPAN_FROM_BUFFER(raw_buffer);

  ASSERT_EQ(20, az_span_size(buffer));

  {
    az_span result = az_span_slice_to_end(buffer, 1);
    ASSERT_EQ(19, az_span_size(result));

    result = az_span_slice_to_end(buffer, 5);
    ASSERT_EQ(15, az_span_size(result));
    PASS();
  }
}

TEST az_span_test_macro_only_allows_byte_buffers(void) {

  {
    uint8_t uint8_buffer[2];
    ASSERT_EQ(1, _az_IS_ARRAY(uint8_buffer));
    ASSERT_EQ(1, _az_IS_BYTE_ARRAY(uint8_buffer));
    {
      az_span valid = AZ_SPAN_FROM_BUFFER(uint8_buffer);
      ASSERT_EQ(2, az_span_size(valid));
      PASS();
    }
  }

  {
    char char_buffer[2];
    ASSERT_EQ(1, _az_IS_ARRAY(char_buffer));
    ASSERT_EQ(1, _az_IS_BYTE_ARRAY(char_buffer));
    {
      az_span valid = AZ_SPAN_FROM_BUFFER(char_buffer);
      ASSERT_EQ(2, az_span_size(valid));
    }
  }

  {
    uint32_t uint32_buffer[2];
    ASSERT_EQ(1, _az_IS_ARRAY(uint32_buffer));
    ASSERT_EQ(0, _az_IS_BYTE_ARRAY(uint32_buffer));
  }

  {
    uint8_t x = 1;
    uint8_t *p1 = &x;
    ASSERT_EQ(0, _az_IS_ARRAY(p1));
    ASSERT_EQ(0, _az_IS_BYTE_ARRAY(p1));
  }

  {
    char *p1 = "HELLO";
    ASSERT_EQ(0, _az_IS_ARRAY(p1));
    ASSERT_EQ(0, _az_IS_BYTE_ARRAY(p1));
  }
}

TEST az_span_create_from_str_succeeds(void) {
  char *str = "HelloWorld";
  az_span buffer = az_span_create_from_str(str);

  ASSERT_EQ(10, az_span_size(buffer));
  ASSERT(az_span_ptr(buffer) != NULL);
  ASSERT((char *)az_span_ptr(buffer) == str);
  PASS();
}

TEST az_span_copy_uint8_succeeds(void) {
  uint8_t raw_buffer[15];
  az_span buffer = AZ_SPAN_FROM_BUFFER(raw_buffer);

  buffer = az_span_copy_u8(buffer, 'a');
  ASSERT_EQ(14, az_span_size(buffer));
  buffer = az_span_copy_u8(buffer, 'b');
  ASSERT_EQ(13, az_span_size(buffer));
  buffer = az_span_copy_u8(buffer, 'c');
  ASSERT_EQ(12, az_span_size(buffer));

  ASSERT(az_span_is_content_equal(
      az_span_slice(AZ_SPAN_FROM_BUFFER(raw_buffer), 0, 3),
      AZ_SPAN_FROM_STR("abc")));
  PASS();
}

TEST az_span_i32toa_succeeds(void) {
  int32_t v = 12345;
  uint8_t raw_buffer[15];
  az_span buffer = AZ_SPAN_FROM_BUFFER(raw_buffer);
  az_span out_span;

  ASSERT(az_result_succeeded(az_span_i32toa(buffer, v, &out_span)));
  ASSERT_EQ(10, az_span_size(out_span));
  ASSERT(az_span_is_content_equal(
      az_span_slice(AZ_SPAN_FROM_BUFFER(raw_buffer), 0, 5),
      AZ_SPAN_FROM_STR("12345")));
  PASS();
}

TEST az_span_i32toa_negative_succeeds(void) {
  int32_t v = -12345;
  uint8_t raw_buffer[15];
  az_span buffer = AZ_SPAN_FROM_BUFFER(raw_buffer);
  az_span out_span;

  ASSERT(az_result_succeeded(az_span_i32toa(buffer, v, &out_span)));
  ASSERT_EQ(9, az_span_size(out_span));
  ASSERT(az_span_is_content_equal(
      az_span_slice(AZ_SPAN_FROM_BUFFER(raw_buffer), 0, 6),
      AZ_SPAN_FROM_STR("-12345")));
  PASS();
}

TEST az_span_i32toa_zero_succeeds(void) {
  int32_t v = 0;
  uint8_t raw_buffer[15];
  az_span buffer = AZ_SPAN_FROM_BUFFER(raw_buffer);
  az_span out_span;

  ASSERT(az_result_succeeded(az_span_i32toa(buffer, v, &out_span)));
  ASSERT_EQ(14, az_span_size(out_span));
  ASSERT(az_span_is_content_equal(
      az_span_slice(AZ_SPAN_FROM_BUFFER(raw_buffer), 0, 1),
      AZ_SPAN_FROM_STR("0")));
  PASS();
}

TEST az_span_i32toa_max_int_succeeds(void) {
  int32_t v = 2147483647;
  uint8_t raw_buffer[15];
  az_span buffer = AZ_SPAN_FROM_BUFFER(raw_buffer);
  az_span out_span;

  ASSERT(az_result_succeeded(az_span_i32toa(buffer, v, &out_span)));
  ASSERT_EQ(5, az_span_size(out_span));
  ASSERT(az_span_is_content_equal(
      az_span_slice(AZ_SPAN_FROM_BUFFER(raw_buffer), 0, 10),
      AZ_SPAN_FROM_STR("2147483647")));
  PASS();
}

TEST az_span_i32toa_overflow_fails(void) {
  int32_t v = 2147483647;
  uint8_t raw_buffer[4];
  az_span buffer = AZ_SPAN_FROM_BUFFER(raw_buffer);
  az_span out_span;

  ASSERT(az_span_i32toa(buffer, v, &out_span) == AZ_ERROR_NOT_ENOUGH_SPACE);
  PASS();
}

TEST az_span_u32toa_succeeds(void) {
  uint32_t v = 12345;
  uint8_t raw_buffer[15];
  az_span buffer = AZ_SPAN_FROM_BUFFER(raw_buffer);
  az_span out_span;

  ASSERT(az_result_succeeded(az_span_u32toa(buffer, v, &out_span)));
  ASSERT_EQ(10, az_span_size(out_span));
  ASSERT(az_span_is_content_equal(
      az_span_slice(AZ_SPAN_FROM_BUFFER(raw_buffer), 0, 5),
      AZ_SPAN_FROM_STR("12345")));
  PASS();
}

TEST az_span_u32toa_zero_succeeds(void) {
  uint32_t v = 0;
  uint8_t raw_buffer[15];
  az_span buffer = AZ_SPAN_FROM_BUFFER(raw_buffer);
  az_span out_span;

  ASSERT(az_result_succeeded(az_span_u32toa(buffer, v, &out_span)));
  ASSERT_EQ(14, az_span_size(out_span));
  ASSERT(az_span_is_content_equal(
      az_span_slice(AZ_SPAN_FROM_BUFFER(raw_buffer), 0, 1),
      AZ_SPAN_FROM_STR("0")));
  PASS();
}

TEST az_span_u32toa_max_uint_succeeds(void) {
  uint32_t v = 4294967295U;
  uint8_t raw_buffer[15];
  az_span buffer = AZ_SPAN_FROM_BUFFER(raw_buffer);
  az_span out_span;

  ASSERT(az_result_succeeded(az_span_u32toa(buffer, v, &out_span)));
  ASSERT_EQ(5, az_span_size(out_span));
  ASSERT(az_span_is_content_equal(
      az_span_slice(AZ_SPAN_FROM_BUFFER(raw_buffer), 0, 10),
      AZ_SPAN_FROM_STR("4294967295")));
  PASS();
}

TEST az_span_u32toa_overflow_fails(void) {
  uint32_t v = 2147483647;
  uint8_t raw_buffer[4];
  az_span buffer = AZ_SPAN_FROM_BUFFER(raw_buffer);
  az_span out_span;

  ASSERT(az_span_u32toa(buffer, v, &out_span) == AZ_ERROR_NOT_ENOUGH_SPACE);
  PASS();
}

#define AZ_SPAN_DTOA_SUCCEEDS_HELPER(v, fractional_digits, expected)           \
  do {                                                                         \
    double round_trip;                                                         \
    az_span buffer = AZ_SPAN_FROM_BUFFER(raw_buffer),                          \
            out_span = az_span_empty(), output;                                \
    ASSERT(az_result_succeeded(                                                \
        az_span_dtoa(buffer, v, fractional_digits, &out_span)));               \
    output = az_span_slice(buffer, 0, _az_span_diff(out_span, buffer));        \
    ASSERT_MEM_EQ(az_span_ptr(expected), az_span_ptr(output),                  \
                  az_span_size(expected));                                     \
    ASSERT(az_result_succeeded(                                                \
        az_span_dtoa(buffer, v, fractional_digits, &out_span)));               \
    round_trip = 0;                                                            \
    ASSERT(az_result_succeeded(az_span_atod(output, &round_trip)));            \
    ASSERT(fabs((v) - round_trip) < 0.01);                                     \
  } while (0)

TEST az_span_dtoa_succeeds(void) {
  /* We don't need more than 33 bytes to hold the supported doubles: */
  /* [-][0-9]{16}.[0-9]{15}, i.e. 1+16+1+15 */
  uint8_t raw_buffer[33] = {0};

  AZ_SPAN_DTOA_SUCCEEDS_HELPER(0, 15, AZ_SPAN_FROM_STR("0"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(1., 15, AZ_SPAN_FROM_STR("1"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(1.e3, 15, AZ_SPAN_FROM_STR("1000"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(-0, 15, AZ_SPAN_FROM_STR("0"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(0.0, 15, AZ_SPAN_FROM_STR("0"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(1, 15, AZ_SPAN_FROM_STR("1"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(-1, 15, AZ_SPAN_FROM_STR("-1"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(1.0, 15, AZ_SPAN_FROM_STR("1"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(-1.0, 15, AZ_SPAN_FROM_STR("-1"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(12345, 15, AZ_SPAN_FROM_STR("12345"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(-12345, 15, AZ_SPAN_FROM_STR("-12345"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(123.123, 15,
                               AZ_SPAN_FROM_STR("123.123000000000004"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(123.1230, 15,
                               AZ_SPAN_FROM_STR("123.123000000000004"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(123.0100, 15,
                               AZ_SPAN_FROM_STR("123.010000000000005"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(123.001, 15,
                               AZ_SPAN_FROM_STR("123.001000000000004"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(0.000000000000001, 15,
                               AZ_SPAN_FROM_STR("0.000000000000001"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(1.0000000001, 15,
                               AZ_SPAN_FROM_STR("1.0000000001"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(-1.0000000001, 15,
                               AZ_SPAN_FROM_STR("-1.0000000001"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(100.001, 15,
                               AZ_SPAN_FROM_STR("100.001000000000004"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(100.00100, 15,
                               AZ_SPAN_FROM_STR("100.001000000000004"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(00100.001, 15,
                               AZ_SPAN_FROM_STR("100.001000000000004"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(00100.00100, 15,
                               AZ_SPAN_FROM_STR("100.001000000000004"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(0.001, 15, AZ_SPAN_FROM_STR("0.001"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(0.0012, 15,
                               AZ_SPAN_FROM_STR("0.001199999999999"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(1.2e4, 15, AZ_SPAN_FROM_STR("12000"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(1.2e-4, 15, AZ_SPAN_FROM_STR("0.00012"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(1.2e+4, 15, AZ_SPAN_FROM_STR("12000"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(-1.2e4, 15, AZ_SPAN_FROM_STR("-12000"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(-1.2e-4, 15, AZ_SPAN_FROM_STR("-0.00012"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(0.0001, 15, AZ_SPAN_FROM_STR("0.0001"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(0.00102, 15, AZ_SPAN_FROM_STR("0.00102"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(.34567, 15, AZ_SPAN_FROM_STR("0.34567"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(+.34567, 15, AZ_SPAN_FROM_STR("0.34567"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(-.34567, 15, AZ_SPAN_FROM_STR("-0.34567"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(9876.54321, 15,
                               AZ_SPAN_FROM_STR("9876.543209999999817"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(-9876.54321, 15,
                               AZ_SPAN_FROM_STR("-9876.543209999999817"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(987654.321, 15,
                               AZ_SPAN_FROM_STR("987654.320999999996274"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(-987654.321, 15,
                               AZ_SPAN_FROM_STR("-987654.320999999996274"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(987654.0000321, 15,
                               AZ_SPAN_FROM_STR("987654.000032100011594"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(2147483647, 15, AZ_SPAN_FROM_STR("2147483647"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(2 * (double)1073741824, 15,
                               AZ_SPAN_FROM_STR("2147483648"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(-2147483647 - 1, 15,
                               AZ_SPAN_FROM_STR("-2147483648"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(4503599627370496, 15,
                               AZ_SPAN_FROM_STR("4503599627370496"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(9007199254740991, 15,
                               AZ_SPAN_FROM_STR("9007199254740991"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER((double)4503599627370496.2, 15,
                               AZ_SPAN_FROM_STR("4503599627370496"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(1e15, 15, AZ_SPAN_FROM_STR("1000000000000000"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(-1e15, 15,
                               AZ_SPAN_FROM_STR("-1000000000000000"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(1.8e10, 15, AZ_SPAN_FROM_STR("18000000000"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(-1.8e10, 15, AZ_SPAN_FROM_STR("-18000000000"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(1e-15, 15,
                               AZ_SPAN_FROM_STR("0.000000000000001"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(1e-10, 15, AZ_SPAN_FROM_STR("0.0000000001"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(1e-5, 15, AZ_SPAN_FROM_STR("0.00001"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(0.1234567890123456, 15,
                               AZ_SPAN_FROM_STR("0.123456789012345"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(123456789012345.123456789012340000, 15,
                               AZ_SPAN_FROM_STR("123456789012345.125"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(1000000000000.123456789012340000, 15,
                               AZ_SPAN_FROM_STR("1000000000000.1234130859375"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(123456789012345.1234567890123400001, 15,
                               AZ_SPAN_FROM_STR("123456789012345.125"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(1000000000000.1234567890123400001, 15,
                               AZ_SPAN_FROM_STR("1000000000000.1234130859375"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(12345.123e-15, 15,
                               AZ_SPAN_FROM_STR("0.000000000012345"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(12345.12300000010e5, 15,
                               AZ_SPAN_FROM_STR("1234512300.000010013580322"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(1e-300, 15, AZ_SPAN_FROM_STR("0"));

  AZ_SPAN_DTOA_SUCCEEDS_HELPER(0, 2, AZ_SPAN_FROM_STR("0"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(1., 2, AZ_SPAN_FROM_STR("1"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(1.e3, 2, AZ_SPAN_FROM_STR("1000"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(-0, 2, AZ_SPAN_FROM_STR("0"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(0.0, 2, AZ_SPAN_FROM_STR("0"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(1, 2, AZ_SPAN_FROM_STR("1"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(-1, 2, AZ_SPAN_FROM_STR("-1"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(1.0, 2, AZ_SPAN_FROM_STR("1"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(-1.0, 2, AZ_SPAN_FROM_STR("-1"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(12345, 2, AZ_SPAN_FROM_STR("12345"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(-12345, 2, AZ_SPAN_FROM_STR("-12345"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(123.123, 2, AZ_SPAN_FROM_STR("123.12"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(123.1230, 2, AZ_SPAN_FROM_STR("123.12"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(123.0100, 2, AZ_SPAN_FROM_STR("123.01"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(123.001, 2, AZ_SPAN_FROM_STR("123"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(0.000000000000001, 2, AZ_SPAN_FROM_STR("0"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(1.0000000001, 2, AZ_SPAN_FROM_STR("1"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(-1.0000000001, 2, AZ_SPAN_FROM_STR("-1"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(100.001, 2, AZ_SPAN_FROM_STR("100"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(100.00100, 2, AZ_SPAN_FROM_STR("100"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(00100.001, 2, AZ_SPAN_FROM_STR("100"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(00100.00100, 2, AZ_SPAN_FROM_STR("100"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(0.001, 2, AZ_SPAN_FROM_STR("0"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(0.0012, 2, AZ_SPAN_FROM_STR("0"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(1.2e4, 2, AZ_SPAN_FROM_STR("12000"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(1.2e-4, 2, AZ_SPAN_FROM_STR("0"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(1.2e+4, 2, AZ_SPAN_FROM_STR("12000"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(-1.2e4, 2, AZ_SPAN_FROM_STR("-12000"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(-1.2e-4, 2, AZ_SPAN_FROM_STR("-0"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(0.0001, 2, AZ_SPAN_FROM_STR("0"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(0.00102, 2, AZ_SPAN_FROM_STR("0"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(.34567, 2, AZ_SPAN_FROM_STR("0.34"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(+.34567, 2, AZ_SPAN_FROM_STR("0.34"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(-.34567, 2, AZ_SPAN_FROM_STR("-0.34"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(9876.54321, 2, AZ_SPAN_FROM_STR("9876.54"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(-9876.54321, 2, AZ_SPAN_FROM_STR("-9876.54"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(987654.321, 2, AZ_SPAN_FROM_STR("987654.32"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(-987654.321, 2, AZ_SPAN_FROM_STR("-987654.32"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(987654.0000321, 2, AZ_SPAN_FROM_STR("987654"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(2147483647, 2, AZ_SPAN_FROM_STR("2147483647"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(2 * (double)1073741824, 2,
                               AZ_SPAN_FROM_STR("2147483648"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(-2147483647 - 1, 2,
                               AZ_SPAN_FROM_STR("-2147483648"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(4503599627370496, 2,
                               AZ_SPAN_FROM_STR("4503599627370496"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(9007199254740991, 2,
                               AZ_SPAN_FROM_STR("9007199254740991"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER((double)4503599627370496.2, 2,
                               AZ_SPAN_FROM_STR("4503599627370496"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(1e15, 2, AZ_SPAN_FROM_STR("1000000000000000"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(-1e15, 2, AZ_SPAN_FROM_STR("-1000000000000000"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(1.8e10, 2, AZ_SPAN_FROM_STR("18000000000"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(-1.8e10, 2, AZ_SPAN_FROM_STR("-18000000000"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(1e-15, 2, AZ_SPAN_FROM_STR("0"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(1e-10, 2, AZ_SPAN_FROM_STR("0"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(1e-5, 2, AZ_SPAN_FROM_STR("0"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(0.1234567890123456, 2, AZ_SPAN_FROM_STR("0.12"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(123456789012345.123456789012340000, 2,
                               AZ_SPAN_FROM_STR("123456789012345.12"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(1000000000000.123456789012340000, 2,
                               AZ_SPAN_FROM_STR("1000000000000.12"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(123456789012345.1234567890123400001, 2,
                               AZ_SPAN_FROM_STR("123456789012345.12"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(1000000000000.1234567890123400001, 2,
                               AZ_SPAN_FROM_STR("1000000000000.12"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(12345.123e-15, 2, AZ_SPAN_FROM_STR("0"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(12345.12300000010e5, 2,
                               AZ_SPAN_FROM_STR("1234512300"));
  AZ_SPAN_DTOA_SUCCEEDS_HELPER(1e-300, 2, AZ_SPAN_FROM_STR("0"));

  PASS();
}

TEST az_span_dtoa_overflow_fails(void) {
  /* We don't need more than 33 bytes to hold the supported doubles: */
  /* [-][0-9]{16}.[0-9]{15}, i.e. 1+16+1+15 */
  uint8_t raw_buffer[33];
  az_span buff = AZ_SPAN_FROM_BUFFER(raw_buffer);
  az_span o;

  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE,
            az_span_dtoa(az_span_slice(buff, 0, 0), 0, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE,
            az_span_dtoa(az_span_slice(buff, 0, 0), 1., 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE,
            az_span_dtoa(az_span_slice(buff, 0, 3), 1.e3, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE,
            az_span_dtoa(az_span_slice(buff, 0, 0), 1, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE,
            az_span_dtoa(az_span_slice(buff, 0, 1), -1, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE,
            az_span_dtoa(az_span_slice(buff, 0, 4), 12345, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE,
            az_span_dtoa(az_span_slice(buff, 0, 5), -12345, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE,
            az_span_dtoa(az_span_slice(buff, 0, 10), 123.123, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE,
            az_span_dtoa(az_span_slice(buff, 0, 5), 123.0100, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE, az_span_dtoa(az_span_slice(buff, 0, 16),
                                                    0.000000000000001, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE,
            az_span_dtoa(az_span_slice(buff, 0, 11), 1.0000000001, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE,
            az_span_dtoa(az_span_slice(buff, 0, 12), -1.0000000001, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE,
            az_span_dtoa(az_span_slice(buff, 0, 7), 100.001, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE,
            az_span_dtoa(az_span_slice(buff, 0, 4), 0.001, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE,
            az_span_dtoa(az_span_slice(buff, 0, 10), 0.0012, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE,
            az_span_dtoa(az_span_slice(buff, 0, 4), 1.2e4, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE,
            az_span_dtoa(az_span_slice(buff, 0, 6), 1.2e-4, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE,
            az_span_dtoa(az_span_slice(buff, 0, 4), 1.2e+4, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE,
            az_span_dtoa(az_span_slice(buff, 0, 5), -1.2e4, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE,
            az_span_dtoa(az_span_slice(buff, 0, 7), -1.2e-4, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE,
            az_span_dtoa(az_span_slice(buff, 0, 4), 0.0001, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE,
            az_span_dtoa(az_span_slice(buff, 0, 5), 0.00102, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE,
            az_span_dtoa(az_span_slice(buff, 0, 5), .34567, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE,
            az_span_dtoa(az_span_slice(buff, 0, 6), -.34567, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE,
            az_span_dtoa(az_span_slice(buff, 0, 10), 9876.54321, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE,
            az_span_dtoa(az_span_slice(buff, 0, 11), -9876.54321, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE,
            az_span_dtoa(az_span_slice(buff, 0, 10), 987654.321, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE,
            az_span_dtoa(az_span_slice(buff, 0, 11), -987654.321, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE,
            az_span_dtoa(az_span_slice(buff, 0, 10), 987654.0000321, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE,
            az_span_dtoa(az_span_slice(buff, 0, 9), 2147483647, 15, &o));
  ASSERT_EQ(
      AZ_ERROR_NOT_ENOUGH_SPACE,
      az_span_dtoa(az_span_slice(buff, 0, 9), 2 * (double)1073741824, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE,
            az_span_dtoa(az_span_slice(buff, 0, 10), -2147483647 - 1, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE,
            az_span_dtoa(az_span_slice(buff, 0, 15), 4503599627370496, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE,
            az_span_dtoa(az_span_slice(buff, 0, 15), 9007199254740991, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE,
            az_span_dtoa(az_span_slice(buff, 0, 15), (double)4503599627370496.2,
                         15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE,
            az_span_dtoa(az_span_slice(buff, 0, 14), 1e15, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE,
            az_span_dtoa(az_span_slice(buff, 0, 15), -1e15, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE,
            az_span_dtoa(az_span_slice(buff, 0, 10), 1.8e10, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE,
            az_span_dtoa(az_span_slice(buff, 0, 11), -1.8e10, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE,
            az_span_dtoa(az_span_slice(buff, 0, 15), 1e-15, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE,
            az_span_dtoa(az_span_slice(buff, 0, 10), 1e-10, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE,
            az_span_dtoa(az_span_slice(buff, 0, 5), 1e-5, 15, &o));
  ASSERT_EQ(
      AZ_ERROR_NOT_ENOUGH_SPACE,
      az_span_dtoa(az_span_slice(buff, 0, 16), 0.1234567890123456, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE,
            az_span_dtoa(az_span_slice(buff, 0, 18),
                         123456789012345.123456789012340000, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE,
            az_span_dtoa(az_span_slice(buff, 0, 26),
                         1000000000000.1234567890123400001, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE,
            az_span_dtoa(az_span_slice(buff, 0, 16), 12345.123e-15, 15, &o));
  ASSERT_EQ(
      AZ_ERROR_NOT_ENOUGH_SPACE,
      az_span_dtoa(az_span_slice(buff, 0, 25), 12345.12300000010e5, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE,
            az_span_dtoa(az_span_slice(buff, 0, 0), 1e-300, 15, &o));

  PASS();
}

TEST az_span_dtoa_too_large(void) {
  /* We don't need more than 33 bytes to hold the supported doubles: */
  /* [-][0-9]{16}.[0-9]{15}, i.e. 1+16+1+15 */
  uint8_t raw_buffer[33] = {0};
  az_span buff = AZ_SPAN_FROM_BUFFER(raw_buffer);
  az_span o = az_span_empty();

  ASSERT_EQ(AZ_ERROR_NOT_SUPPORTED,
            az_span_dtoa(buff, 9007199254740992, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_SUPPORTED,
            az_span_dtoa(buff, (double)9007199254740993, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_SUPPORTED,
            az_span_dtoa(buff, (double)45035996273704961, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_SUPPORTED,
            az_span_dtoa(buff, 2147483647 * (double)4294967298, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_SUPPORTED,
            az_span_dtoa(buff, -2147483647 * (double)4294967298, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_SUPPORTED,
            az_span_dtoa(buff, (double)1844674407370955100, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_SUPPORTED,
            az_span_dtoa(buff, (double)1.844674407370955e+19, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_SUPPORTED,
            az_span_dtoa(buff, (double)1.8446744073709551e+19, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_SUPPORTED,
            az_span_dtoa(buff, (double)1.8446744073709552e+19, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_SUPPORTED,
            az_span_dtoa(buff, (double)18446744073709551615UL, 15, &o));
  ASSERT_EQ(
      AZ_ERROR_NOT_SUPPORTED,
      az_span_dtoa(buff, 18446744073709551615.18446744073709551615, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_SUPPORTED, az_span_dtoa(buff, 1e16, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_SUPPORTED, az_span_dtoa(buff, 12345.123e15, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_SUPPORTED, az_span_dtoa(buff, -12345.123e15, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_SUPPORTED, az_span_dtoa(buff, 1e300, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_SUPPORTED, az_span_dtoa(buff, -1e300, 15, &o));
  ASSERT_EQ(AZ_ERROR_NOT_SUPPORTED, az_span_dtoa(buff, 1.7e308, 15, &o));

  PASS();
}

TEST az_span_copy_empty(void) {
  uint8_t buff[10];
  az_span dst = AZ_SPAN_FROM_BUFFER(buff);
  ASSERT(az_span_is_content_equal(az_span_copy(dst, az_span_empty()), dst));

  PASS();
}

TEST test_az_span_is_valid(void) {
  const az_span empty_span = az_span_empty();

  ASSERT(_az_span_is_valid(empty_span, 0, true));
  ASSERT(!(_az_span_is_valid(empty_span, 0, false)));
  ASSERT(!(_az_span_is_valid(empty_span, 1, true)));
  ASSERT(!(_az_span_is_valid(empty_span, 1, false)));
  ASSERT(!(_az_span_is_valid(empty_span, -1, true)));
  ASSERT(!(_az_span_is_valid(empty_span, -1, false)));

  ASSERT(_az_span_is_valid(empty_span, 0, true));
  ASSERT(!(_az_span_is_valid(empty_span, 0, false)));
  ASSERT(!(_az_span_is_valid(empty_span, 1, true)));
  ASSERT(!(_az_span_is_valid(empty_span, 1, false)));
  ASSERT(!(_az_span_is_valid(empty_span, -1, true)));
  ASSERT(!(_az_span_is_valid(empty_span, -1, false)));

  ASSERT(_az_span_is_valid(AZ_SPAN_FROM_STR(""), 0, true));
  ASSERT(_az_span_is_valid(AZ_SPAN_FROM_STR(""), 0, false));
  ASSERT(!(_az_span_is_valid(AZ_SPAN_FROM_STR(""), 1, true)));
  ASSERT(!(_az_span_is_valid(AZ_SPAN_FROM_STR(""), 1, false)));
  ASSERT(!(_az_span_is_valid(AZ_SPAN_FROM_STR(""), -1, true)));
  ASSERT(!(_az_span_is_valid(AZ_SPAN_FROM_STR(""), -1, false)));

  ASSERT(_az_span_is_valid(AZ_SPAN_FROM_STR("Hello"), 0, true));
  ASSERT(_az_span_is_valid(AZ_SPAN_FROM_STR("Hello"), 0, false));
  ASSERT(_az_span_is_valid(AZ_SPAN_FROM_STR("Hello"), 1, true));
  ASSERT(_az_span_is_valid(AZ_SPAN_FROM_STR("Hello"), 1, false));
  ASSERT(_az_span_is_valid(AZ_SPAN_FROM_STR("Hello"), 5, true));
  ASSERT(_az_span_is_valid(AZ_SPAN_FROM_STR("Hello"), 5, false));
  ASSERT(!(_az_span_is_valid(AZ_SPAN_FROM_STR("Hello"), 6, true)));
  ASSERT(!(_az_span_is_valid(AZ_SPAN_FROM_STR("Hello"), 6, false)));
  ASSERT(!(_az_span_is_valid(AZ_SPAN_FROM_STR("Hello"), -1, true)));
  ASSERT(!(_az_span_is_valid(AZ_SPAN_FROM_STR("Hello"), -1, false)));

  {
    uint8_t *const max_ptr = (uint8_t *)~0;
    ASSERT(_az_span_is_valid(az_span_create(max_ptr, 0), 0, false));
    ASSERT(_az_span_is_valid(az_span_create(max_ptr, 0), 0, true));

    ASSERT(!(_az_span_is_valid(az_span_create(max_ptr, 1), 0, false)));
    ASSERT(!(_az_span_is_valid(az_span_create(max_ptr, 1), 0, true)));

    ASSERT(_az_span_is_valid(az_span_create(max_ptr - 1, 1), 0, false));
    ASSERT(_az_span_is_valid(az_span_create(max_ptr - 1, 1), 0, true));

    ASSERT(!(_az_span_is_valid(az_span_create(max_ptr - 1, 2), 0, false)));
    ASSERT(!(_az_span_is_valid(az_span_create(max_ptr - 1, 2), 0, true)));

    ASSERT(
        !(_az_span_is_valid(az_span_create(max_ptr - 1, INT32_MAX), 0, false)));
    ASSERT(
        !(_az_span_is_valid(az_span_create(max_ptr - 1, INT32_MAX), 0, true)));

    ASSERT(_az_span_is_valid(az_span_create(max_ptr - INT32_MAX, INT32_MAX), 0,
                             false));
    ASSERT(_az_span_is_valid(az_span_create(max_ptr - INT32_MAX, INT32_MAX), 0,
                             true));
    PASS();
  }
}

TEST test_az_span_overlap(void) {

  ASSERT(!(_az_span_overlap(az_span_create((uint8_t *)10, 10),
                            az_span_create((uint8_t *)30, 10))));
  ASSERT(!(_az_span_overlap(az_span_create((uint8_t *)30, 10),
                            az_span_create((uint8_t *)10, 10))));

  ASSERT(!(_az_span_overlap(az_span_create((uint8_t *)10, 10),
                            az_span_create((uint8_t *)20, 10))));
  ASSERT(!(_az_span_overlap(az_span_create((uint8_t *)20, 10),
                            az_span_create((uint8_t *)10, 10))));

  ASSERT(!(_az_span_overlap(az_span_create((uint8_t *)10, 0),
                            az_span_create((uint8_t *)10, 0))));

  ASSERT(_az_span_overlap(az_span_create((uint8_t *)10, 10),
                          az_span_create((uint8_t *)15, 0)));
  ASSERT(_az_span_overlap(az_span_create((uint8_t *)15, 0),
                          az_span_create((uint8_t *)10, 10)));

  ASSERT(_az_span_overlap(az_span_create((uint8_t *)10, 10),
                          az_span_create((uint8_t *)10, 15)));
  ASSERT(_az_span_overlap(az_span_create((uint8_t *)10, 15),
                          az_span_create((uint8_t *)10, 10)));

  ASSERT(_az_span_overlap(az_span_create((uint8_t *)15, 10),
                          az_span_create((uint8_t *)10, 15)));
  ASSERT(_az_span_overlap(az_span_create((uint8_t *)10, 15),
                          az_span_create((uint8_t *)15, 10)));

  ASSERT(_az_span_overlap(az_span_create((uint8_t *)10, 10),
                          az_span_create((uint8_t *)5, 10)));
  ASSERT(_az_span_overlap(az_span_create((uint8_t *)5, 10),
                          az_span_create((uint8_t *)10, 10)));

  ASSERT(_az_span_overlap(az_span_create((uint8_t *)10, 10),
                          az_span_create((uint8_t *)10, 10)));

  ASSERT(_az_span_overlap(az_span_create((uint8_t *)10, 10),
                          az_span_create((uint8_t *)15, 10)));
  ASSERT(_az_span_overlap(az_span_create((uint8_t *)15, 10),
                          az_span_create((uint8_t *)10, 10)));

  ASSERT(_az_span_overlap(az_span_create((uint8_t *)10, 10),
                          az_span_create((uint8_t *)12, 5)));
  ASSERT(_az_span_overlap(az_span_create((uint8_t *)12, 5),
                          az_span_create((uint8_t *)10, 10)));
  PASS();
}

TEST az_span_trim(void) {
  az_span source = _az_span_trim_whitespace(AZ_SPAN_FROM_STR("   abc   "));
  ASSERT(az_span_is_content_equal(source, AZ_SPAN_FROM_STR("abc")));

  PASS();
}

TEST az_span_trim_left(void) {
  az_span source =
      _az_span_trim_whitespace_from_start(AZ_SPAN_FROM_STR("   abc   "));
  ASSERT(az_span_is_content_equal(source, AZ_SPAN_FROM_STR("abc   ")));

  PASS();
}

TEST az_span_trim_right(void) {
  az_span source =
      _az_span_trim_whitespace_from_end(AZ_SPAN_FROM_STR("   abc   "));
  ASSERT(az_span_is_content_equal(source, AZ_SPAN_FROM_STR("   abc")));

  PASS();
}

TEST az_span_trim_all_whitespace(void) {
  az_span source = _az_span_trim_whitespace(AZ_SPAN_FROM_STR("\t\n\r       "));
  ASSERT_EQ(0, az_span_size(source));

  PASS();
}

TEST az_span_trim_none(void) {
  az_span source = _az_span_trim_whitespace(AZ_SPAN_FROM_STR("abc"));
  ASSERT(az_span_is_content_equal(source, AZ_SPAN_FROM_STR("abc")));

  PASS();
}

TEST az_span_trim_spaced(void) {
  az_span source =
      _az_span_trim_whitespace(AZ_SPAN_FROM_STR("\ta\n b     c    "));
  ASSERT(az_span_is_content_equal(source, AZ_SPAN_FROM_STR("a\n b     c")));

  PASS();
}

TEST az_span_trim_zero(void) {
  az_span source = _az_span_trim_whitespace(AZ_SPAN_FROM_STR(""));
  ASSERT(az_span_is_content_equal(source, AZ_SPAN_FROM_STR("")));

  PASS();
}

TEST az_span_trim_null(void) {
  az_span source = _az_span_trim_whitespace(az_span_empty());
  ASSERT_EQ(0, az_span_size(source));

  PASS();
}

TEST az_span_trim_start(void) {
  az_span source = _az_span_trim_whitespace_from_start(az_span_empty());
  ASSERT_EQ(0, az_span_size(source));

  PASS();
}

TEST az_span_trim_end(void) {
  az_span source =
      _az_span_trim_whitespace_from_end(AZ_SPAN_FROM_STR("\ta\n b     c    "));
  ASSERT(az_span_is_content_equal(source, AZ_SPAN_FROM_STR("\ta\n b     c")));

  PASS();
}

TEST az_span_trim_unicode(void) {
  az_span source = _az_span_trim_whitespace_from_end(
      AZ_SPAN_FROM_STR("  \\U+00A0a\n b     c\\U+2028    "));
  ASSERT(az_span_is_content_equal(
      source, AZ_SPAN_FROM_STR("  \\U+00A0a\n b     c\\U+2028")));

  PASS();
}

TEST az_span_trim_two_calls(void) {
  az_span source =
      _az_span_trim_whitespace_from_start(_az_span_trim_whitespace_from_end(
          AZ_SPAN_FROM_STR("  \\U+00A0a\n b     c\\U+2028    ")));
  ASSERT(az_span_is_content_equal(
      source, AZ_SPAN_FROM_STR("\\U+00A0a\n b     c\\U+2028")));

  PASS();
}

TEST az_span_trim_two_calls_inverse(void) {
  az_span source =
      _az_span_trim_whitespace_from_end(_az_span_trim_whitespace_from_start(
          AZ_SPAN_FROM_STR("  \\U+00A0a\n b     c\\U+2028    ")));
  ASSERT(az_span_is_content_equal(
      source, AZ_SPAN_FROM_STR("\\U+00A0a\n b     c\\U+2028")));

  PASS();
}

TEST az_span_trim_repeat_calls(void) {
  az_span source = _az_span_trim_whitespace_from_end(
      _az_span_trim_whitespace_from_start(AZ_SPAN_FROM_STR("  1234    ")));
  source = _az_span_trim_whitespace(source);
  source = _az_span_trim_whitespace(source);
  source = _az_span_trim_whitespace(source);
  ASSERT(az_span_is_content_equal(source, AZ_SPAN_FROM_STR("1234")));

  PASS();
}

TEST test_az_span_token_success(void) {
  az_span span = AZ_SPAN_FROM_STR("abcdefgabcdefgabcdefg");
  az_span delim = AZ_SPAN_FROM_STR("abc");
  az_span token;
  az_span out_span;
  size_t index = 0;

  /* token: "" */
  token = _az_span_token(span, delim, &out_span, &index);
  ASSERT_EQ(0, index);
  ASSERT(NULL != az_span_ptr(token));
  ASSERT(az_span_size(token) == 0);
  ASSERT(az_span_ptr(out_span) == (az_span_ptr(span) + az_span_size(delim)));
  ASSERT(az_span_size(out_span) == (az_span_size(span) - az_span_size(delim)));

  /* token: "defg" (span+3) */
  span = out_span;

  token = _az_span_token(span, delim, &out_span, &index);
  ASSERT_EQ(4, index);
  ASSERT(az_span_ptr(token) == az_span_ptr(span));
  ASSERT_EQ(4, az_span_size(token));
  ASSERT(az_span_ptr(out_span) ==
         (az_span_ptr(span) + az_span_size(token) + az_span_size(delim)));
  ASSERT(az_span_size(out_span) ==
         (az_span_size(span) - az_span_size(token) - az_span_size(delim)));

  /* token: "defg" (span+10) */
  span = out_span;

  token = _az_span_token(span, delim, &out_span, &index);
  ASSERT_EQ(4, index);
  ASSERT(az_span_ptr(token) == az_span_ptr(span));
  ASSERT_EQ(4, az_span_size(token));
  ASSERT(az_span_ptr(out_span) ==
         (az_span_ptr(span) + az_span_size(token) + az_span_size(delim)));
  ASSERT(az_span_size(out_span) ==
         (az_span_size(span) - az_span_size(token) - az_span_size(delim)));

  /* token: "defg" (span+17) */
  span = out_span;

  token = _az_span_token(span, delim, &out_span, &index);
  ASSERT_EQ(FAIL_SIZE_T_VAL, index);
  ASSERT(az_span_ptr(token) == az_span_ptr(span));
  ASSERT_EQ(4, az_span_size(token));
  ASSERT(az_span_size(out_span) == 0);
  PASS();
}

TEST az_span_u64toa_test(void) {
  uint8_t buffer[100];
  az_span b_span = AZ_SPAN_FROM_BUFFER(buffer);
  az_span remainder;
  size_t size_before_write = az_span_size(b_span);
  uint64_t number = 12345678901234567890ULL;
  az_span number_str = AZ_SPAN_FROM_STR("12345678901234567890");
  uint64_t reverse;

  ASSERT_EQ(AZ_OK, az_span_u64toa(b_span, number, &remainder));
  ASSERT_EQ(az_span_size(b_span), size_before_write);
  ASSERT_EQ(size_before_write - 20, az_span_size(remainder));

  b_span = az_span_create(az_span_ptr(b_span),
                          az_span_size(b_span) - az_span_size(remainder));

  ASSERT(az_span_is_content_equal(b_span, number_str));

  reverse = 0;
  ASSERT_EQ(AZ_OK, az_span_atou64(b_span, &reverse));
  ASSERT_EQ(number, reverse);

  /* Test zero */
  b_span = AZ_SPAN_FROM_BUFFER(buffer);
  ASSERT_EQ(AZ_OK, az_span_u64toa(b_span, 0, &remainder));
  ASSERT_EQ(az_span_size(b_span) - 1, az_span_size(remainder));
  ASSERT(az_span_ptr(b_span)[0] == '0');

  PASS();
}

TEST az_span_char_checks_test(void) {
  ASSERT(uint8_t_isalnum('a'));
  ASSERT(uint8_t_isalnum('Z'));
  ASSERT(uint8_t_isalnum('5'));
  ASSERT(!uint8_t_isalnum('!'));

  ASSERT(az_span_isalnum(AZ_SPAN_FROM_STR("abc012XYZ")));
  ASSERT(!az_span_isalnum(AZ_SPAN_FROM_STR("abc-012")));

  ASSERT(uint8_t_isalpha('g'));
  ASSERT(!uint8_t_isalpha('3'));

  ASSERT(az_span_isalpha(AZ_SPAN_FROM_STR("OnlyLetters")));
  ASSERT(!az_span_isalpha(AZ_SPAN_FROM_STR("Letters and Spaces")));

  ASSERT(uint8_t_isblank(' '));
  ASSERT(uint8_t_isblank('\t'));
  ASSERT(!uint8_t_isblank('\n'));

  ASSERT(az_span_isblank(AZ_SPAN_FROM_STR(" \t  ")));
  ASSERT(!az_span_isblank(AZ_SPAN_FROM_STR("  \n ")));

  ASSERT(uint8_t_isdigit('7'));
  ASSERT(!uint8_t_isdigit('a'));

  ASSERT(az_span_isdigit(AZ_SPAN_FROM_STR("1234567890")));
  ASSERT(!az_span_isdigit(AZ_SPAN_FROM_STR("123.45")));

  ASSERT(uint8_t_islower('m'));
  ASSERT(!uint8_t_islower('M'));

  ASSERT(az_span_islower(AZ_SPAN_FROM_STR("alllower")));
  ASSERT(!az_span_islower(AZ_SPAN_FROM_STR("NotAllLower")));

  ASSERT(uint8_t_isupper('P'));
  ASSERT(!uint8_t_isupper('p'));

  ASSERT(az_span_isupper(AZ_SPAN_FROM_STR("ALLUPPER")));
  ASSERT(!az_span_isupper(AZ_SPAN_FROM_STR("NotAllUpper")));

  PASS();
}

TEST az_span_url_encode_test(void) {
  az_span const source = AZ_SPAN_FROM_STR("Hello World! & @");
  size_t const encoded_len = _az_span_url_encode_calc_length(source);
  uint8_t buffer[100];
  az_span const destination = az_span_create(buffer, sizeof(buffer));
  ptrdiff_t out_len = 0;

  ASSERT_EQ(AZ_OK, _az_span_url_encode(destination, source, &out_len));
  ASSERT_EQ((ptrdiff_t)encoded_len, out_len);

  {
    az_span const result = az_span_create(buffer, (size_t)out_len);
    ASSERT(az_span_is_content_equal(
        result, AZ_SPAN_FROM_STR("Hello%20World%21%20%26%20%40")));
  }

  PASS();
}

TEST az_span_printf_test(void) {
  /* We can't easily capture stdout without redirecting, so we just call it to
   * ensure no crashes and coverage. In a real scenario we'd use a variant that
   * writes to a buffer. */
  az_span_printf((const uint8_t *)"Testing az_span_printf: %s %d %f %Q\n",
                 "string", 42, 3.14, AZ_SPAN_FROM_STR("span"));
  PASS();
}

TEST az_result_helpers_test(void) {
  ASSERT(az_result_failed(AZ_ERROR_NOT_ENOUGH_SPACE));
  ASSERT(!az_result_failed(AZ_OK));
  ASSERT(az_result_succeeded(AZ_OK));
  ASSERT(!az_result_succeeded(AZ_ERROR_NOT_ENOUGH_SPACE));
  PASS();
}

SUITE(az_core_result_suite) { RUN_TEST(az_result_helpers_test); }

static int g_precondition_failed_called = 0;
static void test_precondition_failed_callback(void) {
  g_precondition_failed_called++;
}

#ifndef AZ_NO_PRECONDITION_CHECKING
TEST az_precondition_callback_test(void) {
  az_precondition_failed_fn original = az_precondition_failed_get_callback();
  az_precondition_failed_set_callback(test_precondition_failed_callback);

  g_precondition_failed_called = 0;
  _az_PRECONDITION(1 == 0);
  ASSERT_EQ(1, g_precondition_failed_called);

  _az_PRECONDITION(1 == 1);
  ASSERT_EQ(1, g_precondition_failed_called);

  az_precondition_failed_set_callback(original);
  PASS();
}
#endif

SUITE(az_core_precondition_suite) {
#ifndef AZ_NO_PRECONDITION_CHECKING
  RUN_TEST(az_precondition_callback_test);
#endif
}

#include "c_str_hex_private.h"

TEST az_hex_helpers_test(void) {
  ASSERT_EQ('0', _az_number_to_upper_hex(0));
  ASSERT_EQ('9', _az_number_to_upper_hex(9));
  ASSERT_EQ('A', _az_number_to_upper_hex(10));
  ASSERT_EQ('F', _az_number_to_upper_hex(15));
  PASS();
}

TEST az_span_internal_helpers_test(void) {
  az_span s1 = AZ_SPAN_FROM_STR("abc");
  az_span s2 = az_span_slice_to_end(s1, 1);
  ASSERT_EQ(1, _az_span_diff(s2, s1));

  ASSERT(
      az_result_succeeded(_az_is_expected_span(&s1, AZ_SPAN_FROM_STR("abc"))));
  ASSERT(az_result_failed(_az_is_expected_span(&s1, AZ_SPAN_FROM_STR("abd"))));

  PASS();
}

SUITE(az_core_internal_suite) {
  RUN_TEST(az_hex_helpers_test);
  RUN_TEST(az_span_internal_helpers_test);
}

SUITE(az_core_span_suite) {
  RUN_TEST(az_span_url_encode_test);
  RUN_TEST(az_span_printf_test);
  RUN_TEST(az_span_u64toa_test);
  RUN_TEST(az_span_char_checks_test);
  RUN_TEST(az_span_slice_to_end_test);
  RUN_TEST(test_az_span_getters);
  RUN_TEST(az_single_char_ascii_lower_test);
  RUN_TEST(az_span_to_lower_test);
  RUN_TEST(az_span_to_str_test);
  RUN_TEST(test_az_span_is_content_equal);
  RUN_TEST(az_span_find_beginning_success);
  RUN_TEST(az_span_find_middle_success);
  RUN_TEST(az_span_find_end_success);
  RUN_TEST(az_span_find_source_target_identical_success);
  RUN_TEST(az_span_find_not_found_fail);
  RUN_TEST(az_span_find_error_cases_fail);
  RUN_TEST(az_span_find_target_longer_than_source_fails);
  RUN_TEST(az_span_find_target_overlap_continuation_of_source_fails);
  RUN_TEST(az_span_find_target_more_chars_than_prefix_of_source_fails);
  RUN_TEST(az_span_find_overlapping_target_success);
  RUN_TEST(az_span_find_embedded_NULLs_success);
  RUN_TEST(az_span_find_capacity_checks_success);
  RUN_TEST(az_span_find_overlapping_checks_success);
  RUN_TEST(az_span_atox_return_errors);
  RUN_TEST(az_span_atou32_test);
  RUN_TEST(az_span_atoi32_test);
  RUN_TEST(az_span_atou64_test);
  RUN_TEST(az_span_atoi64_test);
  RUN_TEST(test_az_isfinite);
  RUN_TEST(az_span_atod_test);
  RUN_TEST(az_span_atod_non_finite_not_allowed);
  RUN_TEST(az_span_ato_number_whitespace_or_invalid_not_allowed);
  RUN_TEST(az_span_ato_number_no_out_of_bounds_reads);
  RUN_TEST(az_span_i64toa_negative_number_test);
  RUN_TEST(az_span_i64toa_test);
  RUN_TEST(az_span_test_macro_only_allows_byte_buffers);
  RUN_TEST(az_span_create_from_str_succeeds);
  RUN_TEST(az_span_copy_uint8_succeeds);
  RUN_TEST(az_span_i32toa_succeeds);
  RUN_TEST(az_span_i32toa_negative_succeeds);
  RUN_TEST(az_span_i32toa_max_int_succeeds);
  RUN_TEST(az_span_i32toa_zero_succeeds);
  RUN_TEST(az_span_i32toa_overflow_fails);
  RUN_TEST(az_span_u32toa_succeeds);
  RUN_TEST(az_span_u32toa_zero_succeeds);
  RUN_TEST(az_span_u32toa_max_uint_succeeds);
  RUN_TEST(az_span_u32toa_overflow_fails);
  RUN_TEST(az_span_dtoa_succeeds);
  RUN_TEST(az_span_dtoa_overflow_fails);
  RUN_TEST(az_span_dtoa_too_large);
  RUN_TEST(az_span_copy_empty);
  RUN_TEST(test_az_span_is_valid);
  RUN_TEST(test_az_span_overlap);
  RUN_TEST(az_span_trim);
  RUN_TEST(az_span_trim_left);
  RUN_TEST(az_span_trim_right);
  RUN_TEST(az_span_trim_all_whitespace);
  RUN_TEST(az_span_trim_none);
  RUN_TEST(az_span_trim_spaced);
  RUN_TEST(az_span_trim_zero);
  RUN_TEST(az_span_trim_null);
  RUN_TEST(test_az_span_token_success);
  RUN_TEST(az_span_trim_start);
  RUN_TEST(az_span_trim_end);
  RUN_TEST(az_span_trim_unicode);
  RUN_TEST(az_span_trim_two_calls);
  RUN_TEST(az_span_trim_two_calls_inverse);
  RUN_TEST(az_span_trim_repeat_calls);
}
