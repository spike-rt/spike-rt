#include <kernel.h>
#include <t_syslog.h>

#include <unity.h>
#include <unity_fixture.h>

#include <cbricks/cb_device.h>
#include <pbsys/user_program.h>
#include <pbsys/light.h>
#include <pbio/light.h>

TEST_GROUP(UltrasonicSensor);

TEST_GROUP_RUNNER(UltrasonicSensor) {
  RUN_TEST_CASE(UltrasonicSensor, init);
  RUN_TEST_CASE(UltrasonicSensor, distance);
  RUN_TEST_CASE(UltrasonicSensor, presence);
}

TEST_SETUP(UltrasonicSensor)
{
  // pbsys_user_program_prepare(NULL); // pbsys_processをユーザプログラム実行状態に遷移させる．
}

TEST_TEAR_DOWN(UltrasonicSensor)
{
  // pbsys_user_program_unprepare();
}

TEST(UltrasonicSensor, init)
{
  pb_device_t *eyes;

  eyes = UltrasonicSensor_init(PBIO_PORT_ID_C);

  TEST_ASSERT_NOT_NULL(eyes);
}

TEST(UltrasonicSensor, distance)
{
  pb_device_t *eyes;
  int distance;

  eyes = UltrasonicSensor_init(PBIO_PORT_ID_C);

  TEST_ASSERT_NOT_NULL(eyes);
  
  distance = UltrasonicSensor_distance(eyes);
  TEST_PRINTF("Distance : %d mm\n", distance);
}

TEST(UltrasonicSensor, presence)
{
  pb_device_t *eyes;
  int presence;

  eyes = UltrasonicSensor_init(PBIO_PORT_ID_C);

  TEST_ASSERT_NOT_NULL(eyes);
  
  presence = UltrasonicSensor_presence(eyes);
  TEST_PRINTF("Presence : %d \n", presence);
  // TEST_ASSERT_TRUE(presence);
}
