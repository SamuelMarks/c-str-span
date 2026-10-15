/**
 * @file
 *
 * @brief Public API for formatted output of #az_span.
 */

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

#include "c_str_result.h"
#include "c_str_span_export.h"
/* clang-format on */

/*extern C_STR_SPAN_EXPORT enum az_result_core az_span_fprintf(FILE * __restrict
 * OUT, const uint8_t *format, ...);*/

/**
 * @brief Prints formatted output to stdout, supporting #az_span with `%Q`
 * (special characters escaped).
 *
 * @param[in] format Null-terminated format string.
 * @return #AZ_OK on success, or #AZ_ERROR_ARG on failure.
 */
extern C_STR_SPAN_EXPORT enum az_result_core
az_span_printf(const uint8_t *format, ...);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* !C_STR_SPAN_PRINTF_H */
