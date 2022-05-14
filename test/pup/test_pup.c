#include <kernel.h>

#include <unity.h>
#include <unity_fixture.h>

#include <test_config.h>
#include <cbricks/pup_device.h>

TEST_GROUP(PUPDevice);


TEST_GROUP_RUNNER(PUPDevice) {
  RUN_TEST_CASE(PUPDevice, get_device);
}

TEST_SETUP(PUPDevice)
{
  // pbsys_user_program_prepare(NULL); // pbsys_processをユーザプログラム実行状態に遷移させる．

  dly_tsk(3000000);
}

TEST_TEAR_DOWN(PUPDevice)
{
  // pbsys_user_program_unprepare();
}

TEST(PUPDevice, get_device)
{
  pup_device_t *pdev;

  pdev = pup_device_get_device(PBIO_PORT_ID_TEST_PUP_DEVICE, PBIO_IODEV_TYPE_ID_TEST_PUP_DEVICE);
  TEST_ASSERT_NOT_NULL(pdev);
}
