// SPDX-License-Identifier: MIT
/*
 * API for the hub battery.
 *
 * Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *                    Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#include <t_syslog.h>
#include <cbricks/hub/battery.h>

#include <pbdrv/battery.h>


uint16_t hub_battery_get_voltage(void) {
  uint16_t voltage;
  pbio_error_t err = pbdrv_battery_get_voltage_now(&voltage);
  if (err != PBIO_SUCCESS) {
    syslog(LOG_ERROR, "hub_battery_get_voltage() failed.");
    return 0;
  }
  return voltage;
}

uint16_t hub_battery_get_current(void) {
  uint16_t current;
  pbio_error_t err = pbdrv_battery_get_current_now(&current);
  if (err != PBIO_SUCCESS) {
    syslog(LOG_ERROR, "hub_battery_get_current() failed.");
    return 0;
  }
  return current;
}

