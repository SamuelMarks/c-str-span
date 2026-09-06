# Undercovered Files Report (`UNDERCOVERED.md`)

This document tracks all files with test coverage across functions, lines, or branches, measured using GCC/Clang coverage instrumentation (`--coverage`) and analyzed with `gcovr`.

## Coverage Summary

### Core Library Sources (`c_str_span/`)

- **Total Source Files with Executable Code Analyzed:** 8
- **Fully Covered Files (100% functions, lines, branches):** 8
- **Undercovered Files (<100% in function, line, or branch coverage):** 0
- **Library Line Coverage:** 861 / 861 (100.00%)
- **Library Function Coverage:** 63 / 63 (100.00%)
- **Library Branch Coverage:** 494 / 494 (100.00%)

---

## Undercovered Files (<100% Coverage)

### Core Library Files

*All core library files are 100% covered across functions, lines, and branches!*

### Test Harness Files

- [x] `c_str_span/tests/test.c` — **Lines:** 100.00% (13/13) | Test Suite Entry Point
- [x] `c_str_span/tests/test_coverage_main.c` — **Lines:** 100.00% (9/9) | Test Suite Entry Point
- [x] `c_str_span/tests/test_lib.h` — **Lines:** 100.00% (6/6) | **Functions:** 100.00% (2/2) | Test Suite Header
- [x] `c_str_span/tests/test_az_span.c` — **Lines:** 100.00% (1527/1527) | **Functions:** 100.00% (80/80) | Test Suite
- [x] `c_str_span/tests/test_coverage.c` — **Lines:** 100.00% (386/386) | **Functions:** 100.00% (27/27) | Test Suite

---

## Covered Files Overview Table

| Checkbox & File | Function Coverage | Line Coverage | Branch Coverage | Category |
|---|---|---|---|---|
| - [x] `c_str_span/c_str_span.c` | 39/39 (100.0%) | 735/735 (100.0%) | 358/358 (100.0%) | Library Source |
| - [x] `c_str_span/c_str_span_internal.h` | 1/1 (100.0%) | 4/4 (100.0%) | 2/2 (100.0%) | Library Source |
| - [x] `c_str_span/c_str_span_precondition.c` | 4/4 (100.0%) | 21/21 (100.0%) | 10/10 (100.0%) | Library Source |
| - [x] `c_str_span/c_str_span_printf.c` | 1/1 (100.0%) | 1/1 (100.0%) | 84/84 (100.0%) | Library Source |
| - [x] `c_str_span/tests/test.c` | 18/40 (45.0%) | 13/13 (100.0%) | 36/167 (21.6%) | Test Suite |
| - [x] `c_str_span/tests/test_az_span.c` | 80/80 (100.0%) | 1527/1527 (100.0%) | 1797/7460 (24.1%) | Test Suite |
| - [x] `c_str_span/tests/test_coverage.c` | 27/27 (100.0%) | 386/386 (100.0%) | 200/682 (29.3%) | Test Suite |
| - [x] `c_str_span/tests/test_coverage_main.c` | 19/40 (47.5%) | 9/9 (100.0%) | 36/167 (21.6%) | Test Suite |
| - [x] `c_str_span/tests/test_lib.h` | 2/2 (100.0%) | 6/6 (100.0%) | 3/8 (37.5%) | Test Suite |

---

## Fully Covered Files (100% Across All Metrics)

- [x] `c_str_span/c_str_hex_private.h` — **Functions:** 100.00% (1/1) | **Lines:** 100.00% (2/2) | **Branches:** N/A (0 branches)
- [x] `c_str_span/c_str_result.h` — **Functions:** 100.00% (2/2) | **Lines:** 100.00% (3/3) | **Branches:** N/A (0 branches)
- [x] `c_str_span/c_str_span.c` — **Functions:** 100.00% (39/39) | **Lines:** 100.00% (735/735) | **Branches:** 100.00% (358/358)
- [x] `c_str_span/c_str_span.h` — **Functions:** 100.00% (14/14) | **Lines:** 100.00% (91/91) | **Branches:** 100.00% (40/40)
- [x] `c_str_span/c_str_span_internal.h` — **Functions:** 100.00% (1/1) | **Lines:** 100.00% (4/4) | **Branches:** 100.00% (2/2)
- [x] `c_str_span/c_str_span_precondition.c` — **Functions:** 100.00% (4/4) | **Lines:** 100.00% (21/21) | **Branches:** 100.00% (10/10)
- [x] `c_str_span/c_str_span_printf.c` — **Functions:** 100.00% (1/1) | **Lines:** 100.00% (1/1) | **Branches:** 100.00% (84/84)
- [x] `c_str_span/c_str_span_private.h` — **Functions:** 100.00% (1/1) | **Lines:** 100.00% (4/4) | **Branches:** N/A (0 branches)

---

## Declaration-Only Headers (No Executable Code)

The following header files declare types, macros, or function prototypes without inline implementations:

- `c_str_span/c_str_precondition.h`
- `c_str_span/c_str_precondition_internal.h`
- `c_str_span/c_str_result_internal.h`
- `c_str_span/c_str_span_log.h`
- `c_str_span/c_str_span_printf.h`
- `c_str_span/c_str_span_stdbool.h`
- `c_str_span/c_str_span_stdint.h`
- `c_str_span/c_str_span_types.h`
- `c_str_span/tests/c_str_span_crt_mock.h`
- `c_str_span/tests/test_az_span.h`
