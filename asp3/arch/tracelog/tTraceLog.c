/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2005-2020 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: tTraceLog.c 1484 2020-10-28 01:30:39Z ertl-hiro $
 */

/*
 *		トレースログ機能
 */

#include "tTraceLog_tecsgen.h"
#include "kernel/kernel_impl.h"
#include "kernel/task.h"
#include "kernel/time_event.h"
#ifdef TOPPERS_SUPPORT_PROTECT
#include "kernel/domain.h"
#endif /* TOPPERS_SUPPORT_PROTECT */
#include <sil.h>
#include <log_output.h>
#include "target_syssvc.h"

/*
 *  トレース時刻の取り出し
 *
 *  デフォルトでは，ログ時刻として，高分解能タイマのカウント値を用いて
 *  いる．ターゲット依存で変更する場合には，TRACE_GET_LOGTIMに，ログ時
 *  刻を取り出すマクロを定義する．
 */
#ifndef TRACE_GET_LOGTIM
#define TRACE_GET_LOGTIM(p_logtim) \
				(*(p_logtim) = target_hrt_get_current())
#endif /* TRACE_GET_TIM */

/*
 *  トレースログの開始（受け口関数）
 */
ER
eTraceLog_start(MODE mode)
{
	if ((mode & TRACE_CLEAR) != 0U) {
		VAR_count = 0U;
		VAR_head = 0U;
		VAR_tail = 0U;
		mode &= ~TRACE_CLEAR;
	}
	VAR_mode = mode;
	return(E_OK);
}     

/* 
 *  トレースログの書込み（受け口関数）
 */
ER
eTraceLog_write(const TRACE *p_trace)
{
	SIL_PRE_LOC;

	if ((VAR_mode & (TRACE_RINGBUF | TRACE_AUTOSTOP)) != 0U) {
		SIL_LOC_INT();

		/*
		 *  トレース時刻の設定
		 */
		TRACE_GET_LOGTIM(&(((SYSLOG *) p_trace)->logtim));

		/*
		 *  トレースバッファに記録
		 */
		VAR_logBuffer[VAR_tail] = *p_trace;
		VAR_tail++;
		if (VAR_tail >= ATTR_logBufferSize) {
			VAR_tail = 0U;
		}
		if (VAR_count < ATTR_logBufferSize) {
			VAR_count++;
			if (VAR_count >= ATTR_logBufferSize
						&& (VAR_mode & TRACE_AUTOSTOP) != 0U) {
				VAR_mode = TRACE_STOP;
			}
		}
		else {
			VAR_head = VAR_tail;
		}

		SIL_UNL_INT();
	}
	return(E_OK);
}

/*
 *  トレースログの読出し（受け口関数）
 */
ER
eTraceLog_read(TRACE* p_trace)
{
	ER_UINT	ercd;
	SIL_PRE_LOC;

	SIL_LOC_INT();

	/*
	 *  トレースログバッファからの取出し
	 */
	if (VAR_count > 0U) {
		*p_trace = VAR_logBuffer[VAR_head];
		VAR_count--;
		VAR_head++;
		if (VAR_head >= ATTR_logBufferSize) {
			VAR_head = 0U;
		}
		ercd = E_OK;
	}
	else {
		ercd = E_OBJ;
	}

	SIL_UNL_INT();
	return(ercd);
}

/*
 *  アセンブリ言語で記述されるコードからトレースログを出力するための関
 *  数
 */
#ifdef LOG_DSP_ENTER

void
log_dsp_enter(TCB *p_tcb)
{
	LOG_DSP_ENTER(p_tcb);
}

#endif /* LOG_DSP_ENTER */
#ifdef LOG_DSP_LEAVE

void
log_dsp_leave(TCB *p_tcb)
{
	LOG_DSP_LEAVE(p_tcb);
}

#endif /* LOG_DSP_LEAVE */
#ifdef LOG_INH_ENTER

void
log_inh_enter(INHNO inhno)
{
	LOG_INH_ENTER(inhno);
}

#endif /* LOG_INH_ENTER */
#ifdef LOG_INH_LEAVE

void
log_inh_leave(INHNO inhno)
{
	LOG_INH_LEAVE(inhno);
}

#endif /* LOG_INH_LEAVE */
#ifdef LOG_EXC_ENTER

void
log_exc_enter(EXCNO excno)
{
	LOG_EXC_ENTER(excno);
}

#endif /* LOG_EXC_ENTER */
#ifdef LOG_EXC_LEAVE

void
log_exc_leave(EXCNO excno)
{
	LOG_EXC_LEAVE(excno);
}

#endif /* LOG_EXC_LEAVE */

#ifdef TOPPERS_SUPPORT_PROTECT
#ifdef LOG_EXTSVC_ENTER

void
log_extsvc_enter(FN fncd, intptr_t par1, intptr_t par2, intptr_t par3,
								intptr_t par4, intptr_t par5, ID cdmid)
{
	LOG_EXTSVC_ENTER(fncd, par1, par2, par3, par4, par5, cdmid);
}

