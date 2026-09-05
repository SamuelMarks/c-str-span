/* clang-format off */
#define AZ_NO_PRECONDITION_CHECKING
#define C_STR_SPAN_EXPORTS
#define DEBUG 1
#pragma warning(disable: 4273)
#pragma warning(disable: 4286)
#undef C_STR_SPAN_EXPORT
#define C_STR_SPAN_EXPORT
#include "c_str_precondition_internal.h"
#include "c_str_span.h"
#include "c_str_span_internal.h"
#include "c_str_span_log.h"
#include "c_str_span_printf.h"
#include "c_str_span_private.h"

#if defined(__GNUC__) || defined(__clang__)
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
  } while ((void)0, 0)
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

    span = az_span_create_from_const_u8(NULL, 10);
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
#ifndef __CYGWIN__
  uint32_t val_u32;
  int32_t val_i32;
  uint64_t val_u64;
  int64_t val_i64;
  double val_d;
  az_span empty = az_span_empty();
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR, az_span_atou32(empty, &val_u32));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR,
            az_span_atou32(AZ_SPAN_FROM_STR("+"), &val_u32));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR, az_span_atou64(empty, &val_u64));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR, az_span_atoi32(empty, &val_i32));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR, az_span_atoi64(empty, &val_i64));
  ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR, az_span_atod(empty, &val_d));

  /* Test az_span_atod size limit > 99 */
  {
    double val_d_long;
    az_span long_span = AZ_SPAN_FROM_STR(
        "1234567890123456789012345678901234567890123456789012345678901234567890"
        "1234567890123456789012345678901234567890");
    ASSERT_EQ(AZ_ERROR_UNEXPECTED_CHAR, az_span_atod(long_span, &val_d_long));
  }
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
  enum az_result_core rc = az_span_copy(dest, src, &result);
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE, rc);
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
  enum az_result_core rc = az_span_to_str(buf, 3, src);
  ASSERT_EQ(AZ_OK, rc);
  ASSERT_STR_EQ("he", buf);

  /* Test size 0 buffer */
  rc = az_span_to_str(buf, 0, src);
  ASSERT_EQ(AZ_OK, rc);
#endif
  PASS();
}

TEST test_az_span_dtoa_special_cases(void) {
  uint8_t buf[64];
  az_span dest = az_span_create(buf, 64);
  az_span out;
  uint64_t const inf_bits = (((uint64_t)0x7FF00000) << 32);
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
  ASSERT_EQ(AZ_OK,
            az_span_printf(
                (const uint8_t *)"%c %d %i %o %u %x %X %e %E %f %F %g %G %a "
                                 "%A %p %s %S %Z %% %Q %n\n",
                'A', 123, -456, 0777, 456u, 0xabc, 0xABC, 1.23e4, 1.23E4, 1.23,
                1.23, 1.23, 1.23, 1.23, 1.23, (void *)(size_t)0xdeadbeef, "str",
                "STR", "ZTR", span, &n));

  /* Test %Q with special characters */
  ASSERT_EQ(AZ_OK, az_span_printf((const uint8_t *)"%Q\n",
                                  AZ_SPAN_FROM_STR("\"\t\n\\\'")));

  /* Test %Q with control characters */
  {
    uint8_t ctrl_buf[] = {1, 2, 3, 127};
    az_span ctrl_span = az_span_create(ctrl_buf, sizeof(ctrl_buf));
    ASSERT_EQ(AZ_OK, az_span_printf((const uint8_t *)"%Q\n", ctrl_span));
  }

  /* Test % with no character after */
  ASSERT_EQ(AZ_OK, az_span_printf((const uint8_t *)"%\n"));

  /* Test default case in switch */
  ASSERT_EQ(AZ_OK, az_span_printf((const uint8_t *)"%y\n"));

  PASS();
}

TEST test_az_span_copy_u8_empty(void) {
#ifndef __CYGWIN__
  az_span dest = az_span_empty();
  az_span result;
  enum az_result_core rc = az_span_copy_u8(dest, 'a', &result);
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE, rc);
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
  uint64_t inf = (((uint64_t)0x7FF00000) << 32);
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

