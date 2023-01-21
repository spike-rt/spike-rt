// SPDX-License-Identifier: MIT
/*
 * Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *            Graduate School of Information Science, Nagoya Univ., JAPAN
 */

/*
 * Header file for the test task.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <kernel.h>

/*
 *  タスクの優先度の定義
 */
#define MAIN_PRIORITY	5		/* メインタスクの優先度 */
#define SIO_TEST_CBR_PRIORITY	4

#ifndef STACK_SIZE
#define	STACK_SIZE		4096		/* タスクのスタックサイズ */
#endif /* STACK_SIZE */

/*
 *  関数のプロトタイプ宣言
 */
#ifndef TOPPERS_MACRO_ONLY

extern void TestMainTask(intptr_t exinf);
extern void SIOTestCBRHandler(intptr_t exinf);

#ifdef __cplusplus
}
#endif

#endif /* TOPPERS_MACRO_ONLY */
