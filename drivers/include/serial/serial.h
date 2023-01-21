/*
 * SPDX-License-Identifier: MIT
 *
 * Header for Serial.
 *
 * Copyright (c) 2023 Embedded and Real-Time Systems Laboratory,
 *            Graduate School of Information Science, Nagoya Univ., JAPAN
 */

/**
 * \file    serial/serial.h
 * \brief   Serial.
 * \author  Shu Yoshifumi
 */

/**
 * \~English
 * \defgroup  Serial Serial
 * \brief     Serial.
 * @{
 *
 * \~Japanese
 * \defgroup  Serial シリアル
 * \brief     シリアル．
 * @{
 */


#ifndef _SERIAL_SERIAL_H_
#define _SERIAL_SERIAL_H_

/*
 * Definitions of Port ID for Serial Adapter
 */
enum {
  SIO_USB_PORTID = 1,
  SIO_BLUETOOTH_PORTID,
  SIO_TEST_PORTID,
  SIO_UART_F_PORTID,
  SIO_UART_E_PORTID,
};

#endif // _SEIRAL_SERIAL_H_

/**
 * @} // End of group Serial
 */
