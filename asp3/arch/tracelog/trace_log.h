/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2004-2020 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: trace_log.h 1484 2020-10-28 01:30:39Z ertl-hiro $
 */

/*
 *		トレースログ機能のヘッダファイル
 *
 *  このインクルードファイルは，kernel_impl.hおよびトレースログ出力を
 *  行うシステムサービスからインクルードされる．また，トレースログ機能
 *  の初期化や記録の開始／停止，トレースログのダンプを行うプログラムか
 *  らインクルードすることを想定している．
 */

#ifndef TOPPERS_TRACE_LOG_H
#define TOPPERS_TRACE_LOG_H

/*
 *  トレースログのデータ構造
 *
 *  システムログ機能のログ情報のデータ構造と同じものを用いる．
 */
#ifndef TOPPERS_MACRO_ONLY

#include <t_syslog.h>
typedef	SYSLOG	TRACE;

#endif /* TOPPERS_MACRO_ONLY */

/*
 *  トレースモードの定義
 */
#define TRACE_STOP			UINT_C(0x00)	/* トレース停止 */
#define TRACE_RINGBUF		UINT_C(0x01)	/* リングバッファモード */
#define TRACE_AUTOSTOP		UINT_C(0x02)	/* 自動停止モード */
#define TRACE_CLEAR			UINT_C(0x04)	/* トレースログのクリア */

#ifndef TOPPERS_MACRO_ONLY

/*
 *  TECSで記述されたトレースログ機能を直接呼び出すための定義
 *
 *  C言語で記述されたアプリケーションから，TECSで記述されたトレースログ
 *  機能を呼び出すためには，アダプタを用いるのが正当な方法であるが，ト
 *  レースログ機能がシングルトンであることを利用して直接呼び出す．
 */
extern ER	tTraceLog_eTraceLog_start(MODE mode);
extern ER	tTraceLog_eTraceLog_write(const TRACE *p_trace);
extern ER	tTraceLog_eTraceLog_read(TRACE* p_trace);
extern void	tTraceLog_eTraceLog_dump(void);

/*
 *  トレースログの開始
 *
 *  トレースログの記録を開始／停止する．引数により次の動作を行う．
 *
 *  TRACE_STOP：トレースを停止．
 *  TRACE_RINGBUF：リングバッファモードでトレースを開始．
 *  TRACE_AUTOSTOP：自動停止モードでトレースを開始．
 *  TRACE_CLEAR：トレースログをクリア．
 */
Inline ER
trace_sta_log(MODE mode)
{
	return(tTraceLog_eTraceLog_start(mode));
}

/*
 *  トレースログの書込み
 */
Inline ER
trace_wri_log(TRACE *p_trace)
{
	return(tTraceLog_eTraceLog_write(p_trace));
}

/*
 *  トレースログの読出し
 */
Inline ER
trace_rea_log(TRACE *p_trace)
{
	return(tTraceLog_eTraceLog_read(p_trace));
}

/*
 *  トレースログを出力するためのライブラリ関数
 */

Inline void
trace_write_0(uint_t type)
{
	TRACE	trace;

	trace.logtype = type;
	(void) trace_wri_log(&trace);
}

Inline void
trace_write_1(uint_t type, LOGPAR arg1)
{
	TRACE	trace;

	trace.logtype = type;
	trace.logpar[0] = arg1;
	(void) trace_wri_log(&trace);
}

Inline void
trace_write_2(uint_t type, LOGPAR arg1, LOGPAR arg2)
{
	TRACE	trace;

	trace.logtype = type;
	trace.logpar[0] = arg1;
	trace.logpar[1] = arg2;
	(void) trace_wri_log(&trace);
}

Inline void
trace_write_3(uint_t type, LOGPAR arg1, LOGPAR arg2, LOGPAR arg3)
{
	TRACE	trace;

	trace.logtype = type;
	trace.logpar[0] = arg1;
	trace.logpar[1] = arg2;
	trace.logpar[2] = arg3;
	(void) trace_wri_log(&trace);
}

