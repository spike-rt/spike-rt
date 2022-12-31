/*
 * SPDX-License-Identifier: MIT
 *
 * Tests for the hub built-in IMU.
 *
 * Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *            Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#include <kernel.h>

#include <unity.h>
#include <unity_fixture.h>

#include <spike/hub/imu.h>

TEST_GROUP(IMU);

TEST_GROUP_RUNNER(IMU)
{
  RUN_TEST_CASE(IMU, init);
  RUN_TEST_CASE(IMU, get_acceleration);
  RUN_TEST_CASE(IMU, get_angular_velocity);
  RUN_TEST_CASE(IMU, get_temperature);
}

TEST_SETUP(IMU)
{
}

TEST_TEAR_DOWN(IMU)
{
}

TEST(IMU, init)
{
  TEST_ASSERT_EQUAL(hub_imu_init(), PBIO_SUCCESS);
}

TEST(IMU, get_acceleration)
{
  float accel[3];

  TEST_ASSERT_EQUAL(hub_imu_init(), PBIO_SUCCESS);

  hub_imu_get_acceleration(accel);
  TEST_PRINTF("Acceleration (x, y, z) = (%f, %f, %f)\n", accel[0], accel[1], accel[2]);
}

TEST(IMU, get_angular_velocity)
{
  float angv[3];

  TEST_ASSERT_EQUAL(hub_imu_init(), PBIO_SUCCESS);

  hub_imu_get_angular_velocity(angv);
  TEST_PRINTF("Angular Velocity (x, y, z) = (%f, %f, %f)\n",
      angv[0], angv[1], angv[2]);
}

TEST(IMU, get_temperature)
{
  float temp;

  TEST_ASSERT_EQUAL(hub_imu_init(), PBIO_SUCCESS);

  temp = hub_imu_get_temperature();
  TEST_PRINTF("IMU Temperature : %f \n", temp);
}
