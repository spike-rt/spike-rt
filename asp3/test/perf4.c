/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2006-2018 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: perf4.c 1439 2020-05-22 20:02:23Z ertl-hiro $
 */

/*
 *		カーネル性能評価プログラム(4)
 *
 *  act_tskの処理時間とタスク切換え時間を計測するためのプログラム．以下
 *  の3つの時間を測定する．
 *
 *  (1) タスクコンテキストから呼び出し，タスク切換えを起こさない
 *      act_tskの処理時間．自タスクよりも優先度の低いタスクに対して
 *      act_tskを発行し，休止状態から実行できる状態に遷移させる処理の時
 *      間．
 *
 *  (2) タスクコンテキストから呼び出し，タスク切換えを起こすact_tskの処
 *      理時間．自タスクよりも優先度の高いタスクに対してact_tskを発行し，
 *      休止状態から実行できる状態に遷移させ，タスク切換えを起こして，
 *      高い優先度のタスクの実行が始まるまでの時間．
 *
 *  (3) 非タスクコンテキストから呼び出し，タスク切換えを起こすact_tsk
 *      の処理時間．周期ハンドラから，実行状態のタスクよりも高い優先度
 *      のタスクに対してact_tskを発行し，休止状態から実行できる状態に遷
 *      移させたあとに周期ハンドラからリターンし，タスク切換えを起こし
 *      て，高い優先度のタスクの実行が始まるまでの時間．
 */

#include <kernel.h>
#include <t_syslog.h>
#include "syssvc/syslog.h"
#include "syssvc/test_svc.h"
#include "syssvc/histogram.h"
#include "kernel_cfg.h"
#include "perf4.h"

/*
 *  計測回数と実行時間分布を記録する最大時間
 */
#define NO_MEASURE	10000U			/* 計測回数 */

/*
 *  計測タスク1（高優先度）
 */
void task1(EXINF exinf)
{
	ER		ercd;

	ercd = end_measure(2);
	check_ercd(ercd, E_OK);

	ercd = ext_tsk();
	check_ercd(ercd, E_OK);
}

/*
 *  計測タスク2とメインタスクの共有変数
 */
volatile uint_t		task2_count;

/*
 *  計測タスク2（高優先度）
 */
void task2(EXINF exinf)
{
	ER		ercd;

	ercd = end_measure(3);
	check_ercd(ercd, E_OK);

	task2_count++;
	ercd = ext_tsk();
	check_ercd(ercd, E_OK);
}

/*
 *  計測タスク3（低優先度）
 */
void task3(EXINF exinf)
{
	ER		ercd;

	ercd = ext_tsk();
	check_ercd(ercd, E_OK);
}

/*
 *  計測タスク4（最低優先度）
 */
void task4(EXINF exinf)
{
	ER		ercd;

	while (true) {
		ercd = wup_tsk(MAIN_TASK);
		check_ercd(ercd, E_OK);
	}
}

/*
 *  計測タスク2とメインタスクの共有変数
 */
volatile uint_t		cyclic_handler_error_count;

/*
 *  周期ハンドラ
 */
void cyclic_handler(EXINF exinf)
{
	ER		ercd;

	ercd = begin_measure(3);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK2);
	if (ercd == E_QOVR) {
		/*
		 *  シミュレーション環境などで，TASK2の起動が遅れると，E_QOVR
		 *  エラーになる可能性がある．
		 */
		cyclic_handler_error_count++;
	}
	else {
		check_ercd(ercd, E_OK);
	}
}

/*
 *  メインタスク（中優先度）
 */
void main_task(EXINF exinf)
{
	uint_t	i;
	ER		ercd;

	syslog_0(LOG_NOTICE, "Performance evaluation program (4)");
	ercd = init_hist(1);
	check_ercd(ercd, E_OK);

	ercd = init_hist(2);
	check_ercd(ercd, E_OK);

	ercd = init_hist(3);
	check_ercd(ercd, E_OK);

	/*
	 *  タスクコンテキストから呼び出し，タスク切換えを起こさない
	 *  act_tskの処理時間の測定
	 */
	for (i = 0; i < NO_MEASURE; i++) {
		ercd = begin_measure(1);
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK3);
		check_ercd(ercd, E_OK);

		ercd = end_measure(1);
		check_ercd(ercd, E_OK);

		ercd = slp_tsk();
		check_ercd(ercd, E_OK);
	}

	/*
	 *  タスクコンテキストから呼び出し，タスク切換えを起こすact_tskの処
	 *  理時間の測定
	 */
	for (i = 0; i < NO_MEASURE; i++) {
		ercd = begin_measure(2);
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK1);
		check_ercd(ercd, E_OK);
	}

	/*
	 *  非タスクコンテキストから呼び出し，タスク切換えを起こすact_tskの
	 *  処理時間の測定（測定回数は10分の1）
	 */
	task2_count = 0;
	cyclic_handler_error_count = 0;

	ercd = sta_cyc(CYC1);
	check_ercd(ercd, E_OK);

	while (task2_count < NO_MEASURE / 10) ;
	ercd = stp_cyc(CYC1);
	check_ercd(ercd, E_OK);

	/*
	 *  測定結果の出力
	 */
	syslog_0(LOG_NOTICE,
		"Execution times of act_tsk from task context without task switch");
	ercd = print_hist(1);
	check_ercd(ercd, E_OK);

	syslog_0(LOG_NOTICE,
		"Execution times of act_tsk from task context with task switch");
	ercd = print_hist(2);
	check_ercd(ercd, E_OK);

	syslog_0(LOG_NOTICE,
		"Execution times of act_tsk from non-task context with task switch");
	ercd = print_hist(3);
	check_ercd(ercd, E_OK);

	if (cyclic_handler_error_count > 0) {
		syslog_1(LOG_NOTICE,
				"Number of E_QOVR errors : %d", cyclic_handler_error_count);
	}

	check_finish(0);
}
