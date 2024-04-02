/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2018-2020 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: test_ovrhdr1.c 1437 2020-05-20 12:12:16Z ertl-hiro $
 */

/* 
 *		オーバランハンドラ機能のテスト(1)
 *
 * 【テストの目的】
 *
 *	オーバランハンドラ機能に関する仕様の中で，ASP3カーネルとHRP3カーネ
 *	ルの両者に共通で，オーバランハンドラを定義した状態で，機能テストに
 *	よりテストできるものをテストする．
 *
 * 【テスト項目】
 *
 *	test_ovrhdr.txtを参照すること．
 *
 * 【使用リソース】
 *
 *	TASK1: 中優先度タスク，メインタスク，最初から起動
 *	TASK2: 高優先度タスク
 *	ALM1:  アラームハンドラ
 *	OVR:   オーバランハンドラ
 *
 * 【テストシーケンス】
 *
 *	== TASK1（優先度：中）==
 *	1:	ref_ovr(TASK2, &rovr)								...［NGKI2656_T1］
 *															...［NGKI2661_T3］
 *															...［NGKI2662_T3］
 *		assert(rovr.ovrstat == TOVR_STP)					...［NGKI2587_T1］
 *															...［NGKI2665_T1］
 *		// TASK2に対して，残りプロセッサ時間(2 * UNIT_TIME)で，
 *		// オーバランハンドラを動作開始する
 *		sta_ovr(TASK2, 2 * UNIT_TIME)						...［NGKI3546_T1］
 *															...［NGKI2637_T3］
 *		ref_ovr(TASK2, &rovr)
 *		assert(rovr.ovrstat == TOVR_STA)					...［NGKI2639_T1］
 *															...［NGKI2665_T2］
 *		assert(rovr.leftotm == 2 * UNIT_TIME)				...［NGKI3771_T1］
 *															...［NGKI2639_T2］
 *		// TASK2を起動する
 *		act_tsk(TASK2)
 *	== TASK2-1（優先度：高，1回目）==
 *		// UNIT_TIME後にALM1を動作させる
 *	2:	sta_alm(ALM1, UNIT_TIME)
 *		DO(while(!task2_flag1))			// ここでUNIT_TIMEを消費
 *	== ALM1-1（1回目）==
 *		// TASK2の実行途中でALM1が動作する
 *	3:	DO(task2_flag1 = true)
 *		RETURN
 *	== TASK2-1（続き）==
 *	4:	ref_ovr(TSK_SELF, &rovr)							...［NGKI2669_T1］
 *		assert(rovr.ovrstat == TOVR_STA)
 *		assert(0 < rovr.leftotm && rovr.leftotm < UNIT_TIME)...［NGKI2588_T1］
 *															...［NGKI2590_T1］
 *															...［NGKI2666_T1］
 *		DO(while(!task2_flag2))			// ここで残りの時間を消費
 *	== OVR-1（1回目）==										...［NGKI2589_T1］
 *		// TASK2の残りプロセッサ時間がなくなる
 *	5:	assert(tskid == TASK2)								...［NGKI2605_T1］
 *		assert(exinf == 2)									...［NGKI2605_T2］
 *		DO(task2_flag2 = true)
 *		RETURN
 *	== TASK2-1（続き）==
 *	6:	ref_ovr(TSK_SELF, &rovr)
 *		assert(rovr.ovrstat == TOVR_STP)					...［NGKI3993_T1］
 *		ext_tsk()
 *	== TASK1（続き）==
 *	7:	ref_ovr(TASK2, &rovr)
 *		assert(rovr.ovrstat == TOVR_STP)					...［NGKI2587_T2］
 *		stp_ovr(TASK2)										...［NGKI3547_T1］
 *															...［NGKI2651_T3］
 *		assert(rovr.ovrstat == TOVR_STP)					...［NGKI2654_T1］
 *		// TASK2を起動する
 *		act_tsk(TASK2)
 *	== TASK2-2（優先度：高，2回目）==
 *	8:	sta_ovr(TSK_SELF, UNIT_TIME)						...［NGKI2641_T1］
 *		ref_ovr(TSK_SELF, &rovr)
 *		assert(rovr.ovrstat == TOVR_STA)
 *		stp_ovr(TSK_SELF)									...［NGKI2655_T1］
 *		ref_ovr(TSK_SELF, &rovr)
 *		assert(rovr.ovrstat == TOVR_STP)					...［NGKI2653_T1］
 *		// UNIT_TIME後にALM1を動作させる
 *		sta_alm(ALM1, UNIT_TIME)
 *		DO(while(!task2_flag3))			// ここでUNIT_TIMEを消費
 *	== ALM1-2（2回目）==
 *		// TASK2の実行途中でALM1が動作する
 *	9:	sta_ovr(TASK2, UNIT_TIME)							...［NGKI3546_T2］
 *		DO(task2_flag3 = true)
 *		RETURN
 *	== TASK2-2（続き）==
 *	10:	ref_ovr(TSK_SELF, &rovr)
 *		assert(rovr.ovrstat == TOVR_STA)
 *		assert(0 < rovr.leftotm && rovr.leftotm < UNIT_TIME)
 *		sta_ovr(TSK_SELF, 2 * UNIT_TIME)
 *		ref_ovr(TSK_SELF, &rovr)
 *		assert(rovr.ovrstat == TOVR_STA)					...［NGKI2640_T1］
 *		assert(UNIT_TIME < rovr.leftotm && rovr.leftotm < 2 * UNIT_TIME)
 *															...［NGKI2640_T2］
 *		// UNIT_TIME後にALM1を動作させる
 *		sta_alm(ALM1, UNIT_TIME)
 *		DO(while(!task2_flag4))			// ここでUNIT_TIMEを消費
 *	== ALM1-3（3回目）==
 *		// TASK2の実行途中でALM1が動作する
 *	11:	stp_ovr(TASK2)										...［NGKI3547_T2］
 *		DO(task2_flag4 = true)
 *		RETURN
 *	== TASK2-2（続き）==
 *	12:	ref_ovr(TSK_SELF, &rovr)
 *		assert(rovr.ovrstat == TOVR_STP)
 *
 *		// エラーチェックのテスト
 *	13:	loc_cpu()
 *		sta_ovr(TSK_SELF, UNIT_TIME) -> E_CTX				...［NGKI2634_T1］
 *		stp_ovr(TSK_SELF) -> E_CTX							...［NGKI2648_T1］
 *		ref_ovr(TSK_SELF, &rovr) -> E_CTX					...［NGKI2658_T1］
 *		unl_cpu()
 *		sta_ovr(TSKID_TOO_SMALL_TASK, UNIT_TIME) -> E_ID	...［NGKI2635_T1］
 *		sta_ovr(TSKID_TOO_LARGE, UNIT_TIME) -> E_ID			...［NGKI2635_T3］
 *		stp_ovr(TSKID_TOO_SMALL_TASK) -> E_ID				...［NGKI2649_T1］
 *		stp_ovr(TSKID_TOO_LARGE) -> E_ID					...［NGKI2649_T3］
 *		ref_ovr(TSKID_TOO_SMALL_TASK, &rovr) -> E_ID		...［NGKI2659_T1］
 *		ref_ovr(TSKID_TOO_LARGE, &rovr) -> E_ID				...［NGKI2659_T2］
 *		sta_ovr(TSK_SELF, 0U) -> E_PAR						...［NGKI2643_T1］
 *		sta_ovr(TSK_SELF, TMAX_OVRTIM + 1) -> E_PAR			...［NGKI2595_T2］
 *															...［NGKI2643_T2］
 *		sta_ovr(TSK_SELF, TMAX_OVRTIM)						...［NGKI2595_T1］
 *		// UNIT_TIME後にALM1を動作させる
 *		sta_alm(ALM1, UNIT_TIME)
 *		DO(while(!task2_flag5))			// ここでUNIT_TIMEを消費
 *	== ALM1-4（4回目）==
 *		// TASK2の実行途中でALM1が動作する
 *	14:	sta_ovr(TSKID_TOO_SMALL_INT, UNIT_TIME) -> E_ID		...［NGKI2635_T2］
 *		stp_ovr(TSKID_TOO_SMALL_INT) -> E_ID				...［NGKI2649_T2］
 *		ref_ovr(TASK2, &rovr) -> E_CTX						...［NGKI2657_T1］
 *		DO(task2_flag5 = true)
 *		RETURN
 *	== TASK2-2（続き）==
 *	15:	END
 */

