/**
 * @file
 *
 * @brief Compatibility header for stdint.h.
 */

#ifndef C_STR_SPAN_STDINT_H
#define C_STR_SPAN_STDINT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file was originally part of the w64 mingw-runtime package.
 */
/* ISO C9x  7.18  Integer types <stdint.h>
 * Based on ISO/IEC SC22/WG14 9899 Committee draft (SC22 N2794)
 *
 *  THIS SOFTWARE IS NOT COPYRIGHTED
 *
 *  Contributor: Danny Smith <danny_r_smith_2001@yahoo.co.nz>
 *  Modified for libusb/MSVC: Pete Batard <pbatard@gmail.com>
 *
 *  This source code is offered for use in the public domain. You may
 *  use, modify or distribute it freely.
 *
 *  This code is distributed in the hope that it will be useful but
 *  WITHOUT ANY WARRANTY. ALL WARRANTIES, EXPRESS OR IMPLIED ARE HEREBY
 *  DISCLAIMED. This includes but is not limited to warranties of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  Date: 2010-04-02
 */
#ifndef _MSC_VER
#error This header should only be used with Microsoft compilers
#endif
#ifndef _STDINT_H
#define _STDINT_H

#ifndef _INTPTR_T_DEFINED
/** @brief Internal doc. */
#define _INTPTR_T_DEFINED

#ifndef __intptr_t_defined
/** @brief Internal doc. */
#define __intptr_t_defined
#undef intptr_t
#ifdef _WIN64
typedef __int64 intptr_t;
#else
/** @brief Internal doc. */
typedef int intptr_t;
#endif /* _WIN64 */
#endif /* __intptr_t_defined */
#endif /* _INTPTR_T_DEFINED */
#ifndef _UINTPTR_T_DEFINED
/** @brief Internal doc. */
#define _UINTPTR_T_DEFINED

#ifndef __uintptr_t_defined
/** @brief Internal doc. */
#define __uintptr_t_defined
#undef uintptr_t
#ifdef _WIN64
typedef unsigned __int64 uintptr_t;
#else
/** @brief Internal doc. */
typedef unsigned int uintptr_t;
#endif /* _WIN64 */
#endif /* __uintptr_t_defined */
#endif /* _UINTPTR_T_DEFINED */
#ifndef _PTRDIFF_T_DEFINED
/** @brief Internal doc. */
#define _PTRDIFF_T_DEFINED

#ifndef _PTRDIFF_T_
/** @brief Internal doc. */
#define _PTRDIFF_T_

#undef ptrdiff_t
#ifdef _WIN64
typedef __int64 ptrdiff_t;
#else
/** @brief Internal doc. */
typedef int ptrdiff_t;
#endif /* _WIN64 */
#endif /* _PTRDIFF_T_ */
#endif /* _PTRDIFF_T_DEFINED */
#ifndef _WCHAR_T_DEFINED
/** @brief Internal doc. */
#define _WCHAR_T_DEFINED

/** @brief Internal doc. */
typedef unsigned short wchar_t;
#endif /* _WCHAR_T_DEFINED */
#ifndef _WCTYPE_T_DEFINED
/** @brief Internal doc. */
#define _WCTYPE_T_DEFINED

#ifndef _WINT_T
/** @brief Internal doc. */
#define _WINT_T

/** @brief Internal doc. */
typedef unsigned short wint_t;
/** @brief Internal doc. */
typedef unsigned short wctype_t;
#endif /* _WINT_T */
#endif /* _WCTYPE_T_DEFINED */
/* 7.18.1.1  Exact-width integer types */
/** @brief Internal doc. */
typedef __int8 int8_t;
/** @brief Internal doc. */
typedef unsigned __int8 uint8_t;
/** @brief Internal doc. */
typedef __int16 int16_t;
/** @brief Internal doc. */
typedef unsigned __int16 uint16_t;
/** @brief Internal doc. */
typedef __int32 int32_t;
/** @brief Internal doc. */
typedef unsigned __int32 uint32_t;
/** @brief Internal doc. */
typedef __int64 int64_t;
/** @brief Internal doc. */
typedef unsigned __int64 uint64_t;
/* 7.18.1.2  Minimum-width integer types */
/** @brief Internal doc. */
typedef signed char int_least8_t;
/** @brief Internal doc. */
typedef unsigned char uint_least8_t;
/** @brief Internal doc. */
typedef short int_least16_t;
/** @brief Internal doc. */
typedef unsigned short uint_least16_t;
/** @brief Internal doc. */
typedef int int_least32_t;
/** @brief Internal doc. */
typedef unsigned uint_least32_t;
/** @brief Internal doc. */
typedef __int64 int_least64_t;
/** @brief Internal doc. */
typedef unsigned __int64 uint_least64_t;
/*  7.18.1.3  Fastest minimum-width integer types
 *  Not actually guaranteed to be fastest for all purposes
 *  Here we use the exact-width types for 8 and 16-bit ints.
 */
