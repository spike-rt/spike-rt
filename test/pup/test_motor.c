// SPDX-License-Identifier: MIT
/*
 * Tests for servo motors.
 *
 * Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *                    Graduate School of Information Science, Nagoya Univ., JAPAN
 */


#include <kernel.h>
#include <t_syslog.h>

#include <unity.h>
#include <unity_fixture.h>

#include <pbsys/user_program.h>
#include <cbricks/pup/motor.h>

TEST_GROUP(Motor);

TEST_GROUP_RUNNER(Motor) {
  RUN_TEST_CASE(Motor, Run);
}

TEST_SETUP(Motor)
{
  // Prepare the pybricks runtime for running a user program.
  // pbsys_user_program_prepare(NULL);
}

TEST_TEAR_DOWN(Motor)
{
  // Perform cleanup/reset after running a user program.
  // pbsys_user_program_unprepare();
}
TEST(Motor, Run)
{
  pbio_error_t err;
  pup_motor_t *motor;
  
  dly_tsk(3000000);

  // Get pointer to servo
  motor = pup_motor_get_device(PBIO_PORT_ID_A);
  TEST_ASSERT_NOT_NULL(motor);
  
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
  TEST_ASSERT_NOT_EQUAL(err, PBIO_ERROR_NO_DEV);
  TEST_ASSERT_EQUAL(err, PBIO_SUCCESS);
  
  int32_t old_value = pup_motor_set_duty_limit(motor, 30);

  TEST_ASSERT_EQUAL(PBIO_SUCCESS, pup_motor_set_speed(motor, 500));
 
  for (int i = 0; i < 10; i++) { 
    int32_t speed = pup_motor_get_speed(motor);
    int32_t count = pup_motor_get_count(motor);
    bool stalled = pup_motor_is_stalled(motor);
    TEST_PRINTF("speed = %d, count = %d stalled = %c\n", (int) speed, (int) count, stalled ? 'T' : 'F');
    if (stalled) break;
    // Wait 1 sec
    dly_tsk(1000000);
  }
  pup_motor_restore_duty_limit(motor, old_value);

  TEST_PRINTF("%s\n", "BRAKE");
  TEST_ASSERT_EQUAL(PBIO_SUCCESS, pup_motor_brake(motor));
  dly_tsk(3000000);

  TEST_PRINTF("%s\n", "HOLD");
  TEST_ASSERT_EQUAL(PBIO_SUCCESS, pup_motor_hold(motor));
  dly_tsk(3000000);

  TEST_ASSERT_EQUAL(PBIO_SUCCESS, pup_motor_reset_count(motor));
  TEST_ASSERT_EQUAL(0, pup_motor_get_count(motor));

  TEST_PRINTF("%s\n", "STOP");
  TEST_ASSERT_EQUAL(PBIO_SUCCESS, pup_motor_stop(motor));
  dly_tsk(3000000);

  TEST_PRINTF("%s\n", "DONE");
}
