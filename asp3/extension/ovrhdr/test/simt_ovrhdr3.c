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
 *  $Id: simt_ovrhdr3.c 1437 2020-05-20 12:12:16Z ertl-hiro $
 */

/*
 *		オーバランハンドラ機能のテスト(3)
 *
 * 【テストの目的】
 *
 *	機能テストでテストできない精密な時間制御を必要とするテスト項目を，
 *	タイマドライバシミュレータを用いてテストする．
 *
 * 【テスト項目】
 *
 *	test_ovrhdr.txtを参照すること．
 *
 * 【使用リソース】
 *
 *	TASK1: 中優先度タスク，メインタスク，最初から起動
 *	TASK2: 高優先度タスク
 *	ALM1:  アラームハンドラ
 *	CPUEXC: CPU例外ハンドラ
 *	OVR:   オーバランハンドラ
 *
 * 【テストシーケンス】
 *
 *	== TASK1（優先度：中）==
 *	1:	sta_ovr(TSK_SELF, 500U)
 *		ref_ovr(TSK_SELF, &rovr)
 *		assert(rovr.ovrstat == TOVR_STA)
 *		assert(rovr.leftotm == 500U)
 *		DO(simtim_advance(100U))
 *	2:	ref_ovr(TSK_SELF, &rovr)							...［NGKI2588_T1］
 *		assert(rovr.ovrstat == TOVR_STA)					...［NGKI2590_T1］
 *		assert(rovr.leftotm == 400U)
 *		act_tsk(TASK2)
 *	== TASK2（優先度：高）==
 *	3:	DO(simtim_advance(100U))
 *		ext_tsk()
 *	== TASK1（続き）==
 *	4:	ref_ovr(TSK_SELF, &rovr)							...［NGKI2590_T3］
 *		assert(rovr.ovrstat == TOVR_STA)
 *		assert(rovr.leftotm == 400U)
 *	5:	sta_alm(ALM1, 100U)
 *		DO(simtim_advance(200U))
 *	== ALM1 ==												// 時刻が10進む
 *		// TASK1の実行途中でALM1が動作する
 *	6:	DO(simtim_advance(50U))
 *		RETURN
 *	== TASK1（続き）==
 *	7:	ref_ovr(TSK_SELF, &rovr)							...［NGKI2591_T1］
 *		assert(rovr.ovrstat == TOVR_STA)
 *		assert(rovr.leftotm == 200U)
 *		DO(simtim_advance(100U))
 *		DO(RAISE_CPU_EXCEPTION)
 *	== CPUEXC ==
 *	8:	DO(simtim_advance(50U))
 *		RETURN
 *	== TASK1（続き）==
 *	9:	ref_ovr(TSK_SELF, &rovr)							...［NGKI2591_T2］
 *		assert(rovr.ovrstat == TOVR_STA)
 *		assert(rovr.leftotm == 100U)
 *		DO(simtim_advance(99U))
 *	10:	DO(simtim_advance(1U))
 *	== OVR ==												// 時刻が10進む
 *	11:	sta_ovr(TASK1, 500U)								...［NGKI2589_T1］
 *		DO(simtim_advance(50U))
 *		RETURN
 *	== TASK1（続き）==
 *	12:	ref_ovr(TSK_SELF, &rovr)
 *		assert(rovr.ovrstat == TOVR_STA)
 *		assert(rovr.leftotm == 500U)
 *	13:	END
 */

#include <kernel.h>
#include <t_syslog.h>
#include "syssvc/test_svc.h"
#include "arch/simtimer/sim_timer_cntl.h"
#include "kernel_cfg.h"
#include "simt_ovrhdr3.h"

#ifndef HRT_CONFIG1
#error Compiler option "-DHRT_CONFIG1" is missing.
#endif /* HRT_CONFIG1 */

/* DO NOT DELETE THIS LINE -- gentest depends on it. */

void
alarm1_handler(EXINF exinf)
{

	check_point(6);
	simtim_advance(50U);

	return;

	check_point(0);
}

void
cpuexc_handler(void *p_excinf)
{

	check_point(8);
	simtim_advance(50U);

	return;

	check_point(0);
}

void
overrun_handler(ID tskid, EXINF exinf)
{
	ER_UINT	ercd;

	check_point(11);
	ercd = sta_ovr(TASK1, 500U);
	check_ercd(ercd, E_OK);

	simtim_advance(50U);

	return;

	check_point(0);
}

void
task1(EXINF exinf)
{
	ER_UINT	ercd;
	T_ROVR	rovr;

	test_start(__FILE__);

	check_point(1);
	ercd = sta_ovr(TSK_SELF, 500U);
	check_ercd(ercd, E_OK);

	ercd = ref_ovr(TSK_SELF, &rovr);
	check_ercd(ercd, E_OK);

	check_assert(rovr.ovrstat == TOVR_STA);

	check_assert(rovr.leftotm == 500U);

	simtim_advance(100U);

	check_point(2);
	ercd = ref_ovr(TSK_SELF, &rovr);
	check_ercd(ercd, E_OK);

	check_assert(rovr.ovrstat == TOVR_STA);

	check_assert(rovr.leftotm == 400U);

	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(4);
	ercd = ref_ovr(TSK_SELF, &rovr);
	check_ercd(ercd, E_OK);

	check_assert(rovr.ovrstat == TOVR_STA);

	check_assert(rovr.leftotm == 400U);

	check_point(5);
	ercd = sta_alm(ALM1, 100U);
	check_ercd(ercd, E_OK);

	simtim_advance(200U);

	check_point(7);
	ercd = ref_ovr(TSK_SELF, &rovr);
	check_ercd(ercd, E_OK);

	check_assert(rovr.ovrstat == TOVR_STA);

	check_assert(rovr.leftotm == 200U);

	simtim_advance(100U);

	RAISE_CPU_EXCEPTION;

	check_point(9);
	ercd = ref_ovr(TSK_SELF, &rovr);
	check_ercd(ercd, E_OK);

	check_assert(rovr.ovrstat == TOVR_STA);

	check_assert(rovr.leftotm == 100U);

	simtim_advance(99U);

	check_point(10);
	simtim_advance(1U);

	check_point(12);
	ercd = ref_ovr(TSK_SELF, &rovr);
	check_ercd(ercd, E_OK);

	check_assert(rovr.ovrstat == TOVR_STA);

	check_assert(rovr.leftotm == 500U);

	check_finish(13);
	check_point(0);
}

void
task2(EXINF exinf)
{
	ER_UINT	ercd;

	check_point(3);
	simtim_advance(100U);

	ercd = ext_tsk();
	check_ercd(ercd, E_OK);

	check_point(0);
}
