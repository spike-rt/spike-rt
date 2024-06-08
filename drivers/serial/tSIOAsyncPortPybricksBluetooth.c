/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2023 Embedded and Real-Time Systems Laboratory,
 *            Graduate School of Information Science, Nagoya Univ., JAPAN
 */

/*
 *  Pybricks Bluetooth用SIOドライバ
 */

#include "tSIOAsyncPortPybricksBluetooth_tecsgen.h"

#include <spike/hub/bluetooth.h>
#include <pbdrv/bluetooth.h>

bool_t pybricksBluetooth_sendCBREnabled;
bool_t pybricksBluetooth_receiveCBREnabled;


/*
 *  シリアルI/Oポートのオープン
 */
void
eSIOPort_open(void)
{
  /*
   *  Bluetoothアドバタイジングを有効化し， 
   *  Bluetooth が接続されるまで待つ．
   */
  hub_bluetooth_enable_advertising();
  while (!pbdrv_bluetooth_is_connected(PBDRV_BLUETOOTH_CONNECTION_UART)) {
    dly_tsk(1000*1000);
  }
}

/*
 *  シリアルI/Oポートのクローズ
 */
void
eSIOPort_close(void)
{
}

/*
 *  In external/libpybricks/lib/pbio/sys/bluetooth.c
 */
extern void pb_bluetooth_uart_put_notify(void);

/*
 *  シリアルI/Oポートへの文字送信
 */
void
eSIOPort_putNotify(void)
{
  pb_bluetooth_uart_put_notify();
}

/*
 *  シリアルI/Oポートからのコールバックの許可
 */
void
eSIOPort_enableCBR(uint_t cbrtn)
{
  switch (cbrtn) {
  case SIOAsyncSendPop:
    pybricksBluetooth_sendCBREnabled = true;
    break;
  case SIOAsyncReceivePush:
    pybricksBluetooth_receiveCBREnabled = true;
    break;
  }
}

/*
 *  シリアルI/Oポートからのコールバックの禁止
 */
void
eSIOPort_disableCBR(uint_t cbrtn)
{
  switch (cbrtn) {
  case SIOAsyncSendPop:
    pybricksBluetooth_sendCBREnabled = false;
    break;
  case SIOAsyncReceivePush:
    pybricksBluetooth_receiveCBREnabled = false;
    break;
  }
}

ER_UINT
eSIOCBR_sizeSend(void)
{
  return cSIOCBR_sizeSend();
}

ER_UINT
eSIOCBR_popSend(char *dst)
{
  return cSIOCBR_popSend(dst);
}

ER_UINT
eSIOCBR_pushReceive(char src)
{
  return cSIOCBR_pushReceive(src);
}
