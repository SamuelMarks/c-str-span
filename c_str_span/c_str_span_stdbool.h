/**
 * @file
 *
 * @brief Compatibility header for stdbool.h.
 *
 * stdbool isn't always included with every popular C89 implementation.
 * This variant is modified from MUSL.
 */

#ifndef C_STR_SPAN_STDBOOL_H
#define C_STR_SPAN_STDBOOL_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* clang-format off */
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) ||              \
    (defined(_MSC_VER) && _MSC_VER >= 1800)
#include <stdbool.h>
#else
#include <stdlib.h>
#endif
/* clang-format on */

#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) ||              \
    (defined(_MSC_VER) && _MSC_VER >= 1800)
#else

#if !defined(_STDBOOL_H) && !defined(HAS_STDBOOL)
#define _STDBOOL_H

#ifdef bool
#undef bool
#endif /* bool */
#ifdef true
#undef true
#endif /* true */
#ifdef false
#undef false
#endif /* false */

typedef size_t bool;
#define true 1
#define false (!true)

#endif /* !defined(_STDBOOL_H) && !defined(HAS_STDBOOL) */

#endif /* (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) ||        \
          (defined(_MSC_VER) && _MSC_VER >= 1800) */

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* C_STR_SPAN_STDBOOL_H */
