// SPDX-License-Identifier: MIT
/*
 * Tests for color sensors.
 *
 * Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *                    Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#include <kernel.h>

#include <unity.h>
#include <unity_fixture.h>

#include <test_config.h>
#include <cbricks/pup/colorsensor.h>
#include <pbsys/user_program.h>

TEST_GROUP(ColorSensor);

TEST_GROUP_RUNNER(ColorSensor) {
  RUN_TEST_CASE(ColorSensor, get_device);
  RUN_TEST_CASE(ColorSensor, hsv);
  RUN_TEST_CASE(ColorSensor, color);
  RUN_TEST_CASE(ColorSensor, reflection);
  RUN_TEST_CASE(ColorSensor, ambient);
  RUN_TEST_CASE(ColorSensor, light);
}

TEST_SETUP(ColorSensor)
{
  // Prepare the pybricks runtime for running a user program.
  // pbsys_user_program_prepare(NULL);
}

TEST_TEAR_DOWN(ColorSensor)
{
  // Perform cleanup/reset after running a user program.
  // pbsys_user_program_unprepare();
}

TEST(ColorSensor, get_device)
{
  pup_device_t *sensor;

  sensor = pup_color_sensor_get_device(PBIO_PORT_ID_TEST_COLOR_SENSOR);
  TEST_ASSERT_NOT_NULL(sensor);
}

TEST(ColorSensor, hsv)
{
  pup_device_t *sensor;
  pbio_color_hsv_t hsv;

  sensor = pup_color_sensor_get_device(PBIO_PORT_ID_TEST_COLOR_SENSOR);
  TEST_ASSERT_NOT_NULL(sensor);
  
  hsv = pup_color_sensor_hsv(sensor, true);
  //TEST_PRINTF("hsv reflected : h : %u  s : %u v : %u\n", hsv.h, hsv.s, hsv.v);
	TEST_ASSERT_FALSE(hsv.h == 0 && hsv.s == 0 && hsv.v == 0);

  hsv = pup_color_sensor_hsv(sensor, false);
  //TEST_PRINTF("hsv ambient : h : %u  s : %u v : %u\n", hsv.h, hsv.s, hsv.v);
	TEST_ASSERT_FALSE(hsv.h == 0 && hsv.s == 0 && hsv.v == 0);
}

TEST(ColorSensor, color)
{
  pup_device_t *sensor;
  pbio_color_hsv_t hsv;

  sensor = pup_color_sensor_get_device(PBIO_PORT_ID_TEST_COLOR_SENSOR);
  TEST_ASSERT_NOT_NULL(sensor);

  hsv = pup_color_sensor_color(sensor, true);
  TEST_PRINTF("color reflected : h : %u  s : %u v : %u\n", hsv.h, hsv.s, hsv.v);
	//Since hsv = {0, 0, 0} means it detects NONE, 
	//this may cause error, even if pup_color_sensor_color() success.
	TEST_ASSERT_FALSE(hsv.h == 0 && hsv.s == 0 && hsv.v == 0);

  hsv = pup_color_sensor_color(sensor, false);
  TEST_PRINTF("color ambient : h : %u  s : %u v : %u\n", hsv.h, hsv.s, hsv.v);
	TEST_ASSERT_FALSE(hsv.h == 0 && hsv.s == 0 && hsv.v == 0);
}



TEST(ColorSensor, reflection)
{
  pup_device_t *sensor;
	int32_t reflection;

  sensor = pup_color_sensor_get_device(PBIO_PORT_ID_TEST_COLOR_SENSOR);
  TEST_ASSERT_NOT_NULL(sensor);
  
  reflection = pup_color_sensor_reflection(sensor);
  //TEST_PRINTF("reflection : %d \n", reflection);
  TEST_ASSERT_GREATER_THAN(0, reflection);
}

TEST(ColorSensor, ambient)
{
  pup_device_t *sensor;
	int32_t ambient;

  sensor = pup_color_sensor_get_device(PBIO_PORT_ID_TEST_COLOR_SENSOR);
  TEST_ASSERT_NOT_NULL(sensor);
  
  ambient = pup_color_sensor_ambient(sensor);
  //TEST_PRINTF("ambient : %d \n", ambient);
	TEST_ASSERT_GREATER_THAN(0, ambient);
}


TEST(ColorSensor, light)
{
  pup_device_t *sensor;
  pbio_error_t err;

  sensor = pup_color_sensor_get_device(PBIO_PORT_ID_TEST_COLOR_SENSOR);
  TEST_ASSERT_NOT_NULL(sensor);

  err = pup_color_sensor_light_on(sensor);
  TEST_ASSERT_EQUAL(err, PBIO_SUCCESS);

  // Wait 1 sec for visual confirmation the three lights are lit.
  dly_tsk(1000000);

  err = pup_color_sensor_light_off(sensor);
  TEST_ASSERT_EQUAL(err, PBIO_SUCCESS);
}
