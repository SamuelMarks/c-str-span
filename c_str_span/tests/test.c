/* clang-format off */
#include <greatest.h>

#include "test_az_span.h"
#include "test_lib.h"
#include "c_str_precondition.h"
#include "c_str_precondition_internal.h"

SUITE_EXTERN(coverage_suite);
#include <string.h>
#include <stdio.h>
/* clang-format on */

/* Add definitions that need to be in the test runner's main file. */
GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(lib_suite);
  RUN_SUITE(az_core_span_suite);
  RUN_SUITE(az_core_result_suite);
  RUN_SUITE(az_core_precondition_suite);
  RUN_SUITE(az_core_internal_suite);
  RUN_SUITE(coverage_suite);
  GREATEST_PRINT_REPORT();
  return greatest_all_passed() ? EXIT_SUCCESS : EXIT_FAILURE;
}
