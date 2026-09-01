/**
 * @file
 *
 * @brief Implementation of az_span_printf for formatted output.
 */

/* clang-format off */
#include "c_str_span_log.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>

#include "c_str_span.h"
#include "c_str_span_printf.h"
#include <string.h>
/* clang-format on */

#if defined(_MSC_VER)
#define FPRINTF_N(OUT, ap)                                                     \
  do {                                                                         \
    int *ptr = va_arg(ap, int *);                                              \
    if (ptr != NULL)                                                           \
      *ptr = 0;                                                                \
  } while (0)
#define FPRINTF fprintf_s
#else
/** @brief Internal doc. */
#define FPRINTF_N(OUT, ap)                                                     \
  do {                                                                         \
    if (fprintf(OUT, "%n", va_arg(ap, int *)) < 0)                             \
      return AZ_ERROR_ARG;                                                     \
  } while (0)
/** @brief Internal doc. */
#define FPRINTF fprintf
#endif

/** @brief Internal doc. */
#define AZ_SPAN_FPRINTF(func_name, OUT)                                        \
  enum az_result_core func_name(const uint8_t *format, ...) {                  \
    va_list ap;                                                                \
    va_start(ap, format);                                                      \
    if (format == NULL) {                                                      \
      va_end(ap);                                                              \
      return AZ_ERROR_ARG;                                                     \
    }                                                                          \
                                                                               \
    while (*format) {                                                          \
      if (*format == '%') {                                                    \
        format++;                                                              \
        if (!*format)                                                          \
          break;                                                               \
        switch (*format++) {                                                   \
        case 'c':                                                              \
          if (fputc((char)va_arg(ap, int), OUT) < 0) {                         \
            va_end(ap);                                                        \
            return AZ_ERROR_ARG;                                               \
          }                                                                    \
          break;                                                               \
        case 'd':                                                              \
        case 'i':                                                              \
          if (FPRINTF(OUT, "%d", va_arg(ap, int)) < 0) {                       \
            va_end(ap);                                                        \
            return AZ_ERROR_ARG;                                               \
          }                                                                    \
          break;                                                               \
        case 'o':                                                              \
          if (FPRINTF(OUT, "%o", va_arg(ap, int)) < 0) {                       \
            va_end(ap);                                                        \
            return AZ_ERROR_ARG;                                               \
          }                                                                    \
          break;                                                               \
        case 'u':                                                              \
          if (FPRINTF(OUT, "%u", va_arg(ap, unsigned)) < 0) {                  \
            va_end(ap);                                                        \
            return AZ_ERROR_ARG;                                               \
          }                                                                    \
          break;                                                               \
        case 'x':                                                              \
          if (FPRINTF(OUT, "%x", va_arg(ap, unsigned)) < 0) {                  \
            va_end(ap);                                                        \
            return AZ_ERROR_ARG;                                               \
          }                                                                    \
          break;                                                               \
        case 'X':                                                              \
          if (FPRINTF(OUT, "%X", va_arg(ap, unsigned)) < 0) {                  \
            va_end(ap);                                                        \
            return AZ_ERROR_ARG;                                               \
          }                                                                    \
          break;                                                               \
        case 'e':                                                              \
          if (FPRINTF(OUT, "%e", va_arg(ap, double)) < 0) {                    \
            va_end(ap);                                                        \
            return AZ_ERROR_ARG;                                               \
          }                                                                    \
          break;                                                               \
        case 'E':                                                              \
          if (FPRINTF(OUT, "%E", va_arg(ap, double)) < 0) {                    \
            va_end(ap);                                                        \
            return AZ_ERROR_ARG;                                               \
          }                                                                    \
          break;                                                               \
        case 'f':                                                              \
          if (FPRINTF(OUT, "%f", va_arg(ap, double)) < 0) {                    \
            va_end(ap);                                                        \
            return AZ_ERROR_ARG;                                               \
          }                                                                    \
          break;                                                               \
        case 'F':                                                              \
          if (FPRINTF(OUT, "%f", va_arg(ap, double)) < 0) {                    \
            va_end(ap);                                                        \
            return AZ_ERROR_ARG;                                               \
          }                                                                    \
          break;                                                               \
        case 'g':                                                              \
          if (FPRINTF(OUT, "%g", va_arg(ap, double)) < 0) {                    \
            va_end(ap);                                                        \
            return AZ_ERROR_ARG;                                               \
          }                                                                    \
          break;                                                               \
        case 'G':                                                              \
          if (FPRINTF(OUT, "%G", va_arg(ap, double)) < 0) {                    \
            va_end(ap);                                                        \
            return AZ_ERROR_ARG;                                               \
          }                                                                    \
          break;                                                               \
        case 'a':                                                              \
          if (FPRINTF(OUT, "%f", va_arg(ap, double)) < 0) {                    \
            va_end(ap);                                                        \
            return AZ_ERROR_ARG;                                               \
          }                                                                    \
          break;                                                               \
        case 'A':                                                              \
          if (FPRINTF(OUT, "%f", va_arg(ap, double)) < 0) {                    \
            va_end(ap);                                                        \
            return AZ_ERROR_ARG;                                               \
          }                                                                    \
          break;                                                               \
        case 'n':                                                              \
          FPRINTF_N(OUT, ap);                                                  \
          break;                                                               \
        case 'p':                                                              \
          if (FPRINTF(OUT, "%p", va_arg(ap, void *)) < 0) {                    \
            va_end(ap);                                                        \
            return AZ_ERROR_ARG;                                               \
          }                                                                    \
          break;                                                               \
        case 's':                                                              \
        case 'S':                                                              \
        case 'Z':                                                              \
          if (fputs(va_arg(ap, char *), OUT) < 0) {                            \
            va_end(ap);                                                        \
            return AZ_ERROR_ARG;                                               \
          }                                                                    \
          break;                                                               \
        case '%':                                                              \
          if (fputc('%', OUT) < 0) {                                           \
            va_end(ap);                                                        \
            return AZ_ERROR_ARG;                                               \
          }                                                                    \
          break;                                                               \
        case 'Q': {                                                            \
          size_t i;                                                            \
          const az_span span = va_arg(ap, az_span);                            \
          const uint8_t *const span_ptr = az_span_ptr(span);                   \
          for (i = 0; i < az_span_size(span); i++) {                           \
            if (iscntrl(span_ptr[i]) || span_ptr[i] == '\\' ||                 \
                span_ptr[i] == '\"' || span_ptr[i] == '\'') {                  \
              if (FPRINTF(OUT, "\\%03o", span_ptr[i]) < 0) {                   \
                va_end(ap);                                                    \
                return AZ_ERROR_ARG;                                           \
              }                                                                \
            } else {                                                           \
              if (fputc(span_ptr[i], OUT) < 0) {                               \
                va_end(ap);                                                    \
                return AZ_ERROR_ARG;                                           \
              }                                                                \
            }                                                                  \
          }                                                                    \
          break;                                                               \
        }                                                                      \
        default:                                                               \
          if (fputc('%', OUT) < 0) {                                           \
            va_end(ap);                                                        \
            return AZ_ERROR_ARG;                                               \
          }                                                                    \
          if (fputc(*(format - 1), OUT) < 0) {                                 \
            va_end(ap);                                                        \
            return AZ_ERROR_ARG;                                               \
          }                                                                    \
          break;                                                               \
        }                                                                      \
      } else {                                                                 \
        if (fputc(*format++, OUT) < 0) {                                       \
          va_end(ap);                                                          \
          return AZ_ERROR_ARG;                                                 \
        }                                                                      \
      }                                                                        \
    }                                                                          \
    va_end(ap);                                                                \
    return AZ_OK;                                                              \
  }

AZ_SPAN_FPRINTF(az_span_printf, stdout)
