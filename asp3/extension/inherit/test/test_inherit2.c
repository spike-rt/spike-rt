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
 *  $Id: test_inherit2.c 1620 2022-09-19 06:39:09Z ertl-hiro $
 */

/* 
 *		優先度継承ミューテックスのテスト(2)
 *
 * 【テストの目的】
 *
 *  TA_NULL属性のミューテックス，TA_TPRI属性のミューテックス，優先度上
 *  限ミューテックス，優先度継承ミューテックスを1つずつ用意し，振舞い
 *  の違いをテストする．
 *
 *  優先度上限ミューテックスや，複数のミューテックスが必要なエラー条件
 *  についてテストする．
 *
 * 【テスト項目】
 *
 *	(A) コンフィギュレーションのテスト
 *		(A-1) TA_TPRI属性が指定でき，優先度順となること［NGKI2010］
 *		(A-2) TA_CEILING属性が指定でき，優先度順となること［NGKI2010］
 *		(A-3) TA_INHERIT属性が指定でき，優先度順となること［NGKI2010］
 *		(A-4) TA_NULL属性はFIFO順となること［NGKI2011］
 *		(A-5) CTE_MTXによるミューテックス属性の設定［NGKI5193］
 *		(A-6) CTE_MTXによる優先度上限の設定［NGKI5193］
 *	(B) loc_mtx/ploc_mtx/tloc_mtxのE_ILUSEエラー
 *		(B-1) loc_mtx：自タスクのベース優先度が高い［NGKI2085］
 *		(B-2) ploc_mtx：自タスクのベース優先度が高い［NGKI2085］
 *		(B-3) tloc_mtx：自タスクのベース優先度が高い［NGKI2085］
 *	(C) unl_mtxのE_OBJエラー
 *		(C-1) 対象ミューテックスが最後にロックされたものでない［NGKI3611］
 *	(D) chg_priのE_ILUSEエラー
 *		(D-1) 対象タスクが上限優先度の低いミューテックスをロック［NGKI1201］
 *		(D-2) 対象タスクが上限優先度の低いミューテックスをロック待ち［NGKI1201］
 *
 * 【使用リソース】
 *
 *	TASK1: 低優先度タスク，メインタスク，最初から起動
 *	TASK2: 中優先度タスク
 *	TASK3: 高優先度タスク
 *	TASK4: 低優先度タスク
 *	MTX1:  ミューテックス（TA_NULL属性）
 *	MTX2:  ミューテックス（TA_TPRI属性）
 *	MTX3:  ミューテックス（優先度上限，優先度上限：高）
 *	MTX4:  ミューテックス（優先度継承）
 *
 * 【テストシーケンス】
 *
 *	== TASK1（優先度：低）==
 *	1:	loc_mtx(MTX1)
 *		act_tsk(TASK2)
 *	== TASK2-1（優先度：中）==
 *	2:	loc_mtx(MTX1)
 *	== TASK1（続き）==
 *	3:	act_tsk(TASK3)
 *	== TASK3-1（優先度：高）==
 *	4:	loc_mtx(MTX1)
 *	== TASK1（続き）==
 *	5:	ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TASK1)
 *		assert(rmtx.wtskid == TASK2)					... (A-4)(A-5)
 *		unl_mtx(MTX1)
 *	== TASK2-1（続き）==
 *	6:	unl_mtx(MTX1)
 *	== TASK3-1（続き）==
 *	7:	unl_mtx(MTX1)
 *		ext_tsk()
 *	== TASK2-1（続き）==
 *	8:	ext_tsk()
 *
 *	== TASK1（続き）==
 *	9:	loc_mtx(MTX2)
 *		act_tsk(TASK2)
 *	== TASK2-2（優先度：中）==
 *	10:	loc_mtx(MTX2)
 *	== TASK1（続き）==
 *	11:	act_tsk(TASK3)
 *	== TASK3-2（優先度：高）==
 *	12:	loc_mtx(MTX2)
 *	== TASK1（続き）==
 *	13:	ref_mtx(MTX2, &rmtx)
 *		assert(rmtx.htskid == TASK1)
 *		assert(rmtx.wtskid == TASK3)					... (A-1)(A-5)
 *		unl_mtx(MTX2)
 *	== TASK3-2（続き）==
 *	14:	unl_mtx(MTX2)
 *		ext_tsk()
 *	== TASK2-2（続き）==
 *	15:	unl_mtx(MTX2)
 *		ext_tsk()
 *
 *	== TASK1（続き）==
 *	16:	loc_mtx(MTX3)
 *		ref_tsk(TSK_SELF, &rtsk)
 *		assert(rtsk.tskpri == HIGH_PRIORITY)			... (A-6)
 *		assert(rtsk.tskbpri == LOW_PRIORITY)
 *		act_tsk(TASK4)
 *		slp_tsk()
 *	== TASK4（優先度：低）==
 *	17:	act_tsk(TASK2)
 *	== TASK2-3（優先度：中）==
 *	18:	loc_mtx(MTX3)
 *	== TASK4（続き）==
 *	19:	chg_pri(TASK2, HIGH_PRIORITY-1) -> E_ILUSE		... (D-2)
 *	20:	act_tsk(TASK3)
 *	== TASK3-3（優先度：高）==
 *	21:	loc_mtx(MTX3)
 *	== TASK4（続き）==
 *	22:	ref_mtx(MTX3, &rmtx)
 *		assert(rmtx.htskid == TASK1)
 *		assert(rmtx.wtskid == TASK3)					... (A-2)(A-5)
 *		wup_tsk(TASK1)
 *	== TASK1（続き）==
 *	23:	unl_mtx(MTX3)
 *	== TASK3-3（続き）==
 *	24:	unl_mtx(MTX3)
 *		ext_tsk()
 *	== TASK2-3（続き）==
 *	25:	unl_mtx(MTX3)
 *		ext_tsk()
 *
 *	== TASK1（続き）==
 *	26:	loc_mtx(MTX4)
 *		act_tsk(TASK2)
 *	== TASK2-4（優先度：中）==
 *	27:	loc_mtx(MTX4)
 *	== TASK1（続き）==
 *	28:	act_tsk(TASK3)
 *	== TASK3-4（優先度：高）==
 *	29:	loc_mtx(MTX4)
 *	== TASK1（続き）==
 *	30:	ref_mtx(MTX4, &rmtx)
 *		assert(rmtx.htskid == TASK1)
 *		assert(rmtx.wtskid == TASK3)					... (A-3)(A-5)
 *		unl_mtx(MTX4)
 *	== TASK3-4（続き）==
 *	31:	unl_mtx(MTX4)
 *		ext_tsk()
 *	== TASK2-4（続き）==
 *	32:	unl_mtx(MTX4)
 *		ext_tsk()
 *
 *	== TASK1（続き）==
 *	33:	chg_pri(TSK_SELF, HIGH_PRIORITY-1)
 *		loc_mtx(MTX3) -> E_ILUSE						... (B-1)
 *		ploc_mtx(MTX3) -> E_ILUSE						... (B-2)
 *		tloc_mtx(MTX3, TEST_TIME_CP) -> E_ILUSE			... (B-3)
 *		chg_pri(TSK_SELF, TPRI_INI)
 *	== TASK4（続き）==
 *	34:	ext_tsk()
 *	== TASK1（続き）==
 *	35:	loc_mtx(MTX1)
 *		loc_mtx(MTX3)
 *		unl_mtx(MTX1) -> E_OBJ							... (C-1)
 *	36:	chg_pri(TSK_SELF, HIGH_PRIORITY-1) -> E_ILUSE	... (D-1)
 *		unl_mtx(MTX3)
 *		unl_mtx(MTX1)
 *	37:	END
 */