/** @brief Internal doc. */
typedef __int8 int_fast8_t;
/** @brief Internal doc. */
typedef unsigned __int8 uint_fast8_t;
/** @brief Internal doc. */
typedef __int16 int_fast16_t;
/** @brief Internal doc. */
typedef unsigned __int16 uint_fast16_t;
/** @brief Internal doc. */
typedef __int32 int_fast32_t;
/** @brief Internal doc. */
typedef unsigned __int32 uint_fast32_t;
/** @brief Internal doc. */
typedef __int64 int_fast64_t;
/** @brief Internal doc. */
typedef unsigned __int64 uint_fast64_t;
/* 7.18.1.5  Greatest-width integer types */
/** @brief Internal doc. */
typedef __int64 intmax_t;
/** @brief Internal doc. */
typedef unsigned __int64 uintmax_t;
/* 7.18.2  Limits of specified-width integer types */
/* 7.18.2.1  Limits of exact-width integer types */
/** @brief Internal doc. */
#define INT8_MIN (-128)
/** @brief Internal doc. */
#define INT16_MIN (-32768)
/** @brief Internal doc. */
#define INT32_MIN (-2147483647 - 1)
/** @brief Internal doc. */
#define INT64_MIN (-9223372036854775807LL - 1)
/** @brief Internal doc. */
#define INT8_MAX 127
/** @brief Internal doc. */
#define INT16_MAX 32767
/** @brief Internal doc. */
#define INT32_MAX 2147483647
/** @brief Internal doc. */
#define INT64_MAX 9223372036854775807LL
/** @brief Internal doc. */
#define UINT8_MAX 255
/** @brief Internal doc. */
#define UINT16_MAX 65535
/** @brief Internal doc. */
#define UINT32_MAX 0xffffffffU /* 4294967295U */
/** @brief Internal doc. */
#define UINT64_MAX 0xffffffffffffffffULL /* 18446744073709551615ULL */
/* 7.18.2.2  Limits of minimum-width integer types */
/** @brief Internal doc. */
#define INT_LEAST8_MIN INT8_MIN
/** @brief Internal doc. */
#define INT_LEAST16_MIN INT16_MIN
/** @brief Internal doc. */
#define INT_LEAST32_MIN INT32_MIN
/** @brief Internal doc. */
#define INT_LEAST64_MIN INT64_MIN
/** @brief Internal doc. */
#define INT_LEAST8_MAX INT8_MAX
/** @brief Internal doc. */
#define INT_LEAST16_MAX INT16_MAX
/** @brief Internal doc. */
#define INT_LEAST32_MAX INT32_MAX
/** @brief Internal doc. */
#define INT_LEAST64_MAX INT64_MAX
/** @brief Internal doc. */
#define UINT_LEAST8_MAX UINT8_MAX
/** @brief Internal doc. */
#define UINT_LEAST16_MAX UINT16_MAX
/** @brief Internal doc. */
#define UINT_LEAST32_MAX UINT32_MAX
/** @brief Internal doc. */
#define UINT_LEAST64_MAX UINT64_MAX
/* 7.18.2.3  Limits of fastest minimum-width integer types */
/** @brief Internal doc. */
#define INT_FAST8_MIN INT8_MIN
/** @brief Internal doc. */
#define INT_FAST16_MIN INT16_MIN
/** @brief Internal doc. */
#define INT_FAST32_MIN INT32_MIN
/** @brief Internal doc. */
#define INT_FAST64_MIN INT64_MIN
/** @brief Internal doc. */
#define INT_FAST8_MAX INT8_MAX
/** @brief Internal doc. */
#define INT_FAST16_MAX INT16_MAX
/** @brief Internal doc. */
#define INT_FAST32_MAX INT32_MAX
/** @brief Internal doc. */
#define INT_FAST64_MAX INT64_MAX
/** @brief Internal doc. */
#define UINT_FAST8_MAX UINT8_MAX
/** @brief Internal doc. */
#define UINT_FAST16_MAX UINT16_MAX
/** @brief Internal doc. */
#define UINT_FAST32_MAX UINT32_MAX
/** @brief Internal doc. */
#define UINT_FAST64_MAX UINT64_MAX
/* 7.18.2.4  Limits of integer types capable of holding
    object pointers */