#include <kernel.h>
#include <t_syslog.h>
#include "syssvc/test_svc.h"
#include "kernel_cfg.h"
#include "test_ovrhdr1.h"

/*
 *  範囲外のタスクIDの定義
 */
#define TSKID_TOO_SMALL_TASK	(-1)
#define TSKID_TOO_SMALL_INT		0
#define TSKID_TOO_LARGE			((TNUM_TSKID) + 1)

/*
 *  TOPPERS_SUPPORT_OVRHDRがマクロ定義されていることの確認［NGKI2599_T1］
 */
#ifndef TOPPERS_SUPPORT_OVRHDR
#error TOPPERS_SUPPORT_OVRHDR is not defined.
#endif /* TOPPERS_SUPPORT_OVRHDR */

volatile bool_t	task2_flag1 = false;
volatile bool_t	task2_flag2 = false;
volatile bool_t	task2_flag3 = false;
volatile bool_t	task2_flag4 = false;
volatile bool_t	task2_flag5 = false;

/* DO NOT DELETE THIS LINE -- gentest depends on it. */

static uint_t	alarm1_count = 0;

void
alarm1_handler(EXINF exinf)
{
	ER_UINT	ercd;
	T_ROVR	rovr;

	switch (++alarm1_count) {
	case 1:
		check_point(3);
		task2_flag1 = true;

		return;

		check_point(0);

	case 2:
		check_point(9);
		ercd = sta_ovr(TASK2, UNIT_TIME);
		check_ercd(ercd, E_OK);

		task2_flag3 = true;

		return;

		check_point(0);

	case 3:
		check_point(11);
		ercd = stp_ovr(TASK2);
		check_ercd(ercd, E_OK);

		task2_flag4 = true;

		return;

		check_point(0);

	case 4:
		check_point(14);
		ercd = sta_ovr(TSKID_TOO_SMALL_INT, UNIT_TIME);
		check_ercd(ercd, E_ID);

		ercd = stp_ovr(TSKID_TOO_SMALL_INT);
		check_ercd(ercd, E_ID);

		ercd = ref_ovr(TASK2, &rovr);
		check_ercd(ercd, E_CTX);

		task2_flag5 = true;

		return;

		check_point(0);

	default:
		check_point(0);
	}
	check_point(0);
}

