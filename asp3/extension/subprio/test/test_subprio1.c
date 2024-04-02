/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2022 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: test_subprio1.c 1646 2022-09-25 17:51:54Z ertl-hiro $
 */

/* 
 *		サブ優先度機能のテスト(1)
 *
 *	テストの目的とテスト項目の設計については，「サブ優先度機能のテスト」
 *	（test_subprio.txt）を参照すること．
 *
 * 【テスト項目】
 *
 *	(A) サブ優先度の初期化
 *		(A-1) タスクの生成時にUINT_MAXに初期化されること［NGKI3681］
 *		(A-2) タスクの終了と起動で変更されないこと
 *	(B) ENA_SPRの機能［NGKI3679］
 *		(B-2) ENA_SPRによりサブ優先度を使用する優先度を複数設定できる
 *			  こと
 *	(C) chg_sprの静的エラー
 *		(C-1) 非タスクコンテキストからの呼出し［NGKI3666］
 *		(C-2) CPUロック状態からの呼出し［NGKI3667］
 *		(C-3) tskidが有効範囲外（小さすぎる）［NGKI3669］
 *		(C-4) tskidが有効範囲外（大きすぎる）［NGKI3669］
 *	(D) chg_sprの基本機能［NGKI3672］
 *	  (D-1) 対象タスクの優先度が，サブ優先度を使用すると設定されている
 *			場合
 *		(D-1-1) 自タスクに対するchg_spr（TSK_SELFを使用）で，自タスク
 *				のサブ優先度が変更されること［NGKI3674］
 *		(D-1-2) 自タスクに対するchg_spr（自タスクのIDを使用）で，自タ
 *				スクのサブ優先度が変更されること
 *		(D-1-3) 実行可能状態のタスクに対するchg_sprで，対象タスクのサ
 *				ブ優先度が変更されること
 *		(D-1-4) 実行できない状態のタスクに対するchg_sprで，対象タスク
 *				のサブ優先度が変更されること
 *		(D-1-5) 休止状態のタスクに対するchg_sprで，対象タスクのサブ優
 *				先度が変更されること
 *	  (D-2) 対象タスクの優先度が，サブ優先度を使用すると設定されていな
 *			い場合
 *		(D-2-1) 自タスクに対するchg_spr（TSK_SELFを使用）で，自タスク
 *				のサブ優先度が変更されること［NGKI3674］
 *		(D-2-2) 自タスクに対するchg_spr（自タスクのIDを使用）で，自タ
 *				スクのサブ優先度が変更されること
 *		(D-2-3) 実行可能状態のタスクに対するchg_sprで，対象タスクのサ
 *				ブ優先度が変更されること
 *		(D-2-4) 実行できない状態のタスクに対するchg_sprで，対象タスク
 *				のサブ優先度が変更されること
 *		(D-2-5) 休止状態のタスクに対するchg_sprで，対象タスクのサブ優
 *				先度が変更されること
 *	(E) タスクが実行できる状態に遷移した時の優先順位［NGKI0560］
 *	  (E-1) タスクの優先度が，サブ優先度を使用すると設定されている場合
 *		(E-1-1) 同優先度の他のタスクよりサブ優先度が高い場合に，そのタ
 *				スクより優先順位が高くなること（タスク切換えが起こる場
 *				合）
 *		(E-1-2) 同優先度の他のタスクよりサブ優先度が高い場合に，そのタ
 *				スクより優先順位が高くなること（タスク切換えが起こらな
 *				い場合）
 *		(E-1-3) 同優先度の他のタスクと同じサブ優先度の場合に，そのタス
 *				クより優先順位が低くなること［NGKI0561］
 *		(E-1-4) 同優先度の他のタスクよりサブ優先度が低い場合に，そのタ
 *				スクより優先順位が低くなること
 *		(E-1-5) 優先度上昇状態である場合に，同優先度のサブ優先度0のタ
 *				スクよりも優先順位が低く，他の同優先度のタスクよりも優
 *				先度が高くなること［NGKI5219］
 *	  (E-2) タスクの優先度が，サブ優先度を使用すると設定されていない場
 *			合
 *		(E-2-1) 同優先度の他のタスクよりサブ優先度が高い場合に，サブ優
 *				先度に関係なく，そのタスクより優先順位が低くなること
 *				［NGKI0561］
 *	(F) ref_tskによるサブ優先度の参照
 *		(F-1) ref_tskによりサブ優先度を参照できること［NGKI3662］
 *	(G) rot_rdqのE_ILUSEエラー
 *		(G-1) 対象優先度がサブ優先度を使用すると設定されている場合には，
 *			  E_ILUSEエラーとなること［NGKI3663］
 *		(G-2) 対象優先度がサブ優先度を使用すると設定されていない場合に
 *			  は，エラーとならないこと
 *
 * 【使用リソース】
 *
 *	TASK1: 高優先度タスク，メインタスク，最初から起動
 *	TASK2: 中優先度タスク
 *	TASK3: 中優先度タスク
 *	TASK4: 中優先度タスク
 *	TASK5: 低優先度タスク
 *	TASK6: 低優先度タスク
 *	TASK7: 最低優先度タスク
 *	高優先度と中優先度を，サブ優先度を使用して優先順位を決めるように設定
 *	MTX1:  ミューテックス（TA_CEILING属性，上限は中優先度）
 *	ALM1:  アラームハンドラ
 *
 * 【テストシーケンス】
 *
 *	== TASK1（優先度：高）==
 *	1:	ref_tsk(TASK1, &rtsk)						... (A-1)(F-1)
 *		assert(rtsk.subpri == UINT_MAX)
 *		chg_spr(TSK_SELF, 20)						... (D-1-1)
 *		ref_tsk(TASK1, &rtsk)						... (F-1)
 *		assert(rtsk.subpri == 20)
 *	2:	rot_rdq(HIGH_PRIORITY) -> E_ILUSE			... (G-1)(B-2)
 *		rot_rdq(MID_PRIORITY) -> E_ILUSE			... (G-1)(B-2)
 *		rot_rdq(LOW_PRIORITY)						... (G-2)(B-2)
 *	3:	sta_alm(ALM1, TEST_TIME_CP) ... ALM1が実行開始するまで
 *		slp_tsk()
 *	== ALM1 ==
 *	4:	chg_spr(TASK1, 10) -> E_CTX					... (C-1)
 *		wup_tsk(TASK1)
 *	5:	RETURN
 *	== TASK1（続き）==
 *	6:	loc_cpu()
 *		chg_spr(TASK1, 10) -> E_CTX					... (C-2)
 *		unl_cpu()
 *		chg_spr(-1, 10) -> E_ID						... (C-3)
 *		chg_spr(TNUM_TSKID+1, 10) -> E_ID			... (C-4)
 *	7:	act_tsk(TASK7)
 *		act_tsk(TASK2)
 *		chg_spr(TASK2, 20)							... (D-1-3)
 *		ref_tsk(TASK2, &rtsk)
 *		assert(rtsk.subpri == 20)
 *		slp_tsk()
 *	== TASK2-1（優先度：中）==
 *	8:	chg_spr(TASK3, 10)							... (D-1-5)
 *		ref_tsk(TASK3, &rtsk)
 *		assert(rtsk.subpri == 10)
 *		act_tsk(TASK3)								... (E-1-1)
 *	== TASK3-1（優先度：中）==
 *	9:	ext_tsk()
 *	== TASK2-1（続き）==
 *	10:	wup_tsk(TASK1)
 *	== TASK1（続き）==
 *	11:	act_tsk(TASK3)								... (E-1-2)
 *		ref_tsk(TASK3, &rtsk)						... (A-2)
 *		assert(rtsk.subpri == 10)
 *		slp_tsk()
 *	== TASK3-2（優先度：中）==
 *	12:	ext_tsk()
 *	== TASK2-1（続き）==
 *	13:	chg_spr(TASK2, 10)							... (D-1-2)
 *		ref_tsk(TASK2, &rtsk)
 *		assert(rtsk.subpri == 10)
 *		wup_tsk(TASK1)
 *	== TASK1（続き）==
 *	14:	act_tsk(TASK3)								... (E-1-3)
 *		slp_tsk()
 *	== TASK2-1（続き）==
 *	15:	slp_tsk()
 *	== TASK3-3（優先度：中）==
 *	16:	ext_tsk()
 *	== TASK7（優先度：最低）==
 *	17:	wup_tsk(TASK1)
 *	== TASK1（続き）==
 *	18:	chg_spr(TASK2, 0)							... (D-1-4)
 *		wup_tsk(TASK2)
 *		act_tsk(TASK3)								... (E-1-4)
 *		slp_tsk()
 *	== TASK2-1（続き）==
 *	19:	ext_tsk()
 *	== TASK3-4（優先度：中）==
 *	20:	loc_mtx(MTX1)
 *		slp_tsk()
 *	== TASK7（続き）==
 *	21:	wup_tsk(TASK1)
 *	== TASK1（続き）==
 *	22:	chg_spr(TASK4, 1)
 *		act_tsk(TASK2)
 *		act_tsk(TASK4)
 *		wup_tsk(TASK3)								... (E-1-5)
 *		slp_tsk()
 *	== TASK2-2（優先度：中）==
 *	23:	ext_tsk()
 *	== TASK3-4（続き）==
 *	24:	ext_tsk()
 *	== TASK4-1（優先度：中）==
 *	25:	act_tsk(TASK5)
 *		chg_spr(TASK5, 20)							... (D-2-3)
 *		ref_tsk(TASK5, &rtsk)
 *		assert(rtsk.subpri == 20)
 *		chg_spr(TASK6, 20)							... (D-2-5)
 *		ref_tsk(TASK6, &rtsk)
 *		assert(rtsk.subpri == 20)
 *		act_tsk(TASK6)								... (E-2-1)
 *		slp_tsk()
 *	== TASK5-1（優先度：低）==
 *	26:	slp_tsk()
 *	== TASK6-1（優先度：低）==
 *	27:	chg_spr(TSK_SELF, 10)						... (D-2-1)
 *		ref_tsk(TASK6, &rtsk)
 *		assert(rtsk.subpri == 10)
 *		chg_spr(TASK6, 0)							... (D-2-2)
 *		ref_tsk(TASK6, &rtsk)
 *		assert(rtsk.subpri == 0)
 *		chg_spr(TASK5, 10)							... (D-2-3)
 *		ref_tsk(TASK5, &rtsk)
 *		assert(rtsk.subpri == 10)
 *		ext_tsk()
 *	== TASK7（続き）==
 *	28:	wup_tsk(TASK1)
 *	== TASK1（続き）==
 *	29:	END
 */

