/*
 * SPDX-License-Identifier: MIT
 *
 * Tests for the internal LED matrix display.
 *
 * Copyright (c) 2022-2023 Embedded and Real-Time Systems Laboratory,
 *            Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#include <kernel.h>

#include <unity.h>
#include <unity_fixture.h>

#include <spike/hub/display.h>


TEST_GROUP(Display);

TEST_GROUP_RUNNER(Display) {
  RUN_TEST_CASE(Display, off);
  RUN_TEST_CASE(Display, orientation);
  RUN_TEST_CASE(Display, pixel);
  RUN_TEST_CASE(Display, image);
  RUN_TEST_CASE(Display, number);
  RUN_TEST_CASE(Display, char_a);
  RUN_TEST_CASE(Display, char_Z);
  RUN_TEST_CASE(Display, text);
  RUN_TEST_CASE(Display, text_scroll);
}

TEST_SETUP(Display)
{
}

TEST_TEAR_DOWN(Display)
{
  dly_tsk(5*1000*1000);
}


TEST(Display, off)
{
  TEST_ASSERT_EQUAL(hub_display_off(), PBIO_SUCCESS);
}

TEST(Display, orientation)
{
  TEST_ASSERT_EQUAL(hub_display_orientation(PBIO_SIDE_TOP), PBIO_SUCCESS);
}

TEST(Display, pixel)
{
  uint8_t i;
  for (i = 0; i < 6; i++) {
    TEST_ASSERT_EQUAL(hub_display_pixel(i, i, 50+i*10), PBIO_SUCCESS);
    dly_tsk(1000000);
  }
}

TEST(Display, image)
{
  uint8_t image[5][5] = {
    {60, 70, 80, 90, 100},
    {100, 60, 70, 80, 90},
    {90, 100, 60, 70, 80},
    {80, 90, 100, 60, 70},
    {70, 80, 90, 100, 60}
  };

  TEST_ASSERT_EQUAL(hub_display_image(image), PBIO_SUCCESS);
}

TEST(Display, number)
{
  TEST_ASSERT_EQUAL(hub_display_number(-12), PBIO_SUCCESS);
}

TEST(Display, char_a)
{
  TEST_ASSERT_EQUAL(hub_display_char('a'), PBIO_SUCCESS);
}

TEST(Display, char_Z)
{
  TEST_ASSERT_EQUAL(hub_display_char('Z'), PBIO_SUCCESS);
}

TEST(Display, text)
{
  TEST_ASSERT_EQUAL(hub_display_text("TOPPERS", 1000, 500), PBIO_SUCCESS);
}

TEST(Display, text_scroll)
{
  TEST_ASSERT_EQUAL(hub_display_text_scroll("TOPPERS", 80), PBIO_SUCCESS);
}
