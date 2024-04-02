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
 *  $Id: test_ovrhdr2.c 1437 2020-05-20 12:12:16Z ertl-hiro $
 */

/* 
 *		オーバランハンドラ機能のテスト(2)
 *
 * 【テストの目的】
 *
 *	オーバランハンドラ機能を入れることで追加または変更されたコードのC1
 *	カバレッジを100%にするというテスト目標に対して追加で抽出したテスト
 *	項目に対応し，機能テストによりテストできるものをテストする．
 *
 * 【テスト項目】
 *
 *	test_ovrhdr.txtを参照すること．
 *
 * 【使用リソース】
 *
 *	TASK1: 低優先度タスク，メインタスク，最初から起動
 *	TASK2: 中優先度タスク
 *	TASK3: 高優先度タスク … ras_ter(TASK2)を発行するスク
 *	ALM1:  アラームハンドラ
 *	CPUEXC: CPU例外ハンドラ
 *	OVR:   オーバランハンドラ
 *
 * 【テストシーケンス】
 *
 *	== TASK1（優先度：低）==
 *		// TASK2に対して，残りプロセッサ時間(UNIT_TIME)で，
 *		// オーバランハンドラを動作開始する
 *	1:	sta_ovr(TASK2, UNIT_TIME)							...［OVRHDR_T1］
 *		ref_ovr(TASK2, &rovr)								...［OVRHDR_T8］
 *		assert(rovr.ovrstat == TOVR_STA)
 *		assert(rovr.leftotm == UNIT_TIME)
 *		// TASK2を起動する
 *		act_tsk(TASK2)
 *		// dispatchでオーバランタイマを操作しない
 *	== TASK2-1（優先度：中，1回目）==
 *		// start_rでオーバランタイマを動作開始する			...［OVRHDR_T13］
 *	2:	DO(while(!task2_flag1))			// ここで(UNIT_TIME)を消費
 *	== OVR-1（1回目）==
 *	3:	DO(task2_flag1 = true)
 *		RETURN
 *	== TASK2-1（続き）==
 *	4:	sta_ovr(TSK_SELF, UNIT_TIME)						...［OVRHDR_T3］
 *		ref_ovr(TSK_SELF, &rovr)							...［OVRHDR_T7］
 *		assert(rovr.ovrstat == TOVR_STA)
 *		assert(0 < rovr.leftotm && rovr.leftotm < UNIT_TIME)
 *		DO(while(!task2_flag2))			// ここで(UNIT_TIME)を消費
 *	== OVR-2（2回目）==
 *	5:	DO(task2_flag2 = true)
 *		RETURN
 *	== TASK2-1（続き）==
 *	6:	sta_ovr(TSK_SELF, TMAX_OVRTIM)
 *		sta_ovr(TSK_SELF, UNIT_TIME)						...［OVRHDR_T2］
 *		DO(while(!task2_flag3))			// ここで(UNIT_TIME)を消費
 *	== OVR-3（3回目）==
 *	7:	DO(task2_flag3 = true)
 *		RETURN
 *	== TASK2-1（続き）==
 *	8:	ext_tsk()
 *	== TASK1（続き）==
 *	9:	sta_ovr(TASK2, UNIT_TIME)
 *		stp_ovr(TASK2)										...［OVRHDR_T4］
 *		// TASK2を起動する
 *		act_tsk(TASK2)
 *		// dispatchでオーバランタイマを操作しない
 *	== TASK2-2（優先度：中，2回目）==
 *		// start_rでオーバランタイマを操作しない			...［OVRHDR_T14］
 *		// 以下では，オーバランハンドラが起動されないことを確認する
 *	10:	sta_alm(ALM1, 2 * UNIT_TIME)
 *		DO(while(!task2_flag4))			// ここで(2 * UNIT_TIME)を消費
 *	== ALM1-1（1回目）==
 *		// TASK2の実行途中でALM1が動作する
 *		// 割込み入口処理でオーバランタイマを操作しない		...［OVRHDR_T17］
 *	11:	DO(task2_flag4 = true)
 *		RETURN
 *		// 割込み出口処理でオーバランタイマを操作しない		...［OVRHDR_T19］
 *	== TASK2-2（続き）==
 *		// 以下では，オーバランハンドラが起動されないことを確認する
 *	12:	sta_alm(ALM1, 2 * UNIT_TIME)
 *		DO(while(!task2_flag5))			// ここで(2 * UNIT_TIME)を消費
 *	== ALM1-2（2回目）==
 *		// TASK2の実行途中でALM1が動作する
 *		// 割込み入口処理でオーバランタイマを操作しない
 *	13:	DO(task2_flag5 = true)
 *		RETURN
 *	== TASK2-2（続き）==
 *	14:	sta_ovr(TSK_SELF, UNIT_TIME)
 *		stp_ovr(TSK_SELF)									...［OVRHDR_T5］
 *		// 以下では，オーバランハンドラが起動されないことを確認する
 *		sta_alm(ALM1, 2 * UNIT_TIME)
 *		DO(while(!task2_flag6))			// ここで(2 * UNIT_TIME)を消費
 *	== ALM1-3（3回目）==
 *		// TASK2の実行途中でALM1が動作する
 *		// 割込み入口処理でオーバランタイマを操作しない
 *	15:	DO(task2_flag6 = true)
 *		RETURN
 *	== TASK2-2（続き）==
 *	16:	stp_ovr(TSK_SELF)									...［OVRHDR_T6］
 *		// 以下では，オーバランハンドラが起動されないことを確認する
 *		sta_alm(ALM1, 2 * UNIT_TIME)
 *		DO(while(!task2_flag7))			// ここで(2 * UNIT_TIME)を消費
 *	== ALM1-4（4回目）==
 *		// TASK2の実行途中でALM1が動作する
 *		// 割込み入口処理でオーバランタイマを操作しない
 *	17:	DO(task2_flag7 = true)
 *		RETURN
 *	== TASK2-2（続き）==
 *	18:	sta_ovr(TASK2, UNIT_TIME)
 *		// TASK2を待ち状態にする
 *		slp_tsk()
 *		// dispatchでオーバランタイマを停止する				...［OVRHDR_T9］
 *	== TASK1（続き）==
 *		// dispatch_rでオーバランタイマを操作しない			...［OVRHDR_T12］
 *		// 以下では，オーバランハンドラが起動されないことを確認する
 *	19:	sta_alm(ALM1, 2 * UNIT_TIME)
 *		DO(while(!task1_flag1))			// ここで(2 * UNIT_TIME)を消費
 *	== ALM1-5（5回目）==
 *		// TASK1の実行途中でALM1が動作する
 *		// 割込み入口処理でオーバランタイマを操作しない
 *	20:	DO(task1_flag1 = true)
 *		RETURN
 *	== TASK1（続き）==
 *	21:	wup_tsk(TASK2)
 *		// dispatchでオーバランタイマを操作しない			...［OVRHDR_T10］
 *	== TASK2-2（続き）==
 *		// dispatch_rでオーバランタイマを動作開始する		...［OVRHDR_T11］
 *	22:	DO(while(!task2_flag8))			// ここで(UNIT_TIME)を消費
 *	== OVR-4（4回目）==
 *	23:	DO(task2_flag8 = true)
 *		RETURN
 *	== TASK2-2（続き）==
 *	24:	sta_ovr(TSK_SELF, 2 * UNIT_TIME)
 *		sta_alm(ALM1, UNIT_TIME)
 *		DO(while(!task2_flag9))			// ここでUNIT_TIMEを消費
 *	== ALM1-6（6回目）==
 *		// TASK2の実行途中でALM1が動作する
 *		// 割込み入口処理でオーバランタイマが停止する		...［OVRHDR_T16］
 *	25:	DO(task2_flag9 = true)
 *		RETURN
 *		// 割込み出口処理でオーバランタイマを動作開始する	...［OVRHDR_T18］
 *	== TASK2-2（続き）==
 *	26:	DO(while(!task2_flag10))		// ここでUNIT_TIMEを消費
 *	== OVR-5（5回目）==
 *	27:	DO(task2_flag10 = true)
 *		RETURN
 *	== TASK2-2（続き）==
 *	28:	sta_alm(ALM1, UNIT_TIME)
 *		ext_tsk()
 *	== TASK1（続き）==
 *	29:	dly_tsk(UNIT_TIME)
 *	// アイドル
 *	== ALM1-7（7回目）==
 *		// アイドル中にALM1が動作する
 *		// 割込み入口処理で実行状態のタスクがない			...［OVRHDR_T15］
 *	30:	RETURN
 *	// アイドル
 *	== TASK1（続き）==
 *	31:	act_tsk(TASK2)
 *	== TASK2-3（優先度：中，3回目）==
 *	32:	sta_ovr(TSK_SELF, UNIT_TIME)
 *		chg_ipm(TMAX_INTPRI)
 *		act_tsk(TASK3)
 *		// TASK3がras_ter(TASK2)を発行
 *		chg_ipm(TIPM_ENAALL)								...［OVRHDR_T25］
 *	== TASK1（続き）==
 *	33:	act_tsk(TASK2)
 *	== TASK2-4（優先度：中，4回目）==
 *	34:	chg_ipm(TMAX_INTPRI)
 *		act_tsk(TASK3)
 *		// TASK3がras_ter(TASK2)を発行
 *		chg_ipm(TIPM_ENAALL)								...［OVRHDR_T26］
 *	== TASK1（続き）==
 *	35:	act_tsk(TASK2)
 *	== TASK2-5（優先度：中，5回目）==
 *	36:	sta_ovr(TSK_SELF, UNIT_TIME)
 *		dis_dsp()
 *		act_tsk(TASK3)
 *		// TASK3がras_ter(TASK2)を発行
 *		ena_dsp()											...［OVRHDR_T27］
 *	== TASK1（続き）==
 *	37:	act_tsk(TASK2)
 *	== TASK2-6（優先度：中，6回目）==
 *	38:	dis_dsp()
 *		act_tsk(TASK3)
 *		// TASK3がras_ter(TASK2)を発行
 *		ena_dsp()											...［OVRHDR_T28］
 *	== TASK1（続き）==
 *	39:	act_tsk(TASK2)
 *	== TASK2-7（優先度：中，7回目）==
 *	40:	ext_tsk()											...［OVRHDR_T29］
 *	== TASK1（続き）==
 *	41:	act_tsk(TASK2)
 *	== TASK2-8（優先度：中，8回目）==
 *	42:	ext_tsk()											...［OVRHDR_T30］
 *	== TASK1（続き）==
 *	43:	act_tsk(TASK2)
 *	== TASK2-9（優先度：中，9回目）==
 *	44:	sta_ovr(TSK_SELF, UNIT_TIME)
 *		dis_ter()
 *		act_tsk(TASK3)
 *		// TASK3がras_ter(TASK2)を発行
 *		ena_ter()											...［OVRHDR_T31］
 *	== TASK1（続き）==
 *	45:	act_tsk(TASK2)
 *	== TASK2-10（優先度：中，10回目）==
 *	46:	dis_ter()
 *		act_tsk(TASK3)
 *		// TASK3がras_ter(TASK2)を発行
 *		ena_ter()											...［OVRHDR_T32］
 *	== TASK1（続き）==
 *	// CPU例外ハンドラからリターンできない場合は，以下は行わない
 *	47:	HOOK(HOOK_POINT(%d))
 *		act_tsk(TASK2)
 *	== TASK2-11（優先度：中，11回目）==
 *	48:	sta_ovr(TSK_SELF, UNIT_TIME)
 *		DO(RAISE_CPU_EXCEPTION)
 *	== CPUEXC-1（1回目）==
 *		// CPU例外入口処理でオーバランタイマが停止する		...［OVRHDR_T21］
 *		RETURN
 *		// CPU例外出口処理でオーバランタイマを動作開始する	...［OVRHDR_T23］
 *	== TASK2-11（続き）==
 *	49:	DO(while(!task2_flag11))		// ここでUNIT_TIMEを消費
 *	== OVR-6（6回目）==
 *	50:	DO(task2_flag11 = true)
 *		RETURN
 *	== TASK2-11（続き）==
 *	51:	DO(RAISE_CPU_EXCEPTION)
 *	== CPUEXC-2（2回目）==
 *		// CPU例外入口処理でオーバランタイマを操作しない	...［OVRHDR_T22］
 *	52:	RETURN
 *		// CPU例外出口処理でオーバランタイマを操作しない	...［OVRHDR_T24］
 *	== TASK2-11（続き）==
 *		// 以下では，オーバランハンドラが起動されないことを確認する
 *	53:	sta_alm(ALM1, 2 * UNIT_TIME)
 *		DO(while(!task2_flag12))		// ここで(2 * UNIT_TIME)を消費
 *	== ALM1-8（8回目）==
 *		// TASK2の実行途中でALM1が動作する
 *		// 割込み入口処理でオーバランタイマを操作しない
 *	54:	DO(task2_flag12 = true)
 *		RETURN
 *	== TASK2-11（続き）==
 *	55:	END
 */

