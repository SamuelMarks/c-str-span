#ifndef C_STR_SPAN_STR_SPAN_H
#define C_STR_SPAN_STR_SPAN_H

/* Copyright (c) Microsoft Corporation. All rights reserved.
 * SPDX-License-Identifier: MIT
 * Copyright (c) Offscale.io. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

/**
 * @file
 *
 * @brief An #az_span represents a contiguous byte buffer and is used for string
 * manipulations, HTTP requests/responses, reading/writing JSON payloads, and
 * more.
 *
 * @note You MUST NOT use any symbols (macros, functions, structures, enums,
 * etc.) prefixed with an underscore ('_') directly in your application code.
 * These symbols are part of Azure SDK's internal implementation; we do not
 * document these symbols and they are subject to change in future versions of
 * the SDK which would break your code.
 */

/*#include <azure/core/az_result.h>*/

#ifdef __cplusplus
#include <cstddef>
#include <cstdint>
#include <cstring>
extern "C" {
#elif __STDC_VERSION__ >= 199901L
#include <stdbool.h>
#else
#include "c_str_span_stdbool.h"
#endif /* __cplusplus */

#ifndef __cplusplus
#include <stddef.h>
#include <string.h>
#if defined(_MSC_VER) && _MSC_VER < 1600
#include "c_str_span_stdint.h"
#else
#include <stdint.h>
#endif /* defined(_MSC_VER) && _MSC_VER < 1600 */
#endif /* ! __cplusplus */

#include "c_str_result.h"
#include "c_str_span_export.h"
#include "c_str_span_types.h"

/*#include <azure/core/_az_cfg_prefix.h>*/

/**
 * @brief Represents a "view" over a byte buffer that represents a contiguous
 * region of memory. It contains a pointer to the start of the byte buffer and
 * the buffer's size.
 */
typedef struct {
  struct {
    uint8_t *ptr;
    size_t size; // size must be >= 0
  } _internal;
} az_span;

/********************************  SPAN GETTERS */

/**
 * @brief Returns the #az_span byte buffer's starting memory address.
 * @param[in] span The #az_span whose starting memory address to return.
 * @return Starting memory address of \p span buffer.
 */
AZ_NODISCARD AZ_INLINE uint8_t *az_span_ptr(az_span span) {
  return span._internal.ptr;
}

/**
 * @brief Returns the number of bytes within the #az_span.
 * @param[in] span The #az_span whose size to return.
 * @return Size of \p span buffer.
 */
AZ_NODISCARD AZ_INLINE size_t az_span_size(az_span span) {
  return span._internal.size;
}

/********************************  CONSTRUCTORS */

/**
 * @brief Returns an #az_span over a byte buffer.
 *
 * @param[in] ptr The memory address of the first byte in the byte buffer.
 * @param[in] size The total number of bytes in the byte buffer.
 *
 * @return The "view" over the byte buffer.
 */
/* Note: If you are modifying this function, make sure to modify the non-inline
 * version in the */
/* az_span.c file as well, and the _az_ version right below. */
#ifdef AZ_NO_PRECONDITION_CHECKING
AZ_NODISCARD AZ_INLINE az_span az_span_create(uint8_t *ptr, size_t size) {
  return (az_span){._internal = {.ptr = ptr, .size = size}};
}
#else
extern C_STR_SPAN_EXPORT AZ_NODISCARD az_span az_span_create(uint8_t *ptr,
                                                             size_t size);
#endif /* AZ_NO_PRECONDITION_CHECKING */

/* Returns the size (in bytes) of a literal string. */
/* Note: Concatenating "" to S produces a compiler error if S is not a literal
 * string */
/* The stored string's length does not include the \0 terminator. */
#define _az_STRING_LITERAL_LEN(S) (sizeof(S "") - 1)

/**
 * @brief Returns an #az_span expression over a literal string.
 *
 * For example:
 *
 * `some_function(AZ_SPAN_FROM_STR("Hello world"));`
 *
 * where
 *
 * `void some_function(const az_span span);`
 *
 */
#define AZ_SPAN_FROM_STR(STRING_LITERAL)                                       \
  az_span_create_from_str_of_size(STRING_LITERAL,                              \
                                  _az_STRING_LITERAL_LEN(STRING_LITERAL))

/* Returns 1 if the address of the array is equal to the address of its 1st
 * element. */
/* Returns 0 for anything that is not an array (for example any arbitrary
 * pointer). */
#define _az_IS_ARRAY(array) (((void *)&(array)) == ((void *)(&(array)[0])))

/* Returns 1 if the element size of the array is 1 (which is only true for byte
 * arrays such as */
/* uint8_t[] and char[]). */
/* Returns 0 for any other element size (for example int32_t[]). */
#define _az_IS_BYTE_ARRAY(array)                                               \
  ((sizeof((array)[0]) == 1) && _az_IS_ARRAY(array))

/**
 * @brief Returns an #az_span expression over an uninitialized byte buffer.
 *
 * For example:
 *
 * `uint8_t buffer[1024];`
 *
 * `some_function(AZ_SPAN_FROM_BUFFER(buffer));  // Size = 1024`
 *
 * @remarks BYTE_BUFFER MUST be an array defined like `uint8_t buffer[10];` and
 * not `uint8_t* buffer`
 */
/* Force a division by 0 that gets detected by compilers for anything that isn't
 * a byte array. */
#define AZ_SPAN_FROM_BUFFER(BYTE_BUFFER)                                       \
  az_span_create(                                                              \
      (uint8_t *)(BYTE_BUFFER),                                                \
      (sizeof(BYTE_BUFFER) / (_az_IS_BYTE_ARRAY(BYTE_BUFFER) ? 1 : 0)))

/**
 * @brief An empty #az_span.
 *
 * @remark There is no guarantee that the pointer backing this span will be
 * `NULL` and the caller shouldn't rely on it. However, the size will be 0.
 */
extern C_STR_SPAN_EXPORT AZ_NODISCARD az_span az_span_empty(void);

/**
 * @brief Returns an #az_span from a 0-terminated array of bytes (chars).
 *
 * @param[in] str The pointer to the 0-terminated array of bytes (chars).
 *
 * @return An #az_span over the byte buffer where the size is set to the
 * string's length not including the `\0` terminator.
 */
extern C_STR_SPAN_EXPORT AZ_NODISCARD az_span
az_span_create_from_str(char *str);

/**
 * @brief Returns an #az_span from a length-provided array of bytes (chars).
 *
 * @param[in] str The pointer to the array of bytes (chars).
 * @param[in] size Length
 *
 * @return An #az_span over the byte buffer where the size is set to the
 * provided size.
 */
extern C_STR_SPAN_EXPORT AZ_NODISCARD az_span
az_span_create_from_str_of_size(char *str, size_t size);

/******************************  SPAN MANIPULATION */

/**
 * @brief Returns a new #az_span which is a sub-span of the specified \p span.
 *
 * @param[in] span The original #az_span.
 * @param[in] start_index An index into the original #az_span indicating where
 * the returned #az_span will start.
 * @param[in] end_index An index into the original #az_span indicating where the
 * returned #az_span should stop. The byte at the end_index is NOT included in
 * the returned #az_span.
 *
 * @return An #az_span into a portion (from \p start_index to \p end_index - 1)
 * of the original #az_span.
 */
extern C_STR_SPAN_EXPORT AZ_NODISCARD az_span az_span_slice(az_span span,
                                                            size_t start_index,
                                                            size_t end_index);

/**
 * @brief Returns a new #az_span which is a sub-span of the specified \p span.
 *
 * @param[in] span The original #az_span.
 * @param[in] start_index An index into the original #az_span indicating where
 * the returned #az_span will start.
 *
 * @return An #az_span into a portion (from \p start_index to the size) of the
 * original #az_span.
 */
extern C_STR_SPAN_EXPORT AZ_NODISCARD az_span
az_span_slice_to_end(az_span span, size_t start_index);

/**
 * @brief Determines whether two spans are equal by comparing their bytes.
 *
 * @param[in] span1 The first #az_span to compare.
 * @param[in] span2 The second #az_span to compare.
 *
 * @return `true` if the sizes of both spans are identical and the bytes in both
 * spans are also identical. Otherwise, `false`.
 */
AZ_UNUSED AZ_NODISCARD AZ_INLINE bool az_span_is_content_equal(az_span span1,
                                                               az_span span2) {
  size_t const span1_size = az_span_size(span1);
  size_t const span2_size = az_span_size(span2);

  /* Make sure to avoid passing a null pointer to memcmp, which is considered
   * undefined. */
  /* We assume that if the size is non-zero, then the pointer can't be null. */
  if (span1_size == 0) {
    /* Two empty spans are considered equal, even if their pointers are
     * different. */
    return span2_size == 0;
  }

  /* If span2_size == 0, then the first condition which compares sizes will be
   * false, since we */
  /* checked the size of span1 above. And due to short-circuiting we won't be
   * calling memcmp anyway. */
  /* Therefore, we don't need to check for that explicitly. */
  return span1_size == span2_size &&
         memcmp(az_span_ptr(span1), az_span_ptr(span2), span1_size) == 0;
}

/**
 * @brief Determines whether two spans are equal by comparing their characters,
 * except for casing.
 *
 * @param[in] span1 The first #az_span to compare.
 * @param[in] span2 The second #az_span to compare.
 *
 * @return `true` if the sizes of both spans are identical and the ASCII
 * characters in both spans are also identical, except for casing.
 *
 * @remarks This function assumes the bytes in both spans are ASCII characters.
 */
extern C_STR_SPAN_EXPORT AZ_NODISCARD AZ_UNUSED bool
az_span_is_content_equal_ignoring_case(az_span span1, az_span span2);

/**
 * @brief Copies a \p source #az_span containing a string (that is not
 0-terminated) to a \p destination char buffer and appends the 0-terminating
 byte.
 *
 * @param destination A pointer to a buffer where the string should be copied
 into.
 * @param[in] destination_max_size The maximum available space within the buffer
 referred to by
 * \p destination.
 * @param[in] source The #az_span containing the not-0-terminated string to copy
 into \p destination.
 *
 * @remarks The buffer referred to by \p destination must have a size that is at
 least 1 byte bigger
 * than the \p source #az_span for the \p destination string to be
 zero-terminated.
 * Content is copied from the \p source buffer and then `\0` is added at the
 end.
 */
extern C_STR_SPAN_EXPORT void
az_span_to_str(char *destination, size_t destination_max_size, az_span source);

/**
 * @brief Searches for \p target in \p source, returning an #az_span within \p
 * source if it finds it.
 *
 * @param[in] source The #az_span with the content to be searched on.
 * @param[in] target The #az_span containing the tokens to be searched within \p
 * source.
 *
 * @return The position of \p target in \p source if \p source contains the \p
 * target within it.
 * @retval 0 \p target is empty (if its size is equal zero).
 * @retval -1 \p target is not found in `source` OR \p source is empty (if its
 * size is zero) and \p target is non-empty.
 * @retval >=0 The position of \p target in \p source.
 */
extern C_STR_SPAN_EXPORT AZ_NODISCARD size_t az_span_find(az_span source,
                                                          az_span target);

/******************************  SPAN COPYING */

/**
 * @brief Copies the content of the \p source #az_span to the \p destination
 * #az_span.
 *
 * @param destination The #az_span whose bytes will be replaced by the bytes
 * from \p source.
 * @param[in] source The #az_span containing the bytes to copy to the
 * destination.
 *
 * @return An #az_span that is a slice of the \p destination #az_span (i.e. the
 * remainder) after the source bytes have been copied.
 *
 * @remarks This function assumes that the \p destination has a large enough
 * size to hold the \p source.
 *
 * @remarks This function copies all of \p source into the \p destination even
 * if they overlap.
 * @remarks If \p source is an empty #az_span or #az_span_empty(), this function
 * will just return
 * \p destination.
 */
extern C_STR_SPAN_EXPORT az_span az_span_copy(az_span destination,
                                              az_span source);

/**
 * @brief Copies the `uint8_t` \p byte to the \p destination at its 0-th index.
 *
 * @param destination The #az_span where the byte should be copied to.
 * @param[in] byte The `uint8_t` to copy into the \p destination span.
 *
 * @return An #az_span that is a slice of the \p destination #az_span (i.e. the
 * remainder) after the
 * \p byte has been copied.
 *
 * @remarks The function assumes that the \p destination has a large enough size
 * to hold one more byte.
 */
extern C_STR_SPAN_EXPORT az_span az_span_copy_u8(az_span destination,
                                                 uint8_t byte);

/**
 * @brief Fills all the bytes of the \p destination #az_span with the specified
 * value.
 *
 * @param destination The #az_span whose bytes will be set to \p value.
 * @param[in] value The byte to be replicated within the destination #az_span.
 */
AZ_UNUSED AZ_INLINE void az_span_fill(az_span destination, uint8_t value) {
  /* NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
   */
  memset(az_span_ptr(destination), value, az_span_size(destination));
}

/******************************  SPAN PARSING AND FORMATTING */

/**
 * @brief Parses an #az_span containing ASCII digits into a `uint64_t` number.
 *
 * @param[in] source The #az_span containing the ASCII digits to be parsed.
 * @param[out] out_number The pointer to the variable that is to receive the
 * number.
 *
 * @return An #az_result value indicating the result of the operation.
 * @retval #AZ_OK Success.
 * @retval #AZ_ERROR_UNEXPECTED_CHAR A non-ASCII digit is found within the span
 * or the \p source contains a number that would overflow or underflow
 * `uint64_t`.
 */
extern C_STR_SPAN_EXPORT AZ_NODISCARD az_result
az_span_atou64(az_span source, uint64_t *out_number);

/**
 * @brief Parses an #az_span containing ASCII digits into an `int64_t` number.
 *
 * @param[in] source The #az_span containing the ASCII digits to be parsed.
 * @param[out] out_number The pointer to the variable that is to receive the
 * number.
 *
 * @return An #az_result value indicating the result of the operation.
 * @retval #AZ_OK Success.
 * @retval #AZ_ERROR_UNEXPECTED_CHAR A non-ASCII digit is found within the span
 * or the \p source contains a number that would overflow or underflow
 * `int64_t`.
 */
extern C_STR_SPAN_EXPORT AZ_NODISCARD az_result
az_span_atoi64(az_span source, int64_t *out_number);

/**
 * @brief Parses an #az_span containing ASCII digits into a `uint32_t` number.
 *
 * @param[in] source The #az_span containing the ASCII digits to be parsed.
 * @param[out] out_number The pointer to the variable that is to receive the
 * number.
 *
 * @return An #az_result value indicating the result of the operation.
 * @retval #AZ_OK Success.
 * @retval #AZ_ERROR_UNEXPECTED_CHAR A non-ASCII digit is found within the span
 * or the \p source contains a number that would overflow or underflow
 * `uint32_t`.
 */
extern C_STR_SPAN_EXPORT AZ_NODISCARD az_result
az_span_atou32(az_span source, uint32_t *out_number);

/**
 * @brief Parses an #az_span containing ASCII digits into an `int32_t` number.
 *
 * @param[in] source The #az_span containing the ASCII digits to be parsed.
 * @param[out] out_number The pointer to the variable that is to receive the
 * number.
 *
 * @return An #az_result value indicating the result of the operation.
 * @retval #AZ_OK Success.
 * @retval #AZ_ERROR_UNEXPECTED_CHAR A non-ASCII digit is found within the span
 * or if the \p source contains a number that would overflow or underflow
 * `int32_t`.
 */
extern C_STR_SPAN_EXPORT AZ_NODISCARD az_result
az_span_atoi32(az_span source, int32_t *out_number);

/**
 * @brief Parses an #az_span containing ASCII digits into a `double` number.
 *
 * @param[in] source The #az_span containing the ASCII digits to be parsed.
 * @param[out] out_number The pointer to the variable that is to receive the
 * number.
 *
 * @return An #az_result value indicating the result of the operation.
 * @retval #AZ_OK Success.
 * @retval #AZ_ERROR_UNEXPECTED_CHAR A non-ASCII digit or an invalid character
 * is found within the span, or the resulting \p out_number wouldn't be a finite
 * `double` number.
 *
 * @remark The #az_span being parsed must contain a number that is finite.
 * Values such as `NaN`, `INFINITY`, and those that would overflow a `double` to
 * `+/-inf` are not allowed.
 */
extern C_STR_SPAN_EXPORT AZ_NODISCARD az_result
az_span_atod(az_span source, double *out_number);

/**
 * @brief Converts an `int32_t` into its digit characters (base 10) and copies
 * them to the \p destination #az_span starting at its 0-th index.
 *
 * @param destination The #az_span where the bytes should be copied to.
 * @param[in] source The `int32_t` whose number is copied to the \p destination
 * #az_span as ASCII digits.
 * @param[out] out_span A pointer to an #az_span that receives the remainder of
 * the \p destination #az_span after the `int32_t` has been copied.
 *
 * @return An #az_result value indicating the result of the operation.
 * @retval #AZ_OK Success.
 * @retval #AZ_ERROR_NOT_ENOUGH_SPACE The \p destination is not big enough to
 * contain the copied bytes.
 */
extern C_STR_SPAN_EXPORT AZ_NODISCARD az_result
az_span_i32toa(az_span destination, int32_t source, az_span *out_span);

/**
 * @brief Converts an `uint32_t` into its digit characters (base 10) and copies
 * them to the \p destination #az_span starting at its 0-th index.
 *
 * @param destination The #az_span where the bytes should be copied to.
 * @param[in] source The `uint32_t` whose number is copied to the \p destination
 * #az_span as ASCII digits.
 * @param[out] out_span A pointer to an #az_span that receives the remainder of
 * the \p destination #az_span after the `uint32_t` has been copied.
 *
 * @return An #az_result value indicating the result of the operation.
 * @retval #AZ_OK Success.
 * @retval #AZ_ERROR_NOT_ENOUGH_SPACE The \p destination is not big enough to
 * contain the copied bytes.
 */
extern C_STR_SPAN_EXPORT AZ_NODISCARD az_result
az_span_u32toa(az_span destination, uint32_t source, az_span *out_span);

/**
 * @brief Converts an `int64_t` into its digit characters (base 10) and copies
 * them to the \p destination #az_span starting at its 0-th index.
 *
 * @param destination The #az_span where the bytes should be copied to.
 * @param[in] source The `int64_t` whose number is copied to the \p destination
 * #az_span as ASCII digits.
 * @param[out] out_span A pointer to an #az_span that receives the remainder of
 * the \p destination #az_span after the `int64_t` has been copied.
 *
 * @return An #az_result value indicating the result of the operation.
 * @retval #AZ_OK Success.
 * @retval #AZ_ERROR_NOT_ENOUGH_SPACE The \p destination is not big enough to
 * contain the copied bytes.
 */
extern C_STR_SPAN_EXPORT AZ_NODISCARD az_result
az_span_i64toa(az_span destination, int64_t source, az_span *out_span);

/**
 * @brief Converts a `uint64_t` into its digit characters (base 10) and copies
 * them to the \p destination #az_span starting at its 0-th index.
 *
 * @param destination The #az_span where the bytes should be copied to.
 * @param[in] source The `uint64_t` whose number is copied to the \p destination
 * #az_span as ASCII digits.
 * @param[out] out_span A pointer to an #az_span that receives the remainder of
 * the \p destination #az_span after the `uint64_t` has been copied.
 *
 * @return An #az_result value indicating the result of the operation.
 * @retval #AZ_OK Success.
 * @retval #AZ_ERROR_NOT_ENOUGH_SPACE The \p destination is not big enough to
 * contain the copied bytes.
 */
extern C_STR_SPAN_EXPORT AZ_NODISCARD az_result
az_span_u64toa(az_span destination, uint64_t source, az_span *out_span);

/**
 * @brief Converts a `double` into its digit characters (base 10 decimal
 * notation) and copies them to the \p destination #az_span starting at its 0-th
 * index.
 *
 * @param destination The #az_span where the bytes should be copied to.
 * @param[in] source The `double` whose number is copied to the \p destination
 * #az_span as ASCII digits and characters.
 * @param[in] fractional_digits The number of digits to write into the \p
 * destination #az_span after the decimal point and truncate the rest.
 * @param[out] out_span A pointer to an #az_span that receives the remainder of
 * the \p destination #az_span after the `double` has been copied.
 *
 * @return An #az_result value indicating the result of the operation.
 * @retval #AZ_OK Success.
 * @retval #AZ_ERROR_NOT_ENOUGH_SPACE The \p destination is not big enough to
 * contain the copied bytes.
 * @retval #AZ_ERROR_NOT_SUPPORTED The \p source is not a finite decimal number
 * or contains an integer component that is too large and would overflow beyond
 * `2^53 - 1`.
 *
 * @remark Only finite `double` values are supported. Values such as `NaN` and
 * `INFINITY` are not allowed.
 *
 * @remark Non-significant trailing zeros (after the decimal point) are not
 * written, even if \p fractional_digits is large enough to allow the zero
 * padding.
 *
 * @remark The \p fractional_digits must be between 0 and 15 (inclusive). Any
 * value passed in that is larger will be clamped down to 15.
 */
extern C_STR_SPAN_EXPORT AZ_NODISCARD az_result
az_span_dtoa(az_span destination, double source, int32_t fractional_digits,
             az_span *out_span);

/* @brief Checks if char (uint8_t) is alphabetical or numeric (A-z|0-9)
   @remark Locale isn't considered, this is implemented as a simple switch/case
 */
AZ_UNUSED AZ_NODISCARD AZ_INLINE bool uint8_t_isalnum(const uint8_t c) {
  switch (c) {
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
  case 'a':
  case 'b':
  case 'c':
  case 'd':
  case 'e':
  case 'f':
  case 'g':
  case 'h':
  case 'i':
  case 'j':
  case 'k':
  case 'l':
  case 'm':
  case 'n':
  case 'o':
  case 'p':
  case 'q':
  case 'r':
  case 's':
  case 't':
  case 'u':
  case 'v':
  case 'w':
  case 'x':
  case 'y':
  case 'z':
  case 'A':
  case 'B':
  case 'C':
  case 'D':
  case 'E':
  case 'F':
  case 'G':
  case 'H':
  case 'I':
  case 'J':
  case 'K':
  case 'L':
  case 'M':
  case 'N':
  case 'O':
  case 'P':
  case 'Q':
  case 'R':
  case 'S':
  case 'T':
  case 'U':
  case 'V':
  case 'W':
  case 'X':
  case 'Y':
  case 'Z':
    break;
  default:
    return false;
  }
  return true;
}

/* @brief Checks if str (az_span) is alphabetical or numeric (A-z|0-9)
   @remark An empty str will return true
   @remark Locale isn't considered, this is implemented as a simple switch/case
 */
AZ_UNUSED AZ_NODISCARD AZ_INLINE bool az_span_isalnum(const az_span span) {
  const size_t size = az_span_size(span);
  size_t i;
  for (i = 0; i < size; i++) {
    switch (az_span_ptr(span)[i]) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
    case 'g':
    case 'h':
    case 'i':
    case 'j':
    case 'k':
    case 'l':
    case 'm':
    case 'n':
    case 'o':
    case 'p':
    case 'q':
    case 'r':
    case 's':
    case 't':
    case 'u':
    case 'v':
    case 'w':
    case 'x':
    case 'y':
    case 'z':
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
    case 'G':
    case 'H':
    case 'I':
    case 'J':
    case 'K':
    case 'L':
    case 'M':
    case 'N':
    case 'O':
    case 'P':
    case 'Q':
    case 'R':
    case 'S':
    case 'T':
    case 'U':
    case 'V':
    case 'W':
    case 'X':
    case 'Y':
    case 'Z':
      break;
    default:
      return false;
    }
  }
  return true;
}

