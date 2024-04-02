/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2018-2019 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: cfg_common.h 1441 2020-05-25 04:35:47Z ertl-hiro $
 */

/*
 *		コンフィギュレータのテストで共通に使用する定義と宣言
 */

#include <kernel.h>

/*
 *  優先度の定義
 */
#define HIGH_PRIORITY	9		/* 高優先度 */
#define MID_PRIORITY	10		/* 中優先度 */
#define LOW_PRIORITY	11		/* 低優先度 */

/*
 *  範囲外のパラメータ値の定義
 */
#define TSKID_LARGE		((TNUM_TSKID) + 1)
#define SEMID_LARGE		((TNUM_SEMID) + 1)
#define FLGID_LARGE		((TNUM_FLGID) + 1)
#define DTQID_LARGE		((TNUM_DTQID) + 1)

/*
 *  割込み番号，割込みハンドラ番号，CPU例外ハンドラ番号の定義
 */
#define INTNO1			4U
#define INTNO2			5U
#define INTNO3			6U
#define INTNO4			7U
#define INTNO5			8U
#define INTNO6			9U
#define INTNO7			10U
#define INTNO8			11U
#define INTNO_KERNEL	30U
#define INTNO_NONKERNEL	31U
#define INVALID_INTNO	10000U

#define INHNO1			4U
#define INHNO2			5U
#define INHNO3			6U
#define INHNO6			9U
#define INHNO7			10U
#define INHNO_KERNEL	30U
#define INHNO_NONKERNEL	31U
#define INVALID_INHNO	10000U

#define CPUEXC1			1U
#define CPUEXC2			2U
#define INVALID_CPUEXC	10000U

/*
 *  スタックサイズの定義
 */
#define	STACK_SIZE		4096		/* タスクのスタックサイズ */

/*
 *  スタック領域，オブジェクトの管理領域の定義
 */
extern STK_T stack_TASK2[];
extern STK_T stack_TASK3[];
extern STK_T stack_TASK5[];

extern MPF_T mpf_MPF1[];
extern MPF_T mpf_MPF2[];

extern MB_T	dtqmb_DTQ2[];
extern MB_T	pdqmb_PDQ4[];
extern MB_T	mpfmb_MPF4[];

extern STK_T istack[];

/*
 *  通知用の変数の宣言
 */
extern volatile intptr_t nvar1;
extern volatile intptr_t nvar2;

/*
 *  アラインされていないポインタを作る
 */
#define ADD1(par)	((void *)(((char *) par) + 1))

/*
 *  関数のプロトタイプ宣言
 */
extern void	task1(EXINF exinf);
extern void	task2(EXINF exinf);
extern void	task3(EXINF exinf);
extern void	task4(EXINF exinf);
extern void	task5(EXINF exinf);

extern void	cyclic1_handler(EXINF exinf);
extern void	cyclic2_handler(EXINF exinf);
extern void	cyclic3_handler(EXINF exinf);
extern void	cyclic4_handler(EXINF exinf);
extern void	cyclic6_handler(EXINF exinf);

extern void	alarm1_handler(EXINF exinf);
extern void	alarm2_handler(EXINF exinf);
extern void	alarm3_handler(EXINF exinf);

extern void isr1(EXINF exinf);
extern void isr2(EXINF exinf);
extern void isr3(EXINF exinf);
extern void isr4(EXINF exinf);
extern void isr5(EXINF exinf);

extern void	int1_handler(void);
extern void	int2_handler(void);
extern void	int3_handler(void);
extern void	int4_handler(void);
extern void	int5_handler(void);
extern void	int6_handler(void);
extern void	int7_handler(void);

extern void	cpuexc1_handler(void *p_excinf);
extern void	cpuexc2_handler(void *p_excinf);

extern void initialize1_routine(EXINF exinf);
extern void initialize2_routine(EXINF exinf);

extern void terminate1_routine(EXINF exinf);
extern void terminate2_routine(EXINF exinf);
