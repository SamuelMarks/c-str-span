/**
 * @file
 *
 * @brief Definition of types and macros used across the c_str_span library.
 */

#ifndef C_STR_SPAN_TYPES_H
#define C_STR_SPAN_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* clang-format off */
#if defined(_MSC_VER) && _MSC_VER < 1600
#include "c_str_span_stdint.h"
#else
#include <stdint.h>
#endif /* defined(_MSC_VER) && _MSC_VER < 1600 */

#include "c_str_span_stdbool.h"
/* clang-format on */

/* Copyright (c) Microsoft Corporation. All rights reserved.
 * SPDX-License-Identifier: MIT */

/**
 * @brief Enforce that the return value is handled (only applicable on supported
 * compilers).
 */
#if defined(_MSC_VER)
#define AZ_UNUSED
#if _MSC_VER >= 1700
/** @brief Required to use return value. */
#define AZ_NODISCARD _Check_return_
#else
/** @brief Required to use return value. */
#define AZ_NODISCARD
#endif
#elif defined(__GNUC__) || defined(__clang__) /* !_MSC_VER */
#define AZ_UNUSED __attribute__((unused))
/** @brief Required to use return value. */
#define AZ_NODISCARD __attribute__((warn_unused_result))
#else /* !_MSC_VER !__GNUC__ !__clang__ */
#define AZ_UNUSED
/** @brief Required to use return value. */
#define AZ_NODISCARD
#endif /* _MSC_VER */

/**
 * @brief Inline function.
 */
#ifdef _MSC_VER
#define AZ_INLINE static __forceinline
#elif defined(__GNUC__) || defined(__clang__) /* !_MSC_VER */
#if __STDC_VERSION__ >= 199901L
#define AZ_INLINE __attribute__((always_inline)) static inline
#else
#define AZ_INLINE __attribute__((always_inline)) static __inline__
#endif
#else /* !_MSC_VER !__GNUC__ !__clang__ */
#define AZ_INLINE static __inline__
#endif /* _MSC_VER */

#ifdef __cplusplus
}
#endif /* __cplusplus */
#if defined(_MSC_VER) || defined(__MINGW32__)
#define C_STR_SPAN_PRId64 "I64d"
#define C_STR_SPAN_PRIu64 "I64u"
#define C_STR_SPAN_PRIx64 "I64x"
#else
#define C_STR_SPAN_PRId64 "lld"
#define C_STR_SPAN_PRIu64 "llu"
#define C_STR_SPAN_PRIx64 "llx"
#endif

#endif /* C_STR_SPAN_TYPES_H */
