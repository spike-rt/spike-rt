// SPDX-License-Identifier: MIT
/*
 * API for the hub built-in speaker.
 *
 * Copyright (c) 2023 Embedded and Real-Time Systems Laboratory,
 *                    Graduate School of Information Science, Nagoya Univ., JAPAN
 */

/**
 * \file    spike/hub/speaker.h
 * \brief   API for the hub built-in speaker.
 * \author  Makoto Shimojima
 */

/**
 * \addtogroup  Hub Hub
 * @{
 */

/**
 * \~English
 * \defgroup Speaker Speaker
 * \brief    APIs for speaker
 * @{
 *
 * \~Japanese
 * \defgroup Speaker スピーカ
 * \brief    スピーカのAPI．
 * @{
 */

#ifndef _HUB_SPEAKER_H_
#define _HUB_SPEAKER_H_

#define SOUND_MANUAL_STOP (-1)
#define NOTE_C4           ( 261.63)
#define NOTE_CS4          ( 277.18)
#define NOTE_D4           ( 293.66)
#define NOTE_DS4          ( 311.13)
#define NOTE_E4           ( 329.63)
#define NOTE_F4           ( 349.23)
#define NOTE_FS4          ( 369.99)
#define NOTE_G4           ( 392.00)
#define NOTE_GS4          ( 415.30)
#define NOTE_A4           ( 440.00)
#define NOTE_AS4          ( 466.16)
#define NOTE_B4           ( 493.88)
#define NOTE_C5           ( 523.25)
#define NOTE_CS5          ( 554.37)
#define NOTE_D5           ( 587.33)
#define NOTE_DS5          ( 622.25)
#define NOTE_E5           ( 659.25)
#define NOTE_F5           ( 698.46)
#define NOTE_FS5          ( 739.99)
#define NOTE_G5           ( 783.99)
#define NOTE_GS5          ( 830.61)
#define NOTE_A5           ( 880.00)
#define NOTE_AS5          ( 932.33)
#define NOTE_B5           ( 987.77)
#define NOTE_C6           (1046.50)
#define NOTE_CS6          (1108.73)
#define NOTE_D6           (1174.66)
#define NOTE_DS6          (1244.51)
#define NOTE_E6           (1318.51)
#define NOTE_F6           (1396.91)
#define NOTE_FS6          (1479.98)
#define NOTE_G6           (1567.98)
#define NOTE_GS6          (1661.22)
#define NOTE_A6           (1760.00)
#define NOTE_AS6          (1864.66)
#define NOTE_B6           (1975.53)

/**
 * \~English
 * \brief Set the volume level of speaker.
 * \param volume    the percentage of max volume level, between 0 and 100.
 *
 * \~Japanese
 * \brief 音量を調整する.
 * \param volume    ボリュームの値（0..100）.
 */
void hub_speaker_set_volume(uint8_t volume);

/**
 * \~English
 * \brief Play a tone.
 * \param frequency frequency of the note in Hz.
 * \param duraton   duration to play in msec or SOUND_MANUAL_STOP, which requires a call to hub_speaker_stop to stop the tone.
 *
 * \~Japanese
 * \brief 指定した周波数でトーン出力する.
 * \param frequency トーンの周波数（Hz）.
 * \param duration  出力持続時間（ミリ秒）SOUND_MANUAL_STOPを指定した場合はt直ちに関数から戻ってくる（ので手動で停止する必要がある）.
 */
void hub_speaker_play_tone(uint16_t frequency, int32_t duration);

/**
 * \~English
 * \brief Stop the sound.
 *
 * \~Japanese
 * \brief 再生中のサウンドを停止する.
 */
void hub_speaker_stop(void);

#endif // _HUB_SPEAKER_H_

/**
 * @} // End of group
 */

/**
 * @} // End of group
 */