/* @brief Checks if char (uint8_t) is alphabetical (A-z)
 * @remark Locale isn't considered, this is implemented as a simple switch/case
 */
AZ_UNUSED AZ_NODISCARD AZ_INLINE bool uint8_t_isalpha(const uint8_t c) {
  switch (c) {
  case 'a':
  case 'b':
  case 'c':
  case 'd':
  case 'e':
  case 'f':
  case 'g':
  case 'h':
  case 'i':
  case 'j':
  case 'k':
  case 'l':
  case 'm':
  case 'n':
  case 'o':
  case 'p':
  case 'q':
  case 'r':
  case 's':
  case 't':
  case 'u':
  case 'v':
  case 'w':
  case 'x':
  case 'y':
  case 'z':
  case 'A':
  case 'B':
  case 'C':
  case 'D':
  case 'E':
  case 'F':
  case 'G':
  case 'H':
  case 'I':
  case 'J':
  case 'K':
  case 'L':
  case 'M':
  case 'N':
  case 'O':
  case 'P':
  case 'Q':
  case 'R':
  case 'S':
  case 'T':
  case 'U':
  case 'V':
  case 'W':
  case 'X':
  case 'Y':
  case 'Z':
    break;
  default:
    return false;
  }
  return true;
}

/* @brief Checks if str (az_span) is alphabetical (A-z)
 * @remark An empty str will return true
 * @remark Locale isn't considered, this is implemented as a simple switch/case
 */
