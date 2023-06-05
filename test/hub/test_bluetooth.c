// SPDX-License-Identifier: MIT
/*
 * Tests for the bluetooth.
 *
 * Copyright (c) 2023 Embedded and Real-Time Systems Laboratory,
 *            Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#include <kernel.h>

#include <unity.h>
#include <unity_fixture.h>

#include <spike/hub/bluetooth.h>

TEST_GROUP(Bluetooth);

TEST_GROUP_RUNNER(Bluetooth) {
  RUN_TEST_CASE(Bluetooth, enable_advertising);
  RUN_TEST_CASE(Bluetooth, disable_advertising);
  RUN_TEST_CASE(Bluetooth, is_connected);
}

TEST_SETUP(Bluetooth)
{
}

TEST_TEAR_DOWN(Bluetooth)
{
}

TEST(Bluetooth, enable_advertising)
{
  pbio_error_t err;
  err = hub_bluetooth_enable_advertising();
  TEST_ASSERT_EQUAL(PBIO_SUCCESS, err);
  
  // Wait 10 sec for confirmation.
  dly_tsk(10*1000*1000);
}

TEST(Bluetooth, disable_advertising)
{
  pbio_error_t err;
  err = hub_bluetooth_disable_advertising();
  TEST_ASSERT_EQUAL(PBIO_SUCCESS, err);

  // Wait 10 sec for confirmation.
  dly_tsk(10*1000*1000);
}

TEST(Bluetooth, is_connected)
{
  bool connected;
  pbio_error_t err;
  err = hub_bluetooth_is_connected(&connected);
  TEST_ASSERT_EQUAL(PBIO_SUCCESS, err);
  TEST_PRINTF("Bluetooth is connected : %d\n", connected);
}
