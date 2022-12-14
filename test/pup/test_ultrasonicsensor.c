// SPDX-License-Identifier: MIT
/*
 * Tests for ultrasonic sensors.
 *
 * Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *                    Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#include <kernel.h>

#include <unity.h>
#include <unity_fixture.h>

#include <test_config.h>
#include <spike/pup/ultrasonicsensor.h>

TEST_GROUP(UltrasonicSensor);

TEST_GROUP_RUNNER(UltrasonicSensor) {
  RUN_TEST_CASE(UltrasonicSensor, get_device);
  RUN_TEST_CASE(UltrasonicSensor, distance);
  RUN_TEST_CASE(UltrasonicSensor, presence);
  RUN_TEST_CASE(UltrasonicSensor, light);
}

TEST_SETUP(UltrasonicSensor)
{
}

TEST_TEAR_DOWN(UltrasonicSensor)
{
}

TEST(UltrasonicSensor, get_device)
{
  pup_device_t *eyes;

  eyes = pup_ultrasonic_sensor_get_device(PBIO_PORT_ID_TEST_ULTRASONIC_SENSOR);
  TEST_ASSERT_NOT_NULL(eyes);
}

TEST(UltrasonicSensor, distance)
{
  pup_device_t *eyes;
  int distance;

  eyes = pup_ultrasonic_sensor_get_device(PBIO_PORT_ID_TEST_ULTRASONIC_SENSOR);
  TEST_ASSERT_NOT_NULL(eyes);
  
  distance = pup_ultrasonic_sensor_distance(eyes);
  // TEST_PRINTF("Distance : %d mm\n", distance);
  TEST_ASSERT_GREATER_THAN(0, distance);
}

TEST(UltrasonicSensor, presence)
{
  pup_device_t *eyes;
  int presence;

  eyes = pup_ultrasonic_sensor_get_device(PBIO_PORT_ID_TEST_ULTRASONIC_SENSOR);
  TEST_ASSERT_NOT_NULL(eyes);
  
  presence = pup_ultrasonic_sensor_presence(eyes);
  // TEST_PRINTF("Presence : %d \n", presence);
  TEST_ASSERT_FALSE(presence);
}

TEST(UltrasonicSensor, light)
{
  pup_device_t *eyes;
  pbio_error_t err;

  eyes = pup_ultrasonic_sensor_get_device(PBIO_PORT_ID_TEST_ULTRASONIC_SENSOR);
  TEST_ASSERT_NOT_NULL(eyes);

  err = pup_ultrasonic_sensor_light_on(eyes);
  TEST_ASSERT_EQUAL(err, PBIO_SUCCESS);

  // Wait 1 sec for visual confirmation the four lights are lit.
  dly_tsk(1000000);

  err = pup_ultrasonic_sensor_light_off(eyes);
  TEST_ASSERT_EQUAL(err, PBIO_SUCCESS);
}
