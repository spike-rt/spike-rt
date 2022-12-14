// SPDX-License-Identifier: MIT
/*
 * Tests for the bluetooth.
 *
 * Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *            Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#include <kernel.h>
#include <syssvc/serial.h>

#include <unity.h>
#include <unity_fixture.h>

#include <pbsys/status.h>
#include <test.h>

TEST_GROUP(BluetoothSerial);

TEST_GROUP_RUNNER(BluetoothSerial) {
  RUN_TEST_CASE(BluetoothSerial, PrintString);
  RUN_TEST_CASE(BluetoothSerial, PrintManyStrings);
}

TEST_SETUP(BluetoothSerial)
{
}

TEST_TEAR_DOWN(BluetoothSerial)
{
}

TEST(BluetoothSerial, PrintString)
{
  ER ercd;
  const char str[] = "PrintString Test via Bluetooth.\r\n";

  TEST_MESSAGE("Please connect bluetooth.");

  dly_tsk(5*1000*1000);
  
  // Wait until bluetooth is connected.
	ercd = serial_opn_por(BLUETOOTH_PORTID);
  TEST_ASSERT_GREATER_OR_EQUAL(E_OK, ercd);
  
  ercd = serial_wri_dat(BLUETOOTH_PORTID, str, sizeof(str));
  TEST_ASSERT_GREATER_OR_EQUAL(E_OK, ercd);

  dly_tsk(1*1000*1000);
}

TEST(BluetoothSerial, PrintManyStrings)
{
  ER ercd;
  const char str[] = "PrintManyStrings Test via Bluetooth.\r\n";

  //TEST_MESSAGE("Please connect bluetooth.");
  //dly_tsk(5*1000*1000);
  
  // Wait until bluetooth is connected.
	ercd = serial_opn_por(BLUETOOTH_PORTID);
  //TEST_ASSERT_GREATER_OR_EQUAL(E_OK, ercd);
  
  for (int i=0; i<5; i++)
  {
    ercd = serial_wri_dat(BLUETOOTH_PORTID, str, sizeof(str));
    TEST_ASSERT_GREATER_OR_EQUAL(E_OK, ercd);
  }
  dly_tsk(1*1000*1000);
}
