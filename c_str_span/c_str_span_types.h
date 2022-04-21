#ifndef C_STR_SPAN_TYPES_H
#define C_STR_SPAN_TYPES_H

/* Copyright (c) Microsoft Corporation. All rights reserved.
 * SPDX-License-Identifier: MIT */

#include <stdint.h>

/**
 * @brief Enforce that the return value is handled (only applicable on supported compilers).
 */
#ifdef _MSC_VER
#define AZ_NODISCARD _Check_return_
#elif defined(__GNUC__) || defined(__clang__) // !_MSC_VER
#define AZ_NODISCARD __attribute__((warn_unused_result))
#else // !_MSC_VER !__GNUC__ !__clang__
#define AZ_NODISCARD
#endif // _MSC_VER

/**
 * @brief Inline function.
 */
#ifdef _MSC_VER
#define AZ_INLINE static __forceinline
#elif defined(__GNUC__) || defined(__clang__) // !_MSC_VER
#if __STDC_VERSION__ >= 199901L
#define AZ_INLINE __attribute__((always_inline)) static inline
#else
#define AZ_INLINE __attribute__((always_inline)) static
#endif
#else // !_MSC_VER !__GNUC__ !__clang__
#define AZ_INLINE static inline
#endif // _MSC_VER

#endif /* C_STR_SPAN_TYPES_H */
