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
 *  $Id: perf2.c 1439 2020-05-22 20:02:23Z ertl-hiro $
 */

/*
 *		カーネル性能評価プログラム(2)
 *
 *  優先度データキューに蓄積されているデータの数により，snd_pdqの処理時
 *  間がどのように変化するかを計測するためのプログラム．
 */

#include <kernel.h>
#include <t_syslog.h>
#include "syssvc/syslog.h"
#include "syssvc/test_svc.h"
#include "syssvc/histogram.h"
#include "kernel_cfg.h"
#include "perf2.h"

/*
 *  計測回数と実行時間分布を記録する最大時間
 */
#define NO_MEASURE	10000U			/* 計測回数 */
/*
 *  計測ルーチン
 */
void
perf_eval(uint_t n)
{
	uint_t		i;
	intptr_t	data;
	PRI			pri;
	ER			ercd;

	ercd = ini_pdq(PDQ1);
	check_ercd(ercd, E_OK);

	ercd = init_hist(1);
	check_ercd(ercd, E_OK);

	for (i = 0; i < n; i++) {
		data = i;
		ercd = snd_pdq(PDQ1, data, 1);
		check_ercd(ercd, E_OK);
	}

	for (i = 0; i < NO_MEASURE; i++) {
		data = i;

		ercd = begin_measure(1);
		check_ercd(ercd, E_OK);

		ercd = snd_pdq(PDQ1, data, 2);
		check_ercd(ercd, E_OK);

		ercd = end_measure(1);
		check_ercd(ercd, E_OK);

		ercd = rcv_pdq(PDQ1, &data, &pri);
		check_ercd(ercd, E_OK);
	}

	syslog_1(LOG_NOTICE, "Execution times of snd_pdq"
								" when %d data are queued.", n);
	ercd = print_hist(1);
	check_ercd(ercd, E_OK);
}

/*
 *  メインタスク（低優先度）
 */
void main_task(EXINF exinf)
{
	syslog_0(LOG_NOTICE, "Performance evaluation program (2)");

	perf_eval(0);
	perf_eval(10);
	perf_eval(20);
	perf_eval(30);
	perf_eval(40);
	perf_eval(50);
	perf_eval(100);
	perf_eval(200);
	perf_eval(300);
	check_finish(0);
}
