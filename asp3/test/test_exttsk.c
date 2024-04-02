/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2017-2018 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: test_exttsk.c 1439 2020-05-22 20:02:23Z ertl-hiro $
 */

/* 
 *		自タスクの終了に関するテスト
 *
 * 【テストの目的】
 *
 *	ext_tskの振舞いのテスト［NGKI1162］
 *	  ・ext_tskに関するすべての要求（NGKI1163を除く）をテストする．
 *	  ・ext_tsk関数のC1カバレッジを達成する．
 *	タスク終了時に行うべき処理に関するテスト
 *	  ・タスク終了時に行うべき処理に関するすべての要求をテストする．
 *	  ・task_terminate関数のC1カバレッジを達成する（ext_tskから呼び出し
 *		た場合に実行されることがないパスを除く）．
 *
 * 【テスト項目】
 *
 *	(A) ext_tskのエラー検出
 *		(A-1) 非タスクコンテキストからの呼出し［NGKI1164］
 *	(B) ext_tskによるタスク終了［NGKI3449］［NGKI1169］
 *		(B-1) 対象タスクが実行状態から休止状態に［NGKI1178］
 *		(B-2) ロックしているミューテックスのロック解除［NGKI2019］
 *		(B-3) (B-2)の処理により待ち解除されたタスクに切換え
 *	(C) ext_tskによるタスク終了後の再起動［NGKI1179］［NGKI1169］
 *		(C-1) 対象タスクが実行状態に
 *		(C-2) (C-1)の時に起動要求キューイング数が1減少［NGKI1180］
 *		(C-3) 対象タスクが実行可能状態に
 *		(C-4) (C-3)の時に起動要求キューイング数が1減少［NGKI1180］
 *	(D) ext_tskによるシステム状態の変更［NGKI1168］［NGKI1169］
 *		(D-1) CPUロック解除状態に遷移する
 *		(D-2) 割込み優先度マスク全解除状態に遷移する
 *		(D-3) (D-2)の時に実行すべきタスクが更新される
 *		(D-4) ディスパッチ許可状態に遷移する
 *		(D-5) (D-4)の時に実行すべきタスクが更新される
 *
 * 【使用リソース】
 *
 *	TASK1: 低優先度タスク，TA_ACT属性
 *	TASK2: 中優先度タスク
 *	TASK3: 高優先度タスク
 *	TASK4: 中優先度タスク
 *	ALM1: アラームハンドラ
 *	MTX1: ミューテックス
 *
 * 【テストシーケンス】
 *
 *	== TASK1 ==
 *	1:	sta_alm(ALM1, TEST_TIME_CP) ... ALM1が実行開始するまで
 *		slp_tsk()
 *	== ALM1 ==
 *	2:	ext_tsk() -> E_CTX							... (A-1)
 *		wup_tsk(TASK1)
 *		RETURN
 *	== TASK1（続き）==
 *	3:	act_tsk(TASK2)
 *	== TASK2-1（1回目）==
 *	4:	loc_mtx(MTX1)
 *		ext_tsk()
 *	== TASK1（続き）==
 *	5:	ref_tsk(TASK2, &rtsk)
 *		assert(rtsk.tskstat == TTS_DMT)				... (B-1)
 *		ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TSK_NONE)				... (B-2)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		act_tsk(TASK2)
 *	== TASK2-2（2回目）==
 *	6:	loc_mtx(MTX1)
 *		act_tsk(TASK3)
 *	== TASK3-1（1回目）==
 *	7:	loc_mtx(MTX1)
 *	== TASK2-2（続き）==
 *	8:	ext_tsk()
 *	== TASK3-1（続き）==
 *	9:	unl_mtx(MTX1)								... (B-3)
 *		act_tsk(TASK2)
 *		act_tsk(TASK2)
 *	10:	ext_tsk()
 *	== TASK2-3（3回目）==
 *	11:	ext_tsk()
 *	== TASK2-4（4回目）==
 *	12:	ref_tsk(TASK2, &rtsk)
 *		assert(rtsk.tskstat == TTS_RUN)				... (C-1)
 *		assert(rtsk.actcnt == 0U)					... (C-2)
 *		act_tsk(TASK2)
 *		chg_pri(TASK2, TASK3_PRIORITY)
 *		act_tsk(TASK3)
 *	13:	ext_tsk()
 *	== TASK3-2（2回目）==
 *	14:	ref_tsk(TASK2, &rtsk)
 *		assert(rtsk.tskstat == TTS_RDY)				... (C-3)
 *		assert(rtsk.actcnt == 0U)					... (C-4)
 *		loc_cpu()
 *		ext_tsk()
 *	== TASK2-5（5回目）==
 *	15:	assert(sns_loc() == false)					... (D-1)
 *		chg_ipm(TMAX_INTPRI)
 *		ext_tsk()
 *	== TASK1（続き）==
 *	16:	get_ipm(&intpri)
 *		assert(intpri == TIPM_ENAALL)				... (D-2)
 *		act_tsk(TASK2)
 *	== TASK2-6（6回目）==
 *	17:	chg_ipm(TMAX_INTPRI)
 *		act_tsk(TASK3)
 *		act_tsk(TASK4)		// TASK4を起動しておかないと，TASK3-3のext_tsk
 *							// から呼ばれるmake_non_runnableで実行すべきタ
 *							// スクが更新され，ext_tskで実行すべきタスクが
 *							// 更新されることをテストできない．
 *	18:	ext_tsk()
 *	== TASK3-3（3回目）==
 *	19:	get_ipm(&intpri)							... (D-3)
 *		assert(intpri == TIPM_ENAALL)
 *		act_tsk(TASK2)
 *	20:	ext_tsk()
 *	== TASK4-1（1回目） ==
 *	21:	ext_tsk()
 *	== TASK2-7（7回目）==
 *	22:	dis_dsp()
 *		ext_tsk()
 *	== TASK1（続き）==
 *	23:	assert(sns_dsp() == false)					... (D-4)
 *		act_tsk(TASK2)
 *	== TASK2-8（8回目）==
 *	24:	dis_dsp()
 *		act_tsk(TASK3)
 *		act_tsk(TASK4)		// 前述のコメントと同様．
 *	25:	ext_tsk()
 *	== TASK3-4（4回目）==
 *	26:	assert(sns_dsp() == false)					... (D-5)
 *		ext_tsk()
 *	== TASK4-2（2回目） ==
 *	27:	ext_tsk()
 *	== TASK1（続き）==
 *	28:	END
 */

