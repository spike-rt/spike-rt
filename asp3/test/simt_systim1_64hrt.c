/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2014-2022 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: simt_systim1_64hrt.c 1570 2022-06-16 03:49:06Z ertl-hiro $
 */

/* 
 *		システム時刻管理機能のテスト(1)（USE_64BIT_HRTCNT版）
 *
 * 【テストの目的】
 *
 *  設計書の「高分解能タイマ割込みの発生タイミングの設定」「高分解能タ
 *  イマ割込みの処理」「相対時間指定によるタイムイベントの登録」の節に
 *  記述内容をテストする．
 *
 *  以下の関数のC1カバレッジを達成する．
 *		set_hrt_event（高分解能タイマ割込みの発生タイミングの設定）
 *		tmevtb_enqueue_reltim（相対時間指定によるタイムイベントの登録）
 *		signal_time（高分解能タイマ割込みの処理）
 *
 * 【テスト項目】
 *
 *	(A) 「高分解能タイマ割込みの発生タイミングの設定」の節の記述内容の
 *		テストとset_hrt_eventの実行/分岐パスの網羅
 *	  (A-1) タイムイベントが登録されていない時［ASPD1007］
 *	  (A-2) タイムイベントが登録されており，発生時刻を過ぎている時［ASPD1017］
 *	  (A-3) タイムイベントが登録されており，発生時刻を過ぎていない時
 *  (B) 「相対時間指定によるタイムイベントの登録」の節の記述内容のテス
 *		トとtmevtb_enqueue_reltimの実行/分岐パスの網羅
 *	  (B-1) 発生時刻が正しく設定されること［ASPD1026］［ASPD1027］
 *	  (B-2) タイムイベントヒープに挿入されること［ASPD1030］
 *	  (B-3) signal_timeの中で呼ばれた時［ASPD1034］
 *	  (B-4) 登録したタイムイベントが先頭でない時［ASPD1031］
 *	  (B-5) 登録したタイムイベントが先頭になった時（高分解能タイマを設
 *			定する）［ASPD1031］
 *  (C) signal_timeの実行/分岐パスの網羅
 *	  (C-1) タイムイベントヒープが空の状態で，signal_timeが呼び出された
 *			場合
 *	  (C-2) タイムイベントヒープの先頭のイベントの発生時刻前に，
 *			signal_timeが呼び出された場合
 *	  (C-3) signal_timeで，タイムイベントヒープの先頭のイベントのみを処
 *			理する場合
 *	  (C-4) signal_timeで，タイムイベントヒープの先頭から2つのイベント
 *			を，内側のループで処理する場合
 *	  (C-5) signal_timeで，タイムイベントヒープの先頭から2つのイベント
 *			を，外側のループで処理する場合
 *  (D) signal_timeから呼んだ処理で現在時刻が更新された場合
 *	  (D-1) 更新された現在時刻が，次のタイムイベントを過ぎており，次の
 *			タイムイベントを内側のループで処理する場合
 *
 * 【使用リソース】
 *
 *	高分解能タイマモジュールの性質：HRT_CONFIG3
 *		USE_64BIT_HRTCNT	HRTCNT型が64ビット
 *		TCYC_HRTCNT			未定義
 *		TSTEP_HRTCNT		1U
 *		HRTCNT_BOUND		未定義
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
 *	1:		[hook_hrt_clear_event]
 *	== TASK1（優先度：中）==
 *	2:	assert(fch_hrt() == 10U)							// 時刻：10
 *		DO(simtim_advance(90U))
 *		assert(fch_hrt() == 100U)							// 時刻：100
 *	// タイムイベントを1つだけ登録
 *	3:	sta_alm(ALM1, 100U)				... (A-3)(B-1)(B-2)	// 発生：201
 *	4:		[hook_hrt_set_event <- 101U]
 *	5:	slp_tsk()
 *	== HRT_HANDLER ==					... (C-3)			// 時刻：201
 *	== ALM1-1（1回目）==
 *	6:	assert(fch_hrt() == 211U)							// 時刻：211
 *		DO(simtim_advance(10U))
 *		assert(fch_hrt() == 221U)							// 時刻：221
 *	7:	wup_tsk(TASK1)
 *		RETURN
 *	// タイムイベントがなくなる			... (A-1)
 *	8:		[hook_hrt_clear_event]
 *	== TASK1（続き）==
 *	9:	DO(simtim_advance(79U))
 *		assert(fch_hrt() == 300U)							// 時刻：300
 *	// 3つのタイムイベントを登録．その内の2つを内側のループで処理
 *	10:	sta_alm(ALM1, 100U)				... (B-5)			// 発生：401
 *	11:		[hook_hrt_set_event <- 101U]
 *	12:	DO(simtim_advance(10U))
 *		assert(fch_hrt() == 310U)							// 時刻：310
 *	13:	sta_alm(ALM2, 100U)				... (B-4)			// 発生：411
 *	14:	DO(simtim_advance(10U))
 *		assert(fch_hrt() == 320U)							// 時刻：320
 *	15:	sta_alm(ALM3, 110U)									// 発生：431
 *	16:	slp_tsk()
 *	== HRT_HANDLER ==					... (C-4)			// 時刻：401
 *	== ALM1-2（2回目）==
 *	17:	assert(fch_hrt() == 411U)							// 時刻：411
 *	18:	DO(simtim_advance(10U))
 *		assert(fch_hrt() == 421U)							// 時刻：421
 *		RETURN
 *	== ALM2-1（1回目）==
 *	19:	assert(fch_hrt() == 421U)							// 時刻：421
 *		assert(_kernel_current_hrtcnt == 411U)		// 内側のループを確認
 *		RETURN
 *	20:		[hook_hrt_set_event <- 10U]
 *	== HRT_HANDLER ==										// 時刻：431
 *	== ALM3-1（1回目）==
 *	21:	assert(fch_hrt() == 441U)							// 時刻：441
 *	22:	wup_tsk(TASK1)
 *		RETURN
 *	23:		[hook_hrt_clear_event]
 *	== TASK1（続き）==
 *	24:	DO(simtim_advance(59U))
 *		assert(fch_hrt() == 500U)							// 時刻：500
 *	// 2つのタイムイベントを登録．その2つを外側のループで処理
 *	25:	sta_alm(ALM1, 100U)									// 発生：601
 *	26:		[hook_hrt_set_event <- 101U]
 *	27:	DO(simtim_advance(10U))
 *		assert(fch_hrt() == 510U)							// 時刻：510
 *	28:	sta_alm(ALM2, 110U)									// 発生：621
 *	29:	slp_tsk()
 *	== HRT_HANDLER ==					... (C-5)			// 時刻：601
 *	== ALM1-3（3回目）==
 *	30:	assert(fch_hrt() == 611U)							// 時刻：611
 *		DO(simtim_advance(20U))
 *		RETURN
 *	== ALM2-2（2回目）==
 *	31:	assert(fch_hrt() == 631U)							// 時刻：631
 *		assert(_kernel_current_hrtcnt == 631U)		// 外側のループを確認
 *	32:	wup_tsk(TASK1)
 *		RETURN
 *	33:		[hook_hrt_clear_event]
 *	== TASK1（続き）==
 *	34:	DO(simtim_advance(69U))
 *		assert(fch_hrt() == 700U)							// 時刻：700
 *	// 2つのタイムイベントを登録
 *	35:	sta_alm(ALM1, 100U)									// 発生：801
 *	36:		[hook_hrt_set_event <- 101U]
 *	37:	DO(simtim_advance(10U))
 *		assert(fch_hrt() == 710U)							// 時刻：710
 *	38:	sta_alm(ALM2, 100U)									// 発生：811
 *	39:	slp_tsk()
 *	== HRT_HANDLER ==					... (D-1)			// 時刻：801
 *	== ALM1-4（4回目）==
 *	40:	assert(fch_hrt() == 811U)							// 時刻：811
 *	// アラームハンドラ中でさらに1つのタイムイベントを登録
 *	41:	DO(simtim_advance(9U))
 *		assert(fch_hrt() == 820U)							// 時刻：820
 *	42:	sta_alm(ALM3, 10U)				... (B-3)			// 発生：831
 *		RETURN
 *	== ALM2-3（3回目）==
 *	43:	assert(fch_hrt() == 820U)							// 時刻：820
 *		DO(simtim_advance(20U))
 *		assert(fch_hrt() == 840U)							// 時刻：840
 *		RETURN
 *	== ALM3-2（2回目）==
 *	44:	assert(fch_hrt() == 840U)							// 時刻：840
 *	45:	wup_tsk(TASK1)
 *		RETURN
 *	46:		[hook_hrt_clear_event]
 *	== TASK1（続き）==
 *	47:	DO(simtim_advance(60U))
 *		assert(fch_hrt() == 900U)							// 時刻：900
 *	// 2つのタイムイベントを登録．2つめの方が発生時刻が早い場合
 *	48:	sta_alm(ALM1, 100U)									// 発生：1001
 *	49:		[hook_hrt_set_event <- 101U]
 *	50:	DO(simtim_advance(10U))
 *		assert(fch_hrt() == 910U)							// 時刻：910
 *	51:	sta_alm(ALM2, 50U)									// 発生：961
 *	52:		[hook_hrt_set_event <- 51U]
 *	53:	slp_tsk()
 *	== HRT_HANDLER ==										// 時刻：961
 *	== ALM2-4（4回目）==
 *	54:	assert(fch_hrt() == 971U)							// 時刻：971
 *		RETURN
 *	55:		[hook_hrt_set_event <- 30U]
 *	== HRT_HANDLER ==										// 時刻：1001
 *	== ALM1-5（5回目）==
 *	56:	assert(fch_hrt() == 1011U)							// 時刻：1011
 *	57:	wup_tsk(TASK1)
 *		RETURN
 *	58:		[hook_hrt_clear_event]
 *	== TASK1（続き）==
 *	59:	DO(simtim_advance(89U))
 *		assert(fch_hrt() == 1100U)							// 時刻：1100
 *	// 2つのタイムイベントを登録．2つめのタイムイベントの登録時点で，
 *	// 1つめのタイムイベントの発生時刻を過ぎている場合
 *	60:	sta_alm(ALM1, 10U)									// 発生：1111
 *	61:		[hook_hrt_set_event <- 11U]
 *	62:	DO(simtim_add(20U))
 *		assert(fch_hrt() == 1120U)							// 時刻：1120
 *	63:	sta_alm(ALM2, 100U)									// 発生：1221
 *	64:	DO(simtim_advance(10U))					// ここでは時刻が進まない
 *	== HRT_HANDLER ==										// 時刻：1120
 *	== ALM1-6（6回目）==
 *	65:	assert(fch_hrt() == 1130U)							// 時刻：1130
 *	66:	DO(simtim_advance(10U))
 *		assert(fch_hrt() == 1140U)							// 時刻：1140
 *	// 高分解能タイマ割込みを強制的に入れる
 *	67:	DO(raise_hrt_int())
 *		RETURN
 *	68:		[hook_hrt_set_event <- 81U]
 *	== HRT_HANDLER ==					... (C-2)			// 時刻：1140
 *	// スプリアス割込み										// 時刻：1150
 *	69:		[hook_hrt_set_event <- 71U]
 *	== TASK1（続き）==
 *	// ここで時刻が進む
 *	70:	assert(fch_hrt() == 1160U)							// 時刻：1160
 *	71:	slp_tsk()
 *	== HRT_HANDLER ==										// 時刻：1221
 *	== ALM2-5（5回目）==
 *	72:	assert(fch_hrt() == 1231U)							// 時刻：1231
 *	73:	wup_tsk(TASK1)
 *		RETURN
 *	74:		[hook_hrt_clear_event]
 *	== TASK1（続き）==
 *	// 高分解能タイマ割込みを強制的に入れる
 *	75:	DO(raise_hrt_int())
 *	== HRT_HANDLER ==					... (C-1)			// 時刻：1231
 *	// スプリアス割込み										// 時刻：1241
 *	76:		[hook_hrt_clear_event]
 *	== TASK1（続き）==
 *	77:	DO(simtim_advance(59U))
 *		assert(fch_hrt() == 1300U)							// 時刻：1300
 *	// タイムイベントを登録後に時間を進め，先頭のタイムイベントの発生時
 *	// 刻が過ぎた状況を作る
 *	78:	sta_alm(ALM1, 100U)									// 発生：1401
 *	79:		[hook_hrt_set_event <- 101U]
 *	80:	adj_tim(200)					... (A-2)			// ALM1の発生：1201
 *	81:		[hook_hrt_raise_event]
 *	== HRT_HANDLER ==										// 時刻：1300
 *	== ALM1-7（7回目）==
 *	82:	assert(fch_hrt() == 1310U)							// 時刻：1310
 *		RETURN
 *	83:		[hook_hrt_clear_event]
 *	== TASK1（続き）==
 *	// タイムイベントを登録後に時間を戻し，先頭のタイムイベントの発生時
 *	// 刻までの高分解タイマのカウントアップ値が4000000002（HRT_CONFIG1
 *	// の場合のHRTCNT_BOUND）を超える状況を作る
 *	84:	sta_alm(ALM1, TMAX_RELTIM)					// 発生：4,000,001,311
 *	85:		[hook_hrt_set_event <- 4000000001U]
 *	86:	adj_tim(-200)					... (A-3)	// ALM1の発生：4,000,001,511
 *	87:		[hook_hrt_set_event <- 4000000201U]
 *	88:	slp_tsk()
 *	== HRT_HANDLER ==								// 時刻：4,000,001,511
 *	== ALM1-8（8回目）==
 *	89:	assert(fch_hrt() == 4000001521U)			// 時刻：4,000,001,521
 *	90:	wup_tsk(TASK1)
 *		RETURN
 *	91:		[hook_hrt_clear_event]
 *	== TASK1（続き）==
 *	92:	END
 */