#include <kernel.h>
#include <t_syslog.h>
#include "syssvc/test_svc.h"
#include "kernel_cfg.h"
#include "test_common.h"

/* DO NOT DELETE THIS LINE -- gentest depends on it. */

void
task1(EXINF exinf)
{
	ER_UINT	ercd;
	T_RMTX	rmtx;
	T_RTSK	rtsk;

	test_start(__FILE__);

	check_point(1);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(3);
	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	check_point(5);
	ercd = ref_mtx(MTX1, &rmtx);
	check_ercd(ercd, E_OK);

	check_assert(rmtx.htskid == TASK1);

	check_assert(rmtx.wtskid == TASK2);

	ercd = unl_mtx(MTX1);
	check_ercd(ercd, E_OK);

	check_point(9);
	ercd = loc_mtx(MTX2);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(11);
	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	check_point(13);
	ercd = ref_mtx(MTX2, &rmtx);
	check_ercd(ercd, E_OK);

	check_assert(rmtx.htskid == TASK1);

	check_assert(rmtx.wtskid == TASK3);

	ercd = unl_mtx(MTX2);
	check_ercd(ercd, E_OK);

	check_point(16);
	ercd = loc_mtx(MTX3);
	check_ercd(ercd, E_OK);

	ercd = ref_tsk(TSK_SELF, &rtsk);
	check_ercd(ercd, E_OK);

	check_assert(rtsk.tskpri == HIGH_PRIORITY);

	check_assert(rtsk.tskbpri == LOW_PRIORITY);

	ercd = act_tsk(TASK4);
	check_ercd(ercd, E_OK);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(23);
	ercd = unl_mtx(MTX3);
	check_ercd(ercd, E_OK);

	check_point(26);
	ercd = loc_mtx(MTX4);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(28);
	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	check_point(30);
	ercd = ref_mtx(MTX4, &rmtx);
	check_ercd(ercd, E_OK);

	check_assert(rmtx.htskid == TASK1);

	check_assert(rmtx.wtskid == TASK3);

	ercd = unl_mtx(MTX4);
	check_ercd(ercd, E_OK);

	check_point(33);
	ercd = chg_pri(TSK_SELF, HIGH_PRIORITY-1);
	check_ercd(ercd, E_OK);

	ercd = loc_mtx(MTX3);
	check_ercd(ercd, E_ILUSE);

	ercd = ploc_mtx(MTX3);
	check_ercd(ercd, E_ILUSE);

	ercd = tloc_mtx(MTX3, TEST_TIME_CP);
	check_ercd(ercd, E_ILUSE);

	ercd = chg_pri(TSK_SELF, TPRI_INI);
	check_ercd(ercd, E_OK);

	check_point(35);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = loc_mtx(MTX3);
	check_ercd(ercd, E_OK);

	ercd = unl_mtx(MTX1);
	check_ercd(ercd, E_OBJ);

	check_point(36);
	ercd = chg_pri(TSK_SELF, HIGH_PRIORITY-1);
	check_ercd(ercd, E_ILUSE);

	ercd = unl_mtx(MTX3);
	check_ercd(ercd, E_OK);

	ercd = unl_mtx(MTX1);
	check_ercd(ercd, E_OK);

	check_finish(37);
	check_assert(false);
}

