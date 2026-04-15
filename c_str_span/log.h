#ifndef C_STR_SPAN_LOG_H
#define C_STR_SPAN_LOG_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* clang-format off */
#include <stdio.h>
/* clang-format on */

#ifndef LOG_DEBUG
#ifdef DEBUG
void c_str_span_log_debug(const char *fmt, ...);
#define LOG_DEBUG c_str_span_log_debug
#else
void c_str_span_log_debug(const char *fmt, ...);
#define LOG_DEBUG 1 ? (void)0 : c_str_span_log_debug
#endif /* DEBUG */
#endif /* !LOG_DEBUG */

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* !C_STR_SPAN_LOG_H */