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
#include <inttypes.h>
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

#if defined(_MSC_VER)
/** @brief Format specifier for int64_t. */
#define C_STR_SPAN_PRId64 "I64d"
/** @brief Format specifier for uint64_t. */
#define C_STR_SPAN_PRIu64 "I64u"
/** @brief Format specifier for hex uint64_t. */
#define C_STR_SPAN_PRIx64 "I64x"
#elif defined(__LP64__) || defined(_LP64)
/** @brief Format specifier for int64_t. */
#define C_STR_SPAN_PRId64 "ld"
/** @brief Format specifier for uint64_t. */
#define C_STR_SPAN_PRIu64 "lu"
/** @brief Format specifier for hex uint64_t. */
#define C_STR_SPAN_PRIx64 "lx"
#else
/** @brief Format specifier for int64_t. */
#define C_STR_SPAN_PRId64 PRId64
/** @brief Format specifier for uint64_t. */
#define C_STR_SPAN_PRIu64 PRIu64
/** @brief Format specifier for hex uint64_t. */
#define C_STR_SPAN_PRIx64 PRIx64
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* C_STR_SPAN_TYPES_H */
