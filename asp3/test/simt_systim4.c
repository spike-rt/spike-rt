/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2014-2018 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: simt_systim4.c 1439 2020-05-22 20:02:23Z ertl-hiro $
 */

/* 
 *		システム時刻管理機能のテスト(4)
 *
 * 【テストの目的】
 *
 *  高分解能タイマモジュールの性質が異なる場合のテスト．高分解能タイマ
 *  モジュールの性質を表す3つの定数を参照している箇所を網羅的にテストす
 *  る．
 *
 * 【テスト項目】
 *
 *	(A) update_current_evttimでTCYC_HRTCNTを参照している箇所のテスト
 *	  (A-1) カウント値が周回していない場合
 *	  (A-2) カウント値が周回している場合
 *	  (A-3) カウント値が前回と同じ場合（境界のケース）
 *	(B) calc_current_evttim_ub（TSTEP_HRTCNTを参照している）を呼び出し
 *		ている処理のテスト
 *	  (B-1) tmevtb_enqueueでのタイムイベントの発生時刻の計算
 *	  (B-2) tmevt_lefttimでのタイムイベントが発生するまでの時間の計算
 *	(C) set_hrt_eventでHRTCNT_BOUNDを参照している箇所のテスト
 *	  (C-1) 登録されているタイムイベントがない時
 *	  (C-2) 先頭のタイムイベントまでの時間がHRTCNT_BOUNDを超える時
 *	  (C-3) 先頭のタイムイベントまでの時間がHRTCNT_BOUND以下の時
 *
 * 【使用リソース】
 *
 *	高分解能タイマモジュールの性質：HRT_CONFIG2
 *		TCYC_HRTCNT		(0x10000U * 10U)
 *		TSTEP_HRTCNT	10U
 *		HRTCNT_BOUND	(0x10000U * 9U)
 *
 *	タイマドライバシミュレータのパラメータ
 *		SIMTIM_INIT_CURRENT		10
 *		SIMTIM_OVERHEAD_HRTINT	10
 *
 *	TASK1: 中優先度タスク，メインタスク，最初から起動
 *	ALM1:  アラームハンドラ
 *	ALM2:  アラームハンドラ
 *	ALM3:  アラームハンドラ
 *
 * 【補足説明】
 *
 *	以下のテストシーケンスのコメント中で，「時刻：yyy」とは，高分解能
 *	タイマのカウント値がyyyになっていることを意味する．また，「発生：
 *	xxx」とは，高分解能タイマのカウント値がxxxになった時にタイムイベン
 *	トが発生することを意味する．タイムイベントのイベント発生時刻ではな
 *	いことに注意せよ．
 *
 * 【テストシーケンス】
 *
 *	== START ==
 *	// カーネル起動．高分解能タイマのカウント値とイベント時刻は10ずれる
 *	1:		[hook_hrt_set_event <- HRTCNT_EMPTY]
 *	== TASK1（優先度：中）==
 *	// タイムイベントを2つ登録
 *	2:	assert(fch_hrt() == 10U)							// 時刻：10
 *	3:	sta_alm(ALM1, 100U)					... (A-1)		// 発生：120
 *	4:		[hook_hrt_set_event <- 110U]	... (B-1)(C-3)
 *	5:	sta_alm(ALM2, 115U)					... (A-3)		// 発生：135
 *	6:	slp_tsk()
 *	== HRT_HANDLER ==										// 時刻：120
 *	== ALM1-1（1回目）==
 *	7:	assert(fch_hrt() == 130U)							// 時刻：130
 *	8:	wup_tsk(TASK1)
 *		RETURN
 *	9:		[hook_hrt_set_event <- 5U]		... (B-1)
 *	== TASK1（続き）== 								// ALM2までの時間は5
 *	10:	DO(simtim_advance(10U))
 *	== HRT_HANDLER ==										// 時刻：140
 *	== ALM2-1（1回目）==
 *	11:	assert(fch_hrt() == 150U)							// 時刻：150
 *		RETURN
 *	// タイムイベントがなくなる					... (C-1)
 *	12:		[hook_hrt_set_event <- HRTCNT_EMPTY]
 *	== TASK1（続き）==
 *	// 発生時刻までの長いタイムイベントを登録
 *	13:	sta_alm(ALM1, 1000000U)					... (C-2)	// 発生：1000160
 *	14:		[hook_hrt_set_event <- HRTCNT_BOUND]			//		→ 344800
 *	15:	DO(simtim_advance(10U))
 *		assert(fch_hrt() == 160U)							// 時刻：160
 *	16:	ref_alm(ALM1, &ralm)					... (B-2)
 *		assert(ralm.lefttim == 999990U)
 *	// ここで長い時間が経過したと想定
 *	17:	DO(simtim_advance(1000000U))
 *	== HRT_HANDLER ==										// 時刻：589980
 *	18:		[hook_hrt_set_event <- 410170U]
 *	== TASK1（続き）==
 *	// ここで長い時間が経過したと想定（時刻：655359 → 0）
 *	== HRT_HANDLER ==							... (A-2)	// 時刻：344800
 *	== ALM1-2（2回目）==
 *	19:	assert(fch_hrt() == 344810U)						// 時刻：344810
 *		RETURN
 *	20:		[hook_hrt_set_event <- HRTCNT_EMPTY]
 *	== TASK1（続き）==
 *	21:	END
 */

