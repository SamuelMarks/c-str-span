/**
 * @file
 *
 * @brief Implementation of precondition failure handling.
 */

/* Copyright (c) Microsoft Corporation. All rights reserved.
 * SPDX-License-Identifier: MIT */

/* clang-format off */
#include "c_str_precondition_internal.h"
#include "c_str_span_log.h"
#include <string.h>
#include <stdio.h>
/* clang-format on */

/*#include <azure/core/_az_cfg.h>*/

static void az_precondition_failed_default(void) {
  /* By default, when a precondition fails, we log it. */
  c_str_span_log_debug("Precondition failed!\n");
}

/** @brief Internal doc. */
az_precondition_failed_fn _az_precondition_failed_callback =
    az_precondition_failed_default;

void az_precondition_failed_set_callback(
    az_precondition_failed_fn az_precondition_failed_callback) {
  _az_precondition_failed_callback = az_precondition_failed_callback;
}

/** @brief Internal doc. */
az_precondition_failed_fn az_precondition_failed_get_callback(void) {
  return _az_precondition_failed_callback;
}

/** @brief Internal doc. */
bool _az_span_is_valid(az_span span, size_t min_size, bool null_is_valid) {
  uint8_t *const ptr = az_span_ptr(span);
  size_t const span_size = az_span_size(span);

  bool result = false;

  const az_span empty = {{NULL, 0}};
  uint8_t *const default_init_ptr = az_span_ptr(empty);

  if (null_is_valid) {
    result = (ptr == NULL || ptr == default_init_ptr) ? span_size == 0 : true;
  } else {
    result = (ptr != NULL && ptr != default_init_ptr);
  }

  if (result) {
    result = (span_size <= ((size_t)-1 - (uintptr_t)ptr));
  }

  return result && min_size <= span_size;
}
