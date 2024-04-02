/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2006-2015 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: core_timer.c 1799 2023-04-01 00:50:30Z ertl-komori $
 */

/*
 *		タイマドライバ（SYSTICK用）
 *		タイムティックを用いたタイマドライバの実装
 */

#include "kernel_impl.h"
#include "time_event.h"
#include "target_timer.h"
#include <sil.h>

/*
 *  高分解能タイマの現時点の値を保持する変数
 */
HRTCNT hrtcnt_current;

/*
 *  タイマの起動処理
 */
void
target_hrt_initialize(intptr_t exinf)
{
	uint32_t cyc;
	uint32_t tmp;

	cyc = ((CPU_CLOCK_HZ/1000000)*TSTEP_HRTCNT) - 1;
	/* 停止 */
	tmp = sil_rew_mem((void *)SYSTIC_CONTROL_STATUS);
	tmp &= ~SYSTIC_ENABLE;
	sil_wrw_mem((void *)SYSTIC_CONTROL_STATUS, tmp);

	sil_wrw_mem((void *)SYSTIC_RELOAD_VALUE, cyc);
	sil_wrw_mem((void *)SYSTIC_CURRENT_VALUE, cyc);

	tmp = sil_rew_mem((void *)SYSTIC_CONTROL_STATUS);

	/* プロセッサクロックの使用 */
	tmp |= SYSTIC_ENABLE|SYSTIC_CLKSOURCE;

	sil_wrw_mem((void *)SYSTIC_CONTROL_STATUS, tmp);

	hrtcnt_current = 0;
}

/*
 *  タイマの停止処理
 */
void
target_hrt_terminate(intptr_t exinf)
{
	/* タイマを停止 */
	sil_wrw_mem((void*)SYSTIC_CONTROL_STATUS, 0x00);
}

/*
 *  タイマ割込みハンドラ
 */
void
target_hrt_handler(void)
{

	hrtcnt_current += TSTEP_HRTCNT;

	/*
	 *  高分解能タイマ割込みを処理する．
	 */
	signal_time();
}
