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

#include <test.h>

#include <stdlib.h>


TEST_GROUP(NewlibSTDLIB);

TEST_GROUP_RUNNER(NewlibSTDLIB) {
  RUN_TEST_CASE(NewlibSTDLIB, malloc);
}

TEST_SETUP(NewlibSTDLIB)
{
}

TEST_TEAR_DOWN(NewlibSTDLIB)
{
}


TEST(NewlibSTDLIB, malloc)
{
  void *mem;

  mem = malloc(10);
  TEST_ASSERT_NOT_NULL(mem);

  free(mem);
}