#endif /* LOG_EXTSVC_ENTER */
#ifdef LOG_EXTSVC_LEAVE

void
log_extsvc_leave(FN fncd, ER ercd)
{
	LOG_EXTSVC_LEAVE(fncd, ercd);
}

#endif /* LOG_EXTSVC_LEAVE */
#endif /* TOPPERS_SUPPORT_PROTECT */

/* 
 *  カーネル情報の取出し
 */
static intptr_t
get_tskid(intptr_t info)
{
	TCB		*p_tcb;
	ID		tskid;

	p_tcb = (TCB *) info;
	if (p_tcb == NULL) {
		tskid = TSK_NONE;
	}
	else {
		tskid = TSKID(p_tcb);
	}
	return((intptr_t) tskid);
}

static intptr_t
get_tskstat(intptr_t info)
{
	uint_t		tstat;
	const char	*tstatstr;

	tstat = (uint_t) info;
	if (TSTAT_DORMANT(tstat)) {
		tstatstr = "DORMANT";
	}
	else {
		if (TSTAT_SUSPENDED(tstat)) {
			if (TSTAT_WAITING(tstat)) {
				tstatstr = "WAITING-SUSPENDED";
			}
			else {
				tstatstr = "SUSPENDED";
			}
		}
		else if (TSTAT_WAITING(tstat)) {
			tstatstr = "WAITING";
		}
		else {
			tstatstr = "RUNNABLE";
		}
	}
	return((intptr_t) tstatstr);
}

#ifdef TOPPERS_SUPPORT_PROTECT

static intptr_t
get_somid(intptr_t info)
{
	SOMINIB	*p_sominib;
	ID		somid;

	p_sominib = (SOMINIB *) info;
	if (p_sominib == NULL) {
		somid = TSOM_STP;
	}
	else {
		somid = SOMID(p_sominib);
	}
	return((intptr_t) somid);
}

static intptr_t
get_twd_domid(intptr_t info)
{
	TWDINIB	*p_twdinib;
	ID		domid;

	p_twdinib = (TWDINIB *) info;
	domid = (ID)(p_twdinib->p_dominib - dominib_table) + TMIN_DOMID;
	return((intptr_t) domid);
}

#endif /* TOPPERS_SUPPORT_PROTECT */

/* 
 *  トレースログの表示
 */
static void
trace_print(TRACE *p_trace, void (*putc)(char))
{
	intptr_t	traceinfo[TNUM_LOGPAR + 1];
	const char	*tracemsg;
	int_t		i;

	traceinfo[0] = (intptr_t)(p_trace->logtim);
	syslog_printf("[%d] ", traceinfo, putc);

	switch (p_trace->logtype) {
	case LOG_TYPE_TSKSTAT:
		traceinfo[0] = get_tskid(p_trace->logpar[0]);
		traceinfo[1] = get_tskstat(p_trace->logpar[1]);
		tracemsg = "task %d becomes %s.";
		break;
	case LOG_TYPE_DSP|LOG_LEAVE:
		traceinfo[0] = get_tskid(p_trace->logpar[0]);
		tracemsg = "dispatch to task %d.";
		break;

#ifdef TOPPERS_SUPPORT_PROTECT
	case LOG_TYPE_SCYC|LOG_START:
		traceinfo[0] = get_somid(p_trace->logpar[0]);
		tracemsg = "system cycle starts with system operating mode %d.";
		break;
	case LOG_TYPE_TWD|LOG_START:
		if (p_trace->logpar[0] == 0) {
			tracemsg = "idle window starts.";
		}
		else {
			traceinfo[0] = get_twd_domid(p_trace->logpar[0]);
			tracemsg = "time window for domain %d starts.";
		}
		break;
#endif /* TOPPERS_SUPPORT_PROTECT */

	case LOG_TYPE_COMMENT:
		for (i = 1; i < TNUM_LOGPAR; i++) {
			traceinfo[i-1] = p_trace->logpar[i];
		}
		tracemsg = (const char *)(p_trace->logpar[0]);
		break;
	case LOG_TYPE_ASSERT:
		traceinfo[0] = p_trace->logpar[0];
		traceinfo[1] = p_trace->logpar[1];
		traceinfo[2] = p_trace->logpar[2];
		tracemsg = "%s:%u: Assertion `%s' failed.";
		break;
	default:
		traceinfo[0] = p_trace->logtype;
		tracemsg = "unknown trace log type: %d.";
		break;
	}
	syslog_printf(tracemsg, traceinfo, putc);
	(*putc)('\n');
}

/*
 *  低レベル出力の呼出し関数
 */
static void
low_putchar(char c)
{
	cPutLog_putChar(c);
}

/*
 *  トレースログのダンプ（受け口関数）
 */
void
eDump_main(EXINF exinf)
{
	TRACE	trace;

	while (trace_rea_log(&trace) >= 0) {
		trace_print(&trace, low_putchar);
	}
}
