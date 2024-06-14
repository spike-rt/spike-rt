/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2005-2018 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: task_refer.c 1497 2021-03-27 06:38:11Z ertl-hiro $
 */

/*
 *		タスクの状態参照機能
 */

#include "kernel_impl.h"
#include "check.h"
#include "task.h"
#include "wait.h"
#include "semaphore.h"
#include "eventflag.h"
#include "dataqueue.h"
#include "pridataq.h"
#include "mutex.h"
#include "mempfix.h"
#include "time_event.h"

/*
 *  トレースログマクロのデフォルト定義
 */
#ifndef LOG_REF_TSK_ENTER
#define LOG_REF_TSK_ENTER(tskid, pk_rtsk)
#endif /* LOG_REF_TSK_ENTER */

#ifndef LOG_REF_TSK_LEAVE
#define LOG_REF_TSK_LEAVE(ercd, pk_rtsk)
#endif /* LOG_REF_TSK_LEAVE */

/*
 *  タスクの状態参照［NGKI1217］
 */
#ifdef TOPPERS_ref_tsk

ER
ref_tsk(ID tskid, T_RTSK *pk_rtsk)
{
	TCB		*p_tcb;
	uint_t	tstat;
	ER		ercd;

	LOG_REF_TSK_ENTER(tskid, pk_rtsk);
	CHECK_TSKCTX_UNL();							/*［NGKI1218］［NGKI1219］*/
	if (tskid == TSK_SELF) {
		p_tcb = p_runtsk;						/*［NGKI1248］*/
	}
	else {
		CHECK_ID(VALID_TSKID(tskid));			/*［NGKI1220］*/
		p_tcb = get_tcb(tskid);
	}

	lock_cpu();
	tstat = p_tcb->tstat;
	if (TSTAT_DORMANT(tstat)) {
		/*
		 *  対象タスクが休止状態の場合［NGKI1225］
		 */
		pk_rtsk->tskstat = TTS_DMT;
	}
	else {
		/*
		 *  タスク状態の取出し［NGKI1225］
		 */
		if (TSTAT_SUSPENDED(tstat)) {
			if (TSTAT_WAITING(tstat)) {
				pk_rtsk->tskstat = TTS_WAS;
			}
			else {
				pk_rtsk->tskstat = TTS_SUS;
			}
		}
		else if (TSTAT_WAITING(tstat)) {
			pk_rtsk->tskstat = TTS_WAI;
		}
		else if (p_tcb == p_runtsk) {
			pk_rtsk->tskstat = TTS_RUN;
		}
		else {
			pk_rtsk->tskstat = TTS_RDY;
		}

		/*
		 *  現在優先度とベース優先度の取出し［NGKI1227］
		 */
		pk_rtsk->tskpri = EXT_TSKPRI(p_tcb->priority);
		pk_rtsk->tskbpri = EXT_TSKPRI(p_tcb->bpriority);

		if (TSTAT_WAITING(tstat)) {
			/*
			 *  待ち要因と待ち対象のオブジェクトのIDの取出し［NGKI1229］
			 *  ［NGKI1231］
			 */
			switch (tstat & TS_WAITING_MASK) {
			case TS_WAITING_SLP:
				pk_rtsk->tskwait = TTW_SLP;
				break;
			case TS_WAITING_DLY:
				pk_rtsk->tskwait = TTW_DLY;
				break;
			case TS_WAITING_SEM:
				pk_rtsk->tskwait = TTW_SEM;
				pk_rtsk->wobjid = SEMID(((WINFO_SEM *)(p_tcb->p_winfo))
																->p_semcb);
				break;
			case TS_WAITING_FLG:
				pk_rtsk->tskwait = TTW_FLG;
				pk_rtsk->wobjid = FLGID(((WINFO_FLG *)(p_tcb->p_winfo))
																->p_flgcb);
				break;
			case TS_WAITING_SDTQ:
				pk_rtsk->tskwait = TTW_SDTQ;
				pk_rtsk->wobjid = DTQID(((WINFO_SDTQ *)(p_tcb->p_winfo))
																->p_dtqcb);
				break;
			case TS_WAITING_RDTQ:
				pk_rtsk->tskwait = TTW_RDTQ;
				pk_rtsk->wobjid = DTQID(((WINFO_RDTQ *)(p_tcb->p_winfo))
																->p_dtqcb);
				break;
			case TS_WAITING_SPDQ:
				pk_rtsk->tskwait = TTW_SPDQ;
				pk_rtsk->wobjid = PDQID(((WINFO_SPDQ *)(p_tcb->p_winfo))
																->p_pdqcb);
				break;
			case TS_WAITING_RPDQ:
				pk_rtsk->tskwait = TTW_RPDQ;
				pk_rtsk->wobjid = PDQID(((WINFO_RPDQ *)(p_tcb->p_winfo))
																->p_pdqcb);
				break;
			case TS_WAITING_MTX:
				pk_rtsk->tskwait = TTW_MTX;
				pk_rtsk->wobjid = MTXID(((WINFO_MTX *)(p_tcb->p_winfo))
																->p_mtxcb);
				break;
			case TS_WAITING_MPF:
				pk_rtsk->tskwait = TTW_MPF;
				pk_rtsk->wobjid = MPFID(((WINFO_MPF *)(p_tcb->p_winfo))
																->p_mpfcb);
				break;
			}

			/*
			 *  タイムアウトするまでの時間の取出し
			 */
			if (p_tcb->p_winfo->p_tmevtb != NULL) {
				pk_rtsk->lefttmo				/*［NGKI1233］［NGKI1235］*/
						= (TMO) tmevt_lefttim(p_tcb->p_winfo->p_tmevtb);
			}
			else {
				pk_rtsk->lefttmo = TMO_FEVR;	/*［NGKI1234］*/
			}
		}

		/*
		 *  起床要求キューイング数の取出し［NGKI1239］
		 */
		pk_rtsk->wupcnt = p_tcb->wupque ? 1U : 0U;

		/*
		 *  タスク終了要求フラグの取出し［NGKI3467］
		 */
		pk_rtsk->raster = p_tcb->raster;

		/*
		 *  タスク終了禁止状態の取出し［NGKI3468］
		 */
		pk_rtsk->dister = !(p_tcb->enater);
	}

	/*
	 *  サブ優先度の取出し［NGKI3662］
	 */
	pk_rtsk->subpri = p_tcb->subpri;

	/*
	 *  起動要求キューイング数の取出し［NGKI1238］
	 */
	pk_rtsk->actcnt = p_tcb->actque ? 1U : 0U;
	ercd = E_OK;
	unlock_cpu();

  error_exit:
	LOG_REF_TSK_LEAVE(ercd, pk_rtsk);
	return(ercd);
}

#endif /* TOPPERS_ref_tsk */