#include <kernel.h>
#include <t_syslog.h>
#include "syssvc/test_svc.h"
#include "arch/simtimer/sim_timer_cntl.h"
#include "kernel_cfg.h"
#include "test_common.h"

#ifndef HRT_CONFIG3
#error Compiler option "-DHRT_CONFIG3" is missing.
#endif /* HRT_CONFIG3 */

#ifndef HOOK_HRT_EVENT
#error Compiler option "-DHOOK_HRT_EVENT" is missing.
#endif /* HOOK_HRT_EVENT */

/*
 *  カーネル内の直接操作
 *
 *  特殊なテスト（カーネル内の実行パスの確認，スプリアス割込みのテスト）
 *  を実施するために，カーネル内を直接操作する．
 */
#include "target_timer.h"

extern HRTCNT _kernel_current_hrtcnt;

void
raise_hrt_int(void)
{
	if (sns_loc()) {
		target_raise_hrt_int();
	}
	else {
		loc_cpu();
		target_raise_hrt_int();
		unl_cpu();
	}
}

/* DO NOT DELETE THIS LINE -- gentest depends on it. */

static uint_t	alarm1_count = 0;

void
alarm1_handler(EXINF exinf)
{
	ER_UINT	ercd;

	switch (++alarm1_count) {
	case 1:
		check_point(6);
		check_assert(fch_hrt() == 211U);

		simtim_advance(10U);

		check_assert(fch_hrt() == 221U);

		check_point(7);
		ercd = wup_tsk(TASK1);
		check_ercd(ercd, E_OK);

		return;

		check_assert(false);

	case 2:
		check_point(17);
		check_assert(fch_hrt() == 411U);

		check_point(18);
		simtim_advance(10U);

		check_assert(fch_hrt() == 421U);

		return;

		check_assert(false);

	case 3:
		check_point(30);
		check_assert(fch_hrt() == 611U);

		simtim_advance(20U);

		return;

		check_assert(false);

	case 4:
		check_point(40);
		check_assert(fch_hrt() == 811U);

		check_point(41);
		simtim_advance(9U);

		check_assert(fch_hrt() == 820U);

		check_point(42);
		ercd = sta_alm(ALM3, 10U);
		check_ercd(ercd, E_OK);

		return;

		check_assert(false);

	case 5:
		check_point(56);
		check_assert(fch_hrt() == 1011U);

		check_point(57);
		ercd = wup_tsk(TASK1);
		check_ercd(ercd, E_OK);

		return;

		check_assert(false);

	case 6:
		check_point(65);
		check_assert(fch_hrt() == 1130U);

		check_point(66);
		simtim_advance(10U);

		check_assert(fch_hrt() == 1140U);

		check_point(67);
		raise_hrt_int();

		return;

		check_assert(false);

	case 7:
		check_point(82);
		check_assert(fch_hrt() == 1310U);

		return;

		check_assert(false);

	case 8:
		check_point(89);
		check_assert(fch_hrt() == 4000001521U);

		check_point(90);
		ercd = wup_tsk(TASK1);
		check_ercd(ercd, E_OK);

		return;

		check_assert(false);

	default:
		check_assert(false);
	}
	check_assert(false);
}

