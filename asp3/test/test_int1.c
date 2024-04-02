/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2008-2022 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: test_int1.c 1724 2022-10-23 08:09:48Z ertl-hiro $
 */

/* 
 *		割込み管理機能のテスト(1)
 *
 * 【テストの目的】
 *
 *  割込み要求ラインに対するサービスコール（dis_int，ena_int，clr_int，
 *  ras_int，prb_int）の基本動作をテストする．
 *
 * 【制限事項】
 *
 *  このテストを実施するには，dis_int，ena_int，ras_int，prb_intがサポー
 *  トされており，それらを行える割込み要求ラインがあることが必要である．
 *  clr_intがサポートされていない場合，clr_intのテストは実施されない．
 *
 * 【テスト項目】
 *
 *	(A) dis_intのテスト
 *		(A-1) dis_intにより，割込み要求がマスクされる
 *	(B) ena_intのテスト
 *		(B-1) ena_intにより，割込み要求がマスク解除される
 *	(C) clr_intのテスト（clr_intがサポートされている場合）
 *		(C-1) clr_intにより，割込み要求がクリアされる
 *	(D) ras_intのテスト
 *		(D-1) ras_intにより，割込みが要求され，割込みが受け付けられる
 *		(D-2) ras_intにより，割込みが要求されるが，CPUロック状態である
 *		      ために受け付けられない
 *		(D-3) ras_intにより，割込みが要求されるが，割込み要求が禁止さ
 *		      れているために受け付けられない
 *		(D-4) ras_intにより，割込みが要求されるが，割込み優先度マスク
 *		      でマスクされているために受け付けられない
 *		(D-5) ras_intにより，割込みが要求されるが，同じ割込みが処理中
 *		      であるために受け付けられない
 *	(E) prb_intのテスト
 *		(E-1) 割込み要求がない時は，falseを返す
 *		(E-2) 割込み要求がある時が，CPUロック状態で割込みが受け付けら
 *		      れていない場合は，trueを返す
 *		(E-3) 割込み要求がある時が，割込み要求が禁止されていて割込みが
 *		      受け付けられていない場合は，trueを返す
 *		(E-4) 割込み要求がある時が，割込み優先度マスクでマスクされてい
 *		      て割込みが受け付けられていない場合は，trueを返す
 *		(E-5) 割込み要求が受け付けられれば，falseを返す
 *		(E-6) 割込み要求がある時が，同じ割込みが処理中であるために割込
 *		      みが受け付けられていない場合は，trueを返す
 *	(F) CRE_ISRのテスト
 *		(F-1) 割込みサービスルーチンは，割込みサービスルーチン優先度の
 *		      高い順に全て呼び出されること［NGKI2948］
 *		(F-2) 割込みサービスルーチンのリターン時に，CPUロック状態が解
 *		      除されること［NGKI0566］
 *
 * 【使用リソース】
 *
 *	TASK1: 中優先度タスク，TA_ACT属性
 *  ISR1:  割込みサービスルーチン（ISR優先度：2）
 *  ISR2:  割込みサービスルーチン（ISR優先度：1）
 *
 * 【テストシーケンス】
 *
 *	== TASK1（優先度：中）==
 *	1:	ras_int(INTNO1)						... (D-1)
 *	== ISR2-1 ==
 *	2:	RETURN								... (F-1)
 *	== ISR1-1 ==
 *	3:	DO(intno1_clear())
 *		RETURN
 *	== TASK1（続き）==
 *	4:	loc_cpu()
 *		prb_int(INTNO1) -> false			... (E-1)
 *		ras_int(INTNO1)						... (D-2)
 *		prb_int(INTNO1) -> true				... (E-2)
 *	5:	unl_cpu()
 *	== ISR2-2 ==
 *	6:	loc_cpu()
 *		sns_loc() -> true
 *		RETURN
 *	== ISR1-2 ==
 *	7:	sns_loc() -> false					... (F-2)
 *		DO(intno1_clear())
 *		RETURN
 *	== TASK1（続き）==
 *	8:	dis_int(INTNO1)						... (A-1)
 *		prb_int(INTNO1) -> false			... (E-1)
 *		ras_int(INTNO1)						... (D-3)
 *		prb_int(INTNO1) -> true				... (E-3)
 *	9:	ena_int(INTNO1)						... (B-1)
 *	== ISR2-3 ==
 *	10:	RETURN
 *	== ISR1-3 ==
 *	11:	DO(intno1_clear())
 *		RETURN
 *	== TASK1（続き）==
 *	12:	chg_ipm(TMIN_INTPRI)
 *		prb_int(INTNO1) -> false			... (E-1)
 *		ras_int(INTNO1)						... (D-4)
 *		prb_int(INTNO1) -> true				... (E-4)
 *	13:	chg_ipm(TIPM_ENAALL)
 *	== ISR2-4 ==
 *	14:	RETURN
 *	== ISR1-4 ==
 *	15:	DO(intno1_clear())
 *		prb_int(INTNO1) -> false			... (E-5)
 *		ras_int(INTNO1)						... (D-5)
 *		prb_int(INTNO1) -> true				... (E-6)
 *	16:	RETURN
 *	== ISR2-5 ==
 *	17:	RETURN
 *	== ISR1-5 ==
 *	18:	DO(intno1_clear())
 *		RETURN
 *	== TASK1（続き）==
 *	19:	DO(CHECK_SUPPORT_CLR_INT)
 *		dis_int(INTNO1)						... (A-1)
 *		prb_int(INTNO1) -> false			... (E-1)
 *		ras_int(INTNO1)						... (D-3)
 *		prb_int(INTNO1) -> true				... (E-3)
 *		clr_int(INTNO1)						... (C-1)
 *		prb_int(INTNO1) -> false			... (E-1)
 *	20:	ena_int(INTNO1)						... (B-1)
 *	21:	END
 */

