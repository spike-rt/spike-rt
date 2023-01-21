/*
 * SPDX-License-Identifier: MIT
 *
 * Tests for Newlib.
 *
 * Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *            Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#include <kernel.h>

#include <unity.h>
#include <unity_fixture.h>

#include <serial/serial.h>

#include <stdio.h>

#include <test.h>


#define SIO_TEST_NEWLIB_PORTID SIO_UART_F_PORTID


TEST_GROUP(NewlibSTDIO);

TEST_GROUP_RUNNER(NewlibSTDIO) {
  serial_opn_por(SIO_TEST_NEWLIB_PORTID);
  RUN_TEST_CASE(NewlibSTDIO, fprintfToSerial);
  RUN_TEST_CASE(NewlibSTDIO, fscanfToSerial);
}

TEST_SETUP(NewlibSTDIO)
{
}

TEST_TEAR_DOWN(NewlibSTDIO)
{
}


TEST(NewlibSTDIO, fprintfToSerial)
{
  int retval;

  dly_tsk(1*1000*1000);

  extern FILE* serial_open_newlib_file(ID portid);
  FILE *fd = serial_open_newlib_file(SIO_TEST_NEWLIB_PORTID);
  TEST_ASSERT_NOT_NULL(fd);
  retval = fprintf(fd, "FPRINTF TEST to SERIAL!!!\n");
  TEST_ASSERT_GREATER_THAN(0, retval);

  dly_tsk(1*1000*1000);
}

TEST(NewlibSTDIO, fscanfToSerial)
{
  int retval;

  dly_tsk(1*1000*1000);

  extern FILE* serial_open_newlib_file(ID portid);
  FILE *fd = serial_open_newlib_file(SIO_TEST_NEWLIB_PORTID);

  int num;
  TEST_MESSAGE("Input Number:.");
  retval = fscanf(fd, "%d", &num);
  TEST_ASSERT_GREATER_THAN(0, retval);

  dly_tsk(1*1000*1000);
}
