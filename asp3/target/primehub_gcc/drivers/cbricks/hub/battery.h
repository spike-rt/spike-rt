// SPDX-License-Identifier: MIT
/*
 * API for the hub built-in button.
 *
 * Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *                    Graduate School of Information Science, Nagoya Univ., JAPAN
 */

/**
 * \file    cbricks/hub/battery.h
 * \brief   API for the hub battery.
 * \author  Makoto Shimojima
 */

/**
 * \addtogroup  Hub Hub
 * @{
 */


#ifndef _HUB_BATTERY_H_
#define _HUB_BATTERY_H_

/**
 * \~English
 * \brief    Retrieve the battery voltage.
 * \return   Voltage in mV..
 *
 * \~Japanese
 * \brief    バッテリーの電圧を取得する.
 * \return   電圧 [mV]． 
 */
uint16_t hub_battery_get_voltage(void);

/**
 * \~English
 * \brief    Retrieve the battery current.
 * \return   Current in mA..
 *
 * \~Japanese
 * \brief    バッテリーの電流を取得する.
 * \return   電流 [mA]． 
 */
uint16_t hub_battery_get_current(void);

#endif // _HUB_BATTERY_H_

/**
 * @} // End of group
 */

/**
 * @} // End of group
 */
