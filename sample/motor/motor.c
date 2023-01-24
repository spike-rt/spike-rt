/*
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2022-2023 Embedded and Real-Time Systems Laboratory,
 *            Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#include <kernel.h>
#include <kernel_cfg.h>
#include <t_syslog.h>
#include <stdio.h>
#include <serial/serial.h>
#include <spike/hub/system.h>
#include <spike/pup/motor.h>
#include "app_config.h"
#include "motor.h"


/*
 * Application Main Task
 */
void
main_task(intptr_t exinf)
{
  pbio_error_t err;
  pup_motor_t *motor;
  
  dly_tsk(3*1000*1000);
  
  // 以下のようにNewlib経由でシリアル入出力できる．
  //extern FILE* serial_open_newlib_file(ID portid);
  //FILE *fd = serial_open_newlib_file(SIO_USB_PORTID);
  //fprintf(fd, "Set Up Motor\n");

  syslog(LOG_NOTICE, "Set Up Motor\n");

  // Get pointer to servo
  motor = pup_motor_get_device(PBIO_PORT_ID_MOTOR);
  if(motor == NULL) {
    hub_system_shutdown();
  }
  
  bool reset_count = true;
  err = pup_motor_setup(motor, PUP_DIRECTION_CLOCKWISE, reset_count);
  if(motor == NULL) {
    hub_system_shutdown();
  }
  
  err = pup_motor_set_speed(motor, 300);
  if (err != PBIO_SUCCESS) {
    hub_system_shutdown();
  }
  dly_tsk(3*1000*1000);

  hub_system_shutdown();
}
