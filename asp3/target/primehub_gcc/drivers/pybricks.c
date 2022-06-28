// SPDX-License-Identifier: MIT
/*
 * A task to run pybricks and functions to call asp3 api from pybricks.
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

/*
 * Task to run pybricks.
 */
int pb_main_task(int argc, char **argv) {
	  syslog(LOG_INFO, "pybricks main task start");

    // Call pybricks platform initialization function.
    pb_SystemInit();
        
    // Call pybricks.
    main();

    // Never come back here because the shutdown procedure is handled by pybricks.

    return 0;
}


void slp_pybricks(void) {
		slp_tsk();
}

void wup_pybricks(void) {
    wup_tsk(PYBRICKS_TASK);
}

void __attribute__((noreturn)) __fatal_error(const char *msg) {
    TOPPERS_assert_abort();
}

void __attribute__((weak)) __assert_func(const char *file, int line, const char *func, const char *expr) {
	  syslog(LOG_EMERG, "Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    __fatal_error("Assertion failed");
}