static uint_t	alarm2_count = 0;

void
alarm2_handler(EXINF exinf)
{
	ER_UINT	ercd;

	switch (++alarm2_count) {
	case 1:
		check_point(19);
		check_assert(fch_hrt() == 421U);

		check_assert(_kernel_current_hrtcnt == 411U);

		return;

		check_assert(false);

	case 2:
		check_point(31);
		check_assert(fch_hrt() == 631U);

		check_assert(_kernel_current_hrtcnt == 631U);

		check_point(32);
		ercd = wup_tsk(TASK1);
		check_ercd(ercd, E_OK);

		return;

		check_assert(false);

	case 3:
		check_point(43);
		check_assert(fch_hrt() == 820U);

		simtim_advance(20U);

		check_assert(fch_hrt() == 840U);

		return;

		check_assert(false);

	case 4:
		check_point(54);
		check_assert(fch_hrt() == 971U);

		return;

		check_assert(false);

	case 5:
		check_point(72);
		check_assert(fch_hrt() == 1231U);

		check_point(73);
		ercd = wup_tsk(TASK1);
		check_ercd(ercd, E_OK);

		return;

		check_assert(false);

	default:
		check_assert(false);
	}
	check_assert(false);
}

static uint_t	alarm3_count = 0;

void
alarm3_handler(EXINF exinf)
{
	ER_UINT	ercd;

	switch (++alarm3_count) {
	case 1:
		check_point(21);
		check_assert(fch_hrt() == 441U);

		check_point(22);
		ercd = wup_tsk(TASK1);
		check_ercd(ercd, E_OK);

		return;

		check_assert(false);

	case 2:
		check_point(44);
		check_assert(fch_hrt() == 840U);

		check_point(45);
		ercd = wup_tsk(TASK1);
		check_ercd(ercd, E_OK);

		return;

		check_assert(false);

	default:
		check_assert(false);
	}
	check_assert(false);
}

