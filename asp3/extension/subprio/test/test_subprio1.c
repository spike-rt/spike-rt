/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2014-2017 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: test_subprio1.c 856 2017-12-17 01:14:40Z ertl-hiro $
 */

/* 
 *		サブ優先度機能のテスト(1)
 *
 * 【テストの目的】
 *
 *  サブ優先度を用いたスケジューリングに関して網羅的にテストする．
 *
 * 【テスト項目】
 *
 *	(A) サブ優先度の初期化
 *		(A-1) タスクの生成時にUINT_MAXに初期化されること［NGKI3681］
 *		(A-2) タスクの終了と起動で変更されないこと
 *	(B) タスクが実行できる状態へ遷移した時のサブ優先度を用いた優先順位
 *		の決定
 *		(B-1) サブ優先度が高いタスクが高い優先順位を持つ［NGKI0560］（タ
 *			  スク切換えなし）
 *		(B-2) サブ優先度が高いタスクが高い優先順位を持つ［NGKI0560］（タ
 *			  スク切換えあり）
 *		(B-3) サブ優先度が同一のタスクの間では最低優先順位になる［NGKI0561］
 *	(C) chg_priによる優先度の変更時のサブ優先度を用いた優先順位の決定

 *		(C-1) サブ優先度が高いタスクが高い優先順位を持つ［NGKI0560］（タ
 *			  スク切換えなし）
 *		(C-2) サブ優先度が高いタスクが高い優先順位を持つ［NGKI0560］（タ
 *			  スク切換えあり）
 *		(C-3) サブ優先度が同一のタスクの間では最低優先順位になる［NGKI0561］
 *			  （タスク切換えなし）
 *		(C-4) サブ優先度が同一のタスクの間では最低優先順位になる［NGKI0561］
 *			  （タスク切換えあり）
 *		(C-5) 優先度上限ミューテックスをロックしている時 … 実施しない
 *			※［NGKI3682］により，サブ優先度機能と絡むことがない
 *	(D) chg_sprによるサブ優先度の変更［NGKI3672］
 *		(D-1) 実行状態のタスクに対するchg_spr（タスク切換えなし）
 *		(D-2) 実行状態のタスクに対するchg_spr（タスク切換えあり）
 *		(D-3) 実行可能状態のタスクに対するchg_spr（タスク切換えなし）
 *		(D-4) 実行可能状態のタスクに対するchg_spr（タスク切換えあり）
 *		(D-5) 休止状態のタスクに対するchg_spr
 *		(D-6) 同じサブ優先度のタスクの中で最低優先順位になること［NGKI3673］
 *	(E) サブ優先度の参照
 *		(E-1) ref_tskにより，サブ優先度を参照できること［NGKI3662］
 *	(F) ミューテックスによる現在優先度の変更
 *		(F-1) サービスコールの前後とも実行できる状態の時
 *		(F-2) サービスコールにより実行できる状態になった時 … 実施しない
 *			※［NGKI3682］により，サブ優先度機能と絡むことがない
 *	(G) サブ優先度を用いる／用いない優先度を対象としたrot_rdq
 *		(G-1) rot_rdqがE_ILUSEエラーになること［NGKI3663］
 *		(G-2) サブ優先度を用いない優先度を対象にした時
 *	(H) サブ優先度を用いる優先度がENA_SPRにより設定できること［NGKI3675］
 *
 * 【使用リソース】
 *
 *	TASK1: 高優先度タスク，メインタスク，最初から起動
 *	TASK2: 中優先度タスク
 *	TASK3: 中優先度タスク
 *	TASK4: 中優先度タスク
 *	TASK5: 中優先度タスク
 *	中優先度を，サブ優先度を使って優先順位を決めるように設定
 *	MTX1:  ミューテックス（TA_CEILING属性，上限は中優先度）
 *
 * 【テストシーケンス】
 *
 *	== TASK1（優先度：高）==
 *	1:	act_tsk(TASK2)
 *		ref_tsk(TASK2, &rtsk)						... (A-1)(E-1)
 *		assert(rtsk.subpri == UINT_MAX)
 *		act_tsk(TASK3)								... (B-3)
 *		act_tsk(TASK4)								... (B-3)
 *		act_tsk(TASK5)								... (B-3)
 *		slp_tsk()
 *	== TASK2-1（優先度：中，1回め）==
 *	2:	rot_rdq(MID_PRIORITY) -> E_ILUSE			... (G-1)
 *		rot_rdq(LOW_PRIORITY)						... (G-2)
 *		slp_tsk()
 *	== TASK3-1（優先度：中，1回め）==
 *	3:	slp_tsk()
 *	== TASK4-1（優先度：中，1回め）==
 *	4:	slp_tsk()
 *	== TASK5（優先度：中）==
 *	5:	wup_tsk(TASK1)
 *	== TASK1（続き）==
 *	6:	chg_spr(TASK2, 10)
 *		ref_tsk(TASK2, &rtsk)						... (E-1)
 *		assert(rtsk.subpri == 10)
 *		chg_spr(TASK3, 5)
 *		chg_spr(TASK4, 5)
 *		wup_tsk(TASK2)								... (B-1)
 *		wup_tsk(TASK3)								... (B-1)
 *		wup_tsk(TASK4)								... (B-3)
 *		slp_tsk()
 *	== TASK3-1（続き）==
 *	7:	slp_tsk()
 *	== TASK4-1（続き）==
 *	8:	wup_tsk(TASK3)								... (B-3)
 *		ext_tsk()
 *	== TASK3-1（続き）==
 *	9:	slp_tsk()
 *	== TASK2-1（続き）==
 *	10:	wup_tsk(TASK3)								... (B-2)
 *	== TASK3-1（続き）==
 *	11:	chg_spr(TSK_SELF, 5)						... (D-1)
 *	12:	chg_spr(TSK_SELF, 10)						... (D-2)(D-6)
 *	== TASK2-1（続き）==
 *	13:	chg_spr(TASK3, 10)							... (D-3)(D-6)
 *	14:	chg_spr(TASK3, 5)							... (D-4)
 *	== TASK3-1（続き）==
 *	15:	ext_tsk()
 *	== TASK2-1（続き）==
 *	16:	ext_tsk()
 *	== TASK5（続き）==
 *	17:	wup_tsk(TASK1)
 *	== TASK1（続き）==
 *	18:	act_tsk(TASK4)
 *		chg_pri(TASK4, LOW_PRIORITY)
 *		chg_spr(TASK3, 10)							... (D-5)
 *		act_tsk(TASK3)
 *		ref_tsk(TASK3, &rtsk)
 *		assert(rtsk.subpri == 10)
 *		act_tsk(TASK2)
 *		ref_tsk(TASK2, &rtsk)						... (A-2)
 *		assert(rtsk.subpri == 10)
 *		chg_pri(TASK4, TPRI_INI)					... (C-1)
 *		slp_tsk()
 *	== TASK4-2（2回め）==
 *	19:	chg_pri(TSK_SELF, LOW_PRIORITY)
 *	== TASK3-2（2回め）==
 *	20:	chg_pri(TASK2, TPRI_INI)					... (C-3)
 *	21:	chg_pri(TSK_SELF, TPRI_INI)					... (C-4)
 *	== TASK2-2（2回め）==
 *	22:	chg_pri(TASK4, TPRI_INI)					... (C-2)
 *	== TASK4-2（続き）==
 *	23:	ext_tsk()
 *	== TASK2-2（続き）==
 *	24:	loc_mtx(MTX1)
 *		unl_mtx(MTX1)								... (F-1)
 *		ext_tsk()
 *	== TASK3-2（続き）==
 *	25:	ext_tsk()
 *	== TASK5（続き）==
 *	26:	wup_tsk(TASK1)
 *	== TASK1（続き）==
 *	27:	END
 */

