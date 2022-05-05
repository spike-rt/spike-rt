#include <kernel.h>
#include <t_syslog.h>

#include <unity.h>
#include <unity_fixture.h>

#include <pbsys/user_program.h>
#include <pbio/motor_process.h>
#include <pbio/servo.h>

TEST_GROUP(Motor);

TEST_GROUP_RUNNER(Motor) {
  RUN_TEST_CASE(Motor, Run);
}

TEST_SETUP(Motor)
{
  // pbsys_user_program_prepare(NULL); // pbsys_processをユーザプログラム実行状態に遷移させる．
}

TEST_TEAR_DOWN(Motor)
{
  // pbsys_user_program_unprepare();
}

TEST(Motor, Run)
{
  pbio_error_t err;
  pbio_servo_t *servo;
  
  dly_tsk(3000000);

  // Get pointer to servo
  TEST_ASSERT_EQUAL(pbio_motor_process_get_servo(PBIO_PORT_ID_A, &servo), PBIO_SUCCESS);
  
  // Set up servo
  for(int i = 0; i < 10; i++)
  {
    err = pbio_servo_setup(servo, PBIO_DIRECTION_CLOCKWISE, F16C(1, 0), false);
    if(err != PBIO_ERROR_AGAIN)
      break;
    
    // Wait 1s and try one more
    dly_tsk(1000000);
  }
  TEST_ASSERT_NOT_EQUAL(err, PBIO_ERROR_NO_DEV);
  TEST_ASSERT_EQUAL(err, PBIO_SUCCESS);
  
  TEST_ASSERT_EQUAL(pbio_servo_run(servo, 500), PBIO_SUCCESS);
  
  // Wait 1 sec
  dly_tsk(1000000);
  TEST_ASSERT_EQUAL(pbio_servo_stop(servo, PBIO_DCMOTOR_COAST), PBIO_SUCCESS);
}
