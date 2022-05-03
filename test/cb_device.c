#include <kernel.h>

#include <unity.h>
#include <unity_fixture.h>

#include <cbricks/cb_device.h>

TEST_GROUP(PUPDevice);


TEST_GROUP_RUNNER(PUPDevice) {
  RUN_TEST_CASE(PUPDevice, get_device);
}

TEST_SETUP(PUPDevice)
{
  // pbsys_user_program_prepare(NULL); // pbsys_processをユーザプログラム実行状態に遷移させる．
}

TEST_TEAR_DOWN(PUPDevice)
{
  // pbsys_user_program_unprepare();
}

TEST(PUPDevice, get_device)
{
  pb_device_t *pbdev;

  dly_tsk(3000000);

  pbdev = pb_device_get_device(PBIO_PORT_ID_B, PBIO_IODEV_TYPE_ID_SPIKE_FORCE_SENSOR);
  TEST_ASSERT_NOT_NULL(pbdev);
}
