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
 *  $Id: test_inherit1.c 1620 2022-09-19 06:39:09Z ertl-hiro $
 */

/* 
 *		優先度継承ミューテックスのテスト(1)
 *
 * 【テストの目的】
 *
 *  ミューテックス関連のサービスコールで振舞いの中で，優先度継承ミュー
 *  テックス1つだけでテストできるものをすべてテストする．
 *
 * 【テスト項目】
 *
 *	(A) loc_mtxの静的エラーのテスト
 *		(A-1) 非タスクコンテキストからの呼出し［NGKI2070］
 *		(A-2) CPUロック状態からの呼出し［NGKI2071］
 *		(A-3) ディスパッチ禁止状態からの呼出し［NGKI2072］
 *		(A-4) 割込み優先度マスク全解除でない状態からの呼出し［NGKI2072］
 *		(A-5) mtxidが有効範囲外（小さすぎる）［NGKI2074］
 *		(A-6) mtxidが有効範囲外（大きすぎる）［NGKI2074］
 *	(B) loc_mtxの正常系のテスト
 *		(B-1) 自タスクによってロックされている状態に［NGKI2081］
 *		(B-2) ロックしているミューテックスのリストに追加［NGKI2082］
 *		(B-3) ミューテックスのロック待ち状態に［NGKI2083］
 *	(C) loc_mtxの動的エラーのテスト
 *		(C-1) 自タスクによってロックされている［NGKI3609］
 *		(C-2) 待ち状態の強制解除［NGKI2079］
 *		(C-3) タスクの終了要求（呼び出し時点で）［NGKI3463］
 *		(C-4) タスクの終了要求（待ちになった後に）［NGKI3463］
 *		(C-5) 待ちオブジェクトの再初期化［NGKI2080］
 *	(D) ploc_mtxの静的エラーのテスト
 *		(D-1) 非タスクコンテキストからの呼出し［NGKI2070］
 *		(D-2) CPUロック状態からの呼出し［NGKI2071］
 *		(D-3) ディスパッチ禁止状態からの呼出し（エラーにならない）
 *		(D-4) 割込み優先度マスク全解除でない状態からの呼出し（エラーにならない）
 *		(D-5) mtxidが有効範囲外（小さすぎる）［NGKI2074］
 *		(D-6) mtxidが有効範囲外（大きすぎる）［NGKI2074］
 *	(E) ploc_mtxの正常系のテスト
 *		(E-1) 自タスクによってロックされている状態に［NGKI2081］
 *		(E-2) ロックしているミューテックスのリストに追加［NGKI2082］
 *	(F) loc_mtxの動的エラーのテスト
 *		(F-1) 自タスクによってロックされている［NGKI3609］
 *		(F-2) ポーリング失敗［NGKI2078］
 *	(G) tloc_mtxの静的エラーのテスト
 *		(G-1) 非タスクコンテキストからの呼出し［NGKI2070］
 *		(G-2) CPUロック状態からの呼出し［NGKI2071］
 *		(G-3) ディスパッチ禁止状態からの呼出し［NGKI2072］
 *		(G-4) 割込み優先度マスク全解除でない状態からの呼出し［NGKI2072］
 *		(G-5) mtxidが有効範囲外（小さすぎる）［NGKI2074］
 *		(G-6) mtxidが有効範囲外（大きすぎる）［NGKI2074］
 *		(G-7) tmoutが無効（小さすぎる）［NGKI2075］
 *		(G-8) tmoutが無効（大きすぎる）［NGKI2075］
 *	(H) tloc_mtxの正常系のテスト
 *		(H-1) 自タスクによってロックされている状態に［NGKI2081］
 *		(H-2) ロックしているミューテックスのリストに追加［NGKI2082］
 *		(H-3) ミューテックスのロック待ち状態に［NGKI2083］
 *	(I) tloc_mtxの動的エラーのテスト
 *		(I-1) 自タスクによってロックされている［NGKI3609］
 *		(I-2) 待ち状態の強制解除［NGKI2079］
 *		(I-3) タスクの終了要求［NGKI3463］
 *		(I-4) タスクの終了要求（待ちになった後に）［NGKI3463］
 *		(I-5) 待ちオブジェクトの再初期化［NGKI2080］
 *		(I-6) ポーリング失敗［NGKI2078］
 *		(I-7) タイムアウト［NGKI2078］
 *	(J) unl_mtxの静的エラーのテスト
 *		(J-1) 非タスクコンテキストからの呼出し［NGKI2087］
 *		(J-2) CPUロック状態からの呼出し［NGKI2088］
 *		(J-3) ディスパッチ禁止状態からの呼出し（エラーにならない）
 *		(J-4) 割込み優先度マスク全解除でない状態からの呼出し（エラーにならない）
 *		(J-5) mtxidが有効範囲外（小さすぎる）［NGKI2089］
 *		(J-6) mtxidが有効範囲外（大きすぎる）［NGKI2089］
 *	(K) unl_mtxの正常系のテスト
 *		(K-1) ロックしているミューテックスのリストから削除［NGKI2091］
 *		(K-2) 待ち行列の先頭のタスクが待ち解除［NGKI2092］
 *		(K-3) 待ち解除されたタスクがロック［NGKI2093］
 *		(K-4) 待ち解除されたタスクがロックしているミューテックスのリス
 *			  トに追加［NGKI2094］
 *		(K-5) 待ち解除されたタスクにE_OKが返る［NGKI2095］
 *		(K-6) ロックされていない状態に［NGKI2096］
 *	(L) ini_mtxの静的エラーのテスト
 *		(L-1) 非タスクコンテキストからの呼出し［NGKI2099］
 *		(L-2) CPUロック状態からの呼出し［NGKI2100］
 *		(L-3) mtxidが有効範囲外（小さすぎる）［NGKI2101］
 *		(L-4) mtxidが有効範囲外（大きすぎる）［NGKI2101］
 *	(M) ini_mtxの正常系のテスト
 *		(M-1) ロックされていない状態に初期化［NGKI2104］
 *		(M-2) ロックしていたタスクがロックしているミューテックスのリス
 *			  トから削除［NGKI2105］
 *		(M-3) 待ち行列の先頭のタスクから順にすべて待ち解除［NGKI2106］
 *		(M-4) 待ち解除されたタスクにE_DLTが返る［NGKI2107］
 *	(N) ref_mtxの静的エラーのテスト
 *		(N-1) 非タスクコンテキストからの呼出し［NGKI2109］
 *		(N-2) CPUロック状態からの呼出し［NGKI2110］
 *		(N-3) mtxidが有効範囲外（小さすぎる）［NGKI2111］
 *		(N-4) mtxidが有効範囲外（大きすぎる）［NGKI2111］
 *	(O) ref_mtxの正常系のテスト
 *		(O-1) htskidにロックしているタスクIDが返る［NGKI5207］
 *		(O-2) wtskidに待ち行列の先頭のタスクIDが返る［NGKI5207］
 *		(O-3) htskidにTSK_NONEが返る［NGKI2115］
 *		(O-4) wtskidにTSK_NONEが返る［NGKI2116］
 *	(P) 初期状態のテスト
 *		(P-1) ロックされておらず，待ち行列は空の状態に初期化［NGKI2033］
 *		(P-2) ロックしているミューテックスのリストは空に初期化［NGKI2013］
 *			 ※ ロックしているミューテックスのリストを直接見るサービス
 *				コールがない．ミューテックスは1つしかないため，そのミュー
 *				テックスに対してunl_mtxを発行してE_OBJが返ったら，リス
 *				トは空であると判断する．
 *	(Q) カーネル構成マクロのテスト
 *		(Q-1) TNUM_MTXID［NGKI2020］
 *
 * 【使用リソース】
 *
 *	TASK1: 低優先度タスク，メインタスク，最初から起動
 *	TASK2: 中優先度タスク
 *	TASK3: 高優先度タスク
 *	MTX1:  ミューテックス（優先度継承）
 *	ALM1:  アラームハンドラ
 *
 * 【テストシーケンス】
 *
 *	== TASK1（優先度：低）==
 *	1:	ref_mtx(MTX1, &rmtx)							... (P-1)の確認
 *		assert(rmtx.htskid == TSK_NONE)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		unl_mtx(MTX1) -> E_OBJ							... (P-2)の確認
 *		sta_alm(ALM1, TEST_TIME_CP) ... ALM1が実行開始するまで
 *		slp_tsk()
 *	== ALM1 ==
 *	2:	loc_mtx(MTX1) -> E_CTX							... (A-1)
 *		ploc_mtx(MTX1) -> E_CTX							... (D-1)
 *		tloc_mtx(MTX1, TEST_TIME_CP) -> E_CTX			... (G-1)
 *		unl_mtx(MTX1) -> E_CTX							... (J-1)
 *		ini_mtx(MTX1) -> E_CTX							... (L-1)
 *		ref_mtx(MTX1, &rmtx) -> E_CTX					... (N-1)
 *		wup_tsk(TASK1)
 *	3:	RETURN
 *	== TASK1（続き）==
 *	4:	loc_cpu()
 *		loc_mtx(MTX1) -> E_CTX							... (A-2)
 *		ploc_mtx(MTX1) -> E_CTX							... (D-2)
 *		tloc_mtx(MTX1, TEST_TIME_CP) -> E_CTX			... (G-2)
 *		unl_mtx(MTX1) -> E_CTX							... (J-2)
 *		ini_mtx(MTX1) -> E_CTX							... (L-2)
 *		ref_mtx(MTX1, &rmtx) -> E_CTX					... (N-2)
 *		unl_cpu()
 *	5:	dis_dsp()
 *		loc_mtx(MTX1) -> E_CTX							... (A-3)
 *		ploc_mtx(MTX1)									... (D-3)
 *		unl_mtx(MTX1)									... (J-3)
 *		tloc_mtx(MTX1, TEST_TIME_CP) -> E_CTX			... (G-3)
 *		ena_dsp()
 *	6:	chg_ipm(TMAX_INTPRI)
 *		loc_mtx(MTX1) -> E_CTX							... (A-4)
 *		ploc_mtx(MTX1)									... (D-4)
 *		unl_mtx(MTX1)									... (J-4)
 *		tloc_mtx(MTX1, TEST_TIME_CP) -> E_CTX			... (G-4)
 *		chg_ipm(TIPM_ENAALL)
 *	7:	loc_mtx(0) -> E_ID								... (A-5)
 *		ploc_mtx(0) -> E_ID								... (D-5)
 *		tloc_mtx(0, TEST_TIME_CP) -> E_ID				... (G-5)
 *		unl_mtx(0) -> E_ID								... (J-5)
 *		ini_mtx(0) -> E_ID								... (L-3)
 *		ref_mtx(0, &rmtx) -> E_ID						... (N-3)
 *		loc_mtx(TNUM_MTXID+1) -> E_ID					... (A-6)(Q-1)
 *		ploc_mtx(TNUM_MTXID+1) -> E_ID					... (D-6)
 *		tloc_mtx(TNUM_MTXID+1, TEST_TIME_CP) -> E_ID	... (G-6)
 *		unl_mtx(TNUM_MTXID+1) -> E_ID					... (J-6)
 *		ini_mtx(TNUM_MTXID+1) -> E_ID					... (L-4)
 *		ref_mtx(TNUM_MTXID+1, &rmtx) -> E_ID			... (N-4)
 *	8:	tloc_mtx(MTX1, TMO_FEVR-1) -> E_PAR				... (G-7)
 *		tloc_mtx(MTX1, TMAX_RELTIM+1) -> E_PAR			... (G-8)
 *	9:	loc_mtx(MTX1)									... (B-1)(B-2)
 *		ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TASK1)					... (O-1)
 *		assert(rmtx.wtskid == TSK_NONE)					... (O-4)
 *		unl_mtx(MTX1)									... (K-1)(K-6)
 *		unl_mtx(MTX1) -> E_OBJ							... (K-1)の確認
 *		ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TSK_NONE)					... (O-3)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		ploc_mtx(MTX1)									... (E-1)(E-2)
 *		ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TASK1)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		unl_mtx(MTX1)
 *		tloc_mtx(MTX1, TEST_TIME_CP * 10)				... (H-1)(H-2)
 *		ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TASK1)
 *		assert(rmtx.wtskid == TSK_NONE)
 *	10:	loc_mtx(MTX1) -> E_OBJ							... (C-1)
 *		ploc_mtx(MTX1) -> E_OBJ							... (F-1)
 *		tloc_mtx(MTX1, TEST_TIME_CP) -> E_OBJ			... (I-1)
 *		act_tsk(TASK2)
 *	== TASK2-1（優先度：中）==
 *	11:	loc_mtx(MTX1)									... (B-3)
 *	== TASK1（続き）==
 *	12:	ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TASK1)
 *		assert(rmtx.wtskid == TASK2)					... (O-2)
 *		act_tsk(TASK3)
 *	== TASK3-1（優先度：高）==
 *	13:	loc_mtx(MTX1)
 *	== TASK1（続き）==
 *	14:	ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TASK1)
 *		assert(rmtx.wtskid == TASK3)
 *		unl_mtx(MTX1)									... (K-2)(K-3)(K-4)(K-5)
 *	== TASK3-1（続き）==
 *	15:	ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TASK3)
 *		assert(rmtx.wtskid == TASK2)
 *		ext_tsk()
 *		// MTX1をロックしたままタスクを終了する
 *	== TASK2-1（続き）==
 *	16:	unl_mtx(MTX1)
 *		ext_tsk()
 *	== TASK1（続き）==
 *	17:	loc_mtx(MTX1)
 *		act_tsk(TASK2)
 *	== TASK2-2（優先度：中）==
 *	18:	ploc_mtx(MTX1) -> E_TMOUT						... (F-2)
 *		tloc_mtx(MTX1, TMO_POL) -> E_TMOUT				... (I-6)
 *		tloc_mtx(MTX1, TEST_TIME_CP * 10)				... (H-3)
 *	== TASK1（続き）==
 *	19:	ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TASK1)
 *		assert(rmtx.wtskid == TASK2)
 *		unl_mtx(MTX1)
 *	== TASK2-2（続き）==
 *	20:	ext_tsk()
 *	== TASK1（続き）==
 *	21:	loc_mtx(MTX1)
 *		act_tsk(TASK2)
 *	== TASK2-3（優先度：中）==
 *	22:	loc_mtx(MTX1) -> E_RLWAI						... (C-2)
 *	== TASK1（続き）==
 *	23:	rel_wai(TASK2)
 *	== TASK2-3（続き）==
 *	24:	tloc_mtx(MTX1, TEST_TIME_CP * 10) -> E_RLWAI	... (I-2)
 *	== TASK1（続き）==
 *	25:	rel_wai(TASK2)
 *	== TASK2-3（続き）==
 *	26:	dis_ter()
 *		loc_mtx(MTX1) -> E_RASTER						... (C-4)
 *	== TASK1（続き）==
 *	27:	ras_ter(TASK2)
 *	== TASK2-3（続き）==
 *	28:	ena_ter()
 *	== TASK1（続き）==
 *	29:	act_tsk(TASK2)
 *	== TASK2-4（優先度：中）==
 *	30:	dis_ter()
 *		tloc_mtx(MTX1, TEST_TIME_CP * 10) -> E_RASTER	... (I-4)
 *	== TASK1（続き）==
 *	31:	ras_ter(TASK2)
 *	== TASK2-4（続き）==
 *	32:	ena_ter()
 *	== TASK1（続き）==
 *	33:	act_tsk(TASK2)
 *	== TASK2-5（優先度：中）==
 *	34:	loc_mtx(MTX1) -> E_DLT							... (C-5)
 *	== TASK1（続き）==
 *	35:	act_tsk(TASK3)
 *	== TASK3-2（優先度：高）==
 *	36:	unl_mtx(MTX1) -> E_OBJ							... (P-2)の確認
 *		loc_mtx(MTX1) -> E_DLT
 *	== TASK1（続き）==
 *	37:	chg_pri(TASK3, MID_PRIORITY)
 *		// ini_mtxが，待ち行列の先頭のタスクから順に待ち解除しているこ
 *		// とを確認するために，ミューテックスの待ち行列内での順序を変
 *		// 更する．
 *		ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TASK1)
 *		assert(rmtx.wtskid == TASK2)
 *		ini_mtx(MTX1)									... (M-1)(M-2)(M-3)(M-4)
 *	== TASK2-5（続き）==
 *	38:	ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TSK_NONE)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		ext_tsk()
 *	== TASK3-2（続き）==
 *	39:	ext_tsk()
 *	== TASK1（続き）==
 *	40:	loc_mtx(MTX1)
 *		act_tsk(TASK2)
 *	== TASK2-6（優先度：中）==
 *	41:	tloc_mtx(MTX1, TEST_TIME_CP * 10) -> E_DLT		... (I-5)
 *	== TASK1（続き）==
 *	42:	ini_mtx(MTX1)
 *	== TASK2-6（続き）==
 *	43:	ext_tsk()
 *	== TASK1（続き）==
 *	44:	loc_mtx(MTX1)
 *		act_tsk(TASK2)
 *	== TASK2-7（優先度：中）==
 *	45:	tloc_mtx(MTX1, TEST_TIME_CP * 10) -> E_TMOUT	... (I-7)
 *	== TASK1（続き）==
 *	46:	slp_tsk()
 *	// アイドル処理
 *	== TASK2-7（続き）==
 *	47:	wup_tsk(TASK1)
 *		dis_ter()
 *		slp_tsk() -> E_RASTER
 *	== TASK1（続き）==
 *	48:	ras_ter(TASK2)
 *	== TASK2-7（続き）==
 *	49:	loc_mtx(MTX1) -> E_RASTER						... (C-3)
 *		tloc_mtx(MTX1, TEST_TIME_CP) -> E_RASTER		... (I-3)
 *	50:	END
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
	T_RMTX	rmtx;

	check_point(2);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_CTX);

	ercd = ploc_mtx(MTX1);
	check_ercd(ercd, E_CTX);

	ercd = tloc_mtx(MTX1, TEST_TIME_CP);
	check_ercd(ercd, E_CTX);

	ercd = unl_mtx(MTX1);
	check_ercd(ercd, E_CTX);

	ercd = ini_mtx(MTX1);
	check_ercd(ercd, E_CTX);

	ercd = ref_mtx(MTX1, &rmtx);
	check_ercd(ercd, E_CTX);

	ercd = wup_tsk(TASK1);
	check_ercd(ercd, E_OK);

	check_point(3);
	return;

	check_assert(false);
}

void
task1(EXINF exinf)
{
	ER_UINT	ercd;
	T_RMTX	rmtx;

	test_start(__FILE__);

	check_point(1);
	ercd = ref_mtx(MTX1, &rmtx);
	check_ercd(ercd, E_OK);

	check_assert(rmtx.htskid == TSK_NONE);

	check_assert(rmtx.wtskid == TSK_NONE);

	ercd = unl_mtx(MTX1);
	check_ercd(ercd, E_OBJ);

	ercd = sta_alm(ALM1, TEST_TIME_CP);
	check_ercd(ercd, E_OK);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(4);
	ercd = loc_cpu();
	check_ercd(ercd, E_OK);

	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_CTX);

	ercd = ploc_mtx(MTX1);
	check_ercd(ercd, E_CTX);

	ercd = tloc_mtx(MTX1, TEST_TIME_CP);
	check_ercd(ercd, E_CTX);

	ercd = unl_mtx(MTX1);
	check_ercd(ercd, E_CTX);

	ercd = ini_mtx(MTX1);
	check_ercd(ercd, E_CTX);

	ercd = ref_mtx(MTX1, &rmtx);
	check_ercd(ercd, E_CTX);

	ercd = unl_cpu();
	check_ercd(ercd, E_OK);

	check_point(5);
	ercd = dis_dsp();
	check_ercd(ercd, E_OK);

	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_CTX);

	ercd = ploc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = unl_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = tloc_mtx(MTX1, TEST_TIME_CP);
	check_ercd(ercd, E_CTX);

	ercd = ena_dsp();
	check_ercd(ercd, E_OK);

	check_point(6);
	ercd = chg_ipm(TMAX_INTPRI);
	check_ercd(ercd, E_OK);

	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_CTX);

	ercd = ploc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = unl_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = tloc_mtx(MTX1, TEST_TIME_CP);
	check_ercd(ercd, E_CTX);

	ercd = chg_ipm(TIPM_ENAALL);
	check_ercd(ercd, E_OK);

	check_point(7);
	ercd = loc_mtx(0);
	check_ercd(ercd, E_ID);

	ercd = ploc_mtx(0);
	check_ercd(ercd, E_ID);

	ercd = tloc_mtx(0, TEST_TIME_CP);
	check_ercd(ercd, E_ID);

	ercd = unl_mtx(0);
	check_ercd(ercd, E_ID);

	ercd = ini_mtx(0);
	check_ercd(ercd, E_ID);

	ercd = ref_mtx(0, &rmtx);
	check_ercd(ercd, E_ID);

	ercd = loc_mtx(TNUM_MTXID+1);
	check_ercd(ercd, E_ID);

	ercd = ploc_mtx(TNUM_MTXID+1);
	check_ercd(ercd, E_ID);

	ercd = tloc_mtx(TNUM_MTXID+1, TEST_TIME_CP);
	check_ercd(ercd, E_ID);

	ercd = unl_mtx(TNUM_MTXID+1);
	check_ercd(ercd, E_ID);

	ercd = ini_mtx(TNUM_MTXID+1);
	check_ercd(ercd, E_ID);

	ercd = ref_mtx(TNUM_MTXID+1, &rmtx);
	check_ercd(ercd, E_ID);

	check_point(8);
	ercd = tloc_mtx(MTX1, TMO_FEVR-1);
	check_ercd(ercd, E_PAR);

	ercd = tloc_mtx(MTX1, TMAX_RELTIM+1);
	check_ercd(ercd, E_PAR);

	check_point(9);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = ref_mtx(MTX1, &rmtx);
	check_ercd(ercd, E_OK);

	check_assert(rmtx.htskid == TASK1);

	check_assert(rmtx.wtskid == TSK_NONE);

	ercd = unl_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = unl_mtx(MTX1);
	check_ercd(ercd, E_OBJ);

	ercd = ref_mtx(MTX1, &rmtx);
	check_ercd(ercd, E_OK);

	check_assert(rmtx.htskid == TSK_NONE);

	check_assert(rmtx.wtskid == TSK_NONE);

	ercd = ploc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = ref_mtx(MTX1, &rmtx);
	check_ercd(ercd, E_OK);

	check_assert(rmtx.htskid == TASK1);

	check_assert(rmtx.wtskid == TSK_NONE);

	ercd = unl_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = tloc_mtx(MTX1, TEST_TIME_CP * 10);
	check_ercd(ercd, E_OK);

	ercd = ref_mtx(MTX1, &rmtx);
	check_ercd(ercd, E_OK);

	check_assert(rmtx.htskid == TASK1);

	check_assert(rmtx.wtskid == TSK_NONE);

	check_point(10);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OBJ);

	ercd = ploc_mtx(MTX1);
	check_ercd(ercd, E_OBJ);

	ercd = tloc_mtx(MTX1, TEST_TIME_CP);
	check_ercd(ercd, E_OBJ);

	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(12);
	ercd = ref_mtx(MTX1, &rmtx);
	check_ercd(ercd, E_OK);

	check_assert(rmtx.htskid == TASK1);

	check_assert(rmtx.wtskid == TASK2);

	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	check_point(14);
	ercd = ref_mtx(MTX1, &rmtx);
	check_ercd(ercd, E_OK);

	check_assert(rmtx.htskid == TASK1);

	check_assert(rmtx.wtskid == TASK3);

	ercd = unl_mtx(MTX1);
	check_ercd(ercd, E_OK);

	check_point(17);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(19);
	ercd = ref_mtx(MTX1, &rmtx);
	check_ercd(ercd, E_OK);

	check_assert(rmtx.htskid == TASK1);

	check_assert(rmtx.wtskid == TASK2);

	ercd = unl_mtx(MTX1);
	check_ercd(ercd, E_OK);

	check_point(21);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(23);
	ercd = rel_wai(TASK2);
	check_ercd(ercd, E_OK);

	check_point(25);
	ercd = rel_wai(TASK2);
	check_ercd(ercd, E_OK);

	check_point(27);
	ercd = ras_ter(TASK2);
	check_ercd(ercd, E_OK);

	check_point(29);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(31);
	ercd = ras_ter(TASK2);
	check_ercd(ercd, E_OK);

	check_point(33);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(35);
	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	check_point(37);
	ercd = chg_pri(TASK3, MID_PRIORITY);
	check_ercd(ercd, E_OK);

	ercd = ref_mtx(MTX1, &rmtx);
	check_ercd(ercd, E_OK);

	check_assert(rmtx.htskid == TASK1);

	check_assert(rmtx.wtskid == TASK2);

	ercd = ini_mtx(MTX1);
	check_ercd(ercd, E_OK);

	check_point(40);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(42);
	ercd = ini_mtx(MTX1);
	check_ercd(ercd, E_OK);

	check_point(44);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(46);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(48);
	ercd = ras_ter(TASK2);
	check_ercd(ercd, E_OK);

	check_assert(false);
}

static uint_t	task2_count = 0;

void
task2(EXINF exinf)
{
	ER_UINT	ercd;
	T_RMTX	rmtx;

	switch (++task2_count) {
	case 1:
		check_point(11);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(16);
		ercd = unl_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 2:
		check_point(18);
		ercd = ploc_mtx(MTX1);
		check_ercd(ercd, E_TMOUT);

		ercd = tloc_mtx(MTX1, TMO_POL);
		check_ercd(ercd, E_TMOUT);

		ercd = tloc_mtx(MTX1, TEST_TIME_CP * 10);
		check_ercd(ercd, E_OK);

		check_point(20);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 3:
		check_point(22);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_RLWAI);

		check_point(24);
		ercd = tloc_mtx(MTX1, TEST_TIME_CP * 10);
		check_ercd(ercd, E_RLWAI);

		check_point(26);
		ercd = dis_ter();
		check_ercd(ercd, E_OK);

		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_RASTER);

		check_point(28);
		ercd = ena_ter();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 4:
		check_point(30);
		ercd = dis_ter();
		check_ercd(ercd, E_OK);

		ercd = tloc_mtx(MTX1, TEST_TIME_CP * 10);
		check_ercd(ercd, E_RASTER);

		check_point(32);
		ercd = ena_ter();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 5:
		check_point(34);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_DLT);

		check_point(38);
		ercd = ref_mtx(MTX1, &rmtx);
		check_ercd(ercd, E_OK);

		check_assert(rmtx.htskid == TSK_NONE);

		check_assert(rmtx.wtskid == TSK_NONE);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 6:
		check_point(41);
		ercd = tloc_mtx(MTX1, TEST_TIME_CP * 10);
		check_ercd(ercd, E_DLT);

		check_point(43);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 7:
		check_point(45);
		ercd = tloc_mtx(MTX1, TEST_TIME_CP * 10);
		check_ercd(ercd, E_TMOUT);

		check_point(47);
		ercd = wup_tsk(TASK1);
		check_ercd(ercd, E_OK);

		ercd = dis_ter();
		check_ercd(ercd, E_OK);

		ercd = slp_tsk();
		check_ercd(ercd, E_RASTER);

		check_point(49);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_RASTER);

		ercd = tloc_mtx(MTX1, TEST_TIME_CP);
		check_ercd(ercd, E_RASTER);

		check_finish(50);
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
	T_RMTX	rmtx;

	switch (++task3_count) {
	case 1:
		check_point(13);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(15);
		ercd = ref_mtx(MTX1, &rmtx);
		check_ercd(ercd, E_OK);

		check_assert(rmtx.htskid == TASK3);

		check_assert(rmtx.wtskid == TASK2);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 2:
		check_point(36);
		ercd = unl_mtx(MTX1);
		check_ercd(ercd, E_OBJ);

		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_DLT);

		check_point(39);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	default:
		check_assert(false);
	}
	check_assert(false);
}