#ifdef _WIN64
#define INTPTR_MIN INT64_MIN
#define INTPTR_MAX INT64_MAX
#define UINTPTR_MAX UINT64_MAX
#else
/** @brief Internal doc. */
#define INTPTR_MIN INT32_MIN
/** @brief Internal doc. */
#define INTPTR_MAX INT32_MAX
/** @brief Internal doc. */
#define UINTPTR_MAX UINT32_MAX
#endif
/* 7.18.2.5  Limits of greatest-width integer types */
/** @brief Internal doc. */
#define INTMAX_MIN INT64_MIN
/** @brief Internal doc. */
#define INTMAX_MAX INT64_MAX
/** @brief Internal doc. */
#define UINTMAX_MAX UINT64_MAX
/* 7.18.3  Limits of other integer types */
#ifdef _WIN64
#define PTRDIFF_MIN INT64_MIN
#define PTRDIFF_MAX INT64_MAX
#else
/** @brief Internal doc. */
#define PTRDIFF_MIN INT32_MIN
/** @brief Internal doc. */
#define PTRDIFF_MAX INT32_MAX
#endif
/** @brief Internal doc. */
#define SIG_ATOMIC_MIN INT32_MIN
/** @brief Internal doc. */
#define SIG_ATOMIC_MAX INT32_MAX
#ifndef SIZE_MAX
#ifdef _WIN64
#define SIZE_MAX UINT64_MAX
#else
/** @brief Internal doc. */
#define SIZE_MAX UINT32_MAX
#endif
#endif
#ifndef WCHAR_MIN /* also in wchar.h */
/** @brief Internal doc. */
#define WCHAR_MIN 0U
/** @brief Internal doc. */
#define WCHAR_MAX 0xffffU
#endif
/*
 * wint_t is unsigned short for compatibility with MS runtime
 */
/** @brief Internal doc. */
#define WINT_MIN 0U
/** @brief Internal doc. */
#define WINT_MAX 0xffffU
/* 7.18.4  Macros for integer constants */
/* 7.18.4.1  Macros for minimum-width integer constants
    Accoding to Douglas Gwyn <gwyn@arl.mil>:
        "This spec was changed in ISO/IEC 9899:1999 TC1; in ISO/IEC
        9899:1999 as initially published, the expansion was required
        to be an integer constant of precisely matching type, which
        is impossible to accomplish for the shorter types on most
        platforms, because C99 provides no standard way to designate
        an integer constant with width less than that of type int.
        TC1 changed this to require just an integer constant
        *expression* with *promoted* type."
        The trick used here is from Clive D W Feather.
*/
/** @brief Internal doc. */
#define INT8_C(val) (INT_LEAST8_MAX - INT_LEAST8_MAX + (val))
/** @brief Internal doc. */
#define INT16_C(val) (INT_LEAST16_MAX - INT_LEAST16_MAX + (val))
/** @brief Internal doc. */
#define INT32_C(val) (INT_LEAST32_MAX - INT_LEAST32_MAX + (val))
/*  The 'trick' doesn't work in C89 for long long because, without
    suffix, (val) will be evaluated as int, not intmax_t */
/** @brief Internal doc. */
#define INT64_C(val) val##i64
/** @brief Internal doc. */
#define UINT8_C(val) (val)
/** @brief Internal doc. */
#define UINT16_C(val) (val)
/** @brief Internal doc. */
#define UINT32_C(val) (val##i32)
/** @brief Internal doc. */
#define UINT64_C(val) val##ui64
/* 7.18.4.2  Macros for greatest-width integer constants */
/** @brief Internal doc. */
#define INTMAX_C(val) val##i64
/** @brief Internal doc. */
#define UINTMAX_C(val) val##ui64
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* C_STR_SPAN_STDINT_H */