#include <kernel.h>
#include <t_syslog.h>
#include "syssvc/test_svc.h"
#include "arch/simtimer/sim_timer_cntl.h"
#include "kernel_cfg.h"
#include "simt_systim4.h"

#ifndef HRT_CONFIG2
#error Compiler option "-DHRT_CONFIG2" is missing.
#endif /* HRT_CONFIG2 */

#ifndef HOOK_HRT_EVENT
#error Compiler option "-DHOOK_HRT_EVENT" is missing.
#endif /* HOOK_HRT_EVENT */

/*
 *  HRTCNT_EMPTYの定義
 */
#define HRTCNT_EMPTY	HRTCNT_BOUND

void
hook_hrt_raise_event(void)
{
	check_point(0);
}

/* DO NOT DELETE THIS LINE -- gentest depends on it. */

static uint_t	alarm1_count = 0;

void
alarm1_handler(EXINF exinf)
{
	ER_UINT	ercd;

	switch (++alarm1_count) {
	case 1:
		check_point(7);
		check_assert(fch_hrt() == 130U);

		check_point(8);
		ercd = wup_tsk(TASK1);
		check_ercd(ercd, E_OK);

		return;

		check_point(0);

	case 2:
		check_point(19);
		check_assert(fch_hrt() == 344810U);

		return;

		check_point(0);

	default:
		check_point(0);
	}
	check_point(0);
}

static uint_t	alarm2_count = 0;

void
alarm2_handler(EXINF exinf)
{

	switch (++alarm2_count) {
	case 1:
		check_point(11);
		check_assert(fch_hrt() == 150U);

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
	T_RALM	ralm;

	check_point(2);
	check_assert(fch_hrt() == 10U);

	check_point(3);
	ercd = sta_alm(ALM1, 100U);
	check_ercd(ercd, E_OK);

	check_point(5);
	ercd = sta_alm(ALM2, 115U);
	check_ercd(ercd, E_OK);

	check_point(6);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(10);
	simtim_advance(10U);

	check_point(13);
	ercd = sta_alm(ALM1, 1000000U);
	check_ercd(ercd, E_OK);

	check_point(15);
	simtim_advance(10U);

	check_assert(fch_hrt() == 160U);

	check_point(16);
	ercd = ref_alm(ALM1, &ralm);
	check_ercd(ercd, E_OK);

	check_assert(ralm.lefttim == 999990U);

	check_point(17);
	simtim_advance(1000000U);

	check_finish(21);
	check_point(0);
}

static uint_t	hook_hrt_set_event_count = 0;

void
hook_hrt_set_event(HRTCNT hrtcnt)
{

	switch (++hook_hrt_set_event_count) {
	case 1:
		test_start(__FILE__);

		check_point(1);
		check_assert(hrtcnt == HRTCNT_EMPTY);

		return;

		check_point(0);

	case 2:
		check_point(4);
		check_assert(hrtcnt == 110U);

		return;

		check_point(0);

	case 3:
		check_point(9);
		check_assert(hrtcnt == 5U);

		return;

		check_point(0);

	case 4:
		check_point(12);
		check_assert(hrtcnt == HRTCNT_EMPTY);

		return;

		check_point(0);

	case 5:
		check_point(14);
		check_assert(hrtcnt == HRTCNT_BOUND);

		return;

		check_point(0);

	case 6:
		check_point(18);
		check_assert(hrtcnt == 410170U);

		return;

		check_point(0);

	case 7:
		check_point(20);
		check_assert(hrtcnt == HRTCNT_EMPTY);

		return;

		check_point(0);

	default:
		check_point(0);
	}
	check_point(0);
}
