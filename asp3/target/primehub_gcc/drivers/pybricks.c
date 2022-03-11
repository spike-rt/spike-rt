#include <kernel.h>
#include <t_syslog.h>
#include <t_stdlib.h>
#include "syssvc/serial.h"
#include "syssvc/syslog.h"
#include "kernel_cfg.h"

int pb_main_task(int argc, char **argv) {
    void debug_puts(const char *s);
    debug_puts("pybricks main task start");
    pb_SystemInit();
        
    syslog(LOG_EMERG, "sysclk = %09d", HAL_RCC_GetSysClockFreq());
    syslog(LOG_EMERG, "  hclk = %09d", HAL_RCC_GetHCLKFreq());
    syslog(LOG_EMERG, "  pclk1 = %09d", HAL_RCC_GetPCLK1Freq());
	  syslog(LOG_EMERG, "  pclk2 = %09d", HAL_RCC_GetPCLK2Freq());

    main();
    return 0;
}

void debug_puts(const char *s);
void pb_slp_tsk(void) {
    // debug_puts("slp_tsk");
		slp_tsk();
    // debug_puts("after slp_tsk");
}

void pb_wup_tsk(void) {
    // TODO: ref_tsk
    // debug_puts("wup_tsk");
    wup_tsk(PYBRICKS_TASK);
    // debug_puts("after wup_tsk");
}

void pb_can_wup(void) {
    // debug_puts("can_wup");
    // can_wup(MAIN_TASK);
    // debug_puts("after can_tsk");
}

static int debug_count = 0;
void debug_put(void) {
	syslog(LOG_EMERG, "Debug[%03d]", debug_count++);
}

void debug_puts(const char *s) {
	syslog(LOG_EMERG, "Debug[%03d]:%s",
         debug_count++, s);
}
