/*
 * SPDX-License-Identifier: MIT
 *
 * API for the hub system.
 *
 * Copyright (c) 2023 Embedded and Real-Time Systems Laboratory,
 *            Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#include <t_syslog.h>
#include <spike/hub/system.h>

/*
 * TODO: 
 *  - Check context
 *  - Close serial port
 */
void hub_system_shutdown(void) {
  void pybricks_c_pb_type_System_shutdown(void);
  pybricks_c_pb_type_System_shutdown();

  /* Never come back here */
  while(1);
}
