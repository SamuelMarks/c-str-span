/**
 * @file
 *
 * @brief Debug logging interface for c_str_span.
 */

#ifndef C_STR_SPAN_LOG_H
#define C_STR_SPAN_LOG_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* clang-format off */
#include <stdio.h>
#include "c_str_result.h"
#include "c_str_span_export.h"
/* clang-format on */

#ifndef LOG_DEBUG
#ifdef DEBUG
/**
 * @brief Logs a formatted debug message to stderr.
 *
 * @param[in] fmt Printf-style format string.
 * @return #AZ_OK on success, or #AZ_ERROR_ARG on failure.
 */
C_STR_SPAN_EXPORT enum az_result_core c_str_span_log_debug(const char *fmt,
                                                           ...);
/** @brief Debug logging macro. */
#define LOG_DEBUG c_str_span_log_debug
#else
/**
 * @brief Logs a formatted debug message to stderr.
 *
 * @param[in] fmt Printf-style format string.
 * @return #AZ_OK on success, or #AZ_ERROR_ARG on failure.
 */
C_STR_SPAN_EXPORT enum az_result_core c_str_span_log_debug(const char *fmt,
                                                           ...);
/** @brief Debug logging macro (no-op when DEBUG is not defined). */
#define LOG_DEBUG 1 ? AZ_OK : c_str_span_log_debug
#endif /* DEBUG */
#endif /* !LOG_DEBUG */

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* !C_STR_SPAN_LOG_H */