TEST test_all_errors(void) {
  uint8_t buffer[100];
  az_span b;
  az_span rem;
  /* u64toa */
  b = az_span_create(buffer, 0);
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE, az_span_u64toa(b, 0, &rem));
  b = az_span_create(buffer, 1);
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE, az_span_u64toa(b, 12, &rem));
  b = az_span_create(buffer, 2);
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE, az_span_u64toa(b, 123, &rem));

  /* i64toa */
  b = az_span_create(buffer, 0);
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE, az_span_i64toa(b, -1, &rem));
  b = az_span_create(buffer, 1);
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE, az_span_i64toa(b, -12, &rem));
  b = az_span_create(buffer, 2);
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE, az_span_i64toa(b, -123, &rem));

  /* u32toa */
  b = az_span_create(buffer, 0);
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE, az_span_u32toa(b, 0, &rem));
  b = az_span_create(buffer, 1);
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE, az_span_u32toa(b, 12, &rem));
  b = az_span_create(buffer, 2);
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE, az_span_u32toa(b, 123, &rem));

  /* i32toa */
  b = az_span_create(buffer, 0);
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE, az_span_i32toa(b, -1, &rem));
  b = az_span_create(buffer, 1);
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE, az_span_i32toa(b, -12, &rem));
  b = az_span_create(buffer, 2);
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE, az_span_i32toa(b, -123, &rem));

  /* dtoa */
  b = az_span_create(buffer, 0);
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE, az_span_dtoa(b, -1.2, 2, &rem));
  b = az_span_create(buffer, 1);
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE, az_span_dtoa(b, -1.2, 2, &rem));
  b = az_span_create(buffer, 2);
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE, az_span_dtoa(b, -1.2, 2, &rem));
  b = az_span_create(buffer, 3);
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE, az_span_dtoa(b, -1.2, 2, &rem));
  b = az_span_create(buffer, 4);
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE, az_span_dtoa(b, -1.2, 2, &rem));
  b = az_span_create(buffer, 0);
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE, az_span_dtoa(b, 1.2, 2, &rem));
  b = az_span_create(buffer, 1);
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE, az_span_dtoa(b, 1.2, 2, &rem));
  b = az_span_create(buffer, 2);
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE, az_span_dtoa(b, 1.2, 2, &rem));
  b = az_span_create(buffer, 3);
  ASSERT_EQ(AZ_ERROR_NOT_ENOUGH_SPACE, az_span_dtoa(b, 1.2, 2, &rem));
  PASS();
}

#include "c_str_span_crt_mock.h"

#if defined(_MSC_VER)
int g_mock_memcpy_s_fail = 0;
int g_mock_memmove_s_fail = 0;
int g_mock_sprintf_s_fail = 0;
int g_mock_fprintf_s_fail = 0;
int g_mock_vfprintf_s_fail = 0;

int mock_memcpy_s(void *dest, size_t destsz, const void *src, size_t count) {
  if (g_mock_memcpy_s_fail)
    return -1;
  return (memcpy_s)(dest, destsz, src, count);
}
int mock_memmove_s(void *dest, size_t destsz, const void *src, size_t count) {
  if (g_mock_memmove_s_fail)
    return -1;
  return (memmove_s)(dest, destsz, src, count);
}
int mock_sprintf_s(char *buffer, size_t sizeOfBuffer, const char *format, ...) {
  if (g_mock_sprintf_s_fail)
    return -1;
  {
    va_list args;
    int res;
    va_start(args, format);
    res = vsprintf_s(buffer, sizeOfBuffer, format, args);
    va_end(args);
    return res;
  }
}
int mock_fprintf_s(FILE *stream, const char *format, ...) {
  if (g_mock_fprintf_s_fail)
    return -1;
  {
    va_list args;
    int res;
    va_start(args, format);
    res = vfprintf_s(stream, format, args);
    va_end(args);
    return res;
  }
}
int mock_vfprintf_s(FILE *stream, const char *format, va_list argptr) {
  if (g_mock_vfprintf_s_fail)
    return -1;
  return (vfprintf_s)(stream, format, argptr);
}

