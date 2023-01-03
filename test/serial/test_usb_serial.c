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

TEST_GROUP(USBSerial);

TEST_GROUP_RUNNER(USBSerial) {
  RUN_TEST_CASE(USBSerial, Write);
  RUN_TEST_CASE(USBSerial, WriteLargerThanBuffer);
  RUN_TEST_CASE(USBSerial, Read);
  //TODO:RUN_TEST_CASE(USBSerial, ReadLargerThanBuffer);
}

TEST_SETUP(USBSerial)
{
}

TEST_TEAR_DOWN(USBSerial)
{
}

TEST(USBSerial, Write)
{
  ER ercd;
  const char str[] = "Write Data Test via USB.\n";

  dly_tsk(1*1000*1000);
  
	ercd = serial_opn_por(SIO_USB_PORTID);
  //TEST_ASSERT_GREATER_OR_EQUAL(E_OK, ercd);
  
  ercd = serial_wri_dat(SIO_USB_PORTID, str, sizeof(str));
  TEST_ASSERT_GREATER_OR_EQUAL(E_OK, ercd);

  dly_tsk(1*1000*1000);
}

TEST(USBSerial, WriteLargerThanBuffer)
{
  ER ercd;
  const char str[] = "Write Larger Data than Buffer Test via USB.\n";

  ercd = serial_opn_por(SIO_USB_PORTID);
  //TEST_ASSERT_GREATER_OR_EQUAL(E_OK, ercd);
  
  for (int i=0; i<10; i++)
  {
    ercd = serial_wri_dat(SIO_USB_PORTID, str, sizeof(str));
    TEST_ASSERT_GREATER_OR_EQUAL(E_OK, ercd);
  }
  dly_tsk(1*1000*1000);
}

TEST(USBSerial, Read)
{
  ER ercd;
  char input[10];

  dly_tsk(1*1000*1000);
  
	ercd = serial_opn_por(SIO_USB_PORTID);
  //TEST_ASSERT_GREATER_OR_EQUAL(E_OK, ercd);
  
  const char msg1[] = "Input 10 Characters:\n";
  ercd = serial_wri_dat(SIO_USB_PORTID, msg1, sizeof(msg1));
  TEST_ASSERT_GREATER_OR_EQUAL(E_OK, ercd);

  ercd = serial_rea_dat(SIO_USB_PORTID, input, sizeof(input));
  TEST_ASSERT_GREATER_OR_EQUAL(E_OK, ercd);

  const char msg2[] = "Echo:\n";
  ercd = serial_wri_dat(SIO_USB_PORTID, msg2, sizeof(msg2));
  TEST_ASSERT_GREATER_OR_EQUAL(E_OK, ercd);
  ercd = serial_wri_dat(SIO_USB_PORTID, input, sizeof(input));
  TEST_ASSERT_GREATER_OR_EQUAL(E_OK, ercd);

  const char msg3[] = "\n";
  ercd = serial_wri_dat(SIO_USB_PORTID, msg3, sizeof(msg3));
  TEST_ASSERT_GREATER_OR_EQUAL(E_OK, ercd);

  dly_tsk(1*1000*1000);
}

