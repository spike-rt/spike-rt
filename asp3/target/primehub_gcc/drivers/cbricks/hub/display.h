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


pbio_error_t hub_display_orientation(uint8_t up);

pbio_error_t hub_display_off(void);

pbio_error_t hub_display_pixel(uint8_t row, uint8_t column, uint8_t brightness);

pbio_error_t hub_display_image(uint8_t* image);

/**
 * \~English
 * \brief       Display the numbers.
 * \param  number  NUMBER.
 * \retval err  Error number.
 *
 * \~Japanese
 * \brief       指定された番号を表示する．
 * \param  number  番号．
 * \retval err  エラー番号．
 */
pbio_error_t hub_display_number(const int8_t num);

/**
 * \~English
 * \brief         Turn the light on with specified color.
 * \param  color  color.
 * \retval err    Error number.
 *
 * \~Japanese
 * \brief         指定された色でライトを点灯させる．
 * \param  color  色．
 * \retval err    エラー番号．
 */
//pbio_error_t hub_light_on_color(pbio_color_t color);
pbio_error_t hub_display_char(const char c);

pbio_error_t hub_display_text(const char* text, uint32_t on, uint32_t off);

/**
 * \~English
 * \brief       Turn the light off.
 * \retval err  Error number.
 *
 * \~Japanese
 * \brief       ライトを消灯させる．
 * \retval err  エラー番号．
 */
//pbio_error_t hub_light_off(void);

#endif // _HUB_DISPLAY_H_

/**
 * @} // End of group
 */

/**
 * @} // End of group
 */