#include <kernel.h>
#include <t_syslog.h>
#include "syssvc/test_svc.h"
#include "kernel_cfg.h"
#include "test_ovrhdr2.h"

volatile bool_t	task1_flag1 = false;
volatile bool_t	task2_flag1 = false;
volatile bool_t	task2_flag2 = false;
volatile bool_t	task2_flag3 = false;
volatile bool_t	task2_flag4 = false;
volatile bool_t	task2_flag5 = false;
volatile bool_t	task2_flag6 = false;
volatile bool_t	task2_flag7 = false;
volatile bool_t	task2_flag8 = false;
volatile bool_t	task2_flag9 = false;
volatile bool_t	task2_flag10 = false;
volatile bool_t	task2_flag11 = false;
volatile bool_t	task2_flag12 = false;

#ifdef PREPARE_RETURN_CPUEXC
#define HOOK_POINT(count)	do {							\
								PREPARE_RETURN_CPUEXC;		\
								check_point(count);			\
							} while (false)
#else /* PREPARE_RETURN_CPUEXC */
#define HOOK_POINT(count)	check_finish(count)
#endif /* PREPARE_RETURN_CPUEXC */

void
task3(EXINF exinf)
{
	ER_UINT	ercd;

	ercd = ras_ter(TASK2);
	check_ercd(ercd, E_OK);
}

