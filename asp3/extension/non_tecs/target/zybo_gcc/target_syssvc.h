/*
 *		システムサービスのターゲット依存部（ZYBO用）
 *
 *  システムサービスのターゲット依存部のヘッダファイル．システムサービ
 *  スのターゲット依存の設定は，できる限りコンポーネント記述ファイルで
 *  記述し，このファイルに記述するものは最小限とする．
 * 
 *  $Id: target_syssvc.h 1088 2018-11-26 08:41:35Z ertl-hiro $
 */

#ifndef TOPPERS_TARGET_SYSSVC_H
#define TOPPERS_TARGET_SYSSVC_H

#ifdef TOPPERS_OMIT_TECS

#include "zybo.h"
#include "zynq7000.h"

/*
 *  起動メッセージのターゲットシステム名
 */
#define TARGET_NAME    "ZYBO <Zynq-7000, Cortex-A9>"

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
 *  SIOドライバで使用するXUartPsに関する設定
 */
#define SIO_XUARTPS_BASE	ZYNQ_UART1_BASE
#define SIO_XUARTPS_MODE	XUARTPS_MR_CHARLEN_8 \
								| XUARTPS_MR_PARITY_NONE | XUARTPS_MR_STOPBIT_1
#define SIO_XUARTPS_BAUDGEN	XUARTPS_BAUDGEN_115K
#define SIO_XUARTPS_BAUDDIV	XUARTPS_BAUDDIV_115K

/*
 *  SIO割込みを登録するための定義
 */
#define INTNO_SIO		ZYNQ_UART1_IRQ		/* SIO割込み番号 */
#define ISRPRI_SIO		1					/* SIO ISR優先度 */
#define INTPRI_SIO		(-4)				/* SIO割込み優先度 */
#define INTATR_SIO		TA_NULL				/* SIO割込み属性 */

/*
 *  低レベル出力で使用するSIOポート
 */
#define SIOPID_FPUT		1

#endif /* TOPPERS_OMIT_TECS */

/*
 *  コアで共通な定義（チップ依存部は飛ばす）
 */
#include "core_syssvc.h"

#endif /* TOPPERS_TARGET_SYSSVC_H */