static uint_t	overrun_count = 0;

void
overrun_handler(ID tskid, EXINF exinf)
{

	switch (++overrun_count) {
	case 1:
		check_point(5);
		check_assert(tskid == TASK2);

		check_assert(exinf == 2);

		task2_flag2 = true;

		return;

		check_point(0);

	default:
		check_point(0);
	}
	check_point(0);
}

void
task1(EXINF exinf)
{
	ER_UINT	ercd;
	T_ROVR	rovr;

	test_start(__FILE__);

	check_point(1);
	ercd = ref_ovr(TASK2, &rovr);
	check_ercd(ercd, E_OK);

	check_assert(rovr.ovrstat == TOVR_STP);

	ercd = sta_ovr(TASK2, 2 * UNIT_TIME);
	check_ercd(ercd, E_OK);

	ercd = ref_ovr(TASK2, &rovr);
	check_ercd(ercd, E_OK);

	check_assert(rovr.ovrstat == TOVR_STA);

	check_assert(rovr.leftotm == 2 * UNIT_TIME);

	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(7);
	ercd = ref_ovr(TASK2, &rovr);
	check_ercd(ercd, E_OK);

	check_assert(rovr.ovrstat == TOVR_STP);

	ercd = stp_ovr(TASK2);
	check_ercd(ercd, E_OK);

	check_assert(rovr.ovrstat == TOVR_STP);

	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(0);
}

static uint_t	task2_count = 0;

