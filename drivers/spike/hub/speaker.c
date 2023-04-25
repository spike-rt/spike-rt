// SPDX-License-Identifier: MIT
/*
 * API for the hub built-in button.
 *
 * Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *                    Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#include <kernel.h>
#include <spike/hub/speaker.h>
#include <pbdrv/sound.h>
#include <stdint.h>
#include <math.h>

#define WAVEFORM_LENGTH 128
static uint16_t waveform_data[WAVEFORM_LENGTH];
static uint8_t  waveform_volume = 0;

void hub_speaker_set_volume(uint8_t volume)
{
  if (volume == waveform_volume) return;
  uint16_t C = INT16_MAX;
  uint16_t A = (pow(10, volume / 100.0) - 1) / 9 * INT16_MAX;
#ifdef GENERATE_SINE_WAVES
  for (int i = 0; i < WAVEFORM_LENGTH; i++) {
    waveform_data[i] = C + A * sin(M_PI * 2.0 * i / WAVEFORM_LENGTH); 
  }
#else // SQUARE_WAVES
  for (int i = 0; i < WAVEFORM_LENGTH/2; i++) {
    waveform_data[i] = C - A;
  }
  for (int i = WAVEFORM_LENGTH/2; i < WAVEFORM_LENGTH; i++) {
    waveform_data[i] = C + A;
  }
#endif
}

void hub_speaker_play_tone(uint16_t frequency, int32_t duration)
{
  if (frequency <    64) frequency =    64;
  if (frequency > 24000) frequency = 24000;
  pbdrv_sound_start(waveform_data, WAVEFORM_LENGTH, WAVEFORM_LENGTH * frequency / 2);
  if (duration < 0) return;
  dly_tsk(duration*1000);
  hub_speaker_stop();
}

void hub_speaker_stop(void)
{
  pbdrv_sound_stop();
}
