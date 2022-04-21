/* Feature test compiler string function support with:
 * -strict -pedantic -std=c89 &etc. args
 * */
#include <c_str_span_configure.h>

#include <greatest.h>

#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif /* defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__) */

#include <stdio.h>
#define BUFSIZE 9

void init_buf(char *buf, size_t size);
void print_buf(char *buf);

void init_buf(char *buf, size_t size) {
  size_t i;
  char c;
  for (i=0, c = '\0'; i < size; i++, c++)
    buf[i] = c;
}

void print_buf(char *buf) {
  int i;
  char c;
  for (i = 0; i < BUFSIZE; i++) {
    c = buf[i];
    if (c == '\0')
      printf("\\0");
    else
      putchar(buf[i]);
  }
  putchar('\n');
}

TEST x_string_should_be(void) {
  char buf[BUFSIZE];
  init_buf(buf, BUFSIZE);
  print_buf(buf);

  /* hello there! == 12 characters, > BUFSIZE */
  init_buf(buf, BUFSIZE);
  snprintf(buf, BUFSIZE, "hello there!");
  print_buf(buf);

  /* turtle == 6 characters, < BUFSIZE */
  init_buf(buf, BUFSIZE);
  snprintf(buf, BUFSIZE, "turtle");
  print_buf(buf);

  /* 2222220 == 7 characters, > 5 */
  init_buf(buf, BUFSIZE);
  snprintf(buf, 5, "%d", 222222 * 10);
  print_buf(buf);
  PASS();
}

/* Suites can group multiple tests with common setup. */
SUITE(string_suite) { RUN_TEST(x_string_should_be); }

#undef BUFSIZE
