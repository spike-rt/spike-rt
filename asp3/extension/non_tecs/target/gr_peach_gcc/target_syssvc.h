/*
 *		システムサービスのターゲット依存部（GR-PEACH用）
 *
 *  システムサービスのターゲット依存部のヘッダファイル．システムサービ
 *  スのターゲット依存の設定は，できる限りコンポーネント記述ファイルで
 *  記述し，このファイルに記述するものは最小限とする．
 * 
 *  $Id: target_syssvc.h 1435 2020-05-20 07:47:35Z ertl-hiro $
 */

#ifndef TOPPERS_TARGET_SYSSVC_H
#define TOPPERS_TARGET_SYSSVC_H

#ifdef TOPPERS_OMIT_TECS

#include "gr_peach.h"
#include "rza1.h"

/*
 *  起動メッセージのターゲットシステム名
 */
#define TARGET_NAME    "GR-PEACH"

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
 *  SIOドライバで使用するSCIFに関する設定
 */
#define SIO_SCIF_BASE		SCIF2_BASE			/* SCIFのベース番地 */
#define SIO_SCIF_MODE		(SCIF_SCSMR_8BIT	/* モードレジスタ設定値 */ \
								| SCIF_SCSMR_NOPARITY | SCIF_SCSMR_1STOP)
#define SIO_SCIF_BAUDRATE	115200				/* SIOのボーレート */

/*
 *  SIO割込みを登録するための定義
 */
#define INTNO_SIO_RX	INTNO_SCIF2_RXI		/* SIO受信割込み番号 */
#define INTNO_SIO_TX	INTNO_SCIF2_TXI		/* SIO送信割込み番号 */
#define ISRPRI_SIO		1					/* SIO ISR優先度 */
#define INTPRI_SIO		(-4)				/* SIO割込み優先度 */
#define INTATR_SIO		TA_NULL				/* SIO割込み属性 */

/*
 *  低レベル出力で使用するSIOポート
 */
#define SIOPID_FPUT		1

#endif /* TOPPERS_OMIT_TECS */

/*
 *  コアのクロック周波数（パフォーマンスモニタによる性能評価用）
 */
#define CORE_CLK_MHZ	RZA1_CLK_I_MHZ

/*
 *  コアで共通な定義（チップ依存部は飛ばす）
 */
#include "core_syssvc.h"

#endif /* TOPPERS_TARGET_SYSSVC_H */