/* DO NOT DELETE THIS LINE -- gentest depends on it. */

static uint_t	alarm1_count = 0;

void
alarm1_handler(EXINF exinf)
{

	switch (++alarm1_count) {
	case 1:
		check_point(11);
		task2_flag4 = true;

		return;

		check_point(0);

	case 2:
		check_point(13);
		task2_flag5 = true;

		return;

		check_point(0);

	case 3:
		check_point(15);
		task2_flag6 = true;

		return;

		check_point(0);

	case 4:
		check_point(17);
		task2_flag7 = true;

		return;

		check_point(0);

	case 5:
		check_point(20);
		task1_flag1 = true;

		return;

		check_point(0);

	case 6:
		check_point(25);
		task2_flag9 = true;

		return;

		check_point(0);

	case 7:
		check_point(30);
		return;

		check_point(0);

	case 8:
		check_point(54);
		task2_flag12 = true;

		return;

		check_point(0);

	default:
		check_point(0);
	}
	check_point(0);
}

static uint_t	cpuexc_count = 0;

void
cpuexc_handler(void *p_excinf)
{

	switch (++cpuexc_count) {
	case 1:
		return;

		check_point(0);

	case 2:
		check_point(52);
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
		check_point(3);
		task2_flag1 = true;

		return;

		check_point(0);

	case 2:
		check_point(5);
		task2_flag2 = true;

		return;

		check_point(0);

	case 3:
		check_point(7);
		task2_flag3 = true;

		return;

		check_point(0);

	case 4:
		check_point(23);
		task2_flag8 = true;

		return;

		check_point(0);

	case 5:
		check_point(27);
		task2_flag10 = true;

		return;

		check_point(0);

	case 6:
		check_point(50);
		task2_flag11 = true;

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
	ercd = sta_ovr(TASK2, UNIT_TIME);
	check_ercd(ercd, E_OK);

	ercd = ref_ovr(TASK2, &rovr);
	check_ercd(ercd, E_OK);

	check_assert(rovr.ovrstat == TOVR_STA);

	check_assert(rovr.leftotm == UNIT_TIME);

	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(9);
	ercd = sta_ovr(TASK2, UNIT_TIME);
	check_ercd(ercd, E_OK);

	ercd = stp_ovr(TASK2);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(19);
	ercd = sta_alm(ALM1, 2 * UNIT_TIME);
	check_ercd(ercd, E_OK);

	while(!task1_flag1);

	check_point(21);
	ercd = wup_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(29);
	ercd = dly_tsk(UNIT_TIME);
	check_ercd(ercd, E_OK);

	check_point(31);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(33);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(35);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(37);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(39);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(41);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(43);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(45);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	HOOK_POINT(47);
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
		while(!task2_flag1);

		check_point(4);
		ercd = sta_ovr(TSK_SELF, UNIT_TIME);
		check_ercd(ercd, E_OK);

		ercd = ref_ovr(TSK_SELF, &rovr);
		check_ercd(ercd, E_OK);

		check_assert(rovr.ovrstat == TOVR_STA);

		check_assert(0 < rovr.leftotm && rovr.leftotm < UNIT_TIME);

		while(!task2_flag2);

		check_point(6);
		ercd = sta_ovr(TSK_SELF, TMAX_OVRTIM);
		check_ercd(ercd, E_OK);

		ercd = sta_ovr(TSK_SELF, UNIT_TIME);
		check_ercd(ercd, E_OK);

		while(!task2_flag3);

		check_point(8);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_point(0);

	case 2:
		check_point(10);
		ercd = sta_alm(ALM1, 2 * UNIT_TIME);
		check_ercd(ercd, E_OK);

		while(!task2_flag4);

		check_point(12);
		ercd = sta_alm(ALM1, 2 * UNIT_TIME);
		check_ercd(ercd, E_OK);

		while(!task2_flag5);

		check_point(14);
		ercd = sta_ovr(TSK_SELF, UNIT_TIME);
		check_ercd(ercd, E_OK);

		ercd = stp_ovr(TSK_SELF);
		check_ercd(ercd, E_OK);

		ercd = sta_alm(ALM1, 2 * UNIT_TIME);
		check_ercd(ercd, E_OK);

		while(!task2_flag6);

		check_point(16);
		ercd = stp_ovr(TSK_SELF);
		check_ercd(ercd, E_OK);

		ercd = sta_alm(ALM1, 2 * UNIT_TIME);
		check_ercd(ercd, E_OK);

		while(!task2_flag7);

		check_point(18);
		ercd = sta_ovr(TASK2, UNIT_TIME);
		check_ercd(ercd, E_OK);

		ercd = slp_tsk();
		check_ercd(ercd, E_OK);

		check_point(22);
		while(!task2_flag8);

		check_point(24);
		ercd = sta_ovr(TSK_SELF, 2 * UNIT_TIME);
		check_ercd(ercd, E_OK);

		ercd = sta_alm(ALM1, UNIT_TIME);
		check_ercd(ercd, E_OK);

		while(!task2_flag9);

		check_point(26);
		while(!task2_flag10);

		check_point(28);
		ercd = sta_alm(ALM1, UNIT_TIME);
		check_ercd(ercd, E_OK);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_point(0);

	case 3:
		check_point(32);
		ercd = sta_ovr(TSK_SELF, UNIT_TIME);
		check_ercd(ercd, E_OK);

		ercd = chg_ipm(TMAX_INTPRI);
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK3);
		check_ercd(ercd, E_OK);

		ercd = chg_ipm(TIPM_ENAALL);
		check_ercd(ercd, E_OK);

		check_point(0);

	case 4:
		check_point(34);
		ercd = chg_ipm(TMAX_INTPRI);
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK3);
		check_ercd(ercd, E_OK);

		ercd = chg_ipm(TIPM_ENAALL);
		check_ercd(ercd, E_OK);

		check_point(0);

	case 5:
		check_point(36);
		ercd = sta_ovr(TSK_SELF, UNIT_TIME);
		check_ercd(ercd, E_OK);

		ercd = dis_dsp();
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK3);
		check_ercd(ercd, E_OK);

		ercd = ena_dsp();
		check_ercd(ercd, E_OK);

		check_point(0);

	case 6:
		check_point(38);
		ercd = dis_dsp();
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK3);
		check_ercd(ercd, E_OK);

		ercd = ena_dsp();
		check_ercd(ercd, E_OK);

		check_point(0);

	case 7:
		check_point(40);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_point(0);

	case 8:
		check_point(42);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_point(0);

	case 9:
		check_point(44);
		ercd = sta_ovr(TSK_SELF, UNIT_TIME);
		check_ercd(ercd, E_OK);

		ercd = dis_ter();
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK3);
		check_ercd(ercd, E_OK);

		ercd = ena_ter();
		check_ercd(ercd, E_OK);

		check_point(0);

	case 10:
		check_point(46);
		ercd = dis_ter();
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK3);
		check_ercd(ercd, E_OK);

		ercd = ena_ter();
		check_ercd(ercd, E_OK);

		check_point(0);

	case 11:
		check_point(48);
		ercd = sta_ovr(TSK_SELF, UNIT_TIME);
		check_ercd(ercd, E_OK);

		RAISE_CPU_EXCEPTION;

		check_point(49);
		while(!task2_flag11);

		check_point(51);
		RAISE_CPU_EXCEPTION;

		check_point(53);
		ercd = sta_alm(ALM1, 2 * UNIT_TIME);
		check_ercd(ercd, E_OK);

		while(!task2_flag12);

		check_finish(55);
		check_point(0);

	default:
		check_point(0);
	}
	check_point(0);
}
