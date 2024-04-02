/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2015-2023 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: test_mpf1.c 1790 2023-01-18 06:07:25Z ertl-hiro $
 */

/* 
 *		固定長メモリプール機能のテスト(1)
 *
 * 【テストの目的】
 *
 *	get_mpfの振舞いのテスト［NGKI2287］
 *	  ・get_mpfに関するすべての要求をテストする．
 *	  ・get_mpfq関数のC1カバレッジを達成する．
 *	get_mpfが呼び出すデータキュー機能内部関数のテスト
 *	  ・get_mpf_block関数のC1カバレッジを達成する．
 *	rel_mpfの振舞いのテスト［NGKI2304］
 *	  ・rel_mpfに関するすべての要求をテストする．
 *	  ・rel_mpf関数のC1カバレッジを達成する．
 *
 * 【テスト項目】
 *
 *	(A) get_mpfのエラー検出
 *		(A-1) 非タスクコンテキストからの呼出し［NGKI2290］
 *		(A-2) CPUロック状態からの呼出し［NGKI2291］
 *		(A-3) ディスパッチ保留状態からの呼出し［NGKI2292］
 *			(A-3-1) ディスパッチ禁止状態
 *			(A-3-2) 割込み優先度マスク全解除でない状態
 *		(A-4) mpfidが不正［NGKI2294］
 *			(A-4-1) 小さすぎる
 *			(A-4-2) 大きすぎる
 *		(A-5) 待ち状態の強制解除［NGKI2300］
 *		(A-6) タスクの終了要求［NGKI3466］
 *			(A-6-1) 呼び出し時点で終了要求フラグがセット
 *			(A-6-2) 待ち状態になった後に終了要求
 *		(A-7) 待ちオブジェクトの削除または再初期化［NGKI2301］
 *	(B) get_mpfの正常系処理
 *		(B-1) 固定長メモリブロックが割り付けられる［NGKI2302］
 *		(B-2) 待ち行列につながれる［NGKI2303］
 *	(C) get_mpfのC1カバレッジ
 *	  ※ここまでのテストで有効なもの
 *			(A-1)(A-2)(A-3)(A-4-1)(A-4-2)(A-6-1)(B-1)(B-2)
 *	  ※(B-2)の分解が必要
 *		(C-1) E_OKを返す
 *		(C-2) エラーコードを返す．*p_blkが書き変わらない
 *	(D) get_mpf_blockのC1カバレッジ
 *	  ※ここまでのテストで有効なもの
 *			(B-1)
 *	  ※(B-1)の分解が必要
 *		(D-1) 未割当てブロックがある
 *		(D-2) 未割当てブロックがない
 *	(E) rel_mpfのエラー検出
 *		(E-1) 非タスクコンテキストからの呼出し［NGKI2305］
 *		(E-2) CPUロック状態からの呼出し［NGKI2306］
 *		(E-3) mpfidが不正［NGKI2307］
 *			(E-3-1) 小さすぎる
 *			(E-3-2) 大きすぎる
 *		(E-4) blkが固定長メモリブロックの先頭番地でない［NGKI2313］
 *			(E-4-1) 固定長メモリプール領域に含まれない（番地が小さい）
 *			(E-4-2) 固定長メモリプール領域に含まれない（番地が大きい）
 *			(E-4-3) 固定長メモリブロックの途中の番地
 *			(E-4-4) 割り当てられていない固定長メモリブロック
 *	(F) rel_mpfの正常系処理
 *		(F-1) 待ち行列の先頭のタスクが，blkで指定した固定長メモリブロッ
 *			  クを獲得［NGKI2310］
 *		(F-2) 待ち行列の先頭のタスクが，待ち解除される［NGKI2310］
 *		(F-3) 待ち解除されたタスクにE_OKが返る［NGKI2311］
 *		(F-4) 固定長メモリブロックが返却される［NGKI2313］
 *	(G) rel_mpfのC1カバレッジ
 *	  ※ここまでのテストで有効なもの
 *			(E-1)(E-2)(E-3-1)(E-3-2)(E-4-1)(E-4-2)(E-4-3)(E-4-4)
 *			(F-1)(F-2)(F-3)(F-4)
 *	  ※(E-4-4)の分解が必要
 *		(G-1) 一度も割り当てられていない固定長メモリブロック
 *		(G-2) 返却された固定長メモリブロック
 *	  ※(F-2)の分解が必要
 *		(G-3) タスク切換えが発生
 *		(G-4) タスク切換えが発生しない
 *
 * 【使用リソース】
 *
 *	TASK1: 中優先度タスク，TA_ACT属性
 *	TASK2: 高優先度タスク
 *	TASK3: 低優先度タスク
 *	ALM1:  アラームハンドラ
 *	MPF1:  TA_NULL属性，ブロック数：2，ブロックサイズ：100
 *	MPF2:  TA_TPRI属性，ブロック数：2，ブロックサイズ：100 … 使っていない
 *
 * 【テストシーケンス】
 *
 *	== TASK1（優先度：中）==
 *	1:	ref_mpf(MPF1, &rmpf)
 *		assert(rmpf.wtskid == TSK_NONE)
 *		assert(rmpf.fblkcnt == 2)
 *		get_mpf(MPF1, &blk11)							... (B-1)(D-2)
 *		// TASK1がblk11を確保
 *		assert(blk11 == MPF1_blk1 || blk11 == MPF1_blk2)
 *		ref_mpf(MPF1, &rmpf)
 *		assert(rmpf.wtskid == TSK_NONE)
 *		assert(rmpf.fblkcnt == 1)
 *		loc_cpu()
 *		get_mpf(MPF1, &blk12) -> E_CTX					... (A-2)
 *		rel_mpf(MPF1, blk11) -> E_CTX					... (E-2)
 *		unl_cpu()
 *		dis_dsp()
 *		get_mpf(MPF1, &blk12) -> E_CTX					... (A-3-1)
 *		ena_dsp()
 *		chg_ipm(TMAX_INTPRI)
 *		get_mpf(MPF1, &blk12) -> E_CTX					... (A-3-2)
 *		chg_ipm(TIPM_ENAALL)
 *		get_mpf(0, &blk12) -> E_ID						... (A-4-1)
 *		rel_mpf(0, blk11) -> E_ID						... (E-3-1)
 *		get_mpf(TNUM_MPFID+1, &blk12) -> E_ID			... (A-4-2)
 *		rel_mpf(TNUM_MPFID+1, blk11) -> E_ID			... (E-3-2)
 *		rel_mpf(MPF1, (void *)(((char *) blk11) - sizeof(mpf_MPF1))) -> E_PAR
 *														... (E-4-1)
 *		rel_mpf(MPF1, (void *)(((char *) blk11) + sizeof(mpf_MPF1))) -> E_PAR
 *														... (E-4-2)
 *		rel_mpf(MPF1, blk11 == MPF1_blk1 ? MPF1_blk2 : MPF1_blk1) -> E_PAR
 *														... (E-4-4)(G-1)
 *		get_mpf(MPF1, &blk12)							... (B-1)(D-2)
 *		// TASK1がblk11とblk12を確保
 *		assert(blk12 == MPF1_blk1 || blk12 == MPF1_blk2)
 *		assert(blk12 != blk11)
 *		act_tsk(TASK3)
 *	2:	act_tsk(TASK2)
 *	== TASK2（優先度：高）==
 *	3:	get_mpf(MPF1, &blk21)							... (B-2)(C-1)
 *		// TASK2が固定長メモリプール待ち状態に
 *	== TASK1（続き）==
 *	4:	ref_mpf(MPF1, &rmpf)
 *		assert(rmpf.wtskid == TASK2)
 *		assert(rmpf.fblkcnt == 0)
 *		DO(saved_blk1 = blk11)
 *	5:	rel_mpf(MPF1, blk11)							... (F-1)(F-2)(F-3)(G-3)
 *		// TASK1がblk12を確保．TASK2がblk21を確保
 *	== TASK2（続き）==									... (F-3)
 *	6:	assert(blk21 == saved_blk1)						... (F-1)
 *	7:	slp_tsk()
 *	== TASK1（続き）==
 *	8:	get_mpf(MPF1, &blk11)							... (B-2)
 *		// TASK1が固定長メモリプール待ち状態に
 *	== TASK3（優先度：低）==
 *	9:	wup_tsk(TASK2)
 *	== TASK2（続き）==
 *	10:	DO(saved_blk2 = blk21)
 *		rel_mpf(MPF1, blk21)							... (F-1)(F-2)(F-3)(G-4)
 *		// TASK1がblk12とblk11を確保
 *		ref_mpf(MPF1, &rmpf)
 *		assert(rmpf.wtskid == TSK_NONE)
 *		assert(rmpf.fblkcnt == 0)
 *	11:	slp_tsk()
 *	== TASK1（続き）==
 *	12:	assert(blk11 == saved_blk2)						... (F-1)
 *		rel_mpf(MPF1, blk11)							... (F-4)
 *		// TASK1がblk12を確保
 *		rel_mpf(MPF1, (void *)(((char *) blk12) + 50)) -> E_PAR	... (E-4-3)
 *		rel_mpf(MPF1, blk11) -> E_PAR							... (E-4-4)(G-2)
 *	13:	slp_tsk()
 *	== TASK3（続き）==
 *	14:	get_mpf(MPF1, &blk31)
 *		// TASK1がblk12を，TASK3がblk31を確保
 *		sta_alm(ALM1, TEST_TIME_CP) ... ALM1が実行開始するまで
 *	15:	slp_tsk()
 *	== ALM1 ==
 *	16:	get_mpf(MPF1, &blk) -> E_CTX					... (A-1)
 *		rel_mpf(MPF1, blk) -> E_CTX						... (E-1)
 *		wup_tsk(TASK1)
 *		wup_tsk(TASK3)
 *	17:	RETURN
 *	== TASK1（続き）==
 *	18:	DO(blk11 = NULL)
 *	19:	get_mpf(MPF1, &blk11) -> E_RLWAI				... (A-5)(B-2)(C-2)
 *	== TASK3（続き）==
 *	20:	rel_wai(TASK1)
 *	== TASK1（続き）==
 *	21:	assert(blk11 == NULL)
 *	22:	get_mpf(MPF1, &blk11) -> E_DLT					... (A-7)(B-2)
 *	== TASK3（続き）==
 *	23:	ini_mpf(MPF1)
 *	== TASK1（続き）==
 *	24:	get_mpf(MPF1, &blk12)
 *	25:	slp_tsk()
 *	== TASK3（続き）==
 *	26:	get_mpf(MPF1, &blk31)
 *	27:	wup_tsk(TASK1)
 *	== TASK1（続き）==
 *	28:	dis_ter()
 *	29:	get_mpf(MPF1, &blk11) -> E_RASTER				... (A-6-2)(B-2)
 *	== TASK3（続き）==
 *	30:	ras_ter(TASK1)
 *	== TASK1（続き）==
 *	31:	get_mpf(MPF1, &blk11) -> E_RASTER				... (A-6-1)
 *	32:	ena_ter()
 *	== TASK3（続き）==
 *	33:	END
 */