#define memcpy_s mock_memcpy_s
#define memmove_s mock_memmove_s
#define sprintf_s mock_sprintf_s
#define fprintf_s mock_fprintf_s
#define vfprintf_s mock_vfprintf_s

#else
int g_mock_memcpy_fail = 0;
int g_mock_memmove_fail = 0;
int g_mock_sprintf_fail = 0;
int g_mock_fprintf_fail = 0;
int g_mock_vfprintf_fail = 0;

void *mock_memcpy(void *dest, const void *src, size_t n) {
  if (g_mock_memcpy_fail)
    return NULL;
  return (memcpy)(dest, src, n);
}
void *mock_memmove(void *dest, const void *src, size_t n) {
  if (g_mock_memmove_fail)
    return NULL;
  return (memmove)(dest, src, n);
}
int mock_sprintf(char *str, const char *format, ...) {
  if (g_mock_sprintf_fail)
    return -1;
  {
    va_list args;
    int res;
    va_start(args, format);
    res = vsprintf(str, format, args);
    va_end(args);
    return res;
  }
}
int mock_fprintf(FILE *stream, const char *format, ...) {
  if (g_mock_fprintf_fail)
    return -1;
  {
    va_list args;
    int res;
    va_start(args, format);
    res = vfprintf(stream, format, args);
    va_end(args);
    return res;
  }
}
int mock_vfprintf(FILE *stream, const char *format, va_list arg) {
  if (g_mock_vfprintf_fail)
    return -1;
  return (vfprintf)(stream, format, arg);
}

#define memcpy mock_memcpy
#define memmove mock_memmove
#define sprintf mock_sprintf
#define fprintf mock_fprintf
#define vfprintf mock_vfprintf
#endif

int g_mock_fputc_fail = 0;
int g_mock_fputs_fail = 0;
int g_mock_modf_fail = 0;
int g_mock_modf_call_count = 0;

int mock_fputc(int character, FILE *stream) {
  if (g_mock_fputc_fail)
    return -1;
  return (fputc)(character, stream);
}
int mock_fputs(const char *str, FILE *stream) {
  if (g_mock_fputs_fail)
    return -1;
  return (fputs)(str, stream);
}
double mock_modf(double x, double *intpart) {
  if (g_mock_modf_fail) {
    if (g_mock_modf_call_count == 1) {
      return -1.0;
    }
    g_mock_modf_call_count++;
  }
  return (modf)(x, intpart);
}

#define fputc mock_fputc
#define fputs mock_fputs
#define modf mock_modf

static void mock_reset_cb(void *data) {
  (void)data;
#if defined(_MSC_VER)
  g_mock_memcpy_s_fail = 0;
  g_mock_memmove_s_fail = 0;
  g_mock_sprintf_s_fail = 0;
  g_mock_fprintf_s_fail = 0;
  g_mock_vfprintf_s_fail = 0;
#else
  g_mock_memcpy_fail = 0;
  g_mock_memmove_fail = 0;
  g_mock_sprintf_fail = 0;
  g_mock_fprintf_fail = 0;
  g_mock_vfprintf_fail = 0;
#endif
  g_mock_fputc_fail = 0;
  g_mock_fputs_fail = 0;
  g_mock_modf_fail = 0;
  g_mock_modf_call_count = 0;
}

#include "../c_str_span.c"
#include "../c_str_span_precondition.c"
#include "../c_str_span_printf.c"

#undef memcpy_s
#undef memmove_s
#undef sprintf_s
#undef fprintf_s
#undef vfprintf_s
#undef memcpy
#undef memmove
#undef sprintf
#undef fprintf
#undef vfprintf
#undef fputc
#undef fputs
#undef modf

