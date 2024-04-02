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
 *  $Id: simt_systim3.c 1439 2020-05-22 20:02:23Z ertl-hiro $
 */

/* 
 *		システム時刻管理機能のテスト(3)
 *
 * 【テストの目的】
 *
 *	周期ハンドラの起動タイミングをテストする．
 *
 *	また，以下の関数のC1カバレッジを達成する．
 *		tmevtb_dequeue
 *		tmevtb_lefttim
 *
 * 【テスト項目】
 *
 *  (A) 周期ハンドラの起動タイミングの確認
 *	  (A-1) TA_STA属性で，起動位相（初回の起動時刻）が0の場合
 *	  (A-2) TA_STA属性で，起動位相（初回の起動時刻）が0でない場合
 *	  (A-3) sta_cycで，起動位相（初回の起動時刻）が0の場合
 *	  (A-4) sta_cycで，起動位相（初回の起動時刻）が0でない場合
 *	  (A-5) 決められた周期で再起動されること
 *  (B) tmevtb_dequeueの実行/分岐パスの網羅
 *	  (B-1) signal_time中からの呼び出し
 *	  (B-2) 先頭以外のタイムイベントを削除した場合
 *	  (B-3) 先頭のタイムイベントを削除した場合
 *  (C) tmevtb_lefttimの実行/分岐パスの網羅
 *	  (C-1) イベントの発生時刻を過ぎている場合
 *	  (C-2) イベントの発生時刻を過ぎていない場合
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
 *	CYC1:  周期ハンドラ（周期：1000，初期位相：0）
 *	CYC2:  周期ハンドラ（周期：500，初期位相：499）
 *
 * 【補足説明】
 *
 *	タイムイベントが登録されていない時に高分解能タイマに設定する相対は，
 *	ドリフト調整機能を持たない場合はHRTCNT_BOUNDであるのに対して，ドリ
 *	フト調整機能を持つ場合はTMAX_RELTIMをイベント時刻の伸縮率で割った値
 *	とHRTCNT_BOUNDの小さい方の値（このテストでは，ドリフト量を設定せず，
 *	HRTCNT_BOUND＞TMAX_RELTIMであるため，TMAX_RELTIMに一致）となる．そ
 *	こで，HRTCNT_EMPTYをこの値に定義し，ドリフト調整機能の有無によらず
 *	同じテストシーケンスが使えるようにする．
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
 *	1:		[hook_hrt_raise_event]					// CYC1の初回発生：10
 *													// CYC2の初回発生：509
 *	== HRT_HANDLER ==					... (A-1)			// 時刻：10   
 *	== CYC1-1（1回目）==							// CYC1の次回発生：1010
 *	2:	assert(fch_hrt() == 20U)							// 時刻：20
 *		RETURN
 *	3:		[hook_hrt_set_event <- 489U]
 *	== TASK1（優先度：中）==
 *	// ここで時間が経過したことを想定
 *	4:	DO(simtim_advance(489U))
 *	== HRT_HANDLER ==					... (A-2)			// 時刻：509
 *	== CYC2-1（1回目）==							// CYC2の次回発生：1009
 *	5:	assert(fch_hrt() == 519U)							// 時刻：519
 *		RETURN
 *	6:		[hook_hrt_set_event <- 490U]
 *	== TASK1（続き）==
 *	// ここで時間が経過したことを想定
 *	7:	DO(simtim_advance(490U))
 *	== HRT_HANDLER ==					... (A-5)			// 時刻：1009
 *	== CYC2-2（2回目）==							// CYC2の次回発生：1509
 *	8:	assert(fch_hrt() == 1019U)							// 時刻：1019
 *		RETURN
 *	== CYC1-2（2回目）==							// CYC1の次回発生：2010
 *	9:	assert(fch_hrt() == 1019U)							// 時刻：1019
 *		RETURN
 *	10:		[hook_hrt_set_event <- 490U]
 *	== TASK1（続き）==
 *	11:	ref_cyc(CYC1, &rcyc)			... (C-2)
 *		assert(rcyc.lefttim == 990U)
 *	12:	stp_cyc(CYC1)					... (B-2)
 *	13:	stp_cyc(CYC2)					... (B-3)
 *	14:		[hook_hrt_set_event <- HRTCNT_EMPTY]
 *	15:	sta_cyc(CYC1)					... (A-3)	// CYC1の初回発生：1020
 *	16:		[hook_hrt_set_event <- 1U]
 *	17:	DO(simtim_advance(1U))
 *	== HRT_HANDLER ==										// 時刻：1020
 *	== CYC1-3（3回目）==							// CYC1の次回発生：2020
 *	18:	assert(fch_hrt() == 1030U)							// 時刻：1030
 *		RETURN
 *	19:		[hook_hrt_set_event <- 990U]
 *	== TASK1（続き）==
 *	20:	sta_cyc(CYC2)					... (A-4)	// CYC2の初回発生：1530
 *	21:		[hook_hrt_set_event <- 500U]
 *	// ここで時間が経過したことを想定
 *	22:	DO(simtim_advance(490U))
 *		assert(fch_hrt() == 1520U)							// 時刻：1520
 *	23:	ref_cyc(CYC2, &rcyc)			... (C-2)
 *		assert(rcyc.lefttim == 9U)
 *	// 以下のシーケンスは，タイマ割込みの受付が遅れた場合にしか発生しない．
 *	24:	DO(simtim_add(15U))
 *		assert(fch_hrt() == 1535U)							// 時刻：1535
 *	25:	ref_cyc(CYC2, &rcyc)			... (C-1)	// CYC2の発生時刻を
 *		assert(rcyc.lefttim == 0U)					//			過ぎている
 *	26:	DO(simtim_advance(15U))						// ここでは時刻が進まない
 *	== HRT_HANDLER ==										// 時刻：1535
 *	== CYC2-3（3回目）==							// CYC2の次回発生：2030
 *	27:	assert(fch_hrt() == 1545U)							// 時刻：1545
 *		RETURN
 *	28:		[hook_hrt_set_event <- 475U]
 *	== TASK1（続き）==
 *	// ここで時刻が進む
 *	29:	assert(fch_hrt() == 1560U)							// 時刻：1560
 *	// ここで時間が経過したことを想定
 *	30:	DO(simtim_advance(40U))
 *		assert(fch_hrt() == 1600U)							// 時刻：1600
 *	31:	sta_alm(ALM1, 100U)								// ALM1の発生：1701
 *	32:		[hook_hrt_set_event <- 101U]
 *	33:	sta_alm(ALM2, 200U)								// ALM2の発生：1801
 *	// ここで時間が経過したことを想定
 *	34:	DO(simtim_advance(101U))
 *	== HRT_HANDLER ==										// 時刻：1701
 *	== ALM1-1（1回目）==
 *	35:	assert(fch_hrt() == 1711U)							// 時刻：1711
 *	36:	stp_alm(ALM2)					... (B-1)
 *		RETURN
 *	37:		[hook_hrt_set_event <- 309U]
 *	== TASK1（続き）==
 *	38:	END
 */

