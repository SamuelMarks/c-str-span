/* clang-format off */
#define AZ_NO_PRECONDITION_CHECKING
#include "c_str_precondition_internal.h"
#include "c_str_span.h"
#include "c_str_span_internal.h"
#include "c_str_span_log.h"
#include "c_str_span_printf.h"
#include "c_str_span_private.h"

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
#endif

#include <greatest.h>
#include <math.h>
#include <setjmp.h>
#include <stdio.h>
#include <string.h>
/* clang-format on */

#if 0
static jmp_buf g_precondition_jmp;
static volatile bool g_precondition_expected = false;

static void test_precondition_failed_callback(void) {
  if (g_precondition_expected) {
    longjmp(g_precondition_jmp, 1);
  }
}

#define ASSERT_PRECONDITION_FAIL(expr)                                         \
  do {                                                                         \
    g_precondition_expected = true;                                            \
    if (setjmp(g_precondition_jmp) == 0) {                                     \
      (void)(expr);                                                            \
      g_precondition_expected = false;                                         \
      FAIL();                                                                  \
    }                                                                          \
    g_precondition_expected = false;                                           \
  } while (0)
#endif

TEST test_az_span_create_from_str_null(void) {
#ifndef __CYGWIN__
  {
    az_span span = az_span_create_from_str(NULL);
    ASSERT_EQ(0, az_span_size(span));
    ASSERT(az_span_ptr(span) == NULL);

    span = az_span_create_from_str_of_size(NULL, 10);
    ASSERT_EQ(0, az_span_size(span));
    ASSERT(az_span_ptr(span) == NULL);
  }
#endif
  PASS();
}

TEST test_az_span_create_null_size_non_zero(void) {
#ifndef __CYGWIN__
  {
    /* In NO_PRECONDITION mode, az_span_create just sets the fields. */
    az_span span = az_span_create(NULL, 10);
    ASSERT_EQ(10, az_span_size(span));
    ASSERT(az_span_ptr(span) == NULL);
  }
#endif
  PASS();
}

TEST test_az_span_atox_empty(void) {
  uint32_t val_u32;
  int32_t val_i32;
  uint64_t val_u64;
  int64_t val_i64;
  double val_d;
  az_span empty = az_span_empty();

#ifndef __CYGWIN__
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR, az_span_atou32(empty, &val_u32));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atou32(AZ_SPAN_FROM_STR("+"), &val_u32));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR, az_span_atou64(empty, &val_u64));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR, az_span_atoi32(empty, &val_i32));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR, az_span_atoi64(empty, &val_i64));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR, az_span_atod(empty, &val_d));
#endif
  PASS();
}

TEST test_az_span_copy_small_destination(void) {
  uint8_t dest_buf[2];
  az_span dest = az_span_create(dest_buf, 2);
  az_span src = AZ_SPAN_FROM_STR("hello");

#if 0
  az_precondition_failed_fn original = az_precondition_failed_get_callback();
  az_precondition_failed_set_callback(test_precondition_failed_callback);

  az_span out_span;
  ASSERT_PRECONDITION_FAIL(az_span_copy(dest, src, &out_span));

  az_precondition_failed_set_callback(original);
#else
  az_span result;
  az_span_copy(dest, src, &result);
  /* az_span_copy truncates if destination is too small */
  ASSERT_EQ(0, az_span_size(result));
  ASSERT_MEM_EQ("he", dest_buf, 2);
#endif
  PASS();
}

TEST test_az_span_to_str_small_buffer(void) {
  char buf[3];
  az_span src = AZ_SPAN_FROM_STR("hello");

#if 0
  az_precondition_failed_fn original = az_precondition_failed_get_callback();
  az_precondition_failed_set_callback(test_precondition_failed_callback);

  /* az_span_to_str expects size_to_write < destination_max_size */
  ASSERT_PRECONDITION_FAIL(az_span_to_str(buf, 3, src));
  /* and destination_max_size > 0 */
  ASSERT_PRECONDITION_FAIL(az_span_to_str(buf, 0, src));

  az_precondition_failed_set_callback(original);
#else
  az_span_to_str(buf, 3, src);
  ASSERT_STR_EQ("he", buf);

  /* Test size 0 buffer */
  az_span_to_str(buf, 0, src);
#endif
  PASS();
}

