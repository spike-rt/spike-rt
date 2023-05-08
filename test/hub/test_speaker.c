// SPDX-License-Identifier: MIT
/*
 * Tests for the hub built-in speaker.
 *
 * Copyright (c) 2023 Embedded and Real-Time Systems Laboratory,
 *                    Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#include <kernel.h>

#include <unity.h>
#include <unity_fixture.h>

#include <spike/hub/speaker.h>

TEST_GROUP(Speaker);

TEST_GROUP_RUNNER(Speaker) {
  RUN_TEST_CASE(Speaker, beep);
}

TEST_SETUP(Speaker)
{
}

TEST_TEAR_DOWN(Speaker)
{
}

TEST(Speaker, beep)
{
  hub_speaker_set_volume(100);
  hub_speaker_play_tone(NOTE_C5, 200); dly_tsk(800000);
  hub_speaker_play_tone(NOTE_C5, 200); dly_tsk(800000);
  hub_speaker_play_tone(NOTE_C6, SOUND_MANUAL_STOP);
  for (int i = 0; i < 100; i++) {
    dly_tsk(10000);
    hub_speaker_set_volume(100-i);
  }
  hub_speaker_stop();
}
