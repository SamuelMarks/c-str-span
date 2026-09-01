#ifndef C_STR_SPAN_CRT_MOCK_H
#define C_STR_SPAN_CRT_MOCK_H

#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#if defined(_MSC_VER)
extern int g_mock_memcpy_s_fail;
extern int g_mock_memmove_s_fail;
extern int g_mock_sprintf_s_fail;
extern int g_mock_fprintf_s_fail;
extern int g_mock_vfprintf_s_fail;

extern int mock_memcpy_s(void *dest, size_t destsz, const void *src,
                         size_t count);
extern int mock_memmove_s(void *dest, size_t destsz, const void *src,
                          size_t count);
extern int mock_sprintf_s(char *buffer, size_t sizeOfBuffer, const char *format,
                          ...);
extern int mock_fprintf_s(FILE *stream, const char *format, ...);
extern int mock_vfprintf_s(FILE *stream, const char *format, va_list argptr);
#else
extern int g_mock_memcpy_fail;
extern int g_mock_memmove_fail;
extern int g_mock_sprintf_fail;
extern int g_mock_fprintf_fail;
extern int g_mock_vfprintf_fail;

extern void *mock_memcpy(void *dest, const void *src, size_t n);
extern void *mock_memmove(void *dest, const void *src, size_t n);
extern int mock_sprintf(char *str, const char *format, ...);
extern int mock_fprintf(FILE *stream, const char *format, ...);
extern int mock_vfprintf(FILE *stream, const char *format, va_list arg);
#endif

extern int g_mock_fputc_fail;
extern int g_mock_fputs_fail;
extern int g_mock_modf_fail;
extern int g_mock_modf_call_count;

extern int mock_fputc(int character, FILE *stream);
extern int mock_fputs(const char *str, FILE *stream);
extern double mock_modf(double x, double *intpart);

#endif /* !C_STR_SPAN_CRT_MOCK_H */
