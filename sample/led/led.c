/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2022-2024 Embedded and Real-Time Systems Laboratory,
 *            Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#include <kernel.h>
#include <t_syslog.h>
#include <t_stdlib.h>
#include "syssvc/serial.h"
#include "syssvc/syslog.h"
#include "kernel_cfg.h"
#include "led.h"

#include "spike/hub/light.h"
#include "spike/hub/display.h"

/*
 *  メインタスク
 */
void
main_task(intptr_t exinf)
{
  char c = 'a';

  syslog(LOG_NOTICE, "Set Up LED\n");

  while(1)
  {
    syslog(LOG_NOTICE, "Set LED to %c", c);
    hub_display_off();
    hub_display_char(c);
    hub_light_on_color(PBIO_COLOR_YELLOW);

    dly_tsk(2*1000*1000);
    c++;
    if (c == 'z' + 1)
    {
      c = 'a';
    }
  }

  assert(0);
}
