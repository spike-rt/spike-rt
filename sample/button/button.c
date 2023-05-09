// SPDX-License-Identifier: MIT
/*
 * Copyright (c) 2022-2023 Embedded and Real-Time Systems Laboratory,
 *                         Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#include <spike/hub/system.h>
#include <spike/hub/button.h>
#include <serial/serial.h>
#include <stdio.h>
#include "button.h"
extern FILE *serial_open_newlib_file(ID portid);

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
  dly_tsk(1000000);

  FILE *fd = serial_open_newlib_file(SIO_USB_PORTID);
  fprintf(fd, "BUTTON: Press RIGHT, or LEFT buttons; press both to terminate.\n");
  fprintf(fd, "CENTER and BT buttons are ignored.\n");

  bool done = false;
  while (!done) {
    SYSTIM st;
    hub_button_t buttons = wait_for_hub_buttons(HUB_BUTTON_RIGHT|HUB_BUTTON_LEFT);
    get_tim(&st);
    fprintf(fd, "%u: %d ", (unsigned int) st, buttons);
    if (buttons&HUB_BUTTON_LEFT)   fprintf(fd, "LEFT ");
    if (buttons&HUB_BUTTON_RIGHT)  fprintf(fd, "RIGHT ");
    if (buttons&HUB_BUTTON_CENTER) fprintf(fd, "CENTER ");
    if (buttons&HUB_BUTTON_BT)     fprintf(fd, "BT ");
    fprintf(fd, "\n");
    done = (buttons == (HUB_BUTTON_LEFT|HUB_BUTTON_RIGHT));
  }
  fprintf(fd, "Terminating...\n");
  hub_system_shutdown();
}