#include <kernel.h>
#include <t_syslog.h>
#include "syssvc/test_svc.h"
#include "kernel_cfg.h"
#include "test_common.h"

/*
 *  固定長メモリプール領域
 */
MPF_T	mpf_MPF1[2 * COUNT_MPF_T(100)];
MPF_T	mpf_MPF2[2 * COUNT_MPF_T(100)];

#define MPF1_blk1	((void *) &mpf_MPF1[0])
#define MPF1_blk2	((void *) &mpf_MPF1[COUNT_MPF_T(100)])

void	*saved_blk1;
void	*saved_blk2;

/* DO NOT DELETE THIS LINE -- gentest depends on it. */

void
alarm1_handler(EXINF exinf)
{
	ER_UINT	ercd;
	void 	*blk;

	check_point(16);
	ercd = get_mpf(MPF1, &blk);
	check_ercd(ercd, E_CTX);

	ercd = rel_mpf(MPF1, blk);
	check_ercd(ercd, E_CTX);

	ercd = wup_tsk(TASK1);
	check_ercd(ercd, E_OK);

	ercd = wup_tsk(TASK3);
	check_ercd(ercd, E_OK);

	check_point(17);
	return;

	check_assert(false);
}

void
task1(EXINF exinf)
{
	ER_UINT	ercd;
	T_RMPF	rmpf;
	void 	*blk11;
	void 	*blk12;

	test_start(__FILE__);

	check_point(1);
	ercd = ref_mpf(MPF1, &rmpf);
	check_ercd(ercd, E_OK);

	check_assert(rmpf.wtskid == TSK_NONE);

	check_assert(rmpf.fblkcnt == 2);

	ercd = get_mpf(MPF1, &blk11);
	check_ercd(ercd, E_OK);

	check_assert(blk11 == MPF1_blk1 || blk11 == MPF1_blk2);

	ercd = ref_mpf(MPF1, &rmpf);
	check_ercd(ercd, E_OK);

	check_assert(rmpf.wtskid == TSK_NONE);

	check_assert(rmpf.fblkcnt == 1);

	ercd = loc_cpu();
	check_ercd(ercd, E_OK);

	ercd = get_mpf(MPF1, &blk12);
	check_ercd(ercd, E_CTX);

	ercd = rel_mpf(MPF1, blk11);
	check_ercd(ercd, E_CTX);

	ercd = unl_cpu();
	check_ercd(ercd, E_OK);

	ercd = dis_dsp();
	check_ercd(ercd, E_OK);

	ercd = get_mpf(MPF1, &blk12);
	check_ercd(ercd, E_CTX);

	ercd = ena_dsp();
	check_ercd(ercd, E_OK);

	ercd = chg_ipm(TMAX_INTPRI);
	check_ercd(ercd, E_OK);

	ercd = get_mpf(MPF1, &blk12);
	check_ercd(ercd, E_CTX);

	ercd = chg_ipm(TIPM_ENAALL);
	check_ercd(ercd, E_OK);

	ercd = get_mpf(0, &blk12);
	check_ercd(ercd, E_ID);

	ercd = rel_mpf(0, blk11);
	check_ercd(ercd, E_ID);

	ercd = get_mpf(TNUM_MPFID+1, &blk12);
	check_ercd(ercd, E_ID);

	ercd = rel_mpf(TNUM_MPFID+1, blk11);
	check_ercd(ercd, E_ID);

	ercd = rel_mpf(MPF1, (void *)(((char *) blk11) - sizeof(mpf_MPF1)));
	check_ercd(ercd, E_PAR);

	ercd = rel_mpf(MPF1, (void *)(((char *) blk11) + sizeof(mpf_MPF1)));
	check_ercd(ercd, E_PAR);

	ercd = rel_mpf(MPF1, blk11 == MPF1_blk1 ? MPF1_blk2 : MPF1_blk1);
	check_ercd(ercd, E_PAR);

	ercd = get_mpf(MPF1, &blk12);
	check_ercd(ercd, E_OK);

	check_assert(blk12 == MPF1_blk1 || blk12 == MPF1_blk2);

	check_assert(blk12 != blk11);

	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	check_point(2);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(4);
	ercd = ref_mpf(MPF1, &rmpf);
	check_ercd(ercd, E_OK);

	check_assert(rmpf.wtskid == TASK2);

	check_assert(rmpf.fblkcnt == 0);

	saved_blk1 = blk11;

	check_point(5);
	ercd = rel_mpf(MPF1, blk11);
	check_ercd(ercd, E_OK);

	check_point(8);
	ercd = get_mpf(MPF1, &blk11);
	check_ercd(ercd, E_OK);

	check_point(12);
	check_assert(blk11 == saved_blk2);

	ercd = rel_mpf(MPF1, blk11);
	check_ercd(ercd, E_OK);

	ercd = rel_mpf(MPF1, (void *)(((char *) blk12) + 50));
	check_ercd(ercd, E_PAR);

	ercd = rel_mpf(MPF1, blk11);
	check_ercd(ercd, E_PAR);

	check_point(13);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(18);
	blk11 = NULL;

	check_point(19);
	ercd = get_mpf(MPF1, &blk11);
	check_ercd(ercd, E_RLWAI);

	check_point(21);
	check_assert(blk11 == NULL);

	check_point(22);
	ercd = get_mpf(MPF1, &blk11);
	check_ercd(ercd, E_DLT);

	check_point(24);
	ercd = get_mpf(MPF1, &blk12);
	check_ercd(ercd, E_OK);

	check_point(25);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(28);
	ercd = dis_ter();
	check_ercd(ercd, E_OK);

	check_point(29);
	ercd = get_mpf(MPF1, &blk11);
	check_ercd(ercd, E_RASTER);

	check_point(31);
	ercd = get_mpf(MPF1, &blk11);
	check_ercd(ercd, E_RASTER);

	check_point(32);
	ercd = ena_ter();
	check_ercd(ercd, E_OK);

	check_assert(false);
}