void
task2(EXINF exinf)
{
	ER_UINT	ercd;
	T_ROVR	rovr;

	switch (++task2_count) {
	case 1:
		check_point(2);
		ercd = sta_alm(ALM1, UNIT_TIME);
		check_ercd(ercd, E_OK);

		while(!task2_flag1);

		check_point(4);
		ercd = ref_ovr(TSK_SELF, &rovr);
		check_ercd(ercd, E_OK);

		check_assert(rovr.ovrstat == TOVR_STA);

		check_assert(0 < rovr.leftotm && rovr.leftotm < UNIT_TIME);

		while(!task2_flag2);

		check_point(6);
		ercd = ref_ovr(TSK_SELF, &rovr);
		check_ercd(ercd, E_OK);

		check_assert(rovr.ovrstat == TOVR_STP);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_point(0);

	case 2:
		check_point(8);
		ercd = sta_ovr(TSK_SELF, UNIT_TIME);
		check_ercd(ercd, E_OK);

		ercd = ref_ovr(TSK_SELF, &rovr);
		check_ercd(ercd, E_OK);

		check_assert(rovr.ovrstat == TOVR_STA);

		ercd = stp_ovr(TSK_SELF);
		check_ercd(ercd, E_OK);

		ercd = ref_ovr(TSK_SELF, &rovr);
		check_ercd(ercd, E_OK);

		check_assert(rovr.ovrstat == TOVR_STP);

		ercd = sta_alm(ALM1, UNIT_TIME);
		check_ercd(ercd, E_OK);

		while(!task2_flag3);

		check_point(10);
		ercd = ref_ovr(TSK_SELF, &rovr);
		check_ercd(ercd, E_OK);

		check_assert(rovr.ovrstat == TOVR_STA);

		check_assert(0 < rovr.leftotm && rovr.leftotm < UNIT_TIME);

		ercd = sta_ovr(TSK_SELF, 2 * UNIT_TIME);
		check_ercd(ercd, E_OK);

		ercd = ref_ovr(TSK_SELF, &rovr);
		check_ercd(ercd, E_OK);

		check_assert(rovr.ovrstat == TOVR_STA);

		check_assert(UNIT_TIME < rovr.leftotm && rovr.leftotm < 2 * UNIT_TIME);

		ercd = sta_alm(ALM1, UNIT_TIME);
		check_ercd(ercd, E_OK);

		while(!task2_flag4);

		check_point(12);
		ercd = ref_ovr(TSK_SELF, &rovr);
		check_ercd(ercd, E_OK);

		check_assert(rovr.ovrstat == TOVR_STP);

		check_point(13);
		ercd = loc_cpu();
		check_ercd(ercd, E_OK);

		ercd = sta_ovr(TSK_SELF, UNIT_TIME);
		check_ercd(ercd, E_CTX);

		ercd = stp_ovr(TSK_SELF);
		check_ercd(ercd, E_CTX);

		ercd = ref_ovr(TSK_SELF, &rovr);
		check_ercd(ercd, E_CTX);

		ercd = unl_cpu();
		check_ercd(ercd, E_OK);

		ercd = sta_ovr(TSKID_TOO_SMALL_TASK, UNIT_TIME);
		check_ercd(ercd, E_ID);

		ercd = sta_ovr(TSKID_TOO_LARGE, UNIT_TIME);
		check_ercd(ercd, E_ID);

		ercd = stp_ovr(TSKID_TOO_SMALL_TASK);
		check_ercd(ercd, E_ID);

		ercd = stp_ovr(TSKID_TOO_LARGE);
		check_ercd(ercd, E_ID);

		ercd = ref_ovr(TSKID_TOO_SMALL_TASK, &rovr);
		check_ercd(ercd, E_ID);

		ercd = ref_ovr(TSKID_TOO_LARGE, &rovr);
		check_ercd(ercd, E_ID);

		ercd = sta_ovr(TSK_SELF, 0U);
		check_ercd(ercd, E_PAR);

		ercd = sta_ovr(TSK_SELF, TMAX_OVRTIM + 1);
		check_ercd(ercd, E_PAR);

		ercd = sta_ovr(TSK_SELF, TMAX_OVRTIM);
		check_ercd(ercd, E_OK);

		ercd = sta_alm(ALM1, UNIT_TIME);
		check_ercd(ercd, E_OK);

		while(!task2_flag5);

		check_finish(15);
		check_point(0);

	default:
		check_point(0);
	}
	check_point(0);
}
