/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2014-2020 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: simt_drift1.c 1437 2020-05-20 12:12:16Z ertl-hiro $
 */

/* 
 *		ドリフト調整機能のテスト(1)
 *
 * 【テストの目的】
 *
 *  ドリフト調整機能を網羅的にテストする．具体的には，ドリフト調整機能
 *  を追加することで追加・拡張された関数を中心にテストする．
 *
 * 【テスト項目】
 *
 *	(A) set_timの要求ベーステスト
 *	  (A-1) CPUロック状態からの呼出し［NGKI3598］
 *	  (A-2) driftがTMIN_DRIFTより小さい［NGKI3599］
 *	  (A-3) driftがTMAX_DRIFTより大きい［NGKI3599］
 *	  (A-4) ドリフト量がdriftで指定した値に設定される［NGKI3601］
 *	(B) update_current_evttim
 *	  (B-1) ドリフト量が正で端数の繰り上がりがない時
 *	  (B-2) ドリフト量が正で端数の繰り上がりがある時
 *	  (B-3) ドリフト量が負で端数の繰り上がりがない時
 *	  (B-4) ドリフト量が負で端数の繰り上がりがある時
 *	  ※ (current_evttim_frac >= 2000000U)のパスは単体でテストしたい
 *	(C) calc_current_evttim_ub（tmevtb_enqueueからの呼び出し）
 *	  (C-1) ドリフト量が正で端数の繰り上がりがない時
 *	  (C-2) ドリフト量が正で端数の繰り上がりがある時
 *	  (C-3) ドリフト量が負で端数の繰り上がりがない時
 *	  (C-4) ドリフト量が負で端数の繰り上がりがある時
 *	(D) calc_current_evttim_ub（tmevt_lefttimからの呼び出し）
 *	  (D-1) ドリフト量が正で端数の繰り上がりがない時
 *	  (D-2) ドリフト量が正で端数の繰り上がりがある時
 *	  (D-3) ドリフト量が負で端数の繰り上がりがない時
 *	  (D-4) ドリフト量が負で端数の繰り上がりがある時
 *	(E) set_hrt_event
 *	  (E-1) ドリフト量が正でタイムイベントヒープが空の時
 *	  (E-2) ドリフト量が正でhrtcntがHRTCNT_BOUNDを超える時 … 別にテスト
 *	  (E-3) ドリフト量が正でhrtcntがHRTCNT_BOUNDを超えない時
 *	  (E-4) ドリフト量が負でタイムイベントヒープが空の時
 *	  (E-5) ドリフト量が負でhrtcntがHRTCNT_BOUNDを超える時 … 別にテスト
 *	  (E-6) ドリフト量が負でhrtcntがHRTCNT_BOUNDを超えない時
 *	  ※ (c1 > HRTCNT_BOUND / 1000000U)のパスは単体でテストしたい
 *
 * 【使用リソース】
 *
 *	高分解能タイマモジュールの性質：HRT_CONFIG1
 *		TCYC_HRTCNT		未定義（2^32の意味）
 *		TSTEP_HRTCNT	1U
 *		HRTCNT_BOUND	4000000002U
 *
 *	タイマドライバシミュレータのパラメータ
 *		SIMTIM_INIT_CURRENT		10
 *		SIMTIM_OVERHEAD_HRTINT	10
 *
 *	TASK1: 中優先度タスク，メインタスク，最初から起動
 *	ALM1:  アラームハンドラ
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
 *	2:	DO(simtim_advance(10U))
 *		assert(fch_hrt() == 20U)							// 時刻：20
 *	// ドリフト量を+1%に設定
 *	//		現時点：hrtcnt＝20，evttim＝10，evttim_frac＝0
 *	3:	set_dft(+10000)
 *	4:		[hook_hrt_set_event <- HRTCNT_EMPTY_P1P]	... (E-1)
 *	5:	DO(simtim_advance(10U))
 *		assert(fch_hrt() == 30U)							// 時刻：30
 *	// タイムイベントを登録
 *	6:	sta_alm(ALM1, 100)					... (C-2)		// 発生：131
 *	//		現時点：hrtcnt＝30，evttim＝20，evttim_frac＝100000
 *	//		大きい方に丸めた ： evttim＝21，evttim_frac＝110000
 *	//		相対時間：100，イベント発生時刻：122
 *	//		カウント値：(101900000 / 1010000)の切り上げ → 101
 *	7:		[hook_hrt_set_event <- 101U]	... (E-3)
 *	// ここで時間が経過したことを想定
 *	8:	DO(simtim_advance(89U))
 *		assert(fch_hrt() == 119U)							// 時刻：119
 *	//		現時点：hrtcnt＝119，evttim＝109，evttim_frac＝990000
 *	// ドリフト調整が効く直前
 *	9:	get_tim(&systim)					... (B-1)
 *		assert(systim == 109U)
 *	// ここでドリフト調整が効く（システム時刻が2進む）
 *	10:	DO(simtim_advance(1U))
 *		assert(fch_hrt() == 120U)							// 時刻：120
 *	//		現時点：hrtcnt＝120，evttim＝111，evttim_frac＝0
 *	11:	get_tim(&systim)					... (B-2)
 *		assert(systim == 111U)
 *	// ここで時間が経過したことを想定
 *	12:	DO(simtim_advance(11U))
 *	== HRT_HANDLER ==										// 時刻：131
 *	//		現時点：hrtcnt＝131，evttim＝122，evttim_frac＝110000
 *	== ALM1-1（1回目）==
 *	13:	assert(fch_hrt() == 141U)							// 時刻：141
 *		RETURN
 *	// タイムイベントがなくなる
 *	14:		[hook_hrt_set_event <- HRTCNT_EMPTY_P1P]
 *	== TASK1（続き）==
 *	// ここで時間が経過したことを想定
 *	15:	DO(simtim_advance(78U))
 *		assert(fch_hrt() == 219U)							// 時刻：219
 *	//		現時点：hrtcnt＝219，evttim＝210，evttim_frac＝990000
 *	// ドリフト調整が効く直前
 *	16:	get_tim(&systim)					... (B-1)
 *		assert(systim == 210U)
 *	// タイムイベントを登録
 *	17:	sta_alm(ALM1, 100U)					... (C-1)
 *	//		現時点：hrtcnt＝219，evttim＝210，evttim_frac＝990000
 *	//		大きい方に丸めた ：  evttim＝212，evttim_frac＝0
 *	//		相対時間：100，イベント発生時刻：312
 *	//		カウント値：(101010000 / 1010000)の切り上げ → 101
 *	18:		[hook_hrt_set_event <- 101U]
 *	// ここでドリフト調整が効く
 *	19:	DO(simtim_advance(1U))
 *		assert(fch_hrt() == 220U)							// 時刻：220
 *	//		現時点：hrtcnt＝220，evttim＝212，evttim_frac＝0
 *	20:	get_tim(&systim)					... (B-2)
 *		assert(systim == 212U)
 *
 *	// ドリフト量を-1%に設定（端数が0になるタイミングで設定）
 *	21:	set_dft(-10000)
 *	//		現時点：hrtcnt＝220，evttim＝212，evttim_frac＝0
 *	//		カウント値：(100000000 / 990000)の切り上げ → 102
 *	22:		[hook_hrt_set_event <- 102U]	... (E-6)
 *	// ドリフト調整がすぐに効く
 *	23:	DO(simtim_advance(1U))
 *		assert(fch_hrt() == 221U)							// 時刻：221
 *	//		現時点：hrtcnt＝221，evttim＝212，evttim_frac＝990000
 *	24:	get_tim(&systim)					... (B-3)
 *		assert(systim == 212U)
 *	// ここで時間が経過したことを想定
 *	25:	DO(simtim_advance(99U))
 *		assert(fch_hrt() == 320U)							// 時刻：320
 *	//		現時点：hrtcnt＝320，evttim＝311，evttim_frac＝0
 *	// ドリフト調整が次に効く直前
 *	26:	get_tim(&systim)
 *		assert(systim == 311U)
 *	// ここでドリフト調整が効く
 *	27:	DO(simtim_advance(1U))
 *		assert(fch_hrt() == 321U)							// 時刻：321
 *	//		現時点：hrtcnt＝321，evttim＝311，evttim_frac＝990000
 *	28:	get_tim(&systim)					... (B-3)
 *		assert(systim == 311U)
 *	29:	DO(simtim_advance(1U))
 *	== HRT_HANDLER ==										// 時刻：322
 *	//		現時点：hrtcnt＝322，evttim＝312，evttim_frac＝980000
 *	== ALM1-2（2回目）==
 *	30:	assert(fch_hrt() == 332U)							// 時刻：332
 *		RETURN
 *	//		現時点：hrtcnt＝332，evttim＝322，evttim_frac＝880000
 *	// タイムイベントがなくなる
 *	31:		[hook_hrt_set_event <- HRTCNT_EMPTY_M1P]	... (E-4)
 *	== TASK1（続き）==
 *	// ここで時間が経過したことを想定
 *	32:	DO(simtim_advance(38U))
 *		assert(fch_hrt() == 370U)							// 時刻：370
 *	//		現時点：hrtcnt＝370，evttim＝360，evttim_frac＝500000
 *
 *	// ドリフト量を+1%に設定（端数が0.5になるタイミングで設定）
 *	33:	set_dft(+10000)
 *	34:		[hook_hrt_set_event <- HRTCNT_EMPTY_P1P]
 *	35:	get_tim(&systim)
 *		assert(systim == 360U)
 *	// ここで時間が経過したことを想定
 *	36:	DO(simtim_advance(49U))
 *		assert(fch_hrt() == 419U)							// 時刻：419
 *	//		現時点：hrtcnt＝419，evttim＝409，evttim_frac＝990000
 *	// ドリフト調整が効く直前
 *	37:	get_tim(&systim)
 *		assert(systim == 409U)
 *	// ここでドリフト調整が効く
 *	38:	DO(simtim_advance(1U))
 *		assert(fch_hrt() == 420U)							// 時刻：420
 *	//		現時点：hrtcnt＝420，evttim＝411，evttim_frac＝0
 *	39:	get_tim(&systim)
 *		assert(systim == 411U)
 *	40:	END
 */