static uint_t	task2_count = 0;

void
task2(EXINF exinf)
{
	ER_UINT	ercd;

	switch (++task2_count) {
	case 1:
		check_point(2);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(6);
		ercd = unl_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(8);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 2:
		check_point(10);
		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		check_point(15);
		ercd = unl_mtx(MTX2);
		check_ercd(ercd, E_OK);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 3:
		check_point(18);
		ercd = loc_mtx(MTX3);
		check_ercd(ercd, E_OK);

		check_point(25);
		ercd = unl_mtx(MTX3);
		check_ercd(ercd, E_OK);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 4:
		check_point(27);
		ercd = loc_mtx(MTX4);
		check_ercd(ercd, E_OK);

		check_point(32);
		ercd = unl_mtx(MTX4);
		check_ercd(ercd, E_OK);

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
		check_point(4);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(7);
		ercd = unl_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 2:
		check_point(12);
		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		check_point(14);
		ercd = unl_mtx(MTX2);
		check_ercd(ercd, E_OK);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 3:
		check_point(21);
		ercd = loc_mtx(MTX3);
		check_ercd(ercd, E_OK);

		check_point(24);
		ercd = unl_mtx(MTX3);
		check_ercd(ercd, E_OK);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 4:
		check_point(29);
		ercd = loc_mtx(MTX4);
		check_ercd(ercd, E_OK);

		check_point(31);
		ercd = unl_mtx(MTX4);
		check_ercd(ercd, E_OK);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	default:
		check_assert(false);
	}
	check_assert(false);
}

void
task4(EXINF exinf)
{
	ER_UINT	ercd;
	T_RMTX	rmtx;

	check_point(17);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(19);
	ercd = chg_pri(TASK2, HIGH_PRIORITY-1);
	check_ercd(ercd, E_ILUSE);

	check_point(20);
	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	check_point(22);
	ercd = ref_mtx(MTX3, &rmtx);
	check_ercd(ercd, E_OK);

	check_assert(rmtx.htskid == TASK1);

	check_assert(rmtx.wtskid == TASK3);

	ercd = wup_tsk(TASK1);
	check_ercd(ercd, E_OK);

	check_point(34);
	ercd = ext_tsk();
	check_ercd(ercd, E_OK);

	check_assert(false);
}
