// SPDX-License-Identifier: MIT
/*
 * API for the hub built-in display.
 *
 * Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *                    Graduate School of Information Science, Nagoya Univ., JAPAN
 */

/**
 * \file    cbricks/hub/display.h
 * \brief   API for the hub built-in display.
 * \author  Matsubara Yutaka
 */

/**
 * \addtogroup  Hub Hub
 * @{
 */

/**
 * \~English
 * \defgroup  Hub Display
 * \brief     API for the hub built-in display.
 * @{
 *
 * \~Japanese
 * \defgroup  Hub Display 
 * \brief     ハブ内蔵ディスプレイ向けAPI．
 * @{
 */

#ifndef _HUB_DISPLAY_H_
#define _HUB_DISPLAY_H_

#include <stdint.h>
#include <pbio/error.h>
#include <pbio/color.h>

/**
 * \~English
 * \brief       Sets the orientation of the light matrix display.
 * \param  up  top (Choose PBIO_SIDE_TOP, PBIO_SIDE_LEFT, PBIO_SIDE_RIGHT, PBIO_SIDE_BOTTOM).
 * \retval err  Error number.
 *
 * \~Japanese
 * \brief       ハブ内蔵ディスプレイの表示方向を設定する．
 * \param  up  表示方向の上側（PBIO_SIDE_TOP, PBIO_SIDE_LEFT, PBIO_SIDE_RIGHT, PBIO_SIDE_BOTTOM から選択する）．
 * \retval err  エラー番号．
 */
pbio_error_t hub_display_orientation(uint8_t up);

/**
 * \~English
 * \brief       Turns off all the pixels.
 * \param  None  
 * \retval err  Error number.
 *
 * \~Japanese
 * \brief       すべてのピクセルを消灯する．
 * \param  None  
 * \retval err  エラー番号．
 */
pbio_error_t hub_display_off(void);

/**
 * \~English
 * \brief       Turns on one pixel at the specified brightness.
 * \param  row(int)  Vertical grid index, starting at 0 from the top.
 * \param  column(int)   Horizontal grid index, starting at 0 from the left.
 * \param  brightness(brightness: %)   Brightness of the pixel.
 * \retval err  Error number.
 *
 * \~Japanese
 * \brief       指定した輝度で，指定したピクセルを点灯する．
 * \param  row(int)  一番上側を0とした行番号
 * \param  column(int)   一番左側を0とした列番号
 * \param  brightness(brightness: %)   ピクセルの輝度
 * \retval err  エラー番号．
 */
pbio_error_t hub_display_pixel(uint8_t row, uint8_t column, uint8_t brightness);

/**
 * \~English
 * \brief      　Displays an image, represented by a matrix of brightness: % values.
 * \param  matrix (Matrix)   Matrix of intensities (brightness: %). A 2D list is also accepted.
 * \retval err  Error number.
 *
 * \~Japanese
 * \brief       イメージ（輝度（0〜100%）の行列によって表現）を表示する．
 * \param  行列 (Matrix)   輝度の行列.２次元配列でも良い．
 * \retval err  エラー番号．
 */
pbio_error_t hub_display_image(uint8_t* image);

/**
 * \~English
 * \brief       Displays a number in the range -99 to 99. A minus sign (-) is shown as a faint dot in the center of the display.
 * \param  number(int)   The number to be displayed.
 * \retval err  Error number.
 *
 * \~Japanese
 * \brief       -99〜99の数値を表示する．負の符号（-）は，ディスプレイ中央に薄い点として表現される．
 * \param  number  番号．
 * \retval err  エラー番号．
 */
pbio_error_t hub_display_number(const int8_t num);

/**
 * \~English
 * \brief         Displays a character or symbol on the light grid. This may be any letter (a–z), capital letter (A–Z) or one of the following symbols: !"#$%&'()*+,-./:;<=>?@[\]^_`{|}.
 * \param  character (str)   The character or symbol to be displayed.
 * \retval err    Error number.
 *
 * \~Japanese
 * \brief         １文字または記号を表示する．小文字（a-z），大文字（A-Z）または，!"#$%&'()*+,-./:;<=>?@[\]^_`{|}である．
 * \param  character (str)   表示する文字または記号.
 * \retval err    エラー番号．
 */
pbio_error_t hub_display_char(const char c);

/**
 * \~English
 * \brief       Displays a text string, one character at a time, with a pause between each character. After the last character is shown, all lights turn off.
 * \param  text (str)   The text to be displayed.
 * \param  on (time: ms)   For how long a character is shown.
 * \param  off (time: ms)   For how long the display is off between characters.
 * \retval err  Error number.
 *
 * \~Japanese
 * \brief       文字列を表示する．１文字ずつ表示し，各文字を表示する間で一定時間消灯する．最後の文字を表示した後は，すべてのピクセルを消灯する．
 * \param  text (str)   表示する文字列．
 * \param  on (time: ms)   文字が表示される時間．
 * \param  off (time: ms)   文字表示間で消灯する時間．
 * \retval err  エラー番号．
 */
pbio_error_t hub_display_text(const char* text, uint32_t on, uint32_t off);

#endif // _HUB_DISPLAY_H_

/**
 * @} // End of group
 */

/**
 * @} // End of group
 */