#include <kernel.h>
#include <t_syslog.h>
#include "syssvc/test_svc.h"
#include "arch/simtimer/sim_timer_cntl.h"
#include "kernel_cfg.h"
#include "simt_drift1.h"

#ifndef HRT_CONFIG1
#error Compiler option "-DHRT_CONFIG1" is missing.
#endif /* HRT_CONFIG1 */

#ifndef HOOK_HRT_EVENT
#error Compiler option "-DHOOK_HRT_EVENT" is missing.
#endif /* HOOK_HRT_EVENT */

/*
 *  HRTCNT_EMPTYの定義
 */
#define HRTCNT_EMPTY		4000000000U
#define HRTCNT_EMPTY_P1P	3960396040U
#define HRTCNT_EMPTY_M1P	HRTCNT_BOUND

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

	switch (++alarm1_count) {
	case 1:
		check_point(13);
		check_assert(fch_hrt() == 141U);

		return;

		check_point(0);

	case 2:
		check_point(30);
		check_assert(fch_hrt() == 332U);

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
	SYSTIM	systim;

	check_point(2);
	simtim_advance(10U);

	check_assert(fch_hrt() == 20U);

	check_point(3);
	ercd = set_dft(+10000);
	check_ercd(ercd, E_OK);

	check_point(5);
	simtim_advance(10U);

	check_assert(fch_hrt() == 30U);

	check_point(6);
	ercd = sta_alm(ALM1, 100);
	check_ercd(ercd, E_OK);

	check_point(8);
	simtim_advance(89U);

	check_assert(fch_hrt() == 119U);

	check_point(9);
	ercd = get_tim(&systim);
	check_ercd(ercd, E_OK);

	check_assert(systim == 109U);

	check_point(10);
	simtim_advance(1U);

	check_assert(fch_hrt() == 120U);

	check_point(11);
	ercd = get_tim(&systim);
	check_ercd(ercd, E_OK);

	check_assert(systim == 111U);

	check_point(12);
	simtim_advance(11U);

	check_point(15);
	simtim_advance(78U);

	check_assert(fch_hrt() == 219U);

	check_point(16);
	ercd = get_tim(&systim);
	check_ercd(ercd, E_OK);

	check_assert(systim == 210U);

	check_point(17);
	ercd = sta_alm(ALM1, 100U);
	check_ercd(ercd, E_OK);

	check_point(19);
	simtim_advance(1U);

	check_assert(fch_hrt() == 220U);

	check_point(20);
	ercd = get_tim(&systim);
	check_ercd(ercd, E_OK);

	check_assert(systim == 212U);

	check_point(21);
	ercd = set_dft(-10000);
	check_ercd(ercd, E_OK);

	check_point(23);
	simtim_advance(1U);

	check_assert(fch_hrt() == 221U);

	check_point(24);
	ercd = get_tim(&systim);
	check_ercd(ercd, E_OK);

	check_assert(systim == 212U);

	check_point(25);
	simtim_advance(99U);

	check_assert(fch_hrt() == 320U);

	check_point(26);
	ercd = get_tim(&systim);
	check_ercd(ercd, E_OK);

	check_assert(systim == 311U);

	check_point(27);
	simtim_advance(1U);

	check_assert(fch_hrt() == 321U);

	check_point(28);
	ercd = get_tim(&systim);
	check_ercd(ercd, E_OK);

	check_assert(systim == 311U);

	check_point(29);
	simtim_advance(1U);

	check_point(32);
	simtim_advance(38U);

	check_assert(fch_hrt() == 370U);

	check_point(33);
	ercd = set_dft(+10000);
	check_ercd(ercd, E_OK);

	check_point(35);
	ercd = get_tim(&systim);
	check_ercd(ercd, E_OK);

	check_assert(systim == 360U);

	check_point(36);
	simtim_advance(49U);

	check_assert(fch_hrt() == 419U);

	check_point(37);
	ercd = get_tim(&systim);
	check_ercd(ercd, E_OK);

	check_assert(systim == 409U);

	check_point(38);
	simtim_advance(1U);

	check_assert(fch_hrt() == 420U);

	check_point(39);
	ercd = get_tim(&systim);
	check_ercd(ercd, E_OK);

	check_assert(systim == 411U);

	check_finish(40);
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
		check_assert(hrtcnt == HRTCNT_EMPTY_P1P);

		return;

		check_point(0);

	case 3:
		check_point(7);
		check_assert(hrtcnt == 101U);

		return;

		check_point(0);

	case 4:
		check_point(14);
		check_assert(hrtcnt == HRTCNT_EMPTY_P1P);

		return;

		check_point(0);

	case 5:
		check_point(18);
		check_assert(hrtcnt == 101U);

		return;

		check_point(0);

	case 6:
		check_point(22);
		check_assert(hrtcnt == 102U);

		return;

		check_point(0);

	case 7:
		check_point(31);
		check_assert(hrtcnt == HRTCNT_EMPTY_M1P);

		return;

		check_point(0);

	case 8:
		check_point(34);
		check_assert(hrtcnt == HRTCNT_EMPTY_P1P);

		return;

		check_point(0);

	default:
		check_point(0);
	}
	check_point(0);
}