#include <kernel.h>
#include <t_syslog.h>
#include "syssvc/test_svc.h"
#include "kernel_cfg.h"
#include "test_common.h"

/* DO NOT DELETE THIS LINE -- gentest depends on it. */

void
alarm1_handler(EXINF exinf)
{
	ER_UINT	ercd;

	check_point(4);
	ercd = chg_spr(TASK1, 10);
	check_ercd(ercd, E_CTX);

	ercd = wup_tsk(TASK1);
	check_ercd(ercd, E_OK);

	check_point(5);
	return;

	check_assert(false);
}

void
task1(EXINF exinf)
{
	ER_UINT	ercd;
	T_RTSK	rtsk;

	test_start(__FILE__);

	check_point(1);
	ercd = ref_tsk(TASK1, &rtsk);
	check_ercd(ercd, E_OK);

	check_assert(rtsk.subpri == UINT_MAX);

	ercd = chg_spr(TSK_SELF, 20);
	check_ercd(ercd, E_OK);

	ercd = ref_tsk(TASK1, &rtsk);
	check_ercd(ercd, E_OK);

	check_assert(rtsk.subpri == 20);

	check_point(2);
	ercd = rot_rdq(HIGH_PRIORITY);
	check_ercd(ercd, E_ILUSE);

	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_ILUSE);

	ercd = rot_rdq(LOW_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(3);
	ercd = sta_alm(ALM1, TEST_TIME_CP);
	check_ercd(ercd, E_OK);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(6);
	ercd = loc_cpu();
	check_ercd(ercd, E_OK);

	ercd = chg_spr(TASK1, 10);
	check_ercd(ercd, E_CTX);

	ercd = unl_cpu();
	check_ercd(ercd, E_OK);

	ercd = chg_spr(-1, 10);
	check_ercd(ercd, E_ID);

	ercd = chg_spr(TNUM_TSKID+1, 10);
	check_ercd(ercd, E_ID);

	check_point(7);
	ercd = act_tsk(TASK7);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	ercd = chg_spr(TASK2, 20);
	check_ercd(ercd, E_OK);

	ercd = ref_tsk(TASK2, &rtsk);
	check_ercd(ercd, E_OK);

	check_assert(rtsk.subpri == 20);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(11);
	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	ercd = ref_tsk(TASK3, &rtsk);
	check_ercd(ercd, E_OK);

	check_assert(rtsk.subpri == 10);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(14);
	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(18);
	ercd = chg_spr(TASK2, 0);
	check_ercd(ercd, E_OK);

	ercd = wup_tsk(TASK2);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(22);
	ercd = chg_spr(TASK4, 1);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK4);
	check_ercd(ercd, E_OK);

	ercd = wup_tsk(TASK3);
	check_ercd(ercd, E_OK);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_finish(29);
	check_assert(false);
}