AZ_UNUSED AZ_NODISCARD AZ_INLINE bool az_span_isalpha(const az_span span) {
  size_t i;
  for (i = 0; i < az_span_size(span); i++) {
    switch (az_span_ptr(span)[i]) {
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
    case 'g':
    case 'h':
    case 'i':
    case 'j':
    case 'k':
    case 'l':
    case 'm':
    case 'n':
    case 'o':
    case 'p':
    case 'q':
    case 'r':
    case 's':
    case 't':
    case 'u':
    case 'v':
    case 'w':
    case 'x':
    case 'y':
    case 'z':
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
    case 'G':
    case 'H':
    case 'I':
    case 'J':
    case 'K':
    case 'L':
    case 'M':
    case 'N':
    case 'O':
    case 'P':
    case 'Q':
    case 'R':
    case 'S':
    case 'T':
    case 'U':
    case 'V':
    case 'W':
    case 'X':
    case 'Y':
    case 'Z':
      break;
    default:
      return false;
    }
  }
  return true;
}

/* @brief Checks if char (uint8_t) is blank */
AZ_UNUSED AZ_NODISCARD AZ_INLINE bool uint8_t_isblank(const uint8_t c) {
  switch (c) {
  case ' ':
  case '\t':
    break;
  default:
    return false;
  }
  return true;
}