#include <kernel.h>
#include <t_syslog.h>
#include "syssvc/test_svc.h"
#include "arch/simtimer/sim_timer_cntl.h"
#include "kernel_cfg.h"
#include "simt_systim3.h"

#ifndef HRT_CONFIG1
#error Compiler option "-DHRT_CONFIG1" is missing.
#endif /* HRT_CONFIG1 */

#ifndef HOOK_HRT_EVENT
#error Compiler option "-DHOOK_HRT_EVENT" is missing.
#endif /* HOOK_HRT_EVENT */

/*
 *  HRTCNT_EMPTYの定義
 */
#ifdef TOPPERS_SUPPORT_DRIFT
#define HRTCNT_EMPTY	TMAX_RELTIM
#else /* TOPPERS_SUPPORT_DRIFT */
#define HRTCNT_EMPTY	HRTCNT_BOUND
#endif /* TOPPERS_SUPPORT_DRIFT */

void
alarm2_handler(EXINF exinf)
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
		check_point(35);
		check_assert(fch_hrt() == 1711U);

		check_point(36);
		ercd = stp_alm(ALM2);
		check_ercd(ercd, E_OK);

		return;

		check_point(0);

	default:
		check_point(0);
	}
	check_point(0);
}

static uint_t	cyclic1_count = 0;

