// SPDX-License-Identifier: MIT
/*
 * Test main.
 *
 * Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *                    Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#include <kernel.h>

#include <unity_fixture.h>

#include <pbsys/status.h>

static void RunAllTests(void)
{
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
	RUN_TEST_GROUP(PUPDevice);
	RUN_TEST_GROUP(Motor);
	RUN_TEST_GROUP(ColorSensor);
	RUN_TEST_GROUP(ForceSensor);
	RUN_TEST_GROUP(UltrasonicSensor);
}

void TestMainTask(intptr_t exinf)
{
  // Prepare the pybricks runtime for running a user program.
  // TODO: integrate pbsys_status_set and wup_pybricks into one function. 
  pbsys_status_set(PBIO_PYBRICKS_STATUS_USER_PROGRAM_RUNNING);
  wup_pybricks();

  int argc = 2;
  const char **argv = {"asp3-primehub test", "-v"};

  // Wait 1 sec for console connecting.
  dly_tsk(1000*1000);

  UnityMain(argc, argv, RunAllTests);


}
