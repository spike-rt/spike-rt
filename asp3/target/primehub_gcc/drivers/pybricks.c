// SPDX-License-Identifier: MIT
/*
 * A task to run pybricks.
 *
 * Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *                    Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#include <kernel.h>
#include <t_syslog.h>
#include <t_stdlib.h>
#include <syssvc/serial.h>
#include <syssvc/syslog.h>
#include <kernel_cfg.h>

int pb_main_task(int argc, char **argv) {
	  syslog(LOG_INFO, "pybricks main task start");
    pb_SystemInit();
        
    main();
    return 0;
}

void pb_slp_tsk(void) {
		slp_tsk();
}

void pb_wup_tsk(void) {
    wup_tsk(PYBRICKS_TASK);
}

void __attribute__((noreturn)) __fatal_error(const char *msg) {
    TOPPERS_assert_abort();
}

void __attribute__((weak)) __assert_func(const char *file, int line, const char *func, const char *expr) {
	  syslog(LOG_EMERG, "Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    __fatal_error("Assertion failed");
}