static uint_t	task2_count = 0;

void
task2(EXINF exinf)
{
	ER_UINT	ercd;
	T_RTSK	rtsk;

	switch (++task2_count) {
	case 1:
		check_point(8);
		ercd = chg_spr(TASK3, 10);
		check_ercd(ercd, E_OK);

		ercd = ref_tsk(TASK3, &rtsk);
		check_ercd(ercd, E_OK);

		check_assert(rtsk.subpri == 10);

		ercd = act_tsk(TASK3);
		check_ercd(ercd, E_OK);

		check_point(10);
		ercd = wup_tsk(TASK1);
		check_ercd(ercd, E_OK);

		check_point(13);
		ercd = chg_spr(TASK2, 10);
		check_ercd(ercd, E_OK);

		ercd = ref_tsk(TASK2, &rtsk);
		check_ercd(ercd, E_OK);

		check_assert(rtsk.subpri == 10);

		ercd = wup_tsk(TASK1);
		check_ercd(ercd, E_OK);

		check_point(15);
		ercd = slp_tsk();
		check_ercd(ercd, E_OK);

		check_point(19);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 2:
		check_point(23);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	default:
		check_assert(false);
	}
	check_assert(false);
}

static uint_t	task3_count = 0;

void
task3(EXINF exinf)
{
	ER_UINT	ercd;

	switch (++task3_count) {
	case 1:
		check_point(9);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 2:
		check_point(12);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 3:
		check_point(16);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 4:
		check_point(20);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = slp_tsk();
		check_ercd(ercd, E_OK);

		check_point(24);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	default:
		check_assert(false);
	}
	check_assert(false);
}