void
task1(EXINF exinf)
{
	ER_UINT	ercd;

	check_point(2);
	check_assert(fch_hrt() == 10U);

	simtim_advance(90U);

	check_assert(fch_hrt() == 100U);

	check_point(3);
	ercd = sta_alm(ALM1, 100U);
	check_ercd(ercd, E_OK);

	check_point(5);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(9);
	simtim_advance(79U);

	check_assert(fch_hrt() == 300U);

	check_point(10);
	ercd = sta_alm(ALM1, 100U);
	check_ercd(ercd, E_OK);

	check_point(12);
	simtim_advance(10U);

	check_assert(fch_hrt() == 310U);

	check_point(13);
	ercd = sta_alm(ALM2, 100U);
	check_ercd(ercd, E_OK);

	check_point(14);
	simtim_advance(10U);

	check_assert(fch_hrt() == 320U);

	check_point(15);
	ercd = sta_alm(ALM3, 110U);
	check_ercd(ercd, E_OK);

	check_point(16);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(24);
	simtim_advance(59U);

	check_assert(fch_hrt() == 500U);

	check_point(25);
	ercd = sta_alm(ALM1, 100U);
	check_ercd(ercd, E_OK);

	check_point(27);
	simtim_advance(10U);

	check_assert(fch_hrt() == 510U);

	check_point(28);
	ercd = sta_alm(ALM2, 110U);
	check_ercd(ercd, E_OK);

	check_point(29);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(34);
	simtim_advance(69U);

	check_assert(fch_hrt() == 700U);

	check_point(35);
	ercd = sta_alm(ALM1, 100U);
	check_ercd(ercd, E_OK);

	check_point(37);
	simtim_advance(10U);

	check_assert(fch_hrt() == 710U);

	check_point(38);
	ercd = sta_alm(ALM2, 100U);
	check_ercd(ercd, E_OK);

	check_point(39);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(47);
	simtim_advance(60U);

	check_assert(fch_hrt() == 900U);

	check_point(48);
	ercd = sta_alm(ALM1, 100U);
	check_ercd(ercd, E_OK);

	check_point(50);
	simtim_advance(10U);

	check_assert(fch_hrt() == 910U);

	check_point(51);
	ercd = sta_alm(ALM2, 50U);
	check_ercd(ercd, E_OK);

	check_point(53);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(59);
	simtim_advance(89U);

	check_assert(fch_hrt() == 1100U);

	check_point(60);
	ercd = sta_alm(ALM1, 10U);
	check_ercd(ercd, E_OK);

	check_point(62);
	simtim_add(20U);

	check_assert(fch_hrt() == 1120U);

	check_point(63);
	ercd = sta_alm(ALM2, 100U);
	check_ercd(ercd, E_OK);

	check_point(64);
	simtim_advance(10U);

	check_point(70);
	check_assert(fch_hrt() == 1160U);

	check_point(71);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(75);
	raise_hrt_int();

	check_point(77);
	simtim_advance(59U);

	check_assert(fch_hrt() == 1300U);

	check_point(78);
	ercd = sta_alm(ALM1, 100U);
	check_ercd(ercd, E_OK);

	check_point(80);
	ercd = adj_tim(200);
	check_ercd(ercd, E_OK);

	check_point(84);
	ercd = sta_alm(ALM1, TMAX_RELTIM);
	check_ercd(ercd, E_OK);

	check_point(86);
	ercd = adj_tim(-200);
	check_ercd(ercd, E_OK);

	check_point(88);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_finish(92);
	check_assert(false);
}

