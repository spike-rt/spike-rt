// SPDX-License-Identifier: MIT
/*
 * Tests for the internal LED matrix display.
 *
 * Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *                    Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#include <kernel.h>

#include <unity.h>
#include <unity_fixture.h>

#include <pbio/button.h>

TEST_GROUP(Button);

TEST_GROUP_RUNNER(Button) {
  RUN_TEST_CASE(Button, button_is_pressed);
}

TEST_SETUP(Button)
{
  // Prepare the pybricks runtime for running a user program.
  // pbsys_user_program_prepare(NULL);
}

TEST_TEAR_DOWN(Button)
{
  // Perform cleanup/reset after running a user program.
  // pbsys_user_program_unprepare();
}

TEST(Button, button_is_pressed)
{
	pbio_button_flags_t pressed;
  TEST_ASSERT_EQUAL(pbio_button_is_pressed(&pressed), PBIO_SUCCESS);
	// if you enable this line, press left button while testing.
	//TEST_ASSERT_EQUAL(pressed, PBIO_BUTTON_LEFT);
}
