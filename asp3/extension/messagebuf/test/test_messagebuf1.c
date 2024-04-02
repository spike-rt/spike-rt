/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2014-2018 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: test_messagebuf1.c 1718 2022-10-22 14:45:54Z ertl-hiro $
 */

/* 
 *		メッセージバッファのテスト(1)
 *
 * 【テストの目的】
 *
 *  FIFO順メッセージバッファに，メッセージを送信する処理と受信する処理
 *  を一通りテストする．
 *
 * 【テスト項目】
 *
 *	(A) メッセージバッファへの送信処理（snd_mbf，send_message）
 *		(A-1) 受信待ちキューの先頭タスクが受信（タスク切換えなし）
 *		(A-2) 受信待ちキューの先頭タスクが受信（タスク切換えあり）
 *		(A-3) メッセージバッファ管理領域に格納
 *		(A-4) 送信待ちキューにタスクがあるために送信待ち状態に
 *		(A-5) メッセージバッファ管理領域に空きがないために送信待ち状態に
 *	(B) メッセージバッファからの受信処理（rcv_mbf，receive_message）
 *		(B-1) メッセージバッファ管理領域から受信
 *		(B-2) (B-1)＋送信待ちタスク（1つまたは複数）が待ち解除（タスク
 *			  切換えなし）
 *		(B-3) (B-1)＋送信待ちタスク（1つまたは複数）が待ち解除（タスク
 *			  切換えあり）
 *		(B-4) 送信待ちキューの先頭のタスクのメッセージを受信（タスク切
 *			  換えなし）
 *		(B-5) 送信待ちキューの先頭のタスクのメッセージを受信（タスク切
 *			  換えあり）
 *		(B-6) 受信待ち状態に
 *	(C) メッセージバッファ管理領域へのメッセージの格納（enqueue_message）
 *		(C-1) サイズとメッセージ本体を一連の領域に格納
 *		(C-2) サイズを格納後に管理領域の先頭に戻る
 *		(C-3) メッセージ本体の格納途中で管理領域の先頭に戻る
 *		(C-4) メッセージ本体の格納後に管理領域の先頭に戻る
 *	(D) メッセージバッファ管理領域からのメッセージの取出し（dequeue_message）
 *		(D-1) サイズとメッセージ本体を一連の領域から取出し
 *		(D-2) サイズを取出し後に管理領域の先頭に戻る
 *		(D-3) メッセージ本体の取出し途中で管理領域の先頭に戻る
 *		(D-4) メッセージ本体の取出し後に管理領域の先頭に戻る
 *
 * 【使用リソース】
 *
 *	TASK1: 高優先度タスク，メインタスク，最初から起動
 *	TASK2: 中優先度タスク
 *	TASK3: 低優先度タスク
 *	MBF1: メッセージバッファ（TA_NULL属性，最大メッセージサイズ：23，メッ
 *		  セージバッファ管理領域のサイズ：23→実際には24）
 *	 ※ メッセージバッファ管理領域のサイズは，MB_T型（＝intptr_t型）の
 *		サイズの倍数に切り上げられるため，intptr_t型のサイズが4でも8で
 *		も同じになるようなサイズを選んでいる．
 *
 * 【テストシーケンス】
 *
 *	== TASK1（優先度：高）==
 *	1:	act_tsk(TASK2)
 *		act_tsk(TASK3)
 *		ref_mbf(MBF1, &rmbf)
 *		assert(rmbf.stskid == TSK_NONE)
 *		assert(rmbf.rtskid == TSK_NONE)
 *		assert(rmbf.smbfcnt == 0)
 *		snd_mbf(MBF1, string1, 9)		... (A-3)(C-1)，使用：0〜15
 *	2:	snd_mbf(MBF1, string2, 3)		... (A-3)(C-4)，使用：0〜23
 *	3:	snd_mbf(MBF1, string3, 4)		... (A-5)
 *	== TASK2（優先度：中）==
 *	4:	ref_mbf(MBF1, &rmbf)
 *		assert(rmbf.stskid == TASK1)
 *		assert(rmbf.rtskid == TSK_NONE)
 *		assert(rmbf.smbfcnt == 2)
 *	5:	snd_mbf(MBF1, string1, 4)		... (A-4)
 *	== TASK3（優先度：低）==
 *	6:	rcv_mbf(MBF1, buf1) -> 9		... (B-3)(D-1)(C-1)，使用：16〜23,0〜15
 *	== TASK1（続き）==
 *	7:	assert(memcmp(buf1, string1, 9) == 0)
 *		slp_tsk()
 *	== TASK2（続き）==
 *	8:	rcv_mbf(MBF1, buf1) -> 3		... (B-1)(D-4)，使用：0〜15
 *		assert(memcmp(buf1, string2, 3) == 0)
 *		rcv_mbf(MBF1, buf1) -> 4		... (B-1)(D-1)，使用：8〜15
 *		assert(memcmp(buf1, string3, 4) == 0)
 *		rcv_mbf(MBF1, buf1) -> 4		... (B-1)(D-1)，使用：なし
 *		assert(memcmp(buf1, string1, 4) == 0)
 *	9:	rcv_mbf(MBF1, buf1) -> 10		... (B-6)
 *	== TASK3（続き）==
 *	10:	ref_mbf(MBF1, &rmbf)
 *		assert(rmbf.stskid == TSK_NONE)
 *		assert(rmbf.rtskid == TASK2)
 *		assert(rmbf.smbfcnt == 0)
 *	11:	snd_mbf(MBF1, string2, 10)		... (A-2)
 *	== TASK2（続き）==
 *	12:	assert(memcmp(buf1, string2, 10) == 0)
 *		rcv_mbf(MBF1, buf1) -> 11		... (B-6)
 *	== TASK3（続き）==
 *	13:	wup_tsk(TASK1)
 *	== TASK1（続き）==
 *	14:	snd_mbf(MBF1, string3, 11)		... (A-1)
 *		assert(memcmp(buf1, string3, 11) == 0)
 *		snd_mbf(MBF1, string1, 12)		... (A-3)(C-3)，使用：16〜23,0〜7
 *		tslp_tsk(2 * TEST_TIME_CP) -> E_TMOUT ... TASK1が実行再開するまで
 *	== TASK2（続き）==
 *	15:	slp_tsk()
 *	== TASK3（続き）==
 *	16:	snd_mbf(MBF1, string2, 8)		... (A-5)
 *	== TASK1（続き）==
 *	17:	wup_tsk(TASK2)
 *		tslp_tsk(TEST_TIME_CP) -> E_TMOUT ... TASK1が実行再開するまで
 *	== TASK2（続き）==
 *	18:	snd_mbf(MBF1, string3, 4)		... (A-4)
 *	== TASK1（続き）==
 *	19:	rcv_mbf(MBF1, buf1) -> 12		... (B-2)(D-3)(C-1)(C-2)，
 *										... 				使用：8〜23,0〜3
 *		assert(memcmp(buf1, string1, 12) == 0)
 *		slp_tsk()
 *	== TASK2（続き）==
 *	20:	rcv_mbf(MBF1, buf1) -> 8		... (B-1)(D-1)，使用：24〜27，0〜3
 *		assert(memcmp(buf1, string2, 8) == 0)
 *	21:	snd_mbf(MBF1, string1, 23)		... (A-5)
 *	== TASK3（続き）==
 *	22:	rcv_mbf(MBF1, buf1) -> 4		... (B-1)(D-2)，使用：なし
 *		assert(memcmp(buf1, string3, 4) == 0)
 *		rcv_mbf(MBF1, buf1) -> 23		... (B-5)
 *	== TASK2（続き）==
 *	23:	assert(memcmp(buf1, string1, 23) == 0)
 *		snd_mbf(MBF1, string2, 23)		... (A-5)
 *	== TASK3（続き）==
 *	24:	wup_tsk(TASK1)
 *	== TASK1（続き）==
 *	25:	rcv_mbf(MBF1, buf1) -> 23		... (B-4)
 *		assert(memcmp(buf1, string2, 23) == 0)
 *		slp_tsk()
 *	== TASK2（続き）==
 *	26:	slp_tsk()
 *	== TASK3（続き）==
 *	27:	END
 */

