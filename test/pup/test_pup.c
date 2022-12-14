// SPDX-License-Identifier: MIT
/*
 * Tests for controlling PUP devices.
 *
 * Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *                    Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#include <kernel.h>

#include <unity.h>
#include <unity_fixture.h>

#include <test_config.h>
#include <spike/pup_device.h>

TEST_GROUP(PUPDevice);


TEST_GROUP_RUNNER(PUPDevice) {
  RUN_TEST_CASE(PUPDevice, get_device);
}

TEST_SETUP(PUPDevice)
{
  // Prepare the pybricks runtime for running a user program.
  // pbsys_user_program_prepare(NULL);
}

TEST_TEAR_DOWN(PUPDevice)
{
  // Perform cleanup/reset after running a user program.
  // pbsys_user_program_unprepare();
}

TEST(PUPDevice, get_device)
{
  pup_device_t *pdev;

  pdev = pup_device_get_device(PBIO_PORT_ID_TEST_PUP_DEVICE, PBIO_IODEV_TYPE_ID_TEST_PUP_DEVICE);
  TEST_ASSERT_NOT_NULL(pdev);
}