/* @brief Checks if str (az_span) is blank
 * @remark An empty str will return true
 */
AZ_UNUSED AZ_NODISCARD AZ_INLINE bool az_span_isblank(const az_span span) {
  size_t i;
  for (i = 0; i < az_span_size(span); i++) {
    switch (az_span_ptr(span)[i]) {
    case ' ':
    case '\t':
      break;
    default:
      return false;
    }
  }
  return true;
}

/* @brief Checks if char (uint8_t) only contains digits
 * @remark Doesn't deal with decimals or scientific notation
 */
AZ_UNUSED AZ_NODISCARD AZ_INLINE bool uint8_t_isdigit(const uint8_t c) {
  switch (c) {
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
    break;
  default:
    return false;
  }
  return true;
}

/* @brief Checks if str (az_span) only contains digits
 * @remark An empty str will return true
 * @remark Doesn't deal with decimals or scientific notation
 */
AZ_UNUSED AZ_NODISCARD AZ_INLINE bool az_span_isdigit(const az_span span) {
  size_t i;
  for (i = 0; i < az_span_size(span); i++) {
    switch (az_span_ptr(span)[i]) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      break;
    default:
      return false;
    }
  }
  return true;
}

/* @brief Checks if char (uint8_t) is alphabetical and lowercase (A-Z)
 * @remark Locale isn't considered, this is implemented as a simple switch/case
 */
