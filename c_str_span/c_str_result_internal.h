/* Copyright (c) Microsoft Corporation. All rights reserved.
 * SPDX-License-Identifier: MIT */

/**
 * @file
 *
 * @brief Definition of #az_result related internal helper functions.
 *
 * @note You MUST NOT use any symbols (macros, functions, structures, enums,
 * etc.) prefixed with an underscore ('_') directly in your application code.
 * These symbols are part of Azure SDK's internal implementation; we do not
 * document these symbols and they are subject to change in future versions of
 * the SDK which would break your code.
 */

#ifndef C_STR_SPAN_RESULT_INTERNAL_H
#define C_STR_SPAN_RESULT_INTERNAL_H

#ifdef __cplusplus
#include <cstdint>
extern "C" {
#else
#if defined(_MSC_VER) && _MSC_VER < 1600
#include "c_str_span_stdint.h"
#else
#include <stdint.h>
#endif /* defined(_MSC_VER) && _MSC_VER < 1600 */
#endif /* __cplusplus */

#include "c_str_result.h"
#include "c_str_span.h"

/*#include <azure/core/_az_cfg_prefix.h>*/

/**
 * @brief Convenience macro to return if an operation failed.
 */
#define _az_RETURN_IF_FAILED(exp)                                              \
  do {                                                                         \
    az_result const _az_result = (exp);                                        \
    if (az_result_failed(_az_result)) {                                        \
      return _az_result;                                                       \
    }                                                                          \
  } while (0)

/**
 * @brief Convenience macro to return if the provided span is not of the
 * expected, required size.
 */
#define _az_RETURN_IF_NOT_ENOUGH_SIZE(span, required_size)                     \
  do {                                                                         \
    size_t const _az_req_sz = (required_size);                                 \
    if (az_span_size(span) < _az_req_sz || _az_req_sz < 0) {                   \
      return AZ_ERROR_NOT_ENOUGH_SPACE;                                        \
    }                                                                          \
  } while (0)

/*#include <azure/core/_az_cfg_suffix.h>*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !C_STR_SPAN_RESULT_INTERNAL_H */