TEST test_az_span_dtoa_special_cases(void) {
  uint8_t buf[64];
  az_span dest = az_span_create(buf, 64);
  az_span out;
  uint64_t const inf_bits = 0x7FF0000000000000ULL;
  double d_inf;
#if defined(_MSC_VER) && !defined(__clang__)
  memcpy_s(&d_inf, sizeof(d_inf), &inf_bits, sizeof(inf_bits));
#else
  memcpy(&d_inf, &inf_bits, sizeof(d_inf));
#endif

  /* INF source returns AZ_ERROR_NOT_SUPPORTED in NO_PRECONDITION mode */
  ASSERT_EQ(AZ_ERROR_NOT_SUPPORTED, az_span_dtoa(dest, d_inf, 2, &out));

  /* Fractional digits = 0 (processed as 0) */
  ASSERT_EQ(AZ_OK, az_span_dtoa(dest, 1.23, 0, &out));
  ASSERT_EQ('1', buf[0]);

  /* fractional_digits > 15 capped to 15 */
  ASSERT_EQ(AZ_OK, az_span_dtoa(dest, 0.1, 20, &out));

  /* Value too small (negative) */
  ASSERT_EQ(AZ_ERROR_NOT_SUPPORTED, az_span_dtoa(dest, -1e17, 2, &out));

  PASS();
}

TEST test_az_span_printf_all_specifiers(void) {
  /* This test calls az_span_printf with all supported specifiers to ensure
   * coverage. */
  int n;
  az_span span = AZ_SPAN_FROM_STR("test");
  az_span_printf((const uint8_t *)"%c %d %i %o %u %x %X %e %E %f %F %g %G %a "
                                  "%A %p %s %S %Z %% %Q %n\n",
                 'A', 123, -456, 0777, 456u, 0xabc, 0xABC, 1.23e4, 1.23E4, 1.23,
                 1.23, 1.23, 1.23, 1.23, 1.23, (void *)(size_t)0xdeadbeef,
                 "str", "STR", "ZTR", span, &n);

  /* Test %Q with special characters */
  az_span_printf((const uint8_t *)"%Q\n", AZ_SPAN_FROM_STR("\"\t\n\\\'"));

  /* Test %Q with control characters */
  {
    uint8_t ctrl_buf[] = {1, 2, 3, 127};
    az_span ctrl_span = az_span_create(ctrl_buf, sizeof(ctrl_buf));
    az_span_printf((const uint8_t *)"%Q\n", ctrl_span);
  }

  /* Test % with no character after */
  az_span_printf((const uint8_t *)"%\n");

  /* Test default case in switch */
  az_span_printf((const uint8_t *)"%y\n");

  PASS();
}

TEST test_az_span_copy_u8_empty(void) {
  az_span dest = az_span_empty();
#ifndef __CYGWIN__
  az_span result;
  az_span_copy_u8(dest, 'a', &result);
  ASSERT_EQ(0, az_span_size(result));
#endif
  PASS();
}

TEST test_az_span_url_encode_all_chars(void) {
  uint8_t buf[100];
  az_span dest = az_span_create(buf, 100);
  az_span src = AZ_SPAN_FROM_STR("-._~");
  ptrdiff_t out_len;
  ASSERT_EQ(AZ_OK, _az_span_url_encode(dest, src, &out_len));
  ASSERT_EQ(4, out_len);
  ASSERT_MEM_EQ("-._~", buf, 4);

  /* Test small dest in url_encode.
     Destination size (2) is >= source size (2), so it passes precondition.
     But encoded "  " needs 6 bytes (2 * 3), so it should return
     AZ_ERROR_NOT_ENOUGH_SPACE. */
  dest = az_span_create(buf, 2);
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE,
            _az_span_url_encode(dest, AZ_SPAN_FROM_STR("  "), &out_len));

  /* "a" needs 1 byte, destination is 2 bytes, so it SHOULD WORK. */
  ASSERT_EQ(AZ_OK, _az_span_url_encode(dest, AZ_SPAN_FROM_STR("a"), &out_len));