TEST test_mock_failures(void) {
  az_span b;
  az_span rem;
  uint8_t buffer[100];

  /* 1. modf failure */
  g_mock_modf_fail = 1;
  b = az_span_create(buffer, 100);
  ASSERT_EQ(AZ_ERROR_ARG, az_span_dtoa(b, 1.23, 2, &rem));
  g_mock_modf_fail = 0;
  g_mock_modf_call_count = 0;

  /* 2. sprintf failure */
#if defined(_MSC_VER)
  g_mock_sprintf_s_fail = 1;
#else
  g_mock_sprintf_fail = 1;
#endif
  b = az_span_create(buffer, 100);
  ASSERT_EQ(AZ_ERROR_ARG, az_span_i64toa(b, 123, &rem));
  ASSERT_EQ(AZ_ERROR_ARG, az_span_u64toa(b, 123, &rem));
  ASSERT_EQ(AZ_ERROR_ARG, az_span_i32toa(b, 123, &rem));
  ASSERT_EQ(AZ_ERROR_ARG, az_span_u32toa(b, 123, &rem));
#if defined(_MSC_VER)
  g_mock_sprintf_s_fail = 0;
#else
  g_mock_sprintf_fail = 0;
#endif

  /* 3. memcpy failure */
#if defined(_MSC_VER)
  g_mock_memcpy_s_fail = 1;
#else
  g_mock_memcpy_fail = 1;
#endif
  b = az_span_create(buffer, 100);
  {
    double out_val;
    ASSERT_EQ(AZ_ERROR_ARG, az_span_atod(AZ_SPAN_FROM_STR("1.23"), &out_val));
  }
#if defined(_MSC_VER)
  g_mock_memcpy_s_fail = 0;
#else
  g_mock_memcpy_fail = 0;
#endif

  /* 4. memmove failure */
#if defined(_MSC_VER)
  g_mock_memmove_s_fail = 1;
#else
  g_mock_memmove_fail = 1;
#endif
  b = az_span_create(buffer, 100);
  ASSERT_EQ(AZ_ERROR_ARG, az_span_copy(b, AZ_SPAN_FROM_STR("123"), &rem));
#if defined(_MSC_VER)
  g_mock_memmove_s_fail = 0;
#else
  g_mock_memmove_fail = 0;
#endif

  /* 5. fprintf failure */
#if defined(_MSC_VER)
  g_mock_fprintf_s_fail = 1;
#else
  g_mock_fprintf_fail = 1;
#endif
  ASSERT_EQ(AZ_ERROR_ARG, c_str_span_log_debug("Hello"));
  ASSERT_EQ(AZ_ERROR_ARG, az_span_printf((const uint8_t *)"%d\n", 123));
#if defined(_MSC_VER)
  g_mock_fprintf_s_fail = 0;
#else
  g_mock_fprintf_fail = 0;
#endif

  /* 6. vfprintf failure */
#if defined(_MSC_VER)
  g_mock_vfprintf_s_fail = 1;
#else
  g_mock_vfprintf_fail = 1;
#endif
  ASSERT_EQ(AZ_ERROR_ARG, c_str_span_log_debug("Hello %d", 123));
#if defined(_MSC_VER)
  g_mock_vfprintf_s_fail = 0;
#else
  g_mock_vfprintf_fail = 0;
#endif

  /* 7. fputc failure */
  g_mock_fputc_fail = 1;
  ASSERT_EQ(AZ_ERROR_ARG, az_span_printf((const uint8_t *)"%c", 'c'));
  ASSERT_EQ(AZ_ERROR_ARG, az_span_printf((const uint8_t *)"abc"));
  g_mock_fputc_fail = 0;

  /* 8. fputs failure */
  g_mock_fputs_fail = 1;
  ASSERT_EQ(AZ_ERROR_ARG, az_span_printf((const uint8_t *)"%s", "abc"));
  g_mock_fputs_fail = 0;

  PASS();
}

SUITE(coverage_suite) {
  SET_SETUP(mock_reset_cb, NULL);
  printf("1\n");
  RUN_TEST(test_all_errors);
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
  RUN_TEST(test_mock_failures);
}
