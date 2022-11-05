// SPDX-License-Identifier: MIT
/*
 * Tests for force sensors.
 *
 * Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *                    Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#include <kernel.h>

#include <unity.h>
#include <unity_fixture.h>

#include <test_config.h>
#include <spike/pup/forcesensor.h>

TEST_GROUP(ForceSensor);

/*
 * !!NOTE!!
 * This test cases require pushing force sensor's button while test,
 * otherwise tests will be failed.
 */
TEST_GROUP_RUNNER(ForceSensor) {
  RUN_TEST_CASE(ForceSensor, get_device);
  RUN_TEST_CASE(ForceSensor, force);
  RUN_TEST_CASE(ForceSensor, distance);
  RUN_TEST_CASE(ForceSensor, pressed);
  RUN_TEST_CASE(ForceSensor, touched);
}

TEST_SETUP(ForceSensor)
{
	TEST_MESSAGE("Please push force sensor button.");
}

TEST_TEAR_DOWN(ForceSensor)
{
}

TEST(ForceSensor, get_device)
{
  pup_device_t *button;

  button = pup_force_sensor_get_device(PBIO_PORT_ID_TEST_FORCE_SENSOR);
  TEST_ASSERT_NOT_NULL(button);
}

TEST(ForceSensor, force)
{
  pup_device_t *button;
  float force;

  button = pup_force_sensor_get_device(PBIO_PORT_ID_TEST_FORCE_SENSOR);
  TEST_ASSERT_NOT_NULL(button);
  
  force = pup_force_sensor_force(button);
  TEST_PRINTF("Force : %f N\n", force);
  TEST_ASSERT_GREATER_THAN(0, force);
}

TEST(ForceSensor, distance)
{
  pup_device_t *button;
  float distance;

  button = pup_force_sensor_get_device(PBIO_PORT_ID_TEST_FORCE_SENSOR);
  TEST_ASSERT_NOT_NULL(button);
  
  distance = pup_force_sensor_distance(button);
  TEST_PRINTF("Distance : %f mm\n", distance);
  TEST_ASSERT_GREATER_THAN(0, distance);
}

TEST(ForceSensor, pressed)
{
  pup_device_t *button;
  bool pressed;

  button = pup_force_sensor_get_device(PBIO_PORT_ID_TEST_FORCE_SENSOR);
  TEST_ASSERT_NOT_NULL(button);
  
  pressed = pup_force_sensor_pressed(button, 3);
  TEST_PRINTF("Pressed : %d \n", (int) pressed);
  TEST_ASSERT_TRUE(pressed);
}

TEST(ForceSensor, touched)
{
  pup_device_t *button;
	bool touched;

  button = pup_force_sensor_get_device(PBIO_PORT_ID_TEST_FORCE_SENSOR);
  TEST_ASSERT_NOT_NULL(button);

	touched = pup_force_sensor_touched(button);
  TEST_PRINTF("Touched : %d \n", (int) touched);
	TEST_ASSERT_TRUE(touched);
}
