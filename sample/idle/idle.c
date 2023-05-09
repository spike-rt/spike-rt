/*
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2022-2023 Embedded and Real-Time Systems Laboratory,
 *                         Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#include <kernel.h>
#include <t_syslog.h>
#include <t_stdlib.h>
#include "syssvc/serial.h"
#include "syssvc/syslog.h"
#include "kernel_cfg.h"
#include "idle.h"

/*
 *  サービスコールのエラーのログ出力
 */
Inline void
svc_perror(const char *file, int_t line, const char *expr, ER ercd)
{
	if (ercd < 0) {
		t_perror(LOG_ERROR, file, line, expr, ercd);
	}
}

#define	SVC_PERROR(expr)	svc_perror(__FILE__, __LINE__, #expr, (expr))

/*
 *  メインタスク
 */
void
main_task(intptr_t exinf)
{
  while(1)
  {
    slp_tsk();
  }
  assert(0);
}