#include <kernel.h>
#include <t_syslog.h>
#include "syssvc/test_svc.h"
#include "kernel_cfg.h"
#include "test_common.h"
#include <string.h>

const char string1[23] = "abcdefghijklmnopqrstuvw";
const char string2[23] = "ABCDEFGHIJKLMNOPQRSTUVW";
const char string3[16] = "0123456789abcdef";

char buf1[23];

/* DO NOT DELETE THIS LINE -- gentest depends on it. */

void
task1(EXINF exinf)
{
	ER_UINT	ercd;
	T_RMBF	rmbf;

	test_start(__FILE__);

	check_point(1);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	ercd = ref_mbf(MBF1, &rmbf);
	check_ercd(ercd, E_OK);

	check_assert(rmbf.stskid == TSK_NONE);

	check_assert(rmbf.rtskid == TSK_NONE);

	check_assert(rmbf.smbfcnt == 0);

	ercd = snd_mbf(MBF1, string1, 9);
	check_ercd(ercd, E_OK);

	check_point(2);
	ercd = snd_mbf(MBF1, string2, 3);
	check_ercd(ercd, E_OK);

	check_point(3);
	ercd = snd_mbf(MBF1, string3, 4);
	check_ercd(ercd, E_OK);

	check_point(7);
	check_assert(memcmp(buf1, string1, 9) == 0);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(14);
	ercd = snd_mbf(MBF1, string3, 11);
	check_ercd(ercd, E_OK);

	check_assert(memcmp(buf1, string3, 11) == 0);

	ercd = snd_mbf(MBF1, string1, 12);
	check_ercd(ercd, E_OK);

	ercd = tslp_tsk(2 * TEST_TIME_CP);
	check_ercd(ercd, E_TMOUT);

	check_point(17);
	ercd = wup_tsk(TASK2);
	check_ercd(ercd, E_OK);

	ercd = tslp_tsk(TEST_TIME_CP);
	check_ercd(ercd, E_TMOUT);

	check_point(19);
	ercd = rcv_mbf(MBF1, buf1);
	check_ercd(ercd, 12);

	check_assert(memcmp(buf1, string1, 12) == 0);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(25);
	ercd = rcv_mbf(MBF1, buf1);
	check_ercd(ercd, 23);

	check_assert(memcmp(buf1, string2, 23) == 0);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_assert(false);
}

