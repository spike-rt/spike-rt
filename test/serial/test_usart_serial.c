// SPDX-License-Identifier: MIT
/*
 * Tests for Usart.
 *
 * Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *                    Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#include <kernel.h>

#include <unity.h>
#include <unity_fixture.h>

#include <test.h>
#include <test_config.h>

TEST_GROUP(UsartSerial);

TEST_GROUP_RUNNER(UsartSerial) {
  RUN_TEST_CASE(UsartSerial, print);
}

TEST_SETUP(UsartSerial)
{
}

TEST_TEAR_DOWN(UsartSerial)
{
}

TEST(UsartSerial, print)
{
  ER ercd;
  const char str[] = "Print Test via USART.\n";

	ercd = serial_opn_por(USART_TEST_PORTID);
  TEST_ASSERT_GREATER_OR_EQUAL(E_OK, ercd);

  ercd = serial_wri_dat(USART_TEST_PORTID, str, sizeof(str));
  TEST_ASSERT_GREATER_OR_EQUAL(E_OK, ercd);

	ercd = serial_cls_por(USART_TEST_PORTID);
  TEST_ASSERT_GREATER_OR_EQUAL(E_OK, ercd);
}