#include <kernel.h>
#include <t_syslog.h>
#include "syssvc/test_svc.h"
#include "kernel_cfg.h"
#include "test_subprio1.h"

/* DO NOT DELETE THIS LINE -- gentest depends on it. */

void
task1(intptr_t exinf)
{
	ER_UINT	ercd;
	T_RTSK	rtsk;

	test_start(__FILE__);

	check_point(1);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	ercd = ref_tsk(TASK2, &rtsk);
	check_ercd(ercd, E_OK);

	check_assert(rtsk.subpri == UINT_MAX);

	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK4);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK5);
	check_ercd(ercd, E_OK);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(6);
	ercd = chg_spr(TASK2, 10);
	check_ercd(ercd, E_OK);

	ercd = ref_tsk(TASK2, &rtsk);
	check_ercd(ercd, E_OK);

	check_assert(rtsk.subpri == 10);

	ercd = chg_spr(TASK3, 5);
	check_ercd(ercd, E_OK);

	ercd = chg_spr(TASK4, 5);
	check_ercd(ercd, E_OK);

	ercd = wup_tsk(TASK2);
	check_ercd(ercd, E_OK);

	ercd = wup_tsk(TASK3);
	check_ercd(ercd, E_OK);

	ercd = wup_tsk(TASK4);
	check_ercd(ercd, E_OK);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(18);
	ercd = act_tsk(TASK4);
	check_ercd(ercd, E_OK);

	ercd = chg_pri(TASK4, LOW_PRIORITY);
	check_ercd(ercd, E_OK);

	ercd = chg_spr(TASK3, 10);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	ercd = ref_tsk(TASK3, &rtsk);
	check_ercd(ercd, E_OK);

	check_assert(rtsk.subpri == 10);

	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	ercd = ref_tsk(TASK2, &rtsk);
	check_ercd(ercd, E_OK);

	check_assert(rtsk.subpri == 10);

	ercd = chg_pri(TASK4, TPRI_INI);
	check_ercd(ercd, E_OK);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_finish(27);
	check_point(0);
}

