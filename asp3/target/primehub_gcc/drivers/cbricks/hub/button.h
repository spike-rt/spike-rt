// SPDX-License-Identifier: MIT
/*
 * API for the hub built-in button.
 *
 * Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *                    Graduate School of Information Science, Nagoya Univ., JAPAN
 */

/**
 * \file    cbricks/hub/button.h
 * \brief   API for the hub built-in button.
 * \author  Makoto Shimojima
 */

/**
 * \addtogroup  Hub Hub
 * @{
 */


#ifndef _HUB_BUTTON_H_
#define _HUB_BUTTON_H_

#include <stdint.h>
#include <pbio/button.h>
#include <pbio/error.h>

/**
 * \~English
 * \brief    Bitmask definitions for each built-in button on the hub.
 *
 * \~Japanese
 * \brief    各ハブ内蔵ボタンのビットマスク値
 */
typedef enum _hub_button_t {
   HUB_BUTTON_LEFT   = PBIO_BUTTON_LEFT,
   HUB_BUTTON_CENTER = PBIO_BUTTON_CENTER,
   HUB_BUTTON_RIGHT  = PBIO_BUTTON_RIGHT,
   HUB_BUTTON_BT     = PBIO_BUTTON_RIGHT_UP,
} hub_button_t;

/**
 * \~English
 * \brief    Checks which buttons are currently pressed.
 * \param pressed Bitmask-OR of buttons that are pressed (or 0 if none) is written to the variable pointed.
 * \retval err Error number.
 *
 * \~Japanese
 * \brief    ハブ内蔵ボタンが押されているか調べる.
 * \param pressed 押されているボタンのビットOR値（何も押されていなければ 0）を返す.
 * \retval err エラー番号.
 */
pbio_error_t hub_button_is_pressed(hub_button_t *pressed);

#endif // _HUB_BUTTON_H_

/**
 * @} // End of group
 */

/**
 * @} // End of group
 */