void
task2(EXINF exinf)
{
	ER_UINT	ercd;
	void 	*blk21;
	T_RMPF	rmpf;

	check_point(3);
	ercd = get_mpf(MPF1, &blk21);
	check_ercd(ercd, E_OK);

	check_point(6);
	check_assert(blk21 == saved_blk1);

	check_point(7);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(10);
	saved_blk2 = blk21;

	ercd = rel_mpf(MPF1, blk21);
	check_ercd(ercd, E_OK);

	ercd = ref_mpf(MPF1, &rmpf);
	check_ercd(ercd, E_OK);

	check_assert(rmpf.wtskid == TSK_NONE);

	check_assert(rmpf.fblkcnt == 0);

	check_point(11);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_assert(false);
}

void
task3(EXINF exinf)
{
	ER_UINT	ercd;
	void 	*blk31;

	check_point(9);
	ercd = wup_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(14);
	ercd = get_mpf(MPF1, &blk31);
	check_ercd(ercd, E_OK);

	ercd = sta_alm(ALM1, TEST_TIME_CP);
	check_ercd(ercd, E_OK);

	check_point(15);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(20);
	ercd = rel_wai(TASK1);
	check_ercd(ercd, E_OK);

	check_point(23);
	ercd = ini_mpf(MPF1);
	check_ercd(ercd, E_OK);

	check_point(26);
	ercd = get_mpf(MPF1, &blk31);
	check_ercd(ercd, E_OK);

	check_point(27);
	ercd = wup_tsk(TASK1);
	check_ercd(ercd, E_OK);

	check_point(30);
	ercd = ras_ter(TASK1);
	check_ercd(ercd, E_OK);

	check_finish(33);
	check_assert(false);
}
