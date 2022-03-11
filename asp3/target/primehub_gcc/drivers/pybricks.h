/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 *
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2004-2016 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
 *
 *  上記著作権者は，以下の(1)〜(4)の条件を満たす場合に限り，本ソフトウェ
 *  ア（本ソフトウェアを改変したものを含む．以下同じ）を使用・複製・改
 *  変・再配布（以下，利用と呼ぶ）することを無償で許諾する．
 *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *      スコード中に含まれていること．
 *  (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
 *      用できる形で再配布する場合には，再配布に伴うドキュメント（利用
 *      者マニュアルなど）に，上記の著作権表示，この利用条件および下記
 *      の無保証規定を掲載すること．
 *  (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
 *      用できない形で再配布する場合には，次のいずれかの条件を満たすこ
 *      と．
 *    (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
 *        作権表示，この利用条件および下記の無保証規定を掲載すること．
 *    (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
 *        報告すること．
 *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *      害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
 *      また，本ソフトウェアのユーザまたはエンドユーザからのいかなる理
 *      由に基づく請求からも，上記著作権者およびTOPPERSプロジェクトを
 *      免責すること．
 * 
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
 *  よびTOPPERSプロジェクトは，本ソフトウェアに関して，特定の使用目的
 *  に対する適合性も含めて，いかなる保証も行わない．また，本ソフトウェ
 *  アの利用により直接的または間接的に生じたいかなる損害に関しても，そ
 *  の責任を負わない．
 *
 *  $Id: sample1.h 705 2016-03-29 14:16:41Z ertl-hiro $
 */

/*
 *		サンプルプログラム(1)のヘッダファイル
 */

#include <kernel.h>

/*
 *  ターゲット依存の定義
 */

/*
 *  各タスクの優先度の定義
 */

#define PYBRICKS_PRIORITY	4		/* タスクの優先度 */

#define	PYBRICKS_STACK_SIZE		0x2000		/* タスクのスタックサイズ 8KB */

#define INHNO_DMA1_STREAM5	(DMA1_Stream5_IRQn + 16)	/* 割込みハンドラ番号 */
#define INTNO_DMA1_STREAM5	(DMA1_Stream5_IRQn + 16)	/* 割込み番号 */
#define INTPRI_DMA1_STREAM5	(TMIN_INTPRI + 4)	/* 割込み優先度 */
#define INTATR_DMA1_STREAM5	TA_NULL				/* 割込み属性 */

#define INHNO_DMA1_STREAM6	(DMA1_Stream6_IRQn + 16)	/* 割込みハンドラ番号 */
#define INTNO_DMA1_STREAM6	(DMA1_Stream6_IRQn + 16)	/* 割込み番号 */
#define INTPRI_DMA1_STREAM6	(TMIN_INTPRI + 1)	/* 割込み優先度 */
#define INTATR_DMA1_STREAM6	TA_NULL				/* 割込み属性 */

#define INHNO_DMA1_STREAM7	(DMA1_Stream7_IRQn + 16)	/* 割込みハンドラ番号 */
#define INTNO_DMA1_STREAM7	(DMA1_Stream7_IRQn + 16)	/* 割込み番号 */
#define INTPRI_DMA1_STREAM7	(TMIN_INTPRI + 1)	/* 割込み優先度 */
#define INTATR_DMA1_STREAM7	TA_NULL				/* 割込み属性 */

#define INHNO_DMA2_STREAM0	(DMA2_Stream0_IRQn + 16)	/* 割込みハンドラ番号 */
#define INTNO_DMA2_STREAM0	(DMA2_Stream0_IRQn + 16)	/* 割込み番号 */
#define INTPRI_DMA2_STREAM0	(TMIN_INTPRI + 7)	/* 割込み優先度 */
#define INTATR_DMA2_STREAM0	TA_NULL				/* 割込み属性 */

#define INHNO_DMA2_STREAM2	(DMA2_Stream2_IRQn + 16)	/* 割込みハンドラ番号 */
#define INTNO_DMA2_STREAM2	(DMA2_Stream2_IRQn + 16)	/* 割込み番号 */
#define INTPRI_DMA2_STREAM2	(TMIN_INTPRI + 7)	/* 割込み優先度 */
#define INTATR_DMA2_STREAM2	TA_NULL				/* 割込み属性 */

#define INHNO_DMA2_STREAM3	(DMA2_Stream3_IRQn + 16)	/* 割込みハンドラ番号 */
#define INTNO_DMA2_STREAM3	(DMA2_Stream3_IRQn + 16)	/* 割込み番号 */
#define INTPRI_DMA2_STREAM3	(TMIN_INTPRI + 7)	/* 割込み優先度 */
#define INTATR_DMA2_STREAM3	TA_NULL				/* 割込み属性 */

#define INHNO_USART2		(USART2_IRQn + 16)	/* 割込みハンドラ番号 */
#define INTNO_USART2		(USART2_IRQn + 16)	/* 割込み番号 */
#define INTPRI_USART2	(TMIN_INTPRI + 1)	/* 割込み優先度 */
#define INTATR_USART2	TA_NULL				/* 割込み属性 */

#define INHNO_UART4		(UART4_IRQn + 16)	/* 割込みハンドラ番号 */
#define INTNO_UART4		(UART4_IRQn + 16)	/* 割込み番号 */
#define INTPRI_UART4	(TMIN_INTPRI + 0)	/* 割込み優先度 */
#define INTATR_UART4	TA_NULL				/* 割込み属性 */

#define INHNO_UART5		(UART5_IRQn + 16)	/* 割込みハンドラ番号 */
#define INTNO_UART5		(UART5_IRQn + 16)	/* 割込み番号 */
#define INTPRI_UART5	(TMIN_INTPRI + 0)	/* 割込み優先度 */
#define INTATR_UART5	TA_NULL				/* 割込み属性 */

#define INHNO_UART7		(UART7_IRQn + 16)	/* 割込みハンドラ番号 */
#define INTNO_UART7		(UART7_IRQn + 16)	/* 割込み番号 */
#define INTPRI_UART7	(TMIN_INTPRI + 0)	/* 割込み優先度 */
#define INTATR_UART7	TA_NULL				/* 割込み属性 */

#define INHNO_UART8		(UART8_IRQn + 16)	/* 割込みハンドラ番号 */
#define INTNO_UART8		(UART8_IRQn + 16)	/* 割込み番号 */
#define INTPRI_UART8	(TMIN_INTPRI + 0)	/* 割込み優先度 */
#define INTATR_UART8	TA_NULL				/* 割込み属性 */

#define INHNO_UART9		(UART9_IRQn + 16)	/* 割込みハンドラ番号 */
#define INTNO_UART9		(UART9_IRQn + 16)	/* 割込み番号 */
#define INTPRI_UART9	(TMIN_INTPRI + 0)	/* 割込み優先度 */
#define INTATR_UART9	TA_NULL				/* 割込み属性 */

#define INHNO_UART10	(UART10_IRQn + 16)	/* 割込みハンドラ番号 */
#define INTNO_UART10	(UART10_IRQn + 16)	/* 割込み番号 */
#define INTPRI_UART10	(TMIN_INTPRI + 0)	/* 割込み優先度 */
#define INTATR_UART10	TA_NULL				/* 割込み属性 */

#define INHNO_SPI1	(SPI1_IRQn + 16)	/* 割込みハンドラ番号 */
#define INTNO_SPI1	(SPI1_IRQn + 16)	/* 割込み番号 */
#define INTPRI_SPI1	(TMIN_INTPRI + 7)	/* 割込み優先度 */
#define INTATR_SPI1	TA_NULL				/* 割込み属性 */

#define INHNO_OTG_FS	(OTG_FS_IRQn + 16)	/* 割込みハンドラ番号 */
#define INTNO_OTG_FS	(OTG_FS_IRQn + 16)	/* 割込み番号 */
#define INTPRI_OTG_FS	(TMIN_INTPRI + 6)	/* 割込み優先度 */
#define INTATR_OTG_FS	TA_NULL				/* 割込み属性 */

#define INHNO_I2C2_ER	(I2C2_ER_IRQn + 16)	/* 割込みハンドラ番号 */
#define INTNO_I2C2_ER	(I2C2_ER_IRQn + 16)	/* 割込み番号 */
#define INTPRI_I2C2_ER	(TMIN_INTPRI + 3)	/* 割込み優先度 */
#define INTATR_I2C2_ER	TA_NULL				/* 割込み属性 */

#define INHNO_I2C2_EV	(I2C2_EV_IRQn + 16)	/* 割込みハンドラ番号 */
#define INTNO_I2C2_EV	(I2C2_EV_IRQn + 16)	/* 割込み番号 */
#define INTPRI_I2C2_EV	(TMIN_INTPRI + 3)	/* 割込み優先度 */
#define INTATR_I2C2_EV	TA_NULL				/* 割込み属性 */



/*
 *  関数のプロトタイプ宣言
 */
#ifndef TOPPERS_MACRO_ONLY

extern void	pb_main_task(intptr_t exinf);
extern void pbdrv_timer_handler(void);

#endif /* TOPPERS_MACRO_ONLY */
