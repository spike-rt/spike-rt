/*
 * シリアルインタフェースドライバのターゲット依存部
 *
 * $Id: target_serial.h 1364 2020-01-20 01:51:08Z ertl-komori $
 */

#ifndef TOPPERS_TARGET_SERIAL_H
#define TOPPERS_TARGET_SERIAL_H

/*
 * USART関連の定義
 */
#define USART_INTNO  (LPC5500_FLEXCOMM0_IRQn + 16)
#define USART_INTPRI (TMAX_INTPRI - 1)
#define USART_ISRPRI 1

/*
 * チップで共通な定義
 */
#include "chip_serial.h"

#endif /* TOPPERS_TARGET_SERIAL_H */