AZ_UNUSED AZ_NODISCARD AZ_INLINE bool uint8_t_islower(const uint8_t c) {
  switch (c) {
  case 'a':
  case 'b':
  case 'c':
  case 'd':
  case 'e':
  case 'f':
  case 'g':
  case 'h':
  case 'i':
  case 'j':
  case 'k':
  case 'l':
  case 'm':
  case 'n':
  case 'o':
  case 'p':
  case 'q':
  case 'r':
  case 's':
  case 't':
  case 'u':
  case 'v':
  case 'w':
  case 'x':
  case 'y':
  case 'z':
    break;
  default:
    return false;
  }
  return true;
}

/* @brief Checks if str (az_span) is alphabetical and lowercase (A-Z)
 * @remark An empty str will return true
 * @remark Locale isn't considered, this is implemented as a simple switch/case
 */
AZ_UNUSED AZ_NODISCARD AZ_INLINE bool az_span_islower(const az_span span) {
  size_t i;
  for (i = 0; i < az_span_size(span); i++) {
    /* not using `uint8_t_islower` function as that adds a branch */
    switch (az_span_ptr(span)[i]) {
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
    case 'g':
    case 'h':
    case 'i':
    case 'j':
    case 'k':
    case 'l':
    case 'm':
    case 'n':
    case 'o':
    case 'p':
    case 'q':
    case 'r':
    case 's':
    case 't':
    case 'u':
    case 'v':
    case 'w':
    case 'x':
    case 'y':
    case 'z':
      break;
    default:
      return false;
    }
  }
  return true;
}

