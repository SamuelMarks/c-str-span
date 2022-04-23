#include <greatest.h>

#include "test_lib.h"
#include "test_az_span.h"

/* Add definitions that need to be in the test runner's main file. */
GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(lib_suite);
  GREATEST_PRINT_REPORT();
  return greatest_all_passed() ? test_az_span() : EXIT_FAILURE;
}
