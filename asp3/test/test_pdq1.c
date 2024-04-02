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
 *  $Id: test_pdq1.c 1784 2023-01-13 12:12:37Z ertl-hiro $
 */

/* 
 *		優先度データキュー機能のテスト(1)
 *
 * 【テストの目的】
 *
 *	snd_pdqの振舞いのテスト［NGKI1855］
 *	  ・snd_pdqに関するすべての要求をテストする．
 *	  ・snd_pdq関数のC1カバレッジを達成する．
 *	snd_pdqが呼び出すデータキュー機能内部関数のテスト
 *	  ・send_pridata関数のC1カバレッジを達成する．
 *	  ・enqueue_pridata関数のC1カバレッジを達成する．
 *	rcv_pdqの振舞いのテスト［NGKI1877］
 *	  ・rcv_pdqに関するすべての要求をテストする．
 *	  ・rcv_pdq関数のC1カバレッジを達成する．
 *	rcv_pdqが呼び出すデータキュー機能内部関数のテスト
 *	  ・receive_pridata関数のC1カバレッジを達成する．
 *	  ・dequeue_pridata関数のC1カバレッジを達成する．
 *
 * 【テスト項目】
 *
 *	(A) snd_pdqのエラー検出
 *		(A-1) 非タスクコンテキストからの呼出し［NGKI1862］
 *		(A-2) CPUロック状態からの呼出し［NGKI1861］
 *		(A-3) ディスパッチ保留状態からの呼出し［NGKI1862］
 *			(A-3-1) ディスパッチ禁止状態
 *			(A-3-2) 割込み優先度マスク全解除でない状態
 *		(A-4) pdqidが不正［NGKI1864］
 *			(A-4-1) 小さすぎる
 *			(A-4-2) 大きすぎる
 *		(A-5) 待ち状態の強制解除［NGKI1869］
 *		(A-6) タスクの終了要求［NGKI3461］
 *			(A-6-1) 呼び出し時点で終了要求フラグがセット
 *			(A-6-2) 待ち状態になった後に終了要求
 *		(A-7) 待ちオブジェクトの削除または再初期化［NGKI1870］
 *		(A-8) datapriが不正［NGKI1876］
 *			(A-8-1) 小さすぎる
 *			(A-8-2) 大きすぎる
 *	(B) snd_pdqの正常系処理
 *		(B-1) 受信待ち行列中のタスクを待ち解除［NGKI1871］
 *		(B-2) 待ち解除されたタスクにE_OKが返る［NGKI1872］
 *		(B-3) データを優先度順で管理領域に格納［NGKI1873］
 *		(B-4) 管理領域にスペースがなく送信待ち状態に［NGKI1874］
 *	(C) snd_pdqのC1カバレッジ
 *	  ※ここまでのテストで有効なもの
 *			(A-1)(A-2)(A-3)(A-4-1)(A-4-2)(A-8-1)(A-8-2)(A-6-1)(B-1)(B-4)
 *			※(B-1)は(B-3)に代えてもよい
 *	  ※(B-1)の分解が必要
 *		(C-1) 受信待ちタスクを待ち解除してタスク切換えが発生
 *		(C-2) 受信待ちタスクを待ち解除してタスク切換えが発生しない
 *	(D) send_pridatqのC1カバレッジ
 *	  ※ここまでのテストで有効なもの（不足はない）
 *			(B-1)(B-3)(B-4)
 *	(E) enqueue_pridataのC1カバレッジ
 *	  ※(B-3)/(G-3)の分解が必要
 *		(E-1) 未割当てデータ管理ブロックがある
 *		(E-2) 未割当てデータ管理ブロックがない
 *		(E-3)(B-3-1) 管理領域が空の時
 *		(E-4)(B-3-2) 管理領域に高優先度データがある場合
 *		(E-5)(B-3-3) 管理領域に同優先度データがある場合
 *		(E-6)(B-3-4) 管理領域に低優先度データがある場合
 *	(F) rcv_pdqのエラー検出
 *		(F-1) 非タスクコンテキストからの呼出し［NGKI1880］
 *		(F-2) CPUロック状態からの呼出し［NGKI1881］
 *		(F-3) ディスパッチ保留状態からの呼出し［NGKI1882］
 *			(F-3-1) ディスパッチ禁止状態
 *			(F-3-2) 割込み優先度マスク全解除でない状態
 *		(F-4) pdqidが不正［NGKI1884］
 *			(F-4-1) 小さすぎる
 *			(F-4-2) 大きすぎる
 *		(F-5) 待ち状態の強制解除［NGKI1891］
 *		(F-6) タスクの終了要求［NGKI3462］
 *			(F-6-1) 呼び出し時点で終了要求フラグがセット
 *			(F-6-2) 待ち状態になった後に終了要求
 *		(F-7) 待ちオブジェクトの削除または再初期化［NGKI1892］
 *	(G) rcv_pdqの正常系処理
 *		(G-1) 受信したデータはp_dataが指すメモリ領域に，その優先度は
 *			  p_datapriが指すメモリ領域に［NGKI1894］
 *		(G-2) 管理領域の先頭のデータを受信［NGKI1893］
 *		(G-3) 送信待ち行列中の先頭タスクの送信データを優先度順で管理
 *			  領域に格納し，タスクを待ち解除［NGKI1895］
 *		(G-4) 待ち解除されたタスクにE_OKが返る［NGKI1896］
 *		(G-5) 送信待ち行列中のタスクの送信データを受信［NGKI1897］
 *		(G-6) 送信待ち行列中のタスクを待ち解除［NGKI1899］
 *		(G-7) 待ち解除されたタスクにE_OKが返る［NGKI1900］
 *		(G-8) 受信すべきデータがなく受信待ち状態に［NGKI1901］
 *	  ※(G-5)(G-6)(G-7)は格納できるデータ数が0の場合のみ発生する条件
 *	(H) rcv_pdqのC1カバレッジ
 *	  ※ここまでのテストで有効なもの
 *			(F-1)(F-2)(F-3)(F-4-1)(F-4-2)(F-6-1)(G-2)(G-8)
 *			※(G-2)は(G-5)に代えてもよい
 *	  ※(G-3)の分解が必要
 *		(H-1) 送信待ちタスクを待ち解除してタスク切換えが発生
 *		(H-2) タスク切換えが発生しない
 *	  ※(G-8)の分解が必要
 *		(H-3) 待ち状態から正常終了（E_OK）
 *		(H-4) 待ち状態からエラー終了（E_OK以外） → (A-5)でカバー
 *	(I) receive_pridatqのC1カバレッジ
 *	  ※ここまでのテストで有効なもの
 *			(G-2)(G-3)(G-5)(G-8)
 *	  ※(G-2)の分解が必要
 *		(I-1) 送信待ち行列が空
 *		(I-2) 送信待ち行列が空でない → (G-3)でカバー
 *	(J) dequeue_dataのC1カバレッジ
 *	  ※内部に分岐がない
 *
 * 【使用リソース】
 *
 *	TASK1: 中優先度タスク，TA_ACT属性
 *	TASK2: 高優先度タスク
 *	TASK3: 低優先度タスク
 *	ALM1:  アラームハンドラ
 *	PDQ1:  TA_NULL属性，格納できるデータ数2
 *	PDQ2:  TA_NULL属性，格納できるデータ数0
 *	PDQ3:  TA_TPRI属性，格納できるデータ数2 … 使っていない
 *
 * 【テストシーケンス】
 *
 *	== TASK1（優先度：中）==
 *	1:	ref_pdq(PDQ1, &rpdq)
 *		assert(rpdq.stskid == TSK_NONE)
 *		assert(rpdq.rtskid == TSK_NONE)
 *		assert(rpdq.spdqcnt == 0)
 *		loc_cpu()
 *		snd_pdq(PDQ1, DATA0, HIGH_PRIORITY) -> E_CTX	... (A-2)
 *		rcv_pdq(PDQ1, &data, &pridata) -> E_CTX			... (F-2)
 *		unl_cpu()
 *		dis_dsp()
 *		snd_pdq(PDQ1, DATA0, HIGH_PRIORITY) -> E_CTX	... (A-3-1)
 *		rcv_pdq(PDQ1, &data, &pridata) -> E_CTX			... (F-3-1)
 *		ena_dsp()
 *		chg_ipm(TMAX_INTPRI)
 *		snd_pdq(PDQ1, DATA0, HIGH_PRIORITY) -> E_CTX	... (A-3-2)
 *		rcv_pdq(PDQ1, &data, &pridata) -> E_CTX			... (F-3-2)
 *		chg_ipm(TIPM_ENAALL)
 *		snd_pdq(0, DATA0, HIGH_PRIORITY) -> E_ID		... (A-4-1)
 *		rcv_pdq(0, &data, &pridata) -> E_ID				... (F-4-1)
 *		snd_pdq(TNUM_PDQID+1, DATA0, HIGH_PRIORITY) -> E_ID	... (A-4-2)
 *		rcv_pdq(TNUM_PDQID+1, &data, &pridata) -> E_ID	... (F-4-2)
 *		snd_pdq(PDQ1, DATA0, 0) -> E_PAR				... (A-8-1)
 *		snd_pdq(PDQ1, DATA0, 13) -> E_PAR				... (A-8-2)
 *		act_tsk(TASK3)
 *	2:	act_tsk(TASK2)
 *	== TASK2（優先度：高）==
 *	3:	rcv_pdq(PDQ1, &data, &pridata)					... (G-8)(H-3)
 *	== TASK1（続き）==
 *	4:	snd_pdq(PDQ1, DATA1, HIGH_PRIORITY)				... (B-1)(C-1)
 *	== TASK2（続き）==									... (B-2)
 *	5:	assert(data == DATA1)							... (G-1)
 *		assert(pridata == HIGH_PRIORITY)				... (G-1)
 *	6:	slp_tsk()
 *	== TASK1（続き）==
 *	7:	rcv_pdq(PDQ1, &data, &pridata)
 *	== TASK3（優先度：低）==
 *	8:	wup_tsk(TASK2)
 *	== TASK2（続き）==
 *	9:	snd_pdq(PDQ1, DATA2, MID_PRIORITY)				... (B-1)(C-2)
 *	10:	slp_tsk()
 *	== TASK1（続き）==
 *	11:	assert(data == DATA2)
 *		assert(pridata == MID_PRIORITY)
 *		snd_pdq(PDQ1, DATA3, HIGH_PRIORITY)				... (B-3)(E-2)(E-3)
 *		ref_pdq(PDQ1, &rpdq)
 *		assert(rpdq.stskid == TSK_NONE)
 *		assert(rpdq.rtskid == TSK_NONE)
 *		assert(rpdq.spdqcnt == 1)
 *		snd_pdq(PDQ1, DATA4, MID_PRIORITY)				... (B-3)(E-2)(E-4)
 *		ref_pdq(PDQ1, &rpdq)
 *		assert(rpdq.stskid == TSK_NONE)
 *		assert(rpdq.rtskid == TSK_NONE)
 *		assert(rpdq.spdqcnt == 2)
 *		snd_pdq(PDQ1, DATA5, MID_PRIORITY)				... (B-4)
 *	== TASK3（続き）==
 *	12:	rcv_pdq(PDQ1, &data, &pridata)					... (G-2)(G-3)(G-4)
 *	//														(E-1)(E-5)(H-1)
 *	== TASK1（続き）==
 *	13:	slp_tsk()
 *	== TASK3（続き）==
 *	14:	assert(data == DATA3)
 *		assert(pridata == HIGH_PRIORITY)
 *		rcv_pdq(PDQ1, &data, &pridata)					... (G-2)(H-2)(I-1)
 *		assert(data == DATA4)
 *		assert(pridata == MID_PRIORITY)
 *	15:	wup_tsk(TASK1)
 *	== TASK1（続き）==
 *	16:	snd_pdq(PDQ1, DATA0, HIGH_PRIORITY)				... (B-3)(E-1)(E-6)
 *	17:	slp_tsk()
 *	== TASK3（続き）==
 *	18:	rcv_pdq(PDQ1, &data, &pridata)
 *		assert(data == DATA0)
 *		assert(pridata == HIGH_PRIORITY)
 *		rcv_pdq(PDQ1, &data, &pridata)
 *		assert(data == DATA5)
 *		assert(pridata == MID_PRIORITY)
 *	19:	wup_tsk(TASK1)
 *	== TASK1（続き）==
 *	20:	snd_pdq(PDQ2, DATA1, HIGH_PRIORITY)
 *	== TASK3（続き）==
 *	21:	rcv_pdq(PDQ2, &data, &pridata)					... (G-5)(G-6)(G-7)
 *	== TASK1（続き）==
 *	22:	slp_tsk()
 *	== TASK3（続き）==
 *	23:	assert(data == DATA1)
 *		assert(pridata == HIGH_PRIORITY)
 *	24:	sta_alm(ALM1, TEST_TIME_CP) ... ALM1が実行開始するまで
 *	25:	rcv_pdq(PDQ2, &data, &pridata)
 *	== ALM1 ==
 *	26:	snd_pdq(PDQ2, DATA0, MID_PRIORITY) -> E_CTX		... (A-1)
 *		rcv_pdq(PDQ2, &data, &pridata) -> E_CTX			... (F-1)
 *		psnd_pdq(PDQ2, DATA2, LOW_PRIORITY)
 *	27:	RETURN
 *	== TASK3（続き）==
 *	28:	assert(data == DATA2)
 *		assert(pridata == LOW_PRIORITY)
 *	29:	wup_tsk(TASK1)
 *	== TASK1（続き）==
 *	30:	snd_pdq(PDQ2, DATA0, HIGH_PRIORITY) -> E_RLWAI	... (A-5)
 *	== TASK3（続き）==
 *	31:	rel_wai(TASK1)
 *	== TASK1（続き）==
 *	32:	snd_pdq(PDQ2, DATA0, HIGH_PRIORITY) -> E_DLT	... (A-7)
 *	== TASK3（続き）==
 *	33:	ini_pdq(PDQ2)
 *	== TASK1（続き）==
 *	34:	dis_ter()
 *	35:	snd_pdq(PDQ2, DATA0, HIGH_PRIORITY) -> E_RASTER	... (A-6-2)
 *	== TASK3（続き）==
 *	36:	ras_ter(TASK1)
 *	== TASK1（続き）==
 *	37:	snd_pdq(PDQ2, DATA0, HIGH_PRIORITY) -> E_RASTER	... (A-6-1)
 *	38:	wup_tsk(TASK2)
 *	== TASK2（続き）==
 *	39:	rcv_pdq(PDQ2, &data, &pridata) -> E_RLWAI		... (F-5)
 *	== TASK1（続き）==
 *	40:	ena_ter()
 *	== TASK3（続き）==
 *	41:	rel_wai(TASK2)
 *	== TASK2（続き）==
 *	42:	rcv_pdq(PDQ2, &data, &pridata) -> E_DLT			... (F-7)
 *	== TASK3（続き）==
 *	43:	ini_pdq(PDQ2)
 *	== TASK2（続き）==
 *	44:	dis_ter()
 *	45:	rcv_pdq(PDQ2, &data, &pridata) -> E_RASTER		... (F-6-2)
 *	== TASK3（続き）==
 *	46:	ras_ter(TASK2)
 *	== TASK2（続き）==
 *	47:	rcv_pdq(PDQ2, &data, &pridata) -> E_RASTER		... (F-6-1)
 *	48:	ena_ter()
 *	== TASK3（続き）==
 *	49:	END
 */

