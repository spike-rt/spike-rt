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
 *  $Id: syslog.c 1437 2020-05-20 12:12:16Z ertl-hiro $
 */

/*
 *		システムログ機能（非TECS版専用）
 */

#include <sil.h>
#undef TOPPERS_OMIT_SYSLOG
#include <t_syslog.h>
#include <log_output.h>
#include "target_syssvc.h"
#include "syslog.h"

/*
 *  ログバッファのサイズ
 */
#ifndef TCNT_SYSLOG_BUFFER
#define TCNT_SYSLOG_BUFFER	32		/* ログバッファのサイズ */
#endif /* TCNT_SYSLOG_BUFFER */

/*
 *  トレースログマクロのデフォルト定義
 *
 *  システムログに出力されたログ情報をトレースログにも記録できるように，
 *  syslog_wri_logの入口にトレースログマクロを置く．
 */
#ifndef LOG_SYSLOG_WRI_LOG_ENTER
#define LOG_SYSLOG_WRI_LOG_ENTER(prio, p_syslog)
#endif /* LOG_SYSLOG_WRI_LOG_ENTER */

/*
 *  ログ時刻の取り出し
 *
 *  デフォルトでは，ログ時刻として，高分解能タイマのカウント値を用いて
 *  いる．ターゲット依存で変更する場合には，SYSLOG_GET_LOGTIMに，ログ時
 *  刻を取り出すマクロを定義する．
 */
#ifndef SYSLOG_GET_LOGTIM
#define target_hrt_get_current		_kernel_target_hrt_get_current
#include "target_timer.h"
#define SYSLOG_GET_LOGTIM(p_logtim) \
				(*(p_logtim) = _kernel_target_hrt_get_current())
#endif /* SYSLOG_GET_LOGTIM */

/*
 *  ログバッファとそれにアクセスするためのポインタ
 */
static SYSLOG	syslog_buffer[TCNT_SYSLOG_BUFFER];	/* ログバッファ */
static uint_t	syslog_count;			/* ログバッファ中のログの数 */
static uint_t	syslog_head;			/* 先頭のログの格納位置 */
static uint_t	syslog_tail;			/* 次のログの格納位置 */
static uint_t	syslog_lost;			/* 失われたログの数 */

/*
 *  出力すべきログ情報の重要度（ビットマップ）
 */
static uint_t	syslog_logmask;			/* ログバッファに記録すべき重要度 */
static uint_t	syslog_lowmask_not;		/* 低レベル出力すべき重要度（反転）*/

/*
 *  システムログ機能の初期化
 */
void
syslog_initialize(EXINF exinf)
{
	syslog_count = 0U;
	syslog_head = 0U;
	syslog_tail = 0U;
	syslog_lost = 0U;
	syslog_logmask = 0U;
	syslog_lowmask_not = 0U;
}     

/* 
 *  ログ情報の出力
 *
 *  CPUロック状態や実行コンテキストによらず動作できるように実装してある．
 */
ER
syslog_wri_log(uint_t prio, const SYSLOG *p_syslog)
{
	SIL_PRE_LOC;

	LOG_SYSLOG_WRI_LOG_ENTER(prio, p_syslog);
	SIL_LOC_INT();

	/*
	 *  ログバッファに記録
	 */
	if ((syslog_logmask & LOG_MASK(prio)) != 0U) {
		syslog_buffer[syslog_tail] = *p_syslog;
		SYSLOG_GET_LOGTIM(&(syslog_buffer[syslog_tail].logtim));

		syslog_tail++;
		if (syslog_tail >= TCNT_SYSLOG_BUFFER) {
			syslog_tail = 0U;
		}
		if (syslog_count < TCNT_SYSLOG_BUFFER) {
			syslog_count++;
		}
		else {
			syslog_head = syslog_tail;
			syslog_lost++;
		}
	}

	/*
	 *  低レベル出力
	 */
	if (((~syslog_lowmask_not) & LOG_MASK(prio)) != 0U) {
		SYSLOG	logbuf;

		logbuf = *p_syslog;
		SYSLOG_GET_LOGTIM(&(logbuf.logtim));
		syslog_print(&logbuf, target_fput_log);
	}

	SIL_UNL_INT();
	return(E_OK);
}

/*
 *  ログバッファからの読出し
 *
 *  CPUロック状態や実行コンテキストによらず動作できるように実装してある．
 */
static ER_UINT
read_log(SYSLOG *p_syslog)
{
	ER_UINT	ercd;

	/*
	 *  ログバッファからの取出し
	 */
	if (syslog_count > 0U) {
		*p_syslog = syslog_buffer[syslog_head];
		syslog_count--;
		syslog_head++;
		if (syslog_head >= TCNT_SYSLOG_BUFFER) {
			syslog_head = 0U;
		}
		ercd = (ER_UINT) syslog_lost;
		syslog_lost = 0U;
	}
	else {
		ercd = E_OBJ;
	}
	return(ercd);
}

ER_UINT
syslog_rea_log(SYSLOG *p_syslog)
{
	ER_UINT	ercd;
	SIL_PRE_LOC;

	SIL_LOC_INT();
	ercd = read_log(p_syslog);
	SIL_UNL_INT();
	return(ercd);
}

/* 
 *  出力すべきログ情報の重要度の設定
 */
ER
syslog_msk_log(uint_t logmask, uint_t lowmask)
{
	SIL_PRE_LOC;

	SIL_LOC_INT();
	syslog_logmask = logmask;
	syslog_lowmask_not = ~lowmask;
	SIL_UNL_INT();
	return(E_OK);
}

/*
 *  ログバッファの状態参照
 */
ER
syslog_ref_log(T_SYSLOG_RLOG *pk_rlog)
{
	SIL_PRE_LOC;

	SIL_LOC_INT();
	pk_rlog->count = syslog_count;
	pk_rlog->lost = syslog_lost;
	pk_rlog->logmask = syslog_logmask;
	pk_rlog->lowmask = ~syslog_lowmask_not;
	SIL_UNL_INT();
	return(E_OK);
}

/* 
 *  低レベル出力によるすべてのログ情報の出力
 */
ER
syslog_fls_log(void)
{
	SYSLOG	logbuf;
	ER_UINT	rercd;
	SIL_PRE_LOC;

	SIL_LOC_INT();

	while ((rercd = read_log(&logbuf)) >= 0) {
		if (((uint_t) rercd) > 0U) {
			syslog_lostmsg((uint_t) rercd, target_fput_log);
		}
		syslog_print(&logbuf, target_fput_log);
	}

	SIL_UNL_INT();
	return(E_OK);
}
