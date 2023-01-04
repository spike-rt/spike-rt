/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2023 Embedded and Real-Time Systems Laboratory,
 *            Graduate School of Information Science, Nagoya Univ., JAPAN
 */

/*
 *  テスト用SIOドライバ
 */

#include "tSIOAsyncPortTest_tecsgen.h"

/*
 *  シリアルI/Oポートのオープン
 */
void
eSIOPort_open(void)
{
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

#if 0
ER_UINT
eSIOCBR_popSend(char *dst_data, uint_t max_size)
{
  return cSIOCBR_popSend(dst_data, max_size);
}
#endif

ER_UINT
eSIOCBR_pushReceive(char src)
{
  return cSIOCBR_pushReceive(src);
}

#if 0
ER_UINT
eSIOCBR_pushReceive(const char *src_data, uint_t size)
{
  return cSIOCBR_pushReceive(src_data, size);
}
#endif