static uint_t	hook_hrt_clear_event_count = 0;

void
hook_hrt_clear_event(void)
{

	switch (++hook_hrt_clear_event_count) {
	case 1:
		test_start(__FILE__);

		check_point(1);
		return;

		check_assert(false);

	case 2:
		check_point(8);
		return;

		check_assert(false);

	case 3:
		check_point(23);
		return;

		check_assert(false);

	case 4:
		check_point(33);
		return;

		check_assert(false);

	case 5:
		check_point(46);
		return;

		check_assert(false);

	case 6:
		check_point(58);
		return;

		check_assert(false);

	case 7:
		check_point(74);
		return;

		check_assert(false);

	case 8:
		check_point(76);
		return;

		check_assert(false);

	case 9:
		check_point(83);
		return;

		check_assert(false);

	case 10:
		check_point(91);
		return;

		check_assert(false);

	default:
		check_assert(false);
	}
	check_assert(false);
}

static uint_t	hook_hrt_raise_event_count = 0;

void
hook_hrt_raise_event(void)
{

	switch (++hook_hrt_raise_event_count) {
	case 1:
		check_point(81);
		return;

		check_assert(false);

	default:
		check_assert(false);
	}
	check_assert(false);
}

static uint_t	hook_hrt_set_event_count = 0;

