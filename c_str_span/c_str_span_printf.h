#ifndef C_STR_SPAN_PRINTF_H
#define C_STR_SPAN_PRINTF_H

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

#include "c_str_span_export.h"

/* Formatting macros for 64-bit integers and size_t */
#if defined(_MSC_VER) || defined(__MINGW32__)
#define C_STR_SPAN_PRId64 "%I64d"
#define C_STR_SPAN_PRIu64 "%I64u"
#define C_STR_SPAN_PRIx64 "%I64x"
#define C_STR_SPAN_PRIX64 "%I64X"
#else
#define C_STR_SPAN_PRId64 "%lld"
#define C_STR_SPAN_PRIu64 "%llu"
#define C_STR_SPAN_PRIx64 "%llx"
#define C_STR_SPAN_PRIX64 "%llX"
#endif

#if defined(_MSC_VER) || defined(__MINGW32__)
#define C_STR_SPAN_PRI_SIZE "%Iu"
#else
#define C_STR_SPAN_PRI_SIZE "%zu"
#endif
/* clang-format on */

/*extern C_STR_SPAN_EXPORT void az_span_fprintf(FILE * __restrict OUT, const
 * uint8_t *format, ...);*/

/* `az_span`s supported with "%Q", with special characters escaped.
 * doesn't support "%03d" or other fancier syntax
 * */
extern C_STR_SPAN_EXPORT void az_span_printf(const uint8_t *format, ...);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* !C_STR_SPAN_PRINTF_H */
