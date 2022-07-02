// SPDX-License-Identifier: MIT
/*
 * API for the hub built-in status light.
 *
 * Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *                    Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#include <t_syslog.h>
#include <cbricks/hub/light.h>

#include <pbsys/light.h>
#include <pbio/light.h>


/*
 * We implement light API by just wrapping functions in external/libpybricks/lib/pbio/src/light/color_light.c.
 * However, we don't support animation functions currently.
 */

pbio_error_t hub_light_on_hsv(const pbio_color_hsv_t *hsv) {
  return pbio_color_light_on_hsv(pbsys_status_light, hsv);
}

pbio_error_t hub_light_on_color(pbio_color_t color) {
  return pbio_color_light_on(pbsys_status_light, color);
}

pbio_error_t hub_light_off(void) {
  return pbio_color_light_off(pbsys_status_light);
}