void
task2(EXINF exinf)
{
	ER_UINT	ercd;
	T_RMBF	rmbf;

	check_point(4);
	ercd = ref_mbf(MBF1, &rmbf);
	check_ercd(ercd, E_OK);

	check_assert(rmbf.stskid == TASK1);

	check_assert(rmbf.rtskid == TSK_NONE);

	check_assert(rmbf.smbfcnt == 2);

	check_point(5);
	ercd = snd_mbf(MBF1, string1, 4);
	check_ercd(ercd, E_OK);

	check_point(8);
	ercd = rcv_mbf(MBF1, buf1);
	check_ercd(ercd, 3);

	check_assert(memcmp(buf1, string2, 3) == 0);

	ercd = rcv_mbf(MBF1, buf1);
	check_ercd(ercd, 4);

	check_assert(memcmp(buf1, string3, 4) == 0);

	ercd = rcv_mbf(MBF1, buf1);
	check_ercd(ercd, 4);

	check_assert(memcmp(buf1, string1, 4) == 0);

	check_point(9);
	ercd = rcv_mbf(MBF1, buf1);
	check_ercd(ercd, 10);

	check_point(12);
	check_assert(memcmp(buf1, string2, 10) == 0);

	ercd = rcv_mbf(MBF1, buf1);
	check_ercd(ercd, 11);

	check_point(15);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(18);
	ercd = snd_mbf(MBF1, string3, 4);
	check_ercd(ercd, E_OK);

	check_point(20);
	ercd = rcv_mbf(MBF1, buf1);
	check_ercd(ercd, 8);

	check_assert(memcmp(buf1, string2, 8) == 0);

	check_point(21);
	ercd = snd_mbf(MBF1, string1, 23);
	check_ercd(ercd, E_OK);

	check_point(23);
	check_assert(memcmp(buf1, string1, 23) == 0);

	ercd = snd_mbf(MBF1, string2, 23);
	check_ercd(ercd, E_OK);

	check_point(26);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_assert(false);
}

void
task3(EXINF exinf)
{
	ER_UINT	ercd;
	T_RMBF	rmbf;

	check_point(6);
	ercd = rcv_mbf(MBF1, buf1);
	check_ercd(ercd, 9);

	check_point(10);
	ercd = ref_mbf(MBF1, &rmbf);
	check_ercd(ercd, E_OK);

	check_assert(rmbf.stskid == TSK_NONE);

	check_assert(rmbf.rtskid == TASK2);

	check_assert(rmbf.smbfcnt == 0);

	check_point(11);
	ercd = snd_mbf(MBF1, string2, 10);
	check_ercd(ercd, E_OK);

	check_point(13);
	ercd = wup_tsk(TASK1);
	check_ercd(ercd, E_OK);

	check_point(16);
	ercd = snd_mbf(MBF1, string2, 8);
	check_ercd(ercd, E_OK);

	check_point(22);
	ercd = rcv_mbf(MBF1, buf1);
	check_ercd(ercd, 4);

	check_assert(memcmp(buf1, string3, 4) == 0);

	ercd = rcv_mbf(MBF1, buf1);
	check_ercd(ercd, 23);

	check_point(24);
	ercd = wup_tsk(TASK1);
	check_ercd(ercd, E_OK);

	check_finish(27);
	check_assert(false);
}
