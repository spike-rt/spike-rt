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

#include <spike/hub/bluetooth.h>
#include <test.h>

TEST_GROUP(BluetoothSerial);

TEST_GROUP_RUNNER(BluetoothSerial) {
  hub_bluetooth_enable_advertising();

  RUN_TEST_CASE(BluetoothSerial, Write);
  RUN_TEST_CASE(BluetoothSerial, WriteLargerThanBuffer);
  RUN_TEST_CASE(BluetoothSerial, Read);
  //TODO:RUN_TEST_CASE(BluetoothSerial, ReadLargerThanBuffer);
}

TEST_SETUP(BluetoothSerial)
{
}

TEST_TEAR_DOWN(BluetoothSerial)
{
}

TEST(BluetoothSerial, Write)
{
  ER ercd;
  const char str[] = "Write Data Test via Bluetooth.\n";

  TEST_MESSAGE("Please connect bluetooth.");

  // Wait until bluetooth is connected.
  ercd = serial_opn_por(SIO_BLUETOOTH_PORTID);
  
  ercd = serial_wri_dat(SIO_BLUETOOTH_PORTID, str, sizeof(str));
  TEST_ASSERT_GREATER_OR_EQUAL(E_OK, ercd);

  dly_tsk(1*1000*1000);
}

TEST(BluetoothSerial, WriteLargerThanBuffer)
{
  ER ercd;
  const char str[] = "Write Larger Data than Buffer Test via Bluetooth.\n";

  ercd = serial_opn_por(SIO_BLUETOOTH_PORTID);
  //TEST_ASSERT_GREATER_OR_EQUAL(E_OK, ercd);
  
  for (int i=0; i<10; i++)
  {
    ercd = serial_wri_dat(SIO_BLUETOOTH_PORTID, str, sizeof(str));
    TEST_ASSERT_GREATER_OR_EQUAL(E_OK, ercd);
  }
  dly_tsk(1*1000*1000);
}

TEST(BluetoothSerial, Read)
{
  ER ercd;
  char input[10];

	ercd = serial_opn_por(SIO_BLUETOOTH_PORTID);
  //TEST_ASSERT_GREATER_OR_EQUAL(E_OK, ercd);
  
  const char msg1[] = "Input 10 Characters:\n";
  ercd = serial_wri_dat(SIO_BLUETOOTH_PORTID, msg1, sizeof(msg1));
  TEST_ASSERT_GREATER_OR_EQUAL(E_OK, ercd);

  ercd = serial_rea_dat(SIO_BLUETOOTH_PORTID, input, sizeof(input));
  TEST_ASSERT_GREATER_OR_EQUAL(E_OK, ercd);

  const char msg2[] = "Echo:\n";
  ercd = serial_wri_dat(SIO_BLUETOOTH_PORTID, msg2, sizeof(msg2));
  TEST_ASSERT_GREATER_OR_EQUAL(E_OK, ercd);
  ercd = serial_wri_dat(SIO_BLUETOOTH_PORTID, input, sizeof(input));
  TEST_ASSERT_GREATER_OR_EQUAL(E_OK, ercd);

  const char msg3[] = "\n";
  ercd = serial_wri_dat(SIO_USB_PORTID, msg3, sizeof(msg3));
  TEST_ASSERT_GREATER_OR_EQUAL(E_OK, ercd);

  dly_tsk(1*1000*1000);
}

