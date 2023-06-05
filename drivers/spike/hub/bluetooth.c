// SPDX-License-Identifier: MIT
/*
 * API for the bluetooth.
 *
 * Copyright (c) 2023 Embedded and Real-Time Systems Laboratory,
 *            Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#include <kernel.h>
#include <spike/hub/bluetooth.h>
#include <pbdrv/bluetooth.h>

bool hub_bluetooth_advertising_enabled = false;

extern struct process *pbsys_bluetooth_process;

pbio_error_t hub_bluetooth_enable_advertising(void) {
  loc_cpu(); // TODO: Error Handling
  hub_bluetooth_advertising_enabled = true;
  process_poll(&pbsys_bluetooth_process);
  unl_cpu(); // TODO: Error Handling
  return PBIO_SUCCESS;
}

pbio_error_t hub_bluetooth_disable_advertising(void) {
  loc_cpu(); // TODO: Error Handling
  hub_bluetooth_advertising_enabled = false;
  process_poll(&pbsys_bluetooth_process);
  unl_cpu(); // TODO: Error Handling
  return PBIO_SUCCESS;
}

pbio_error_t hub_bluetooth_is_connected(bool *connected) {
  *connected = pbdrv_bluetooth_is_connected(PBDRV_BLUETOOTH_CONNECTION_LE);
  return PBIO_SUCCESS;
}
