#include <kernel.h>
#include <t_syslog.h>

#include <unity.h>
#include <unity_fixture.h>

#include <cbricks/pup/ultrasonicsensor.h>
#include <pbsys/user_program.h>

TEST_GROUP(UltrasonicSensor);

TEST_GROUP_RUNNER(UltrasonicSensor) {
  RUN_TEST_CASE(UltrasonicSensor, get_device);
  RUN_TEST_CASE(UltrasonicSensor, distance);
  RUN_TEST_CASE(UltrasonicSensor, presence);
  RUN_TEST_CASE(UltrasonicSensor, light);
}

TEST_SETUP(UltrasonicSensor)
{
  // pbsys_user_program_prepare(NULL); // pbsys_processをユーザプログラム実行状態に遷移させる．
}

TEST_TEAR_DOWN(UltrasonicSensor)
{
  // pbsys_user_program_unprepare();
}

TEST(UltrasonicSensor, get_device)
{
  pup_device_t *eyes;

  eyes = UltrasonicSensor_init(PBIO_PORT_ID_C);
  TEST_ASSERT_NOT_NULL(eyes);
}

TEST(UltrasonicSensor, distance)
{
  pup_device_t *eyes;
  int distance;

  eyes = UltrasonicSensor_init(PBIO_PORT_ID_C);
  TEST_ASSERT_NOT_NULL(eyes);
  
  distance = UltrasonicSensor_distance(eyes);
  TEST_PRINTF("Distance : %d mm\n", distance);
}

TEST(UltrasonicSensor, presence)
{
  pup_device_t *eyes;
  int presence;

  eyes = UltrasonicSensor_init(PBIO_PORT_ID_C);
  TEST_ASSERT_NOT_NULL(eyes);
  
  presence = UltrasonicSensor_presence(eyes);
  TEST_PRINTF("Presence : %d \n", presence);
  // TEST_ASSERT_TRUE(presence);
}

TEST(UltrasonicSensor, light)
{
  pup_device_t *eyes;

  eyes = UltrasonicSensor_init(PBIO_PORT_ID_C);
  TEST_ASSERT_NOT_NULL(eyes);

  UltrasonicSensor_light_on(eyes);
  // Wait 1 sec
  dly_tsk(1000000);

  UltrasonicSensor_light_off(eyes);
}