#ifndef __CYGWIN__
  dest = az_span_create(buf, 0);
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE,
            _az_span_url_encode(dest, AZ_SPAN_FROM_STR("a"), &out_len));
#endif

  PASS();
}

TEST test_az_span_is_content_equal_all_paths(void) {
  az_span s1 = AZ_SPAN_FROM_STR("a");
  az_span s2 = AZ_SPAN_FROM_STR("a");
  az_span s3 = AZ_SPAN_FROM_STR("b");
  az_span s4 = az_span_empty();

  ASSERT(az_span_is_content_equal(s1, s2));
  ASSERT(!az_span_is_content_equal(s1, s3));
  ASSERT(!az_span_is_content_equal(s1, s4));
  ASSERT(az_span_is_content_equal(s4, s4));
  PASS();
}

TEST test_az_isfinite_false(void) {
  uint64_t inf = 0x7FF0000000000000ULL;
  double d_inf;
#if defined(_MSC_VER) && !defined(__clang__)
  memcpy_s(&d_inf, sizeof(d_inf), &inf, sizeof(inf));
#else
  memcpy(&d_inf, &inf, sizeof(d_inf));
#endif
  ASSERT(!_az_isfinite(d_inf));
  PASS();
}

TEST test_az_is_expected_span_fail(void) {
  az_span s = AZ_SPAN_FROM_STR("abc");
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            _az_is_expected_span(&s, AZ_SPAN_FROM_STR("abd")));

  /* Cover line 246 of c_str_span.h when called via _az_is_expected_span in
   * c_str_span.c */
  {
    az_span s2 = AZ_SPAN_FROM_STR("abc");
    ASSERT_EQ(AZ_OK, _az_is_expected_span(&s2, az_span_empty()));
    ASSERT_EQ(3, az_span_size(s2));
  }

  PASS();
}

TEST test_c_str_span_log_debug(void) {
  c_str_span_log_debug("Test %d\n", 1);
  PASS();
}

TEST test_precondition_default_callback(void) {
  az_precondition_failed_fn cb = az_precondition_failed_get_callback();
  if (cb) {
    cb();
  }
  PASS();
}

TEST test_az_span_overlap_null(void) {
  az_span empty = az_span_empty();
  az_span valid = AZ_SPAN_FROM_STR("valid");
  ASSERT(!_az_span_overlap(empty, valid));
  ASSERT(!_az_span_overlap(valid, empty));
  ASSERT(!_az_span_overlap(empty, empty));
  PASS();
}

SUITE(coverage_suite) {
  printf("1\n");
  RUN_TEST(test_c_str_span_log_debug);
  printf("2\n");
  RUN_TEST(test_precondition_default_callback);
  RUN_TEST(test_az_span_overlap_null);
  RUN_TEST(test_az_span_create_from_str_null);
  printf("3\n");
  RUN_TEST(test_az_span_create_null_size_non_zero);
  printf("4\n");
  RUN_TEST(test_az_span_atox_empty);
  printf("5\n");
  RUN_TEST(test_az_span_copy_small_destination);
  printf("6\n");
  RUN_TEST(test_az_span_to_str_small_buffer);
  printf("7\n");
  RUN_TEST(test_az_span_dtoa_special_cases);
  printf("8\n");
  RUN_TEST(test_az_span_copy_u8_empty);
  printf("9\n");
  RUN_TEST(test_az_span_url_encode_all_chars);
  printf("10\n");
  RUN_TEST(test_az_span_is_content_equal_all_paths);
  printf("11\n");
  RUN_TEST(test_az_isfinite_false);
  printf("12\n");
  RUN_TEST(test_az_is_expected_span_fail);
  printf("13\n");
  RUN_TEST(test_az_span_printf_all_specifiers);
}
