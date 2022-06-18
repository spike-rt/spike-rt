// SPDX-License-Identifier: MIT
/* 
 * Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *                    Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#ifndef _CBRICKS_CB_ERROR_H_
#define _CBRICKS_CB_ERROR_H_

#include <pbio/error.h>

#define check_pbio_error_r(err, retval) \
  if (err != PBIO_SUCCESS) { \
    return retval; \
  }

#define check_pbio_error(err) check_pbio_error_r(err, err)

#endif // _CBRICKS_CB_ERROR_H_
