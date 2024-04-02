/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2018-2021 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: test_suspend1.c 1498 2021-03-27 09:45:22Z ertl-hiro $
 */

/* 
 *		強制待ち状態に関するテスト(1)
 *
 * 【テストの目的】
 *
 *  sus_tskとrsm_tskを網羅的にテストする．
 *
 * 【テスト項目】
 *
 *	(A) sus_tskのエラー検出
 *		(A-1) 非タスクコンテキストからの呼出し［NGKI1299］
 *		(A-2) CPUロック状態からの呼出し［NGKI1300］
 *		(A-3) tskidが有効範囲外［NGKI1302］
 *			(A-3-1) 小さすぎる
 *			(A-3-2) 大きすぎる
 *		(A-4) 対象タスクが休止状態［NGKI1305］
 *		(A-5) 対象タスクのタスク終了要求フラグがセットされている［NGKI3605］
 *		(A-6) 対象タスクが強制待ち状態（二重待ち状態を含む）［NGKI1306］
 *			(A-6-1) （狭義の）強制待ち状態
 *			(A-6-2) 二重待ち状態
 *		(A-7) ディスパッチ保留状態で，対象タスクが自タスク［NGKI1311］
 *			(A-7-1) 割込み優先度マスクが全解除でない状態
 *			(A-7-2) ディスパッチ禁止状態
 *	(B) sus_tskの正常処理
 *		(B-1) 対象タスクを実行できる状態から強制待ち状態に［NGKI1307］
 *			(B-1-1) 対象タスクが実行状態（自タスク）
 *			(B-1-2) 対象タスクが実行可能状態（他のタスク）
 *		(B-2) 対象タスクを待ち状態から二重待ち状態に［NGKI1308］
 *		(B-3) tskidにTSK_SELF（＝0）を指定［NGKI1310］
 *		(B-4) ディスパッチ保留状態で，対象タスクが自タスクでない［NGKI3604］
 *			(B-4-1) 割込み優先度マスクが全解除でない状態
 *			(B-4-2) ディスパッチ禁止状態
 *	(C) sus_tskの特殊ケース
 *		(C-1) ディスパッチ保留状態で，対象タスクが自タスクでない最高優
 *			  先順位のタスク
 *	(D) rsm_tskのエラー検出
 *		(D-1) 非タスクコンテキストからの呼出し［NGKI1313］
 *		(D-2) CPUロック状態からの呼出し［NGKI1314］
 *		(D-3) tskidが有効範囲外［NGKI1316］
 *			(D-3-1) 小さすぎる
 *			(D-3-2) 大きすぎる
 *		(D-4) 対象タスクが強制待ち状態（二重待ち状態を含む）でない［NGKI1319］
 *			(D-4-1) 対象タスクが休止状態
 *			(D-4-2) 対象タスクが実行状態
 *			(D-4-3) 対象タスクが実行可能状態
 *			(D-4-4) 対象タスクが待ち状態
 *	(E) rsm_tskの正常処理
 *		(E-1) 対象タスクを強制待ち状態から再開させる［NGKI1320］
 *			(E-1-1) （狭義の）強制待ち状態から実行状態に遷移
 *			(E-1-2) （狭義の）強制待ち状態から実行可能状態に遷移
 *			(E-1-3) 二重待ち状態から待ち状態に遷移
 *
 * 【使用リソース】
 *
 *	TASK1: 中優先度タスク，メインタスク，最初から起動
 *	TASK2: 高優先度タスク
 *	TASK3: 低優先度タスク
 *	ALM1:  アラームハンドラ
 *
 * 【テストシーケンス】
 *
 *	== TASK1（優先度：中）==
 *	1:	act_tsk(TASK3)
 *		loc_cpu()
 *		sus_tsk(TASK3) -> E_CTX				... (A-2)
 *		unl_cpu()
 *		sus_tsk(-1)	-> E_ID					... (A-3-1)
 *		sus_tsk(TNUM_TSKID+1) -> E_ID		... (A-3-2)
 *		sus_tsk(TASK2) -> E_OBJ				... (A-4)
 *		sus_tsk(TSK_SELF)					... (B-1-1)(B-3)
 *	== TASK3（優先度：低）==
 *	2:	ref_tsk(TASK1, &rtsk)
 *		assert(rtsk.tskstat == TTS_SUS)
 *		sus_tsk(TASK1) -> E_QOVR			... (A-6-1)
 *		loc_cpu()
 *		rsm_tsk(TASK3) -> E_CTX				... (D-2)
 *		unl_cpu()
 *		rsm_tsk(-1)	-> E_ID					... (D-3-1)
 *		rsm_tsk(TNUM_TSKID+1) -> E_ID		... (D-3-2)
 *		rsm_tsk(TASK1)						... (E-1-1)
 *	== TASK1（続き）==
 *	3:	sus_tsk(TASK3)						... (B-1-2)
 *		ref_tsk(TASK3, &rtsk)
 *		assert(rtsk.tskstat == TTS_SUS)
 *		rsm_tsk(TASK3)						... (E-1-2)
 *		ref_tsk(TASK3, &rtsk)
 *		assert(rtsk.tskstat == TTS_RDY)
 *		rsm_tsk(TASK2) -> E_OBJ				... (D-4-1)
 *		rsm_tsk(TASK1) -> E_OBJ				... (D-4-2)
 *		rsm_tsk(TASK3) -> E_OBJ				... (D-4-3)
 *		slp_tsk()
 *	== TASK3（続き）==
 *	4:	ref_tsk(TASK1, &rtsk)
 *		assert(rtsk.tskstat == TTS_WAI)
 *		rsm_tsk(TASK1) -> E_OBJ				... (D-4-4)
 *		sus_tsk(TASK1)						... (B-2)
 *		ref_tsk(TASK1, &rtsk)
 *		assert(rtsk.tskstat == TTS_WAS)
 *		sus_tsk(TASK1) -> E_QOVR			... (A-6-2)
 *		dis_ter()
 *		rsm_tsk(TASK1)						... (E-1-3)
 *		wup_tsk(TASK1)
 *	== TASK1（続き）==
 *	5:	chg_ipm(TMAX_INTPRI)
 *		sus_tsk(TASK1) -> E_CTX				... (A-7-1)
 *		chg_ipm(TIPM_ENAALL)
 *		dis_dsp()
 *		sus_tsk(TASK1) -> E_CTX				... (A-7-2)
 *		ena_dsp()
 *		ras_ter(TASK3)
 *		sus_tsk(TASK3) -> E_RASTER			... (A-5)
 *		chg_ipm(TMAX_INTPRI)
 *		act_tsk(TASK2)
 *		sus_tsk(TASK2)						... (B-4-1)(C-1)
 *		chg_ipm(TIPM_ENAALL)
 *		rsm_tsk(TASK2)
 *	== TASK2（優先度：高）==
 *	6:	slp_tsk()
 *	== TASK1（続き）==
 *	7:	dis_dsp()
 *		wup_tsk(TASK2)
 *		sus_tsk(TASK2)						... (B-4-2)(C-1)
 *		ena_dsp()
 *		rsm_tsk(TASK2)
 *	== TASK2（優先度：高）==
 *	8:	ext_tsk()
 *	== TASK1（続き）==
 *	9:	sta_alm(ALM1, TEST_TIME_CP)
 *		slp_tsk()
 *	== TASK3（続き）==
 *	10:	ena_ter()
 *	== ALM1 ==
 *	11:	sus_tsk(TASK1) -> E_CTX				... (A-1)
 *		rsm_tsk(TASK1) -> E_CTX				... (D-1)
 *		wup_tsk(TASK1)
 *		RETURN
 *	== TASK1（続き）==
 *	12: END
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

	check_point(11);
	ercd = sus_tsk(TASK1);
	check_ercd(ercd, E_CTX);

	ercd = rsm_tsk(TASK1);
	check_ercd(ercd, E_CTX);

	ercd = wup_tsk(TASK1);
	check_ercd(ercd, E_OK);

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
	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	ercd = loc_cpu();
	check_ercd(ercd, E_OK);

	ercd = sus_tsk(TASK3);
	check_ercd(ercd, E_CTX);

	ercd = unl_cpu();
	check_ercd(ercd, E_OK);

	ercd = sus_tsk(-1);
	check_ercd(ercd, E_ID);

	ercd = sus_tsk(TNUM_TSKID+1);
	check_ercd(ercd, E_ID);

	ercd = sus_tsk(TASK2);
	check_ercd(ercd, E_OBJ);

	ercd = sus_tsk(TSK_SELF);
	check_ercd(ercd, E_OK);

	check_point(3);
	ercd = sus_tsk(TASK3);
	check_ercd(ercd, E_OK);

	ercd = ref_tsk(TASK3, &rtsk);
	check_ercd(ercd, E_OK);

	check_assert(rtsk.tskstat == TTS_SUS);

	ercd = rsm_tsk(TASK3);
	check_ercd(ercd, E_OK);

	ercd = ref_tsk(TASK3, &rtsk);
	check_ercd(ercd, E_OK);

	check_assert(rtsk.tskstat == TTS_RDY);

	ercd = rsm_tsk(TASK2);
	check_ercd(ercd, E_OBJ);

	ercd = rsm_tsk(TASK1);
	check_ercd(ercd, E_OBJ);

	ercd = rsm_tsk(TASK3);
	check_ercd(ercd, E_OBJ);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(5);
	ercd = chg_ipm(TMAX_INTPRI);
	check_ercd(ercd, E_OK);

	ercd = sus_tsk(TASK1);
	check_ercd(ercd, E_CTX);

	ercd = chg_ipm(TIPM_ENAALL);
	check_ercd(ercd, E_OK);

	ercd = dis_dsp();
	check_ercd(ercd, E_OK);

	ercd = sus_tsk(TASK1);
	check_ercd(ercd, E_CTX);

	ercd = ena_dsp();
	check_ercd(ercd, E_OK);

	ercd = ras_ter(TASK3);
	check_ercd(ercd, E_OK);

	ercd = sus_tsk(TASK3);
	check_ercd(ercd, E_RASTER);

	ercd = chg_ipm(TMAX_INTPRI);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	ercd = sus_tsk(TASK2);
	check_ercd(ercd, E_OK);

	ercd = chg_ipm(TIPM_ENAALL);
	check_ercd(ercd, E_OK);

	ercd = rsm_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(7);
	ercd = dis_dsp();
	check_ercd(ercd, E_OK);

	ercd = wup_tsk(TASK2);
	check_ercd(ercd, E_OK);

	ercd = sus_tsk(TASK2);
	check_ercd(ercd, E_OK);

	ercd = ena_dsp();
	check_ercd(ercd, E_OK);

	ercd = rsm_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(9);
	ercd = sta_alm(ALM1, TEST_TIME_CP);
	check_ercd(ercd, E_OK);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_finish(12);
	check_assert(false);
}

void
task2(EXINF exinf)
{
	ER_UINT	ercd;

	check_point(6);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(8);
	ercd = ext_tsk();
	check_ercd(ercd, E_OK);

	check_assert(false);
}

void
task3(EXINF exinf)
{
	ER_UINT	ercd;
	T_RTSK	rtsk;

	check_point(2);
	ercd = ref_tsk(TASK1, &rtsk);
	check_ercd(ercd, E_OK);

	check_assert(rtsk.tskstat == TTS_SUS);

	ercd = sus_tsk(TASK1);
	check_ercd(ercd, E_QOVR);

	ercd = loc_cpu();
	check_ercd(ercd, E_OK);

	ercd = rsm_tsk(TASK3);
	check_ercd(ercd, E_CTX);

	ercd = unl_cpu();
	check_ercd(ercd, E_OK);

	ercd = rsm_tsk(-1);
	check_ercd(ercd, E_ID);

	ercd = rsm_tsk(TNUM_TSKID+1);
	check_ercd(ercd, E_ID);

	ercd = rsm_tsk(TASK1);
	check_ercd(ercd, E_OK);

	check_point(4);
	ercd = ref_tsk(TASK1, &rtsk);
	check_ercd(ercd, E_OK);

	check_assert(rtsk.tskstat == TTS_WAI);

	ercd = rsm_tsk(TASK1);
	check_ercd(ercd, E_OBJ);

	ercd = sus_tsk(TASK1);
	check_ercd(ercd, E_OK);

	ercd = ref_tsk(TASK1, &rtsk);
	check_ercd(ercd, E_OK);

	check_assert(rtsk.tskstat == TTS_WAS);

	ercd = sus_tsk(TASK1);
	check_ercd(ercd, E_QOVR);

	ercd = dis_ter();
	check_ercd(ercd, E_OK);

	ercd = rsm_tsk(TASK1);
	check_ercd(ercd, E_OK);

	ercd = wup_tsk(TASK1);
	check_ercd(ercd, E_OK);

	check_point(10);
	ercd = ena_ter();
	check_ercd(ercd, E_OK);

	check_assert(false);
}
