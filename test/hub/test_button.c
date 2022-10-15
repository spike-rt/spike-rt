// SPDX-License-Identifier: MIT
/*
 * Tests for the internal buttons.
 *
 * Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *                    Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#include <kernel.h>

#include <unity.h>
#include <unity_fixture.h>

#include <cbricks/hub/button.h>

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
	hub_button_t pressed;
  TEST_ASSERT_EQUAL(hub_button_is_pressed(&pressed), PBIO_SUCCESS);
	// if you enable this line, press left button while testing.
  TEST_ASSERT_EQUAL(HUB_BUTTON_LEFT, pressed);
}
