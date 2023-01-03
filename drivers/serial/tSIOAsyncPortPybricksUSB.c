/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2023 Embedded and Real-Time Systems Laboratory,
 *            Graduate School of Information Science, Nagoya Univ., JAPAN
 */

/*
 *  Pybricks USB用SIOドライバ
 */

#include "tSIOAsyncPortPybricksUSB_tecsgen.h"

/*
 *  シリアルI/Oポートのオープン
 */
void
eSIOPort_open(void)
{
  /*
   *  USB が接続されるまで待つ．
   */
  // TODO:
}

/*
 *  シリアルI/Oポートのクローズ
 */
void
eSIOPort_close(void)
{
  // 何もしない
}

/*
 *  シリアルI/Oポートへの文字送信
 */
void
eSIOPort_putNotify(void)
{
  // 何もしない
}

/*
 *  シリアルI/Oポートからのコールバックの許可
 */
void
eSIOPort_enableCBR(uint_t cbrtn)
{
  // 何もしない
}

/*
 *  シリアルI/Oポートからのコールバックの禁止
 */
void
eSIOPort_disableCBR(uint_t cbrtn)
{
  // 何もしない
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