#include <kernel.h>
#include <t_syslog.h>
#include "syssvc/test_svc.h"
#include "kernel_cfg.h"
#include "test_common.h"

/*
 *  clr_intがサポートされていない場合に，途中で終了させるための処理
 *
 *  CHECK_SUPPORT_CLR_INTに置かれているチェックポイントの番号が変わっ
 *  た場合には，以下のコード中のチェックポイント番号の修正が必要である．
 */
#ifdef TOPPERS_SUPPORT_CLR_INT
#define CHECK_SUPPORT_CLR_INT
#else /* TOPPERS_SUPPORT_CLR_INT */
#define CHECK_SUPPORT_CLR_INT	check_finish(15)
#define clr_int(intno)	(E_OK)
#endif /* TOPPERS_SUPPORT_CLR_INT */

/* DO NOT DELETE THIS LINE -- gentest depends on it. */

static uint_t	isr1_count = 0;

void
isr1(EXINF exinf)
{
	ER_UINT	ercd;

	switch (++isr1_count) {
	case 1:
		check_point(3);
		intno1_clear();

		return;

		check_assert(false);

	case 2:
		check_point(7);
		ercd = sns_loc();
		check_ercd(ercd, false);

		intno1_clear();

		return;

		check_assert(false);

	case 3:
		check_point(11);
		intno1_clear();

		return;

		check_assert(false);

	case 4:
		check_point(15);
		intno1_clear();

		ercd = prb_int(INTNO1);
		check_ercd(ercd, false);

		ercd = ras_int(INTNO1);
		check_ercd(ercd, E_OK);

		ercd = prb_int(INTNO1);
		check_ercd(ercd, true);

		check_point(16);
		return;

		check_assert(false);

	case 5:
		check_point(18);
		intno1_clear();

		return;

		check_assert(false);

	default:
		check_assert(false);
	}
	check_assert(false);
}

static uint_t	isr2_count = 0;

void
isr2(EXINF exinf)
{
	ER_UINT	ercd;

	switch (++isr2_count) {
	case 1:
		check_point(2);
		return;

		check_assert(false);

	case 2:
		check_point(6);
		ercd = loc_cpu();
		check_ercd(ercd, E_OK);

		ercd = sns_loc();
		check_ercd(ercd, true);

		return;

		check_assert(false);

	case 3:
		check_point(10);
		return;

		check_assert(false);

	case 4:
		check_point(14);
		return;

		check_assert(false);

	case 5:
		check_point(17);
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

	test_start(__FILE__);

	check_point(1);
	ercd = ras_int(INTNO1);
	check_ercd(ercd, E_OK);

	check_point(4);
	ercd = loc_cpu();
	check_ercd(ercd, E_OK);

	ercd = prb_int(INTNO1);
	check_ercd(ercd, false);

	ercd = ras_int(INTNO1);
	check_ercd(ercd, E_OK);

	ercd = prb_int(INTNO1);
	check_ercd(ercd, true);

	check_point(5);
	ercd = unl_cpu();
	check_ercd(ercd, E_OK);

	check_point(8);
	ercd = dis_int(INTNO1);
	check_ercd(ercd, E_OK);

	ercd = prb_int(INTNO1);
	check_ercd(ercd, false);

	ercd = ras_int(INTNO1);
	check_ercd(ercd, E_OK);

	ercd = prb_int(INTNO1);
	check_ercd(ercd, true);

	check_point(9);
	ercd = ena_int(INTNO1);
	check_ercd(ercd, E_OK);

	check_point(12);
	ercd = chg_ipm(TMIN_INTPRI);
	check_ercd(ercd, E_OK);

	ercd = prb_int(INTNO1);
	check_ercd(ercd, false);

	ercd = ras_int(INTNO1);
	check_ercd(ercd, E_OK);

	ercd = prb_int(INTNO1);
	check_ercd(ercd, true);

	check_point(13);
	ercd = chg_ipm(TIPM_ENAALL);
	check_ercd(ercd, E_OK);

	check_point(19);
	CHECK_SUPPORT_CLR_INT;

	ercd = dis_int(INTNO1);
	check_ercd(ercd, E_OK);

	ercd = prb_int(INTNO1);
	check_ercd(ercd, false);

	ercd = ras_int(INTNO1);
	check_ercd(ercd, E_OK);

	ercd = prb_int(INTNO1);
	check_ercd(ercd, true);

	ercd = clr_int(INTNO1);
	check_ercd(ercd, E_OK);

	ercd = prb_int(INTNO1);
	check_ercd(ercd, false);

	check_point(20);
	ercd = ena_int(INTNO1);
	check_ercd(ercd, E_OK);

	check_finish(21);
	check_assert(false);
}