/* @brief Checks if char (uint8_t) is alphabetical and lowercase (A-Z)
 * @remark Locale isn't considered, this is implemented as a simple switch/case
 */
AZ_UNUSED AZ_NODISCARD AZ_INLINE bool uint8_t_isupper(const uint8_t c) {
  switch (c) {
  case 'A':
  case 'B':
  case 'C':
  case 'D':
  case 'E':
  case 'F':
  case 'G':
  case 'H':
  case 'I':
  case 'J':
  case 'K':
  case 'L':
  case 'M':
  case 'N':
  case 'O':
  case 'P':
  case 'Q':
  case 'R':
  case 'S':
  case 'T':
  case 'U':
  case 'V':
  case 'W':
  case 'X':
  case 'Y':
  case 'Z':
    break;
  default:
    return false;
  }
  return true;
}

/* @brief Checks if str (az_span) is alphabetical and lowercase (A-Z)
 * @remark An empty str will return true
 * @remark Locale isn't considered, this is implemented as a simple switch/case
 */
AZ_UNUSED AZ_NODISCARD AZ_INLINE bool az_span_isupper(const az_span span) {
  size_t i;
  for (i = 0; i < az_span_size(span); i++) {
    switch (az_span_ptr(span)[i]) {
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
    case 'G':
    case 'H':
    case 'I':
    case 'J':
    case 'K':
    case 'L':
    case 'M':
    case 'N':
    case 'O':
    case 'P':
    case 'Q':
    case 'R':
    case 'S':
    case 'T':
    case 'U':
    case 'V':
    case 'W':
    case 'X':
    case 'Y':
    case 'Z':
      break;
    default:
      return false;
    }
  }
  return true;
}