static uint_t	task2_count = 0;

void
task2(intptr_t exinf)
{
	ER_UINT	ercd;

	switch (++task2_count) {
	case 1:
		check_point(2);
		ercd = rot_rdq(MID_PRIORITY);
		check_ercd(ercd, E_ILUSE);

		ercd = rot_rdq(LOW_PRIORITY);
		check_ercd(ercd, E_OK);

		ercd = slp_tsk();
		check_ercd(ercd, E_OK);

		check_point(10);
		ercd = wup_tsk(TASK3);
		check_ercd(ercd, E_OK);

		check_point(13);
		ercd = chg_spr(TASK3, 10);
		check_ercd(ercd, E_OK);

		check_point(14);
		ercd = chg_spr(TASK3, 5);
		check_ercd(ercd, E_OK);

		check_point(16);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_point(0);

	case 2:
		check_point(22);
		ercd = chg_pri(TASK4, TPRI_INI);
		check_ercd(ercd, E_OK);

		check_point(24);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = unl_mtx(MTX1);
		check_ercd(ercd, E_OK);

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
task3(intptr_t exinf)
{
	ER_UINT	ercd;

	switch (++task3_count) {
	case 1:
		check_point(3);
		ercd = slp_tsk();
		check_ercd(ercd, E_OK);

		check_point(7);
		ercd = slp_tsk();
		check_ercd(ercd, E_OK);

		check_point(9);
		ercd = slp_tsk();
		check_ercd(ercd, E_OK);

		check_point(11);
		ercd = chg_spr(TSK_SELF, 5);
		check_ercd(ercd, E_OK);

		check_point(12);
		ercd = chg_spr(TSK_SELF, 10);
		check_ercd(ercd, E_OK);

		check_point(15);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_point(0);

	case 2:
		check_point(20);
		ercd = chg_pri(TASK2, TPRI_INI);
		check_ercd(ercd, E_OK);

		check_point(21);
		ercd = chg_pri(TSK_SELF, TPRI_INI);
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

static uint_t	task4_count = 0;

void
task4(intptr_t exinf)
{
	ER_UINT	ercd;

	switch (++task4_count) {
	case 1:
		check_point(4);
		ercd = slp_tsk();
		check_ercd(ercd, E_OK);

		check_point(8);
		ercd = wup_tsk(TASK3);
		check_ercd(ercd, E_OK);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_point(0);

	case 2:
		check_point(19);
		ercd = chg_pri(TSK_SELF, LOW_PRIORITY);
		check_ercd(ercd, E_OK);

		check_point(23);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_point(0);

	default:
		check_point(0);
	}
	check_point(0);
}

void
task5(intptr_t exinf)
{
	ER_UINT	ercd;

	check_point(5);
	ercd = wup_tsk(TASK1);
	check_ercd(ercd, E_OK);

	check_point(17);
	ercd = wup_tsk(TASK1);
	check_ercd(ercd, E_OK);

	check_point(26);
	ercd = wup_tsk(TASK1);
	check_ercd(ercd, E_OK);

	check_point(0);
}
