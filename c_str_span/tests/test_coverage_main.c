/* clang-format off */
#include <greatest.h>

#include "test_az_span.h"

#include "c_str_precondition.h"
#include "c_str_precondition_internal.h"

SUITE_EXTERN(coverage_suite);
#include <string.h>
#include <stdio.h>
/* clang-format on */

/* Add definitions that need to be in the test runner's main file. */
GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  int passed = 0;
  enum greatest_err ge;
  GREATEST_MAIN_BEGIN();

  RUN_SUITE(coverage_suite);
  GREATEST_PRINT_REPORT();
  ge = greatest_all_passed(&passed);
  if (ge != GREATEST_ERR_SUCCESS) {
    return EXIT_FAILURE;
  }
  return passed ? EXIT_SUCCESS : EXIT_FAILURE;
}