/******************************  NON-CONTIGUOUS SPAN  */

/**
 * @brief Defines a container of required and user-defined fields that provide
 * the necessary information and parameters for the implementation of the
 * #az_span_allocator_fn callback.
 */
typedef struct {
  /* / Any struct that was provided by the user for their specific
   * implementation, passed through to */
  /* / the #az_span_allocator_fn. */
  void *user_context;

  /* / The amount of space consumed (i.e. written into) within the previously
   * provided destination, */
  /* / which can be used to infer the remaining number of bytes of the #az_span
   * that are leftover. */
  int32_t bytes_used;

  /* / The minimum length of the destination #az_span required to be provided by
   * the callback. If 0, */
  /* / any non-empty sized buffer must be returned. */
  size_t minimum_required_size;
} az_span_allocator_context;

/**
 * @brief Defines the signature of the callback function that the caller must
 * implement to provide the potentially discontiguous destination buffers where
 * output can be written into.
 *
 * @param[in] allocator_context A container of required and user-defined fields
 * that provide the necessary information and parameters for the implementation
 * of the callback.
 * @param[out] out_next_destination A pointer to an #az_span that can be used as
 * a destination to write data into, that is at least the required size
 * specified within the \p allocator_context.
 *
 * @return An #az_result value indicating the result of the operation.
 * @retval #AZ_OK Success.
 * @retval other Failure.
 *
 * @remarks The caller must no longer hold onto, use, or write to the previously
 * provided #az_span after this allocator returns a new destination #az_span.
 *
 * @remarks There is no guarantee that successive calls will return the same or
 * same-sized buffer. This function must never return an empty #az_span, unless
 * the requested buffer size is not available. In which case, it must return an
 * error #az_result.
 *
 * @remarks The caller must check the return value using #az_result_failed()
 * before continuing to use the \p out_next_destination.
 */
typedef az_result (*az_span_allocator_fn)(
    az_span_allocator_context *allocator_context,
    az_span *out_next_destination);

/*#include <azure/core/_az_cfg_suffix.h>*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !C_STR_SPAN_STR_SPAN_H */