#include <kernel.h>
#include <t_syslog.h>
#include "syssvc/test_svc.h"
#include "kernel_cfg.h"
#include "test_common.h"

/*
 *  送受信するデータの定義
 */
#define	DATA0		((intptr_t) 0)
#define	DATA1		((intptr_t) 1)
#define	DATA2		((intptr_t) 2)
#define	DATA3		((intptr_t) 3)
#define	DATA4		((intptr_t) 4)
#define	DATA5		((intptr_t) 5)

/* DO NOT DELETE THIS LINE -- gentest depends on it. */

void
alarm1_handler(EXINF exinf)
{
	ER_UINT	ercd;
	intptr_t	data;
	PRI		pridata;

	check_point(26);
	ercd = snd_pdq(PDQ2, DATA0, MID_PRIORITY);
	check_ercd(ercd, E_CTX);

	ercd = rcv_pdq(PDQ2, &data, &pridata);
	check_ercd(ercd, E_CTX);

	ercd = psnd_pdq(PDQ2, DATA2, LOW_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(27);
	return;

	check_assert(false);
}

void
task1(EXINF exinf)
{
	ER_UINT	ercd;
	T_RPDQ	rpdq;
	intptr_t	data;
	PRI		pridata;

	test_start(__FILE__);

	check_point(1);
	ercd = ref_pdq(PDQ1, &rpdq);
	check_ercd(ercd, E_OK);

	check_assert(rpdq.stskid == TSK_NONE);

	check_assert(rpdq.rtskid == TSK_NONE);

	check_assert(rpdq.spdqcnt == 0);

	ercd = loc_cpu();
	check_ercd(ercd, E_OK);

	ercd = snd_pdq(PDQ1, DATA0, HIGH_PRIORITY);
	check_ercd(ercd, E_CTX);

	ercd = rcv_pdq(PDQ1, &data, &pridata);
	check_ercd(ercd, E_CTX);

	ercd = unl_cpu();
	check_ercd(ercd, E_OK);

	ercd = dis_dsp();
	check_ercd(ercd, E_OK);

	ercd = snd_pdq(PDQ1, DATA0, HIGH_PRIORITY);
	check_ercd(ercd, E_CTX);

	ercd = rcv_pdq(PDQ1, &data, &pridata);
	check_ercd(ercd, E_CTX);

	ercd = ena_dsp();
	check_ercd(ercd, E_OK);

	ercd = chg_ipm(TMAX_INTPRI);
	check_ercd(ercd, E_OK);

	ercd = snd_pdq(PDQ1, DATA0, HIGH_PRIORITY);
	check_ercd(ercd, E_CTX);

	ercd = rcv_pdq(PDQ1, &data, &pridata);
	check_ercd(ercd, E_CTX);

	ercd = chg_ipm(TIPM_ENAALL);
	check_ercd(ercd, E_OK);

	ercd = snd_pdq(0, DATA0, HIGH_PRIORITY);
	check_ercd(ercd, E_ID);

	ercd = rcv_pdq(0, &data, &pridata);
	check_ercd(ercd, E_ID);

	ercd = snd_pdq(TNUM_PDQID+1, DATA0, HIGH_PRIORITY);
	check_ercd(ercd, E_ID);

	ercd = rcv_pdq(TNUM_PDQID+1, &data, &pridata);
	check_ercd(ercd, E_ID);

	ercd = snd_pdq(PDQ1, DATA0, 0);
	check_ercd(ercd, E_PAR);

	ercd = snd_pdq(PDQ1, DATA0, 13);
	check_ercd(ercd, E_PAR);

	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	check_point(2);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(4);
	ercd = snd_pdq(PDQ1, DATA1, HIGH_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(7);
	ercd = rcv_pdq(PDQ1, &data, &pridata);
	check_ercd(ercd, E_OK);

	check_point(11);
	check_assert(data == DATA2);

	check_assert(pridata == MID_PRIORITY);

	ercd = snd_pdq(PDQ1, DATA3, HIGH_PRIORITY);
	check_ercd(ercd, E_OK);

	ercd = ref_pdq(PDQ1, &rpdq);
	check_ercd(ercd, E_OK);

	check_assert(rpdq.stskid == TSK_NONE);

	check_assert(rpdq.rtskid == TSK_NONE);

	check_assert(rpdq.spdqcnt == 1);

	ercd = snd_pdq(PDQ1, DATA4, MID_PRIORITY);
	check_ercd(ercd, E_OK);

	ercd = ref_pdq(PDQ1, &rpdq);
	check_ercd(ercd, E_OK);

	check_assert(rpdq.stskid == TSK_NONE);

	check_assert(rpdq.rtskid == TSK_NONE);

	check_assert(rpdq.spdqcnt == 2);

	ercd = snd_pdq(PDQ1, DATA5, MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(13);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(16);
	ercd = snd_pdq(PDQ1, DATA0, HIGH_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(17);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(20);
	ercd = snd_pdq(PDQ2, DATA1, HIGH_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(22);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(30);
	ercd = snd_pdq(PDQ2, DATA0, HIGH_PRIORITY);
	check_ercd(ercd, E_RLWAI);

	check_point(32);
	ercd = snd_pdq(PDQ2, DATA0, HIGH_PRIORITY);
	check_ercd(ercd, E_DLT);

	check_point(34);
	ercd = dis_ter();
	check_ercd(ercd, E_OK);

	check_point(35);
	ercd = snd_pdq(PDQ2, DATA0, HIGH_PRIORITY);
	check_ercd(ercd, E_RASTER);

	check_point(37);
	ercd = snd_pdq(PDQ2, DATA0, HIGH_PRIORITY);
	check_ercd(ercd, E_RASTER);

	check_point(38);
	ercd = wup_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(40);
	ercd = ena_ter();
	check_ercd(ercd, E_OK);

	check_assert(false);
}

void
task2(EXINF exinf)
{
	ER_UINT	ercd;
	intptr_t	data;
	PRI		pridata;

	check_point(3);
	ercd = rcv_pdq(PDQ1, &data, &pridata);
	check_ercd(ercd, E_OK);

	check_point(5);
	check_assert(data == DATA1);

	check_assert(pridata == HIGH_PRIORITY);

	check_point(6);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(9);
	ercd = snd_pdq(PDQ1, DATA2, MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(10);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(39);
	ercd = rcv_pdq(PDQ2, &data, &pridata);
	check_ercd(ercd, E_RLWAI);

	check_point(42);
	ercd = rcv_pdq(PDQ2, &data, &pridata);
	check_ercd(ercd, E_DLT);

	check_point(44);
	ercd = dis_ter();
	check_ercd(ercd, E_OK);

	check_point(45);
	ercd = rcv_pdq(PDQ2, &data, &pridata);
	check_ercd(ercd, E_RASTER);

	check_point(47);
	ercd = rcv_pdq(PDQ2, &data, &pridata);
	check_ercd(ercd, E_RASTER);

	check_point(48);
	ercd = ena_ter();
	check_ercd(ercd, E_OK);

	check_assert(false);
}

void
task3(EXINF exinf)
{
	ER_UINT	ercd;
	intptr_t	data;
	PRI		pridata;

	check_point(8);
	ercd = wup_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(12);
	ercd = rcv_pdq(PDQ1, &data, &pridata);
	check_ercd(ercd, E_OK);

	check_point(14);
	check_assert(data == DATA3);

	check_assert(pridata == HIGH_PRIORITY);

	ercd = rcv_pdq(PDQ1, &data, &pridata);
	check_ercd(ercd, E_OK);

	check_assert(data == DATA4);

	check_assert(pridata == MID_PRIORITY);

	check_point(15);
	ercd = wup_tsk(TASK1);
	check_ercd(ercd, E_OK);

	check_point(18);
	ercd = rcv_pdq(PDQ1, &data, &pridata);
	check_ercd(ercd, E_OK);

	check_assert(data == DATA0);

	check_assert(pridata == HIGH_PRIORITY);

	ercd = rcv_pdq(PDQ1, &data, &pridata);
	check_ercd(ercd, E_OK);

	check_assert(data == DATA5);

	check_assert(pridata == MID_PRIORITY);

	check_point(19);
	ercd = wup_tsk(TASK1);
	check_ercd(ercd, E_OK);

	check_point(21);
	ercd = rcv_pdq(PDQ2, &data, &pridata);
	check_ercd(ercd, E_OK);

	check_point(23);
	check_assert(data == DATA1);

	check_assert(pridata == HIGH_PRIORITY);

	check_point(24);
	ercd = sta_alm(ALM1, TEST_TIME_CP);
	check_ercd(ercd, E_OK);

	check_point(25);
	ercd = rcv_pdq(PDQ2, &data, &pridata);
	check_ercd(ercd, E_OK);

	check_point(28);
	check_assert(data == DATA2);

	check_assert(pridata == LOW_PRIORITY);

	check_point(29);
	ercd = wup_tsk(TASK1);
	check_ercd(ercd, E_OK);

	check_point(31);
	ercd = rel_wai(TASK1);
	check_ercd(ercd, E_OK);

	check_point(33);
	ercd = ini_pdq(PDQ2);
	check_ercd(ercd, E_OK);

	check_point(36);
	ercd = ras_ter(TASK1);
	check_ercd(ercd, E_OK);

	check_point(41);
	ercd = rel_wai(TASK2);
	check_ercd(ercd, E_OK);

	check_point(43);
	ercd = ini_pdq(PDQ2);
	check_ercd(ercd, E_OK);

	check_point(46);
	ercd = ras_ter(TASK2);
	check_ercd(ercd, E_OK);

	check_finish(49);
	check_assert(false);
}
