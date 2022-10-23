// SPDX-License-Identifier: MIT
/*
 * Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *            Graduate School of Information Science, Nagoya Univ., JAPAN
 */

// Output characters via spike-rt/external/libpybricks/lib/pbio/drv/usb/stm32_usb_serial.c
// Add "APPL_COBJS += usb_serial.o" in Makefile.inc
#include <stdbool.h>
#include <stdint.h>
bool pbsys_usb_stdout_put_char(uint8_t c);
void low_putchar(char c)
{
	pbsys_usb_stdout_put_char(c);
}
