// SPDX-License-Identifier: MIT
/*
 * Tests for the SerialAsyncPort component.
 *
 * Copyright (c) 2023 Embedded and Real-Time Systems Laboratory,
 *            Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#include <kernel.h>
#include <kernel_cfg.h>
#include <t_syslog.h>
#include <syssvc/serial.h>

#include <unity.h>
#include <unity_fixture.h>

#include <string.h>

#include <test.h>


#define SEND_BUFFER_SIZE      (10)
#define RECEIVE_BUFFER_SIZE   (10)

static char output_buf[1000];
static int output_size = 0;

static const char *input_p = NULL;
static int input_size = 0;
static int input_head = 0;


TEST_GROUP(SerialAsyncPort);

TEST_GROUP_RUNNER(SerialAsyncPort) {
  RUN_TEST_CASE(SerialAsyncPort, writeShort);
  RUN_TEST_CASE(SerialAsyncPort, writeEqualToBuffer);
  RUN_TEST_CASE(SerialAsyncPort, writeLongerThanBuffer);
  RUN_TEST_CASE(SerialAsyncPort, writeShortLF);
  RUN_TEST_CASE(SerialAsyncPort, writeEqualToBufferLF);
  RUN_TEST_CASE(SerialAsyncPort, writeLongerThanBufferLF);

  RUN_TEST_CASE(SerialAsyncPort, readShort);
  RUN_TEST_CASE(SerialAsyncPort, readEqualToBuffer);
  RUN_TEST_CASE(SerialAsyncPort, readLongerThanBuffer);
  RUN_TEST_CASE(SerialAsyncPort, readShortWithEcho);
  RUN_TEST_CASE(SerialAsyncPort, readEqualToBufferWithEcho);
  RUN_TEST_CASE(SerialAsyncPort, readLongerThanBufferWithEcho);
}

TEST_SETUP(SerialAsyncPort)
{
}

TEST_TEAR_DOWN(SerialAsyncPort)
{
}


static void test_write(const char *input, int input_len, const char *expected, int expected_len)
{
  ER ercd;
  T_SERIAL_RPOR rpor;

  output_size = 0;

  ercd = serial_opn_por(SIO_TEST_PORTID);
  TEST_ASSERT_EQUAL(E_OK, ercd);

  ercd = sta_cyc(SIO_TEST_CBR_CYCHDR);
  TEST_ASSERT_EQUAL(E_OK, ercd);

  // Wait 2 msec.
  ercd = dly_tsk(2*1000);
  TEST_ASSERT_EQUAL(E_OK, ercd);

  ercd = serial_wri_dat(SIO_TEST_PORTID, input, input_len);
  TEST_ASSERT_GREATER_OR_EQUAL(E_OK, ercd);

  // Wait 1 sec.
  ercd = dly_tsk(1000*1000);
  TEST_ASSERT_EQUAL(E_OK, ercd);

  ercd = stp_cyc(SIO_TEST_CBR_CYCHDR);
  TEST_ASSERT_EQUAL(E_OK, ercd);

  // Check output.
  TEST_ASSERT_EQUAL(expected_len, output_size);
  TEST_ASSERT_EQUAL_STRING_LEN(expected, output_buf, expected_len);

  // Make sure all buffers inside the driver are empty.
  ercd = serial_ref_por(SIO_TEST_PORTID, &rpor);
  TEST_ASSERT_EQUAL(E_OK, ercd);
  TEST_ASSERT_EQUAL(0, rpor.reacnt);
  TEST_ASSERT_EQUAL(0, rpor.wricnt);

  ercd = serial_cls_por(SIO_TEST_PORTID);
  TEST_ASSERT_EQUAL(E_OK, ercd);
}

TEST(SerialAsyncPort, writeShort)
{
  const char input[] = "Test";
  int input_len = sizeof(input);
  const char expected[] = "Test";
  int expected_len = sizeof(expected);

  test_write(input, input_len, expected, expected_len);
}

TEST(SerialAsyncPort, writeEqualToBuffer)
{
  const char input[SEND_BUFFER_SIZE] = "123456789";
  int input_len = sizeof(input);
  const char expected[SEND_BUFFER_SIZE] = "123456789";
  int expected_len = sizeof(expected);

  test_write(input, input_len, expected, expected_len);
}


TEST(SerialAsyncPort, writeLongerThanBuffer)
{
  const char input[] = "WriteLongerThanBuffer";
  int input_len = sizeof(input);
  const char expected[] = "WriteLongerThanBuffer";
  int expected_len = sizeof(expected);

  test_write(input, input_len, expected, expected_len);
}

TEST(SerialAsyncPort, writeShortLF)
{
  const char input[] = "Test\n";
  int input_len = sizeof(input);
  const char expected[] = "Test\r\n";
  int expected_len = sizeof(expected);

  test_write(input, input_len, expected, expected_len);
}

TEST(SerialAsyncPort, writeEqualToBufferLF)
{
  const char input[SEND_BUFFER_SIZE] = "12345678\n";
  int input_len = sizeof(input);
  const char expected[] = "12345678\r\n";
  int expected_len = sizeof(expected);

  test_write(input, input_len, expected, expected_len);
}

TEST(SerialAsyncPort, writeLongerThanBufferLF)
{
  const char input[] = "Write\nLonger\nThan\nBuffer\n";
  int input_len = sizeof(input);
  const char expected[] = "Write\r\nLonger\r\nThan\r\nBuffer\r\n";
  int expected_len = sizeof(expected);

  test_write(input, input_len, expected, expected_len);
}


static void test_read(const char *input, int input_len, const char *expected, int expected_len, bool echo)
{
  char buf[100];
  ER ercd;
  T_SERIAL_RPOR rpor;

  input_p = input;
  input_size = input_len;
  input_head = 0;

  output_size = 0;


  ercd = serial_opn_por(SIO_TEST_PORTID);
  TEST_ASSERT_EQUAL(E_OK, ercd);

  // Configure the serial port. 
  if (echo == false)
  { 
    // Disable echo back.
    ercd = serial_ctl_por(SIO_TEST_PORTID, IOCTL_CRLF);
    TEST_ASSERT_EQUAL(E_OK, ercd);
  } else {
    // Enable echo back.
    ercd = serial_ctl_por(SIO_TEST_PORTID, IOCTL_ECHO|IOCTL_CRLF);
    TEST_ASSERT_EQUAL(E_OK, ercd);
  }

  ercd = sta_cyc(SIO_TEST_CBR_CYCHDR);
  TEST_ASSERT_EQUAL(E_OK, ercd);

  // Wait 2 msec.
  ercd = dly_tsk(2*1000);
  TEST_ASSERT_EQUAL(E_OK, ercd);

  ercd = serial_rea_dat(SIO_TEST_PORTID, buf, expected_len);
  TEST_ASSERT_GREATER_OR_EQUAL(E_OK, ercd);

  // Wait 1 sec.
  ercd = dly_tsk(1000*1000);
  TEST_ASSERT_EQUAL(E_OK, ercd);

  ercd = stp_cyc(SIO_TEST_CBR_CYCHDR);
  TEST_ASSERT_EQUAL(E_OK, ercd);

  // Make sure all input are read.
  TEST_ASSERT_EQUAL(0, input_size);

  TEST_ASSERT_EQUAL_STRING_LEN(expected, buf, expected_len);

  // Check output.
  if (echo == false)
  {
    TEST_ASSERT_EQUAL(0, output_size);
  } else {
    // Check echo back.
    TEST_ASSERT_EQUAL(expected_len, output_size);
    TEST_ASSERT_EQUAL_STRING_LEN(expected, output_buf, expected_len);
  }

  // Make sure all buffers inside the driver are empty.
  ercd = serial_ref_por(SIO_TEST_PORTID, &rpor);
  TEST_ASSERT_EQUAL(E_OK, ercd);
  TEST_ASSERT_EQUAL(0, rpor.reacnt);
  TEST_ASSERT_EQUAL(0, rpor.wricnt);

  ercd = serial_cls_por(SIO_TEST_PORTID);
  TEST_ASSERT_EQUAL(E_OK, ercd);
}

TEST(SerialAsyncPort, readShort)
{
  const char input[] = "Test";
  int input_len = sizeof(input);
  const char expected[] = "Test";
  int expected_len = sizeof(expected);

  test_read(input, input_len, expected, expected_len, false);
}

TEST(SerialAsyncPort, readEqualToBuffer)
{
  const char input[RECEIVE_BUFFER_SIZE] = "123456789";
  int input_len = sizeof(input);
  const char expected[RECEIVE_BUFFER_SIZE] = "123456789";
  int expected_len = sizeof(expected);

  test_read(input, input_len, expected, expected_len, false);
}


TEST(SerialAsyncPort, readLongerThanBuffer)
{
  const char input[] = "ReadLongerThanBuffer";
  int input_len = sizeof(input);
  const char expected[] = "ReadLongerThanBuffer";
  int expected_len = sizeof(expected);

  test_read(input, input_len, expected, expected_len, false);
}


TEST(SerialAsyncPort, readShortWithEcho)
{
  const char input[] = "Test";
  int input_len = sizeof(input);
  const char expected[] = "Test";
  int expected_len = sizeof(expected);

  test_read(input, input_len, expected, expected_len, true);
}

TEST(SerialAsyncPort, readEqualToBufferWithEcho)
{
  const char input[RECEIVE_BUFFER_SIZE] = "123456789";
  int input_len = sizeof(input);
  const char expected[RECEIVE_BUFFER_SIZE] = "123456789";
  int expected_len = sizeof(expected);

  test_read(input, input_len, expected, expected_len, true);
}

TEST(SerialAsyncPort, readLongerThanBufferWithEcho)
{
  const char input[] = "ReadLongerThanBuffer";
  int input_len = sizeof(input);
  const char expected[] = "ReadLongerThanBuffer";
  int expected_len = sizeof(expected);

  test_read(input, input_len, expected, expected_len, true);
}


void SIOTestCBRHandler(intptr_t exinf)
{
  extern int tSIOAsyncPortTest_eSIOCBR_pushReceive(char src);

  extern int tSIOAsyncPortTest_eSIOCBR_sizeSend(void);
  extern int tSIOAsyncPortTest_eSIOCBR_popSend(char *dst);

  int receive_size;
  int send_size;

  if (input_size > 0)
  {
    receive_size = tSIOAsyncPortTest_eSIOCBR_pushReceive(input_p[input_head]);
    if (receive_size > 0) {
      input_size -= receive_size;
      input_head += receive_size;
    }
  }

  if (tSIOAsyncPortTest_eSIOCBR_sizeSend() > 0)
  {
    send_size = tSIOAsyncPortTest_eSIOCBR_popSend(&output_buf[output_size]);
    if (send_size > 0) {
      output_size += send_size;
    }
  }
}

#if 0
// Handle the callback in 1 ms cycle.
void SIOTestCBRHandler(intptr_t exinf)
{
  extern int tSIOAsyncPortTest_eSIOCBR_lenSend(void);
  extern int tSIOAsyncPortTest_eSIOCBR_popSend(char *dst_data, uint32_t size);

  int send_size;
  send_size = tSIOAsyncPortTest_eSIOCBR_popSend(
              &output_buf[output_size], SEND_BUFFER_SIZE);
  if (send_size >= 0) {
    output_size += send_size;
  }
}
#endif
