// SPDX-License-Identifier: MIT
/*
 * API for the bluetooth.
 *
 * Copyright (c) 2023 Embedded and Real-Time Systems Laboratory,
 *            Graduate School of Information Science, Nagoya Univ., JAPAN
 */

/**
 * \file    spike/hub/bluetooth.h
 * \brief   API for the bluetooth.
 * \author  SHU Yoshifumi
 */

/**
 * \addtogroup  Hub Hub
 * @{
 */

/**
 * \~English
 * \defgroup Bluetooth Bluetooth
 * \brief    API for bluetooth.
 * @{
 *
 * \~Japanese
 * \defgroup Bluetooth Bluetooth
 * \brief    BluetoothのAPI．
 * @{
 */

#ifndef _SPIKE_HUB_BLUETOOTH_H_
#define _SPIKE_HUB_BLUETOOTH_H_

#include <pbio/error.h>
#include <stdbool.h>

/**
 * \~English
 * \brief   Enable the bluetooth advertising.
 * \details Advertising is active if and only if it is enabled and the bluetooth is not connected.
 * \return  Error number.
 *
 * \~Japanese
 * \brief   Bluetoothのアドバタイジング（接続待ち）を許可する.
 * \details 許可されており，未接続のとき，かつそのときに限り，Bluetoothのアドバタイジングは有効化される.
 * \return  エラー番号．
 */
pbio_error_t hub_bluetooth_enable_advertising(void);

/**
 * \~English
 * \brief   Disable the bluetooth advertising.
 * \details Advertising is inactive if and only if it is disabled and the bluetooth is connected.
 * \return  Error number.
 *
 * \~Japanese
 * \brief   Bluetoothのアドバタイジング（接続待ち）を禁止する.
 * \details 禁止されている，または，接続されているとき，Bluetoothのアドバタイジングは無効化される.
 * \return  エラー番号．
 */
pbio_error_t hub_bluetooth_disable_advertising(void);

/**
 * \~English
 * \brief   Check whether the bluetooth is connected.
 * \return  Error number.
 * \param [out] connected Whether the Bluetooth is connected．
 *                        \a true if connected, \a false if not conencted.
 *
 * \~Japanese
 * \brief   Bluetoothが接続されているかどうかを検出する.
 * \param [out] connected Bluetoothが接続されているかどうか．
 *                        接続されているならば，\a true，未接続ならば，\a false．
 * \return  エラー番号．
 */
pbio_error_t hub_bluetooth_is_connected(bool *connected);


#endif // _SPIKE_HUB_BLUETOOTH_H_

/**
 * @} // End of group
 */

/**
 * @} // End of group
 */
