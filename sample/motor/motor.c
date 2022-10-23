// SPDX-License-Identifier: MIT
/*
 * Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *                    Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#include <t_syslog.h>
#include "kernel_cfg.h"
#include "motor.h"
#include "cbricks/pup/motor.h"

//#include <pbsys/user_program.h>

/*
 * Main Task
 */
#include <stdio.h>
void
main_task(intptr_t exinf)
{
  //  pbsys_user_program_prepare(NULL); // pbsys_processをユーザプログラム実行状態に遷移させる．

  pbio_error_t err;
  pup_motor_t *motor;
  
  dly_tsk(3000000);
  // printf("Set Up Motor\n");

  // Get pointer to servo
  motor = pup_motor_get_device(PBIO_PORT_ID_A);
  if(motor == NULL)
  {

  }
  
  // Set up servo
  for(int i = 0; i < 10; i++)
  {
    bool reset_count = true;
    err = pup_motor_setup(motor, PUP_DIRECTION_CLOCKWISE, reset_count);
    if(err != PBIO_ERROR_AGAIN)
      break;
    
    // Wait 1s and try one more
    dly_tsk(1000000);
  }
  // TEST_ASSERT_NOT_EQUAL(err, PBIO_ERROR_NO_DEV);
  // TEST_ASSERT_EQUAL(err, PBIO_SUCCESS);
  
  pup_motor_set_duty_limit(motor, 30);

  err = pup_motor_set_speed(motor, 500);

  if (err != PBIO_SUCCESS)
  {
  }
 

  while (1)
  {
    slp_tsk();
  }
  //  pbsys_user_program_unprepare();
}
