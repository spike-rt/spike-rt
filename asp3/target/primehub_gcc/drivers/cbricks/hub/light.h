// SPDX-License-Identifier: MIT
/*
 * API for the hub built-in status light.
 *
 * Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *                    Graduate School of Information Science, Nagoya Univ., JAPAN
 */

/**
 * \file    cbricks/hub/light.h
 * \brief   API for the hub built-in status light.
 * \author  Shu Yoshifumi
 */

/**
 * \addtogroup  Hub Hub
 * @{
 */


/**
 * \~English
 * \defgroup  Hub Light
 * \brief     API for the hub built-in status light.
 * @{
 *
 * \~Japanese
 * \defgroup  Hub Light
 * \brief     ハブ内蔵ステータ・スライト向けAPI．
 * @{
 */


#ifndef _HUB_LIGHT_H_
#define _HUB_LIGHT_H_

#include <stdint.h>
#include <pbio/error.h>
#include <pbio/color.h>

/**
 * \~English
 * \brief       Turn the light on with specified color.
 * \param  hsv  HSV.
 * \retval err  Error number.
 *
 * \~Japanese
 * \brief       指定された HSV でライトを点灯させる．
 * \param  hsv  HSV．
 * \retval err  エラー番号．
 */
pbio_error_t hub_light_on_hsv(const pbio_color_hsv_t *hsv);

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
pbio_error_t hub_light_on_color(pbio_color_t color);

/**
 * \~English
 * \brief       Turn the light off.
 * \retval err  Error number.
 *
 * \~Japanese
 * \brief       ライトを消灯させる．
 * \retval err  エラー番号．
 */
pbio_error_t hub_light_off(void);

#endif // _HUB_LIGHT_H_

/**
 * @} // End of group
 */

/**
 * @} // End of group
 */
