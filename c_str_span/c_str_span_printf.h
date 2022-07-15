#ifndef C_STR_SPAN_C_STR_SPAN_PRINTF_H
#define C_STR_SPAN_C_STR_SPAN_PRINTF_H

#include <stdint.h>

#include "c_str_span_export.h"

/* `az_span`s supported with "%Q", with special characters escaped.
 * doesn't support "%03d" or other fancier syntax
 * */
extern C_STR_SPAN_EXPORT void az_span_printf(uint8_t *format, ...);

#endif /* !C_STR_SPAN_C_STR_SPAN_PRINTF_H */