void
hook_hrt_set_event(HRTCNT hrtcnt)
{

	switch (++hook_hrt_set_event_count) {
	case 1:
		check_point(4);
		check_assert(hrtcnt == 101U);

		return;

		check_assert(false);

	case 2:
		check_point(11);
		check_assert(hrtcnt == 101U);

		return;

		check_assert(false);

	case 3:
		check_point(20);
		check_assert(hrtcnt == 10U);

		return;

		check_assert(false);

	case 4:
		check_point(26);
		check_assert(hrtcnt == 101U);

		return;

		check_assert(false);

	case 5:
		check_point(36);
		check_assert(hrtcnt == 101U);

		return;

		check_assert(false);

	case 6:
		check_point(49);
		check_assert(hrtcnt == 101U);

		return;

		check_assert(false);

	case 7:
		check_point(52);
		check_assert(hrtcnt == 51U);

		return;

		check_assert(false);

	case 8:
		check_point(55);
		check_assert(hrtcnt == 30U);

		return;

		check_assert(false);

	case 9:
		check_point(61);
		check_assert(hrtcnt == 11U);

		return;

		check_assert(false);

	case 10:
		check_point(68);
		check_assert(hrtcnt == 81U);

		return;

		check_assert(false);

	case 11:
		check_point(69);
		check_assert(hrtcnt == 71U);

		return;

		check_assert(false);

	case 12:
		check_point(79);
		check_assert(hrtcnt == 101U);

		return;

		check_assert(false);

	case 13:
		check_point(85);
		check_assert(hrtcnt == 4000000001U);

		return;

		check_assert(false);

	case 14:
		check_point(87);
		check_assert(hrtcnt == 4000000201U);

		return;

		check_assert(false);

	default:
		check_assert(false);
	}
	check_assert(false);
}
