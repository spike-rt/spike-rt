/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2008-2019 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: test_sem1.c 1785 2023-01-15 11:44:24Z ertl-hiro $
 */

/* 
 *		セマフォ機能のテスト(1)
 *
 * 【テストの目的】
 *
 *  sig_sem（タスクコンテキストからの呼出し），wai_sem，CRE_SEMを網羅的
 *  にテストする（ただし，CRE_SEMのエラーのテストは除く）．
 *
 * 【テスト項目】
 *
 *	(A) sig_semの静的エラーのテスト
 *		(A-1) CPUロック状態からの呼出し［NGKI1501］
 *		(A-2) semidが不正（小さすぎる）［NGKI1502］
 *		(A-3) semidが不正（大きすぎる）［NGKI1502］
 *	(B) sig_semによりセマフォ待ち状態のタスクが待ち解除される［NGKI1505］
 *		＊この時，対象セマフォの資源数は変化しない［NGKI1506］
 *		＊待ち解除されたタスクにはE_OKが返る［NGKI1507］
 *		(B-1) 待ち解除されたタスクに切り換わる
 *		(B-2) ディスパッチ保留状態で，切り換わらない
 *		(B-3) 待ち解除されたタスクが強制待ち状態で，切り換わらない
 *		(B-4) 待ち解除されたタスクが優先度が低く，切り換わらない
 *	(C) sig_semによりセマフォの資源数が1増える［NGKI1508］
 *		(C-1) セマフォの資源数が0から1になる
 *		(C-2) セマフォの資源数が1から2になる
 *	(D) sig_semがE_QOVRエラーとなる［NGKI1509］
 *		(D-1) セマフォの最大資源数が1の時
 *		(D-2) セマフォの最大資源数が2の時
 *	(E) wai_semの静的エラーのテスト
 *		(E-1) 非タスクコンテキストからの呼出し［NGKI1513］
 *		(E-2) CPUロック状態からの呼出し［NGKI1514］
 *		(E-3) ディスパッチ禁止状態からの呼出し［NGKI1515］
 *		(E-4) 割込み優先度マスク全解除でない状態からの呼出し［NGKI1515］
 *		(E-5) semidが不正（小さすぎる）［NGKI1517］
 *		(E-6) semidが不正（大きすぎる）［NGKI1517］
 *	(F) wai_semによりセマフォの資源数が1減る［NGKI1524］
 *		(F-1) セマフォの資源数が1から0になる
 *		(F-2) セマフォの資源数が2から1になる
 *	(G) wai_semによりセマフォ待ち状態になる［NGKI1525］
 *		(G-1) TA_TNULL属性のセマフォで，待っているタスクがなかった場合
 *		(G-2) TA_TNULL属性のセマフォで，待っているタスクがあった場合
 *		(G-3) TA_TPRI属性のセマフォで，待っているタスクがなかった場合
 *		(G-4) TA_TPRI属性のセマフォで，優先度が高いタスクが待っている場合
 *		(G-5) TA_TPRI属性のセマフォで，優先度が同じタスクが待っている場合
 *		(G-6) TA_TPRI属性のセマフォで，優先度が低いタスクが待っている場合
 *	(H) セマフォ待ち状態が強制解除される［NGKI1522］
 *	(I) セマフォ待ち状態の間にセマフォが初期化される［NGKI1523］
 *	(J) 生成されたセマフォの資源数は初期資源数に，待ち行列は空の状態に
 *	    初期化される［NGKI1464］
 *		(J-1) セマフォの初期資源数が0
 *		(J-2) セマフォの初期資源数が1
 *		(J-3) セマフォの初期資源数が2
 *	(K) タスクに終了要求があると待ち状態にならない［NGKI3457］
 *		(K-1) タスク終了要求フラグとタスク終了禁止フラグがセットされた
 *		      タスクが，wai_semを呼んで待ち状態になろうとした場合
 *		(K-2) タスク終了禁止フラグがセットされておらず，セマフォ待ち状
 *		      態になっているタスクに，終了要求が出された場合
 *		(K-3) タスク終了禁止フラグがセットされ，セマフォ待ち状態になっ
 *		      ているタスクに，終了要求が出された場合
 *
 * 【使用リソース】
 *
 *	TASK1: 中優先度タスク，TA_ACT属性
 *	TASK2: 高優先度タスク
 *	TASK3: 低優先度タスク
 *	TASK4: 中優先度タスク
 *	TASK5: 中優先度タスク
 *	ALM1:  アラームハンドラ
 *  SEM1:  TA_NULL属性，初期資源数1，最大資源数1
 *  SEM2:  TA_NULL属性，初期資源数2，最大資源数2
 *  SEM3:  TA_TPRI属性，初期資源数0，最大資源数1
 *
 * 【テストシーケンス】
 *
 *	== TASK1（優先度：中）==
 *	1:	ref_sem(SEM1, &rsem)
 *		assert(rsem.wtskid == TSK_NONE)
 *		assert(rsem.semcnt == 1)			... (J-2)
 *		ref_sem(SEM2, &rsem)
 *		assert(rsem.wtskid == TSK_NONE)
 *		assert(rsem.semcnt == 2)			... (J-3)
 *		ref_sem(SEM3, &rsem)
 *		assert(rsem.wtskid == TSK_NONE)
 *		assert(rsem.semcnt == 0)			... (J-1)
 *	2:	loc_cpu()
 *		sig_sem(SEM1) -> E_CTX				... (A-1)
 *		wai_sem(SEM1) -> E_CTX				... (E-2)
 *		unl_cpu()
 *	3:	dis_dsp()
 *		wai_sem(SEM1) -> E_CTX				... (E-3)
 *		ena_dsp()
 *	4:	chg_ipm(TMAX_INTPRI)
 *		wai_sem(SEM1) -> E_CTX				... (E-4)
 *		chg_ipm(TIPM_ENAALL)
 *		sig_sem(0) -> E_ID					... (A-2)
 *		wai_sem(0) -> E_ID					... (E-5)
 *		sig_sem(TNUM_SEMID+1) -> E_ID		... (A-3)
 *		wai_sem(TNUM_SEMID+1) -> E_ID		... (E-6)
 *	5:	act_tsk(TASK3)
 *	6:	slp_tsk()
 *	== TASK3（優先度：低）==
 *	7:	wai_sem(SEM1)						... (F-1)
 *		ref_sem(SEM1, &rsem)
 *		assert(rsem.wtskid == TSK_NONE)
 *		assert(rsem.semcnt == 0)
 *	8:	sta_alm(ALM1, TEST_TIME_CP) ... ALM1が実行開始するまで
 *	9:	wai_sem(SEM1)						... (G-1)
 *	== ALM1 ==
 *	10:	wai_sem(SEM1) -> E_CTX				... (E-1)
 *		wup_tsk(TASK1)
 *	11:	RETURN
 *	== TASK1（続き）==
 *	12:	act_tsk(TASK2)
 *	== TASK2（優先度：高）==
 *	13:	wai_sem(SEM1)						... (G-2)
 *	== TASK1（続き）==
 *	14:	ref_sem(SEM1, &rsem)
 *		assert(rsem.wtskid == TASK3)
 *		assert(rsem.semcnt == 0)
 *	15:	sig_sem(SEM1)						... (B-4)
 *		ref_sem(SEM1, &rsem)
 *		assert(rsem.wtskid == TASK2)
 *		assert(rsem.semcnt == 0)
 *	16:	sig_sem(SEM1)						... (B-1)
 *	== TASK2（続き）==
 *	17:	ref_sem(SEM1, &rsem)
 *		assert(rsem.wtskid == TSK_NONE)
 *		assert(rsem.semcnt == 0)
 *	18:	wai_sem(SEM1)						... (G-1)
 *	== TASK1（続き）==
 *	19:	dis_dsp()
 *		sig_sem(SEM1)						... (B-2)
 *		ref_sem(SEM1, &rsem)
 *		assert(rsem.wtskid == TSK_NONE)
 *		assert(rsem.semcnt == 0)
 *	20:	ena_dsp()
 *	== TASK2（続き）==
 *	21:	wai_sem(SEM1)						... (G-1)
 *	== TASK1（続き）==
 *	22:	sus_tsk(TASK2)
 *		sig_sem(SEM1)						... (B-3)
 *		ref_sem(SEM1, &rsem)
 *		assert(rsem.wtskid == TSK_NONE)
 *		assert(rsem.semcnt == 0)
 *	23:	sig_sem(SEM1)						... (C-1)
 *		sig_sem(SEM1) -> E_QOVR				... (D-1)
 *		ref_sem(SEM1, &rsem)
 *		assert(rsem.wtskid == TSK_NONE)
 *		assert(rsem.semcnt == 1)
 *	24:	rsm_tsk(TASK2)
 *	== TASK2（続き）==
 *	25:	wai_sem(SEM2)						... (F-2)
 *		ref_sem(SEM2, &rsem)
 *		assert(rsem.wtskid == TSK_NONE)
 *		assert(rsem.semcnt == 1)
 *		wai_sem(SEM2)						... (F-1)
 *	26:	wai_sem(SEM2)						... (G-1)
 *	== TASK1（続き）==
 *	27:	sig_sem(SEM2)						... (B-1)
 *	== TASK2（続き）==
 *	28:	wai_sem(SEM3)						... (G-3)
 *	== TASK1（続き）==
 *	29:	sig_sem(SEM2)						... (C-1)
 *		sig_sem(SEM2)						... (C-2)
 *  	ref_sem(SEM2, &rsem)
 *		assert(rsem.wtskid == TSK_NONE)
 *		assert(rsem.semcnt == 2)
 *	30:	sig_sem(SEM2) -> E_QOVR				... (D-2)
 *  	ref_sem(SEM2, &rsem)
 *		assert(rsem.wtskid == TSK_NONE)
 *		assert(rsem.semcnt == 2)
 *	31:	tslp_tsk(TEST_TIME_CP) -> E_TMOUT ... TASK1が実行再開するまで
 *	== TASK3（続き）==
 *	32:	wai_sem(SEM3)						... (G-4)
 *	== TASK1（続き）==
 *	33:	act_tsk(TASK4)
 *		act_tsk(TASK5)
 *	34:	rot_rdq(TPRI_SELF)
 *	== TASK4（優先度：中）==
 *	35:	dis_ter()
 *	36:	wai_sem(SEM3)						... (G-6)
 *	== TASK5（優先度：中）==
 *	37:	wai_sem(SEM3)						... (G-5)
 *	== TASK1（続き）==
 *	38:	sig_sem(SEM3)						... (B-1)
 *	== TASK2（続き）==
 *	39:	wai_sem(SEM1)
 *	40:	wai_sem(SEM1) -> E_RLWAI
 *	== TASK1（続き）==
 *	41:	sig_sem(SEM3)						... (B-4)
 *		ras_ter(TASK4)
 *	42:	tslp_tsk(TEST_TIME_CP) -> E_TMOUT ... TASK1が実行再開するまで
 *	== TASK4（続き）==
 *	43:	wai_sem(SEM1) -> E_RASTER			... (K-1)
 *	44:	ena_ter() -> noreturn
 *	== TASK1（続き）==
 *	45:	sig_sem(SEM3)						... (B-4)
 *	46:	tslp_tsk(TEST_TIME_CP) -> E_TMOUT ... TASK1が実行再開するまで
 *	== TASK5（続き）==
 *	47:	wai_sem(SEM3) -> noreturn			... (K-2)
 *	== TASK1（続き）==
 *	48:	ras_ter(TASK5)
 *		sig_sem(SEM3)						... (B-4)
 *	49:	tslp_tsk(TEST_TIME_CP) -> E_TMOUT ... TASK1が実行再開するまで
 *	== TASK3（続き）==
 *	50:	dis_ter()
 *	51:	wai_sem(SEM3) -> E_RASTER			... (K-3)
 *	== TASK1（続き）==
 *	52:	ras_ter(TASK3)
 *	53:	tslp_tsk(TEST_TIME_CP) -> E_TMOUT ... TASK1が実行再開するまで
 *	== TASK3（続き）==
 *	54:	ena_ter() -> noreturn
 *	== TASK1（続き）==
 *	55:	rel_wai(TASK2)						... (H)
 *	== TASK2（続き）==
 *	56:	wai_sem(SEM1) -> E_DLT
 *	== TASK1（続き）==
 *	57:	ini_sem(SEM1)						... (I)
 *	== TASK2（続き）==
 *	58:	ext_tsk() -> noreturn
 *	== TASK1（続き）==
 *	59:	END
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

	check_point(10);
	ercd = wai_sem(SEM1);
	check_ercd(ercd, E_CTX);

	ercd = wup_tsk(TASK1);
	check_ercd(ercd, E_OK);

	check_point(11);
	return;

	check_assert(false);
}

void
task1(EXINF exinf)
{
	ER_UINT	ercd;
	T_RSEM	rsem;

	test_start(__FILE__);

	check_point(1);
	ercd = ref_sem(SEM1, &rsem);
	check_ercd(ercd, E_OK);

	check_assert(rsem.wtskid == TSK_NONE);

	check_assert(rsem.semcnt == 1);

	ercd = ref_sem(SEM2, &rsem);
	check_ercd(ercd, E_OK);

	check_assert(rsem.wtskid == TSK_NONE);

	check_assert(rsem.semcnt == 2);

	ercd = ref_sem(SEM3, &rsem);
	check_ercd(ercd, E_OK);

	check_assert(rsem.wtskid == TSK_NONE);

	check_assert(rsem.semcnt == 0);

	check_point(2);
	ercd = loc_cpu();
	check_ercd(ercd, E_OK);

	ercd = sig_sem(SEM1);
	check_ercd(ercd, E_CTX);

	ercd = wai_sem(SEM1);
	check_ercd(ercd, E_CTX);

	ercd = unl_cpu();
	check_ercd(ercd, E_OK);

	check_point(3);
	ercd = dis_dsp();
	check_ercd(ercd, E_OK);

	ercd = wai_sem(SEM1);
	check_ercd(ercd, E_CTX);

	ercd = ena_dsp();
	check_ercd(ercd, E_OK);

	check_point(4);
	ercd = chg_ipm(TMAX_INTPRI);
	check_ercd(ercd, E_OK);

	ercd = wai_sem(SEM1);
	check_ercd(ercd, E_CTX);

	ercd = chg_ipm(TIPM_ENAALL);
	check_ercd(ercd, E_OK);

	ercd = sig_sem(0);
	check_ercd(ercd, E_ID);

	ercd = wai_sem(0);
	check_ercd(ercd, E_ID);

	ercd = sig_sem(TNUM_SEMID+1);
	check_ercd(ercd, E_ID);

	ercd = wai_sem(TNUM_SEMID+1);
	check_ercd(ercd, E_ID);

	check_point(5);
	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	check_point(6);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(12);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(14);
	ercd = ref_sem(SEM1, &rsem);
	check_ercd(ercd, E_OK);

	check_assert(rsem.wtskid == TASK3);

	check_assert(rsem.semcnt == 0);

	check_point(15);
	ercd = sig_sem(SEM1);
	check_ercd(ercd, E_OK);

	ercd = ref_sem(SEM1, &rsem);
	check_ercd(ercd, E_OK);

	check_assert(rsem.wtskid == TASK2);

	check_assert(rsem.semcnt == 0);

	check_point(16);
	ercd = sig_sem(SEM1);
	check_ercd(ercd, E_OK);

	check_point(19);
	ercd = dis_dsp();
	check_ercd(ercd, E_OK);

	ercd = sig_sem(SEM1);
	check_ercd(ercd, E_OK);

	ercd = ref_sem(SEM1, &rsem);
	check_ercd(ercd, E_OK);

	check_assert(rsem.wtskid == TSK_NONE);

	check_assert(rsem.semcnt == 0);

	check_point(20);
	ercd = ena_dsp();
	check_ercd(ercd, E_OK);

	check_point(22);
	ercd = sus_tsk(TASK2);
	check_ercd(ercd, E_OK);

	ercd = sig_sem(SEM1);
	check_ercd(ercd, E_OK);

	ercd = ref_sem(SEM1, &rsem);
	check_ercd(ercd, E_OK);

	check_assert(rsem.wtskid == TSK_NONE);

	check_assert(rsem.semcnt == 0);

	check_point(23);
	ercd = sig_sem(SEM1);
	check_ercd(ercd, E_OK);

	ercd = sig_sem(SEM1);
	check_ercd(ercd, E_QOVR);

	ercd = ref_sem(SEM1, &rsem);
	check_ercd(ercd, E_OK);

	check_assert(rsem.wtskid == TSK_NONE);

	check_assert(rsem.semcnt == 1);

	check_point(24);
	ercd = rsm_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(27);
	ercd = sig_sem(SEM2);
	check_ercd(ercd, E_OK);

	check_point(29);
	ercd = sig_sem(SEM2);
	check_ercd(ercd, E_OK);

	ercd = sig_sem(SEM2);
	check_ercd(ercd, E_OK);

	ercd = ref_sem(SEM2, &rsem);
	check_ercd(ercd, E_OK);

	check_assert(rsem.wtskid == TSK_NONE);

	check_assert(rsem.semcnt == 2);

	check_point(30);
	ercd = sig_sem(SEM2);
	check_ercd(ercd, E_QOVR);

	ercd = ref_sem(SEM2, &rsem);
	check_ercd(ercd, E_OK);

	check_assert(rsem.wtskid == TSK_NONE);

	check_assert(rsem.semcnt == 2);

	check_point(31);
	ercd = tslp_tsk(TEST_TIME_CP);
	check_ercd(ercd, E_TMOUT);

	check_point(33);
	ercd = act_tsk(TASK4);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK5);
	check_ercd(ercd, E_OK);

	check_point(34);
	ercd = rot_rdq(TPRI_SELF);
	check_ercd(ercd, E_OK);

	check_point(38);
	ercd = sig_sem(SEM3);
	check_ercd(ercd, E_OK);

	check_point(41);
	ercd = sig_sem(SEM3);
	check_ercd(ercd, E_OK);

	ercd = ras_ter(TASK4);
	check_ercd(ercd, E_OK);

	check_point(42);
	ercd = tslp_tsk(TEST_TIME_CP);
	check_ercd(ercd, E_TMOUT);

	check_point(45);
	ercd = sig_sem(SEM3);
	check_ercd(ercd, E_OK);

	check_point(46);
	ercd = tslp_tsk(TEST_TIME_CP);
	check_ercd(ercd, E_TMOUT);

	check_point(48);
	ercd = ras_ter(TASK5);
	check_ercd(ercd, E_OK);

	ercd = sig_sem(SEM3);
	check_ercd(ercd, E_OK);

	check_point(49);
	ercd = tslp_tsk(TEST_TIME_CP);
	check_ercd(ercd, E_TMOUT);

	check_point(52);
	ercd = ras_ter(TASK3);
	check_ercd(ercd, E_OK);

	check_point(53);
	ercd = tslp_tsk(TEST_TIME_CP);
	check_ercd(ercd, E_TMOUT);

	check_point(55);
	ercd = rel_wai(TASK2);
	check_ercd(ercd, E_OK);

	check_point(57);
	ercd = ini_sem(SEM1);
	check_ercd(ercd, E_OK);

	check_finish(59);
	check_assert(false);
}

void
task2(EXINF exinf)
{
	ER_UINT	ercd;
	T_RSEM	rsem;

	check_point(13);
	ercd = wai_sem(SEM1);
	check_ercd(ercd, E_OK);

	check_point(17);
	ercd = ref_sem(SEM1, &rsem);
	check_ercd(ercd, E_OK);

	check_assert(rsem.wtskid == TSK_NONE);

	check_assert(rsem.semcnt == 0);

	check_point(18);
	ercd = wai_sem(SEM1);
	check_ercd(ercd, E_OK);

	check_point(21);
	ercd = wai_sem(SEM1);
	check_ercd(ercd, E_OK);

	check_point(25);
	ercd = wai_sem(SEM2);
	check_ercd(ercd, E_OK);

	ercd = ref_sem(SEM2, &rsem);
	check_ercd(ercd, E_OK);

	check_assert(rsem.wtskid == TSK_NONE);

	check_assert(rsem.semcnt == 1);

	ercd = wai_sem(SEM2);
	check_ercd(ercd, E_OK);

	check_point(26);
	ercd = wai_sem(SEM2);
	check_ercd(ercd, E_OK);

	check_point(28);
	ercd = wai_sem(SEM3);
	check_ercd(ercd, E_OK);

	check_point(39);
	ercd = wai_sem(SEM1);
	check_ercd(ercd, E_OK);

	check_point(40);
	ercd = wai_sem(SEM1);
	check_ercd(ercd, E_RLWAI);

	check_point(56);
	ercd = wai_sem(SEM1);
	check_ercd(ercd, E_DLT);

	check_point(58);
	ercd = ext_tsk();

	check_assert(false);
}

void
task3(EXINF exinf)
{
	ER_UINT	ercd;
	T_RSEM	rsem;

	check_point(7);
	ercd = wai_sem(SEM1);
	check_ercd(ercd, E_OK);

	ercd = ref_sem(SEM1, &rsem);
	check_ercd(ercd, E_OK);

	check_assert(rsem.wtskid == TSK_NONE);

	check_assert(rsem.semcnt == 0);

	check_point(8);
	ercd = sta_alm(ALM1, TEST_TIME_CP);
	check_ercd(ercd, E_OK);

	check_point(9);
	ercd = wai_sem(SEM1);
	check_ercd(ercd, E_OK);

	check_point(32);
	ercd = wai_sem(SEM3);
	check_ercd(ercd, E_OK);

	check_point(50);
	ercd = dis_ter();
	check_ercd(ercd, E_OK);

	check_point(51);
	ercd = wai_sem(SEM3);
	check_ercd(ercd, E_RASTER);

	check_point(54);
	ercd = ena_ter();

	check_assert(false);
}

void
task4(EXINF exinf)
{
	ER_UINT	ercd;

	check_point(35);
	ercd = dis_ter();
	check_ercd(ercd, E_OK);

	check_point(36);
	ercd = wai_sem(SEM3);
	check_ercd(ercd, E_OK);

	check_point(43);
	ercd = wai_sem(SEM1);
	check_ercd(ercd, E_RASTER);

	check_point(44);
	ercd = ena_ter();

	check_assert(false);
}

void
task5(EXINF exinf)
{
	ER_UINT	ercd;

	check_point(37);
	ercd = wai_sem(SEM3);
	check_ercd(ercd, E_OK);

	check_point(47);
	ercd = wai_sem(SEM3);

	check_assert(false);
}