static uint_t	task4_count = 0;

void
task4(EXINF exinf)
{
	ER_UINT	ercd;
	T_RTSK	rtsk;

	switch (++task4_count) {
	case 1:
		check_point(25);
		ercd = act_tsk(TASK5);
		check_ercd(ercd, E_OK);

		ercd = chg_spr(TASK5, 20);
		check_ercd(ercd, E_OK);

		ercd = ref_tsk(TASK5, &rtsk);
		check_ercd(ercd, E_OK);

		check_assert(rtsk.subpri == 20);

		ercd = chg_spr(TASK6, 20);
		check_ercd(ercd, E_OK);

		ercd = ref_tsk(TASK6, &rtsk);
		check_ercd(ercd, E_OK);

		check_assert(rtsk.subpri == 20);

		ercd = act_tsk(TASK6);
		check_ercd(ercd, E_OK);

		ercd = slp_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	default:
		check_assert(false);
	}
	check_assert(false);
}

static uint_t	task5_count = 0;

void
task5(EXINF exinf)
{
	ER_UINT	ercd;

	switch (++task5_count) {
	case 1:
		check_point(26);
		ercd = slp_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	default:
		check_assert(false);
	}
	check_assert(false);
}

static uint_t	task6_count = 0;

void
task6(EXINF exinf)
{
	ER_UINT	ercd;
	T_RTSK	rtsk;

	switch (++task6_count) {
	case 1:
		check_point(27);
		ercd = chg_spr(TSK_SELF, 10);
		check_ercd(ercd, E_OK);

		ercd = ref_tsk(TASK6, &rtsk);
		check_ercd(ercd, E_OK);

		check_assert(rtsk.subpri == 10);

		ercd = chg_spr(TASK6, 0);
		check_ercd(ercd, E_OK);

		ercd = ref_tsk(TASK6, &rtsk);
		check_ercd(ercd, E_OK);

		check_assert(rtsk.subpri == 0);

		ercd = chg_spr(TASK5, 10);
		check_ercd(ercd, E_OK);

		ercd = ref_tsk(TASK5, &rtsk);
		check_ercd(ercd, E_OK);

		check_assert(rtsk.subpri == 10);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	default:
		check_assert(false);
	}
	check_assert(false);
}

void
task7(EXINF exinf)
{
	ER_UINT	ercd;

	check_point(17);
	ercd = wup_tsk(TASK1);
	check_ercd(ercd, E_OK);

	check_point(21);
	ercd = wup_tsk(TASK1);
	check_ercd(ercd, E_OK);

	check_point(28);
	ercd = wup_tsk(TASK1);
	check_ercd(ercd, E_OK);

	check_assert(false);
}
