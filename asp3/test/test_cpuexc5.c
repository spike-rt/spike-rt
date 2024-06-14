/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2007-2022 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: test_cpuexc5.c 1535 2022-05-18 14:33:59Z ertl-hiro $
 */

/* 
 *		CPU例外処理のテスト(5)
 *
 * 【テストの目的】
 *
 *  タスクコンテキスト，割込ロック解除状態，CPUロック状態で発生したCPU
 *  例外におけるシステム状態のテスト．割込み優先度マスク全解除状態，ディ
 *  スパッチ禁止状態でテストする．
 *
 * 【テスト項目】
 *
 *  いずれも，タスクコンテキスト，割込ロック解除状態，CPUロック状態で発
 *  生したCPU例外において，
 *
 *	(A) CPU例外ハンドラ実行開始時にCPUロックフラグが変化しないこと
 *	(B) CPU例外ハンドラ実行開始時に割込み優先度マスクが変化しないこと
 *		！CPU例外ハンドラ中で割込み優先度マスクを読めないため，テストで
 *		　きない．
 *	(C) CPU例外ハンドラ実行開始時にディスパッチ禁止フラグが変化しないこと
 *	(D) CPU例外ハンドラリターン時にCPUロックフラグが元に戻ること
 *		！CPU例外ハンドラからリターンできる場合のみテストする．
 *	(E) CPU例外ハンドラリターン時に割込み優先度マスクが元に戻ること
 *		！CPU例外ハンドラからリターンできる場合のみテストする．
 *	(F) CPU例外ハンドラリターン時にディスパッチ禁止フラグが変化しないこと
 *		！CPU例外ハンドラからリターンできる場合のみテストする．
 *	(G) xsns_dpnがtrueを返すこと
 *
 * 【使用リソース】
 *
 *	TASK1: 中優先度タスク，TA_ACT属性
 *	CPUEXC: CPU例外ハンドラ
 *
 * 【テストシーケンス】
 *
 *	== TASK1 ==
 *	1:	state(false, false, false, false, false)
 *		ipm(TIPM_ENAALL)
 *		dis_dsp()
 *		loc_cpu()
 *	2:	state(false, true, true, true, false)
 *		DO(RAISE_CPU_EXCEPTION)
 *	== CPUEXC ==
 *	3:	state(true, true, true, true, false)			... (A)(C)
 *		assert(xsns_dpn(p_excinf) == true)				... (G)
 *	4:	HOOK(HOOK_POINT(%d))
 *				... CPU例外ハンドラからリターンできない場合はここで終了
 *		RETURN
 *	== TASK1（続き）==
 *	5:	state(false, true, true, true, false)			... (D)(E)(F)
 *	6:	END
 */

#include <kernel.h>
#include <t_syslog.h>
#include "syssvc/test_svc.h"
#include "kernel_cfg.h"
#include "test_cpuexc.h"

void
task2(EXINF exinf)
{
	check_assert(false);
}

void
alarm1_handler(EXINF exinf)
{
	check_assert(false);
}

#ifdef PREPARE_RETURN_CPUEXC
#define HOOK_POINT(count)	do {							\
								PREPARE_RETURN_CPUEXC;		\
								check_point(count);			\
							} while (false)
#else /* PREPARE_RETURN_CPUEXC */
#define HOOK_POINT(count)	check_finish(count)
#endif /* PREPARE_RETURN_CPUEXC */

/* DO NOT DELETE THIS LINE -- gentest depends on it. */

void
cpuexc_handler(void *p_excinf)
{

	check_point(3);
	check_state(true, true, true, true, false);

	check_assert(xsns_dpn(p_excinf) == true);

	HOOK_POINT(4);
	return;

	check_assert(false);
}

void
task1(EXINF exinf)
{
	ER_UINT	ercd;

	test_start(__FILE__);

	check_point(1);
	check_state(false, false, false, false, false);

	check_ipm(TIPM_ENAALL);

	ercd = dis_dsp();
	check_ercd(ercd, E_OK);

	ercd = loc_cpu();
	check_ercd(ercd, E_OK);

	check_point(2);
	check_state(false, true, true, true, false);

	RAISE_CPU_EXCEPTION;

	check_point(5);
	check_state(false, true, true, true, false);

	check_finish(6);
	check_assert(false);
}
