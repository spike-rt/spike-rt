// SPDX-License-Identifier: MIT
/*
 * Tests for the battery.
 *
 * Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *                    Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#include <kernel.h>

#include <unity.h>
#include <unity_fixture.h>

#include <cbricks/hub/battery.h>

TEST_GROUP(Battery);

TEST_GROUP_RUNNER(Battery) {
  RUN_TEST_CASE(Battery, get_voltage);
  RUN_TEST_CASE(Battery, get_current);
}

TEST_SETUP(Battery)
{
  // Prepare the pybricks runtime for running a user program.
  // pbsys_user_program_prepare(NULL);
}

TEST_TEAR_DOWN(Battery)
{
  // Perform cleanup/reset after running a user program.
  // pbsys_user_program_unprepare();
}

TEST(Battery, get_voltage)
{
  uint16_t voltage = hub_battery_get_voltage();
 // TEST_PRINTF("Voltage = %d mV\n", (int) voltage);
  TEST_ASSERT_GREATER_THAN(0, voltage);
}

TEST(Battery, get_current)
{
  uint16_t current = hub_battery_get_current();
 // TEST_PRINTF("Current = %d mA\n", (int) current);
  TEST_ASSERT_GREATER_THAN(0, current);
}
