/* clang-format off */
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>

#include "c_str_span.h"
#include "c_str_span_printf.h"
/* clang-format on */

#if defined(_MSC_VER)
#define FPRINTF fprintf_s
#define FPRINTF_N(OUT, ap)
#else
#define FPRINTF fprintf
#define FPRINTF_N(OUT, ap) FPRINTF(OUT, "%n", va_arg(ap, int *))
#endif

#define AZ_SPAN_FPRINTF(func_name, OUT)                                        \
  void func_name(const uint8_t *format, ...) {                                 \
    /* `az_span`s supported with "%Q", with special characters escaped.        \
     * doesn't support "%03d" or other fancier syntax                          \
     * */                                                                      \
    size_t i;                                                                  \
    va_list ap;                                                                \
    va_start(ap, format);                                                      \
                                                                               \
    while (*format) {                                                          \
      switch (*format++) {                                                     \
      case 'c':                                                                \
        fputc((char)va_arg(ap, int), OUT);                                     \
        break;                                                                 \
      case 'd':                                                                \
      case 'i':                                                                \
        FPRINTF(OUT, "%d", va_arg(ap, int));                                   \
        break;                                                                 \
      case 'o':                                                                \
        FPRINTF(OUT, "%o", va_arg(ap, int));                                   \
        break;                                                                 \
      case 'u':                                                                \
        FPRINTF(OUT, "%u", va_arg(ap, unsigned));                              \
        break;                                                                 \
      case 'x':                                                                \
        FPRINTF(OUT, "%x", va_arg(ap, unsigned));                              \
        break;                                                                 \
      case 'X':                                                                \
        FPRINTF(OUT, "%X", va_arg(ap, unsigned));                              \
        break;                                                                 \
      case 'e':                                                                \
        FPRINTF(OUT, "%e", va_arg(ap, double));                                \
        break;                                                                 \
      case 'E':                                                                \
        FPRINTF(OUT, "%E", va_arg(ap, double));                                \
        break;                                                                 \
      case 'f':                                                                \
        FPRINTF(OUT, "%f", va_arg(ap, double));                                \
        break;                                                                 \
      case 'F':                                                                \
        FPRINTF(OUT, "%f", va_arg(ap, double));                                \
        break;                                                                 \
      case 'g':                                                                \
        FPRINTF(OUT, "%g", va_arg(ap, double));                                \
        break;                                                                 \
      case 'G':                                                                \
        FPRINTF(OUT, "%G", va_arg(ap, double));                                \
        break;                                                                 \
      case 'a':                                                                \
        FPRINTF(OUT, "%f", va_arg(ap, double));                                \
        break;                                                                 \
      case 'A':                                                                \
        FPRINTF(OUT, "%f", va_arg(ap, double));                                \
        break;                                                                 \
      case 'n':                                                                \
        FPRINTF_N(OUT, ap);                                                    \
        break;                                                                 \
      case 'p':                                                                \
        FPRINTF(OUT, "%p", va_arg(ap, void *));                                \
        break;                                                                 \
      case 's':                                                                \
      case 'S':                                                                \
      case 'Z':                                                                \
        fputs(va_arg(ap, char *), OUT);                                        \
        break;                                                                 \
      case '%':                                                                \
        putc('%', OUT);                                                        \
        break;                                                                 \
      case 'Q': {                                                              \
        /* Custom for az_span, escapes */                                      \
                                                                               \
        const az_span span = va_arg(ap, az_span);                              \
        const uint8_t *const span_ptr = az_span_ptr(span);                     \
                                                                               \
        for (i = 0; i < az_span_size(span); i++)                               \
          if (iscntrl(span_ptr[i]) || span_ptr[i] == '\\' ||                   \
              span_ptr[i] == '\"' || span_ptr[i] == '\'')                      \
            FPRINTF(OUT, "\\%03o", span_ptr[i]);                               \
          else                                                                 \
            fputc(span_ptr[i], OUT);                                           \
        break;                                                                 \
      }                                                                        \
        va_end(ap);                                                            \
      }                                                                        \
    }                                                                          \
  }

AZ_SPAN_FPRINTF(az_span_printf, stdout)
