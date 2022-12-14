/*
 * SPDX-License-Identifier: MIT
 *
 * Test main.
 *
 * Copyright (c) 2022-2023 Embedded and Real-Time Systems Laboratory,
 *            Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#include <kernel.h>

#include <unity.h>
#include <unity_fixture.h>

#include <spike/hub/system.h>

#include <test_config.h>


static void RunAllTests(void)
{
  // Tests for Serial Output
  //RUN_TEST_GROUP(UsartSerial);
  //RUN_TEST_GROUP(SerialAsyncPort);
  RUN_TEST_GROUP(USBSerial);
  RUN_TEST_GROUP(BluetoothSerial);

  // Tests for Hub Device
  RUN_TEST_GROUP(Light);
  RUN_TEST_GROUP(Display);
  RUN_TEST_GROUP(Button);
  RUN_TEST_GROUP(IMU);
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

#if UNITY_OUTPUT_VIA_SERIAL_ADAPTER
  serial_opn_por(SIO_UNITY_PORTID);
#endif

  // Wait 1 sec for console connecting.
  dly_tsk(1*1000*1000);

  UnityMain(argc, argv, RunAllTests);

  exit(0);
  //hub_system_shutdown();
}
