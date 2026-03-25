#ifndef C_STR_SPAN_PRINTF_H
#define C_STR_SPAN_PRINTF_H

/* clang-format off */
#if defined(_MSC_VER) && _MSC_VER < 1600
#include "c_str_span_stdint.h"
#else
#include <stdint.h>
#endif /* defined(_MSC_VER) && _MSC_VER < 1600 */

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#include <stdbool.h>
#elif defined(_MSC_VER) && _MSC_VER < 1800
#include "c_str_span_stdbool.h"
#else
#include <stdbool.h>
#endif /* defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#include "c_str_span_export.h"
/* clang-format on */

/*extern C_STR_SPAN_EXPORT void az_span_fprintf(FILE * __restrict OUT, const
 * uint8_t *format, ...);*/

/* `az_span`s supported with "%Q", with special characters escaped.
 * doesn't support "%03d" or other fancier syntax
 * */
extern C_STR_SPAN_EXPORT void az_span_printf(const uint8_t *format, ...);

#endif /* !C_STR_SPAN_PRINTF_H */

#ifdef __cplusplus
}
#endif /* __cplusplus */