void
cyclic1_handler(EXINF exinf)
{

	switch (++cyclic1_count) {
	case 1:
		check_point(2);
		check_assert(fch_hrt() == 20U);

		return;

		check_point(0);

	case 2:
		check_point(9);
		check_assert(fch_hrt() == 1019U);

		return;

		check_point(0);

	case 3:
		check_point(18);
		check_assert(fch_hrt() == 1030U);

		return;

		check_point(0);

	default:
		check_point(0);
	}
	check_point(0);
}

static uint_t	cyclic2_count = 0;

void
cyclic2_handler(EXINF exinf)
{

	switch (++cyclic2_count) {
	case 1:
		check_point(5);
		check_assert(fch_hrt() == 519U);

		return;

		check_point(0);

	case 2:
		check_point(8);
		check_assert(fch_hrt() == 1019U);

		return;

		check_point(0);

	case 3:
		check_point(27);
		check_assert(fch_hrt() == 1545U);

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
	T_RCYC	rcyc;

	check_point(4);
	simtim_advance(489U);

	check_point(7);
	simtim_advance(490U);

	check_point(11);
	ercd = ref_cyc(CYC1, &rcyc);
	check_ercd(ercd, E_OK);

	check_assert(rcyc.lefttim == 990U);

	check_point(12);
	ercd = stp_cyc(CYC1);
	check_ercd(ercd, E_OK);

	check_point(13);
	ercd = stp_cyc(CYC2);
	check_ercd(ercd, E_OK);

	check_point(15);
	ercd = sta_cyc(CYC1);
	check_ercd(ercd, E_OK);

	check_point(17);
	simtim_advance(1U);

	check_point(20);
	ercd = sta_cyc(CYC2);
	check_ercd(ercd, E_OK);

	check_point(22);
	simtim_advance(490U);

	check_assert(fch_hrt() == 1520U);

	check_point(23);
	ercd = ref_cyc(CYC2, &rcyc);
	check_ercd(ercd, E_OK);

	check_assert(rcyc.lefttim == 9U);

	check_point(24);
	simtim_add(15U);

	check_assert(fch_hrt() == 1535U);

	check_point(25);
	ercd = ref_cyc(CYC2, &rcyc);
	check_ercd(ercd, E_OK);

	check_assert(rcyc.lefttim == 0U);

	check_point(26);
	simtim_advance(15U);

	check_point(29);
	check_assert(fch_hrt() == 1560U);

	check_point(30);
	simtim_advance(40U);

	check_assert(fch_hrt() == 1600U);

	check_point(31);
	ercd = sta_alm(ALM1, 100U);
	check_ercd(ercd, E_OK);

	check_point(33);
	ercd = sta_alm(ALM2, 200U);
	check_ercd(ercd, E_OK);

	check_point(34);
	simtim_advance(101U);

	check_finish(38);
	check_point(0);
}

static uint_t	hook_hrt_raise_event_count = 0;

void
hook_hrt_raise_event(void)
{

	switch (++hook_hrt_raise_event_count) {
	case 1:
		test_start(__FILE__);

		check_point(1);
		return;

		check_point(0);

	default:
		check_point(0);
	}
	check_point(0);
}

static uint_t	hook_hrt_set_event_count = 0;

void
hook_hrt_set_event(HRTCNT hrtcnt)
{

	switch (++hook_hrt_set_event_count) {
	case 1:
		check_point(3);
		check_assert(hrtcnt == 489U);

		return;

		check_point(0);

	case 2:
		check_point(6);
		check_assert(hrtcnt == 490U);

		return;

		check_point(0);

	case 3:
		check_point(10);
		check_assert(hrtcnt == 490U);

		return;

		check_point(0);

	case 4:
		check_point(14);
		check_assert(hrtcnt == HRTCNT_EMPTY);

		return;

		check_point(0);

	case 5:
		check_point(16);
		check_assert(hrtcnt == 1U);

		return;

		check_point(0);

	case 6:
		check_point(19);
		check_assert(hrtcnt == 990U);

		return;

		check_point(0);

	case 7:
		check_point(21);
		check_assert(hrtcnt == 500U);

		return;

		check_point(0);

	case 8:
		check_point(28);
		check_assert(hrtcnt == 475U);

		return;

		check_point(0);

	case 9:
		check_point(32);
		check_assert(hrtcnt == 101U);

		return;

		check_point(0);

	case 10:
		check_point(37);
		check_assert(hrtcnt == 309U);

		return;

		check_point(0);

	default:
		check_point(0);
	}
	check_point(0);
}
