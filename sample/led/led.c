
#include <spike/hub/system.h>
#include <spike/hub/display.h>
#include "led.h"

extern const uint8_t pb_font_5x5[95][5];

/*
 *  メインタスク
 */
void
main_task(intptr_t exinf)
{
  dly_tsk(1000000);
  hub_display_orientation(PBIO_SIDE_TOP);

  hub_display_text("TOPPERS", 800, 200);
  dly_tsk(1000000);

  hub_display_orientation(PBIO_SIDE_RIGHT);
  hub_display_text_scroll("SPIKE-RT", 100);

  hub_display_off();
  hub_system_shutdown();
}
