/*
 *		システムサービスのターゲット依存部（CT11MPCore用）
 *
 *  システムサービスのターゲット依存部のヘッダファイル．システムサービ
 *  スのターゲット依存の設定は，できる限りコンポーネント記述ファイルで
 *  記述し，このファイルに記述するものは最小限とする．
 * 
 *  $Id: target_syssvc.h 1087 2018-11-26 08:31:07Z ertl-hiro $
 */

#ifndef TOPPERS_TARGET_SYSSVC_H
#define TOPPERS_TARGET_SYSSVC_H

#ifdef TOPPERS_OMIT_TECS

#include "ct11mpcore.h"

/*
 *  起動メッセージのターゲットシステム名
 */
#define TARGET_NAME    "ARM CT11MPCore"

/*
 *  システムログの低レベル出力のための文字出力
 *
 *  ターゲット依存の方法で，文字cを表示/出力/保存する．
 */
extern void	target_fput_log(char c);

/*
 *  シリアルポートの数
 */
#define TNUM_PORT		1		/* サポートするシリアルポートの数 */

/*
 *  SIOドライバで使用するUARTに関する設定
 */
#define SIO_UART_BASE	EB_UART0_BASE		/* UARTのベース番地 */
#define SIO_UART_IBRD	EB_UART_IBRD_38400	/* ボーレート設定値（整数部）*/
#define SIO_UART_FBRD	EB_UART_FBRD_38400	/* ボーレート設定値（小数部）*/
#define SIO_UART_LCR_H	UART_LCR_H_WLEN8	/* ライン制御レジスタの設定値 */

/*
 *  SIO割込みを登録するための定義
 */
#define INTNO_SIO		EB_IRQNO_UART0		/* UART割込み番号 */
#define ISRPRI_SIO		1					/* UART ISR優先度 */
#define INTPRI_SIO		(-2)				/* UART割込み優先度 */
#define INTATR_SIO		TA_NULL				/* UART割込み属性 */

/*
 *  低レベル出力で使用するSIOポート
 */
#define SIOPID_FPUT		1

#endif /* TOPPERS_OMIT_TECS */

/*
 *  コアで共通な定義
 */
#include "core_syssvc.h"

#endif /* TOPPERS_TARGET_SYSSVC_H */