Inline void
trace_write_4(uint_t type, LOGPAR arg1, LOGPAR arg2, LOGPAR arg3, LOGPAR arg4)
{
	TRACE	trace;

	trace.logtype = type;
	trace.logpar[0] = arg1;
	trace.logpar[1] = arg2;
	trace.logpar[2] = arg3;
	trace.logpar[3] = arg4;
	(void) trace_wri_log(&trace);
}

Inline void
trace_write_5(uint_t type, LOGPAR arg1, LOGPAR arg2, LOGPAR arg3,
											LOGPAR arg4, LOGPAR arg5)
{
	TRACE	trace;

	trace.logtype = type;
	trace.logpar[0] = arg1;
	trace.logpar[1] = arg2;
	trace.logpar[2] = arg3;
	trace.logpar[3] = arg4;
	trace.logpar[4] = arg5;
	(void) trace_wri_log(&trace);
}

Inline void
trace_write_6(uint_t type, LOGPAR arg1, LOGPAR arg2, LOGPAR arg3,
								LOGPAR arg4, LOGPAR arg5, LOGPAR arg6)
{
	TRACE	trace;

	trace.logtype = type;
	trace.logpar[0] = arg1;
	trace.logpar[1] = arg2;
	trace.logpar[2] = arg3;
	trace.logpar[3] = arg4;
	trace.logpar[4] = arg5;
	trace.logpar[5] = arg6;
	(void) trace_wri_log(&trace);
}
														
/*
 *  トレースログを出力するためのマクロ
 */

#define trace_0(type) \
				trace_write_0(type)

#define trace_1(type, arg1) \
				trace_write_1(type, (LOGPAR)(arg1))

#define trace_2(type, arg1, arg2) \
				trace_write_2(type, (LOGPAR)(arg1), (LOGPAR)(arg2))

#define trace_3(type, arg1, arg2, arg3) \
				trace_write_3(type, (LOGPAR)(arg1), (LOGPAR)(arg2), \
														(LOGPAR)(arg3))

#define trace_4(type, arg1, arg2, arg3, arg4) \
				trace_write_4(type, (LOGPAR)(arg1), (LOGPAR)(arg2), \
										(LOGPAR)(arg3), (LOGPAR)(arg4))

#define trace_5(type, arg1, arg2, arg3, arg4, arg5) \
				trace_write_5(type, (LOGPAR)(arg1), (LOGPAR)(arg2), \
						(LOGPAR)(arg3), (LOGPAR)(arg4), (LOGPAR)(arg5))

#define trace_6(type, arg1, arg2, arg3, arg4, arg5, arg6) \
				trace_write_6(type, (LOGPAR)(arg1), (LOGPAR)(arg2), \
										(LOGPAR)(arg3), (LOGPAR)(arg4), \
										(LOGPAR)(arg4), (LOGPAR)(arg5))

/* 
 *  トレースログのダンプ
 */
Inline void
trace_dump(void)
{
	tTraceLog_eTraceLog_dump();
}

#endif /* TOPPERS_MACRO_ONLY */

/*
 *  トレースログ方法の設定
 */
#define LOG_TSKSTAT(p_tcb)		trace_2(LOG_TYPE_TSKSTAT, p_tcb, p_tcb->tstat)

#define LOG_DSP_LEAVE(p_tcb)	trace_1(LOG_TYPE_DSP|LOG_LEAVE, p_tcb)

#ifdef TOPPERS_SUPPORT_PROTECT
#define LOG_SCYC_START(p_sominib)	trace_1(LOG_TYPE_SCYC|LOG_START, p_sominib)
#define LOG_TWD_START(p_twdinib)	trace_1(LOG_TYPE_TWD|LOG_START, p_twdinib)
#endif /* TOPPERS_SUPPORT_PROTECT */

#define LOG_TSYSLOG_ESYSLOG_WRITE_ENTER(priority, p_syslog) \
								trace_wri_log((TRACE *) p_syslog)

#endif /* TOPPERS_TRACE_LOG_H */
