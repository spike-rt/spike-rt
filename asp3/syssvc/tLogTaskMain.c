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
 *  $Id: tLogTaskMain.c 1437 2020-05-20 12:12:16Z ertl-hiro $
 */

/*
 *		システムログタスク
 */

#include <kernel.h>
#include "tLogTaskMain_tecsgen.h"
#include <t_syslog.h>
#include <log_output.h>

/*
 *  シリアルインタフェースへの1文字出力
 */
static void
logtask_putc(char c)
{
	cSerialPort_write(&c, 1);
}

/*
 *  低レベル出力への1文字出力
 */
static void
target_fput_log(char c)
{
	cPutLog_putChar(c);
}

/*
 *  システムログ出力の待ち合わせ（受け口関数）
 */
ER
eLogTask_flush(uint_t count)
{
	T_SYSLOG_RLOG	rlog;
	T_SERIAL_RPOR	rpor;
	ER				ercd, rercd;

	if (sns_dpn()) {
		ercd = E_CTX;
	}
	else {
		while (true) {
			if (cSysLog_refer(&rlog) < 0) {
				ercd = E_SYS;
				goto error_exit;
			}
			if (rlog.count <= count) {
				if (count == 0U) {
					/*
					 *  countが0の場合には，シリアルバッファが空かを確
					 *  認する．
					 */
					if (cSerialPort_refer(&rpor) < 0) {
						ercd = E_SYS;
						goto error_exit;
					}
					if (rpor.wricnt == 0U) {
						ercd = E_OK;
						goto error_exit;
					}
				}
				else {
					ercd = E_OK;
					goto error_exit;
				}
			}

			/*
			 *  フラッシュ待ちの単位時間（ATTR_flushWaitμ秒）待つ．
			 */
			rercd = dly_tsk(ATTR_flushWait);
			if (rercd < 0) {
				ercd = (MERCD(rercd) == E_RLWAI) ? rercd : E_SYS;
				goto error_exit;
			}
		}
	}

  error_exit:
	return(ercd);
}

/*
 *  システムログタスクの本体（受け口関数）
 */
void
eLogTaskBody_main(void)
{
	SYSLOG	syslog;
	ER_UINT	rercd;

	cSerialPort_open();
	syslog_0(LOG_NOTICE, "System logging task is started.");

	while (true) {
		while ((rercd = cSysLog_read(&syslog)) >= 0) {
			if (((uint_t) rercd) > 0U) {
				syslog_lostmsg((uint_t) rercd, logtask_putc);
			}
			syslog_print(&syslog, logtask_putc);
		}
		(void) dly_tsk(ATTR_interval);
	}
}

/*
 *  システムログタスクの終了処理（受け口関数）
 */
void
eLogTaskTerminate_main(EXINF exinf)
{
	char	c;
	SYSLOG	syslog;
	bool_t	msgflg = false;
	ER_UINT	rercd;

	/*
	 *  シリアルインタフェースドライバの送信バッファに蓄積されたデータ
	 *  を，低レベル出力機能を用いて出力する．
	 */
	while (cnSerialPortManage_getChar(&c)) {
		target_fput_log(c);
	}

	/*
	 *  ログバッファに記録されたログ情報を，低レベル出力機能を用いて出
	 *  力する．
	 */
	while ((rercd = cSysLog_read(&syslog)) >= 0) {
		if (!msgflg) {
			/*
			 *  ログバッファに残ったログ情報であることを示す文字列を出
			 *  力する．
			 */
			syslog_printf("-- buffered messages --\n", NULL, target_fput_log);
			msgflg = true;
		}
		if (rercd > 0) {
			syslog_lostmsg((uint_t) rercd, target_fput_log);
		}
		syslog_print(&syslog, target_fput_log);
	}
}
