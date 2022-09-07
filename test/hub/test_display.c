// SPDX-License-Identifier: MIT
/*
 * Tests for the internal LED matrix display.
 *
 * Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *            Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#include <kernel.h>

#include <unity.h>
#include <unity_fixture.h>

#include <pbsys/user_program.h>
#include <pbio/light_matrix.h>

#include <cbricks/hub/display.h>

TEST_GROUP(Display);

TEST_GROUP_RUNNER(Display) {
  RUN_TEST_CASE(Display, putc);
}

TEST_SETUP(Display)
{
  // Prepare the pybricks runtime for running a user program.
  // pbsys_user_program_prepare(NULL);
}

TEST_TEAR_DOWN(Display)
{
  // Perform cleanup/reset after running a user program.
  // pbsys_user_program_unprepare();
}

TEST(Display, putc)
{
  uint8_t i;

  TEST_ASSERT_EQUAL(hub_display_off(), PBIO_SUCCESS);
  dly_tsk(1000000);

  TEST_ASSERT_EQUAL(hub_display_orientation(PBIO_SIDE_TOP), PBIO_SUCCESS);
  dly_tsk(1000000);
  
  for (i = 0; i < 6; i++) {
    TEST_ASSERT_EQUAL(hub_display_pixel(i, i, 50+i*10), PBIO_SUCCESS);
    dly_tsk(1000000);
  }
  dly_tsk(5000000);

  uint8_t image[5][5] = {
    {60, 70, 80, 90, 100},
    {100, 60, 70, 80, 90},
    {90, 100, 60, 70, 80},
    {80, 90, 100, 60, 70},
    {70, 80, 90, 100, 60}
  };

  TEST_ASSERT_EQUAL(hub_display_image(image), PBIO_SUCCESS);
  dly_tsk(5000000);

  TEST_ASSERT_EQUAL(hub_display_number(-12), PBIO_SUCCESS);
  dly_tsk(5000000);

  TEST_ASSERT_EQUAL(hub_display_char('a'), PBIO_SUCCESS);
  dly_tsk(5000000);

  TEST_ASSERT_EQUAL(hub_display_char('Z'), PBIO_SUCCESS);
  dly_tsk(5000000);

  TEST_ASSERT_EQUAL(hub_display_text("TOPPERS", 1000, 500), PBIO_SUCCESS);
}
