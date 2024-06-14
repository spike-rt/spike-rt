/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2007-2018 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: perf3.c 1439 2020-05-22 20:02:23Z ertl-hiro $
 */

/*
 *		カーネル性能評価プログラム(3)
 *
 *  待ち解除するタスクの数により，set_flgの処理時間がどのように変化する
 *  かを計測するためのプログラム．
 */

#include <kernel.h>
#include <t_syslog.h>
#include "syssvc/syslog.h"
#include "syssvc/test_svc.h"
#include "syssvc/histogram.h"
#include "kernel_cfg.h"
#include "perf3.h"

/*
 *  計測回数と実行時間分布を記録する最大時間
 */
#define NO_MEASURE	10000U			/* 計測回数 */

/*
 *  計測タスクのリスト
 */
static ID task_list[20] = {
	TASK1, TASK2, TASK3, TASK4, TASK5,
	TASK6, TASK7, TASK8, TASK9, TASK10,
	TASK11, TASK12, TASK13, TASK14, TASK15,
	TASK16, TASK17, TASK18, TASK19, TASK20
};

/*
 *  計測タスク（中優先度）
 */
void task(EXINF exinf)
{
	FLGPTN	flgptn;
	ER		ercd;

	ercd = wai_flg(FLG1, 0x01U, TWF_ORW, &flgptn);
	check_ercd(ercd, E_OK);
}

/*
 *  計測ルーチン
 */
void
perf_eval(uint_t n)
{
	uint_t	i, j;
	ER		ercd;

	ercd = init_hist(1);
	check_ercd(ercd, E_OK);

	for (i = 0; i < NO_MEASURE; i++) {
		ercd = ini_flg(FLG1);
		check_ercd(ercd, E_OK);

		for (j = 0; j < n; j++) {
			ercd = act_tsk(task_list[j]);
			check_ercd(ercd, E_OK);
		}
		ercd = chg_pri(TSK_SELF, MAIN_PRIORITY_LOW);
		check_ercd(ercd, E_OK);
		/* タスクが待ち状態に入るのを待つ */
		ercd = chg_pri(TSK_SELF, TPRI_INI);
		check_ercd(ercd, E_OK);

		ercd = begin_measure(1);
		check_ercd(ercd, E_OK);

		ercd = set_flg(FLG1, 0x01U);
		check_ercd(ercd, E_OK);

		ercd = end_measure(1);
		check_ercd(ercd, E_OK);

		ercd = chg_pri(TSK_SELF, MAIN_PRIORITY_LOW);
		check_ercd(ercd, E_OK);
		/* タスクが終了するのを待つ */
		ercd = chg_pri(TSK_SELF, TPRI_INI);
		check_ercd(ercd, E_OK);
	}

	syslog_1(LOG_NOTICE, "Execution times of set_flg"
							" when %d tasks are released from waiting.", n);
	ercd = print_hist(1);
	check_ercd(ercd, E_OK);
}

/*
 *  メインタスク（高優先度）
 */
void main_task(EXINF exinf)
{
	syslog_0(LOG_NOTICE, "Performance evaluation program (3)");

	perf_eval(0);
	perf_eval(1);
	perf_eval(2);
	perf_eval(3);
	perf_eval(4);
	perf_eval(5);
	perf_eval(10);
	perf_eval(20);
	check_finish(0);
}
