// SPDX-License-Identifier: MIT
/*
 * Test main.
 *
 * Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *                    Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#include <kernel.h>

#include <unity_fixture.h>
#include <stdio.h>

#include <pbsys/status.h>

#include <test_config.h>

static void RunAllTests(void)
{
  // Tests for Serial Output
	//RUN_TEST_GROUP(UsartSerial);
	RUN_TEST_GROUP(BluetoothSerial);

  // Tests for Hub Device
	RUN_TEST_GROUP(Light);
	RUN_TEST_GROUP(Display);
	RUN_TEST_GROUP(Button);
	// RUN_TEST_GROUP(IMU);
	// RUN_TEST_GROUP(Speaker);
	// RUN_TEST_GROUP(System);
	RUN_TEST_GROUP(Battery);
	// RUN_TEST_GROUP(Charger);

  // Tests for Powerd Up Device
	//RUN_TEST_GROUP(PUPDevice);
	RUN_TEST_GROUP(Motor);
	RUN_TEST_GROUP(ColorSensor);
	RUN_TEST_GROUP(ForceSensor);
	RUN_TEST_GROUP(UltrasonicSensor);
}

void TestMainTask(intptr_t exinf)
{
  int argc = 2;
  const char *argv[] = {"SPIKE-RT test", "-v"};

  // Wait 1 sec for console connecting.
  dly_tsk(1*1000*1000);

  UnityMain(argc, argv, RunAllTests);

  pbsys_status_clear(PBIO_PYBRICKS_STATUS_USER_PROGRAM_RUNNING);
  void wup_pybricks(void);
  wup_pybricks();
}
