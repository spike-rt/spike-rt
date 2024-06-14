// SPDX-License-Identifier: MIT
/*
 * Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *                    Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#include <t_syslog.h>
#include "kernel_cfg.h"
#include "button.h"
#include "spike/hub/button.h"

static inline hub_button_t hub_buttons_pressed(hub_button_t buttons)
{
  hub_button_t pressed;
  hub_button_is_pressed(&pressed);
  return pressed & buttons;
}

static hub_button_t wait_for_hub_buttons(hub_button_t buttons_to_watch)
{
  hub_button_t buttons = 0;
  hub_button_t pressed;
  while (hub_buttons_pressed(buttons_to_watch) == 0) dly_tsk(10000);
  while (pressed = hub_buttons_pressed(buttons_to_watch), pressed != 0) {
    buttons |= pressed;
    dly_tsk(10000);
  }
  return buttons;
}

/*
 *  メインタスク
 */
void
main_task(intptr_t exinf)
{    
  syslog(LOG_NOTICE, "BUTTON");
  dly_tsk(3000000);
    
  while (1) {
    SYSTIM st;
    hub_button_t buttons = wait_for_hub_buttons(HUB_BUTTON_RIGHT|HUB_BUTTON_LEFT|HUB_BUTTON_CENTER|HUB_BUTTON_BT);
    get_tim(&st);
    syslog(LOG_NOTICE, "%u: %d ", (unsigned int) st, buttons);
    if (buttons & HUB_BUTTON_LEFT)   syslog(LOG_NOTICE, "LEFT ");
    if (buttons & HUB_BUTTON_RIGHT)  syslog(LOG_NOTICE, "RIGHT ");
    if (buttons & HUB_BUTTON_CENTER) syslog(LOG_NOTICE, "CENTER ");
    if (buttons & HUB_BUTTON_BT)     syslog(LOG_NOTICE, "BT ");
  }
}
