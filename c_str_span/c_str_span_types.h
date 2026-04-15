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
#ifdef _MSC_VER
#define AZ_UNUSED
#if _MSC_VER >= 1700
#define AZ_NODISCARD _Check_return_
#else
#define AZ_NODISCARD
#endif
#elif defined(__GNUC__) || defined(__clang__) /* !_MSC_VER */
#define AZ_UNUSED __attribute__((unused))
#define AZ_NODISCARD __attribute__((warn_unused_result))
#else /* !_MSC_VER !__GNUC__ !__clang__ */
#define AZ_UNUSED
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
#endif /* C_STR_SPAN_TYPES_H */
