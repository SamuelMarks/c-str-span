#ifndef C_STR_SPAN_PRINTF_H
#define C_STR_SPAN_PRINTF_H

#ifdef __cplusplus
#include <cstdint>
extern "C" {
#else
#if defined(_MSC_VER) && _MSC_VER < 1600
#include "c_str_span_stdint.h"
#else
#include <stdint.h>
#endif /* defined(_MSC_VER) && _MSC_VER < 1600 */
#endif /* __cplusplus */

#include "c_str_span_export.h"

/*extern C_STR_SPAN_EXPORT void az_span_fprintf(FILE * __restrict OUT, const uint8_t *format, ...);*/

/* `az_span`s supported with "%Q", with special characters escaped.
 * doesn't support "%03d" or other fancier syntax
 * */
extern C_STR_SPAN_EXPORT void az_span_printf(const uint8_t * format, ...);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !C_STR_SPAN_PRINTF_H */
