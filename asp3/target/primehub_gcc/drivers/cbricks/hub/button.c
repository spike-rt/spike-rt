// SPDX-License-Identifier: MIT
/*
 * API for the hub built-in button.
 *
 * Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *                    Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#include <t_syslog.h>
#include <cbricks/hub/button.h>

#include <pbdrv/button.h>


/*
 * We implement button API by just wrapping functions in external/libpybricks/lib/pbio/drv/button/button_gpio.c.
 */

pbio_error_t hub_button_is_pressed(hub_button_t *pressed) {
  return pbdrv_button_is_pressed(pressed);
}


