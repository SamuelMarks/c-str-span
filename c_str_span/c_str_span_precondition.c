/* Copyright (c) Microsoft Corporation. All rights reserved.
 * SPDX-License-Identifier: MIT */

#include "c_str_precondition_internal.h"

/*#include <azure/core/_az_cfg.h>*/

static void az_precondition_failed_default(void) {
  /* By default, when a precondition fails the calling thread spins forever */
  while (1) {
  }
}

az_precondition_failed_fn _az_precondition_failed_callback =
    az_precondition_failed_default;

void az_precondition_failed_set_callback(
    az_precondition_failed_fn az_precondition_failed_callback) {
  _az_precondition_failed_callback = az_precondition_failed_callback;
}

az_precondition_failed_fn az_precondition_failed_get_callback(void) {
  return _az_precondition_failed_callback;
}