#include <kernel.h>
#include <t_syslog.h>
#include "syssvc/test_svc.h"
#include "kernel_cfg.h"
#include "test_exttsk.h"

/* DO NOT DELETE THIS LINE -- gentest depends on it. */

void
alarm1_handler(EXINF exinf)
{
	ER_UINT	ercd;

	check_point(2);
	ercd = ext_tsk();
	check_ercd(ercd, E_CTX);

	ercd = wup_tsk(TASK1);
	check_ercd(ercd, E_OK);

	return;

	check_point(0);
}

void
task1(EXINF exinf)
{
	ER_UINT	ercd;
	T_RTSK	rtsk;
	T_RMTX	rmtx;
	PRI		intpri;

	test_start(__FILE__);

	check_point(1);
	ercd = sta_alm(ALM1, TEST_TIME_CP);
	check_ercd(ercd, E_OK);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(3);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(5);
	ercd = ref_tsk(TASK2, &rtsk);
	check_ercd(ercd, E_OK);

	check_assert(rtsk.tskstat == TTS_DMT);

	ercd = ref_mtx(MTX1, &rmtx);
	check_ercd(ercd, E_OK);

	check_assert(rmtx.htskid == TSK_NONE);

	check_assert(rmtx.wtskid == TSK_NONE);

	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(16);
	ercd = get_ipm(&intpri);
	check_ercd(ercd, E_OK);

	check_assert(intpri == TIPM_ENAALL);

	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(23);
	check_assert(sns_dsp() == false);

	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_finish(28);
	check_point(0);
}

static uint_t	task2_count = 0;

void
task2(EXINF exinf)
{
	ER_UINT	ercd;
	T_RTSK	rtsk;

	switch (++task2_count) {
	case 1:
		check_point(4);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_point(0);

	case 2:
		check_point(6);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK3);
		check_ercd(ercd, E_OK);

		check_point(8);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_point(0);

	case 3:
		check_point(11);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_point(0);

	case 4:
		check_point(12);
		ercd = ref_tsk(TASK2, &rtsk);
		check_ercd(ercd, E_OK);

		check_assert(rtsk.tskstat == TTS_RUN);

		check_assert(rtsk.actcnt == 0U);

		ercd = act_tsk(TASK2);
		check_ercd(ercd, E_OK);

		ercd = chg_pri(TASK2, TASK3_PRIORITY);
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK3);
		check_ercd(ercd, E_OK);

		check_point(13);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_point(0);

	case 5:
		check_point(15);
		check_assert(sns_loc() == false);

		ercd = chg_ipm(TMAX_INTPRI);
		check_ercd(ercd, E_OK);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_point(0);

	case 6:
		check_point(17);
		ercd = chg_ipm(TMAX_INTPRI);
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK3);
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK4);
		check_ercd(ercd, E_OK);

		check_point(18);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_point(0);

	case 7:
		check_point(22);
		ercd = dis_dsp();
		check_ercd(ercd, E_OK);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_point(0);

	case 8:
		check_point(24);
		ercd = dis_dsp();
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK3);
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK4);
		check_ercd(ercd, E_OK);

		check_point(25);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_point(0);

	default:
		check_point(0);
	}
	check_point(0);
}

static uint_t	task3_count = 0;

void
task3(EXINF exinf)
{
	ER_UINT	ercd;
	T_RTSK	rtsk;
	PRI		intpri;

	switch (++task3_count) {
	case 1:
		check_point(7);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(9);
		ercd = unl_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK2);
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK2);
		check_ercd(ercd, E_OK);

		check_point(10);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_point(0);

	case 2:
		check_point(14);
		ercd = ref_tsk(TASK2, &rtsk);
		check_ercd(ercd, E_OK);

		check_assert(rtsk.tskstat == TTS_RDY);

		check_assert(rtsk.actcnt == 0U);

		ercd = loc_cpu();
		check_ercd(ercd, E_OK);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_point(0);

	case 3:
		check_point(19);
		ercd = get_ipm(&intpri);
		check_ercd(ercd, E_OK);

		check_assert(intpri == TIPM_ENAALL);

		ercd = act_tsk(TASK2);
		check_ercd(ercd, E_OK);

		check_point(20);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_point(0);

	case 4:
		check_point(26);
		check_assert(sns_dsp() == false);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_point(0);

	default:
		check_point(0);
	}
	check_point(0);
}

static uint_t	task4_count = 0;

void
task4(EXINF exinf)
{
	ER_UINT	ercd;

	switch (++task4_count) {
	case 1:
		check_point(21);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_point(0);

	case 2:
		check_point(27);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_point(0);

	default:
		check_point(0);
	}
	check_point(0);
}
