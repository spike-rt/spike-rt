/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
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
 *  $Id: messagebuf.c 1450 2020-06-26 14:40:17Z ertl-hiro $
 */

/*
 *		メッセージバッファ機能
 */

#include "kernel_impl.h"
#include "check.h"
#include "task.h"
#include "taskhook.h"
#include "wait.h"
#include "messagebuf.h"
#include <string.h>

/*
 *  トレースログマクロのデフォルト定義
 */
#ifndef LOG_SND_MBF_ENTER
#define LOG_SND_MBF_ENTER(mbfid, msg, msgsz)
#endif /* LOG_SND_MBF_ENTER */

#ifndef LOG_SND_MBF_LEAVE
#define LOG_SND_MBF_LEAVE(ercd)
#endif /* LOG_SND_MBF_LEAVE */

#ifndef LOG_PSND_MBF_ENTER
#define LOG_PSND_MBF_ENTER(mbfid, msg, msgsz)
#endif /* LOG_PSND_MBF_ENTER */

#ifndef LOG_PSND_MBF_LEAVE
#define LOG_PSND_MBF_LEAVE(ercd)
#endif /* LOG_PSND_MBF_LEAVE */

#ifndef LOG_TSND_MBF_ENTER
#define LOG_TSND_MBF_ENTER(mbfid, msg, msgsz, tmout)
#endif /* LOG_TSND_MBF_ENTER */

#ifndef LOG_TSND_MBF_LEAVE
#define LOG_TSND_MBF_LEAVE(ercd)
#endif /* LOG_TSND_MBF_LEAVE */

#ifndef LOG_RCV_MBF_ENTER
#define LOG_RCV_MBF_ENTER(mbfid, msg)
#endif /* LOG_RCV_MBF_ENTER */

#ifndef LOG_RCV_MBF_LEAVE
#define LOG_RCV_MBF_LEAVE(msgsz, msg)
#endif /* LOG_RCV_MBF_LEAVE */

#ifndef LOG_PRCV_MBF_ENTER
#define LOG_PRCV_MBF_ENTER(mbfid, msg)
#endif /* LOG_PRCV_MBF_ENTER */

#ifndef LOG_PRCV_MBF_LEAVE
#define LOG_PRCV_MBF_LEAVE(msgsz, msg)
#endif /* LOG_PRCV_MBF_LEAVE */

#ifndef LOG_TRCV_MBF_ENTER
#define LOG_TRCV_MBF_ENTER(mbfid, msg, tmout)
#endif /* LOG_TRCV_MBF_ENTER */

#ifndef LOG_TRCV_MBF_LEAVE
#define LOG_TRCV_MBF_LEAVE(msgsz, msg)
#endif /* LOG_TRCV_MBF_LEAVE */

#ifndef LOG_INI_MBF_ENTER
#define LOG_INI_MBF_ENTER(mbfid)
#endif /* LOG_INI_MBF_ENTER */

#ifndef LOG_INI_MBF_LEAVE
#define LOG_INI_MBF_LEAVE(ercd)
#endif /* LOG_INI_MBF_LEAVE */

#ifndef LOG_REF_MBF_ENTER
#define LOG_REF_MBF_ENTER(mbfid, pk_rmbf)
#endif /* LOG_REF_MBF_ENTER */

#ifndef LOG_REF_MBF_LEAVE
#define LOG_REF_MBF_LEAVE(ercd, pk_rmbf)
#endif /* LOG_REF_MBF_LEAVE */

/*
 *  メッセージバッファの数
 */
#define tnum_mbf	((uint_t)(tmax_mbfid - TMIN_MBFID + 1))

/*
 *  メッセージバッファIDからメッセージバッファ管理ブロックを取り出すた
 *  めのマクロ
 */
#define INDEX_MBF(mbfid)	((uint_t)((mbfid) - TMIN_MBFID))
#define get_mbfcb(mbfid)	(&(mbfcb_table[INDEX_MBF(mbfid)]))

/*
 *  メッセージバッファ機能の初期化
 */
#ifdef TOPPERS_mbfini

void
initialize_messagebuf(void)
{
	uint_t	i;
	MBFCB	*p_mbfcb;

	mbfhook_dequeue_wobj = messagebuf_dequeue_wobj;

	for (i = 0; i < tnum_mbf; i++) {
		p_mbfcb = &(mbfcb_table[i]);
		queue_initialize(&(p_mbfcb->swait_queue));
		p_mbfcb->p_mbfinib = &(mbfinib_table[i]);
		queue_initialize(&(p_mbfcb->rwait_queue));
		p_mbfcb->fmbfsz = p_mbfcb->p_mbfinib->mbfsz;
		p_mbfcb->head = 0U;
		p_mbfcb->tail = 0U;
		p_mbfcb->smbfcnt = 0U;
	}
}

#endif /* TOPPERS_mbfini */

/*
 *  メッセージバッファ管理領域へのメッセージの格納
 */
#ifdef TOPPERS_mbfenq

bool_t
enqueue_message(MBFCB *p_mbfcb, const void *msg, uint_t msgsz)
{
	const MBFINIB	*p_mbfinib = p_mbfcb->p_mbfinib;
	char	*mbuffer = (char *)(p_mbfinib->mbfmb);
	uint_t	allocsz;
	size_t	remsz;

	allocsz = sizeof(uint_t) + TOPPERS_ROUND_SZ(msgsz, sizeof(uint_t));
	if (allocsz <= p_mbfcb->fmbfsz) {
		*((uint_t *) &(mbuffer[p_mbfcb->tail])) = msgsz;
		p_mbfcb->tail += sizeof(uint_t);
		if (p_mbfcb->tail >= p_mbfinib->mbfsz) {
			p_mbfcb->tail = 0U;
		}

		remsz = p_mbfinib->mbfsz - p_mbfcb->tail;
		if (remsz < msgsz) {
			memcpy(&(mbuffer[p_mbfcb->tail]), msg, remsz);
			msg = ((char *) msg) + remsz;
			msgsz -= remsz;
			p_mbfcb->tail = 0U;
		}
		memcpy(&(mbuffer[p_mbfcb->tail]), msg, msgsz);
		p_mbfcb->tail += TOPPERS_ROUND_SZ(msgsz, sizeof(uint_t));
		if (p_mbfcb->tail >= p_mbfinib->mbfsz) {
			p_mbfcb->tail = 0U;
		}

		p_mbfcb->fmbfsz -= allocsz;
		p_mbfcb->smbfcnt++;
		return(true);
	}
	return(false);
}

#endif /* TOPPERS_mbfenq */

/*
 *  メッセージバッファ管理領域からのメッセージの取出し
 */
#ifdef TOPPERS_mbfdeq

uint_t
dequeue_message(MBFCB *p_mbfcb, void *msg)
{
	const MBFINIB	*p_mbfinib = p_mbfcb->p_mbfinib;
	char	*mbuffer = (char *)(p_mbfinib->mbfmb);
	uint_t	msgsz, copysz;
	size_t	remsz;

	assert(p_mbfcb->smbfcnt > 0);
	msgsz = *((uint_t *) &(mbuffer[p_mbfcb->head]));
	p_mbfcb->head += sizeof(uint_t);
	if (p_mbfcb->head >= p_mbfinib->mbfsz) {
		p_mbfcb->head = 0U;
	}

	remsz = p_mbfinib->mbfsz - p_mbfcb->head;
	copysz = msgsz;
	if (remsz < copysz) {
		memcpy(msg, &(mbuffer[p_mbfcb->head]), remsz);
		msg = ((char *) msg) + remsz;
		copysz -= remsz;
		p_mbfcb->head = 0U;
	}
	memcpy(msg, &(mbuffer[p_mbfcb->head]), copysz);
	p_mbfcb->head += TOPPERS_ROUND_SZ(copysz, sizeof(uint_t));
	if (p_mbfcb->head >= p_mbfinib->mbfsz) {
		p_mbfcb->head = 0U;
	}

	p_mbfcb->fmbfsz += sizeof(uint_t) + TOPPERS_ROUND_SZ(msgsz, sizeof(uint_t));
	p_mbfcb->smbfcnt--;
	return(msgsz);
}

#endif /* TOPPERS_mbfdeq */

/*
 *  メッセージバッファへのメッセージ送信
 */
#ifdef TOPPERS_mbfsnd

bool_t
send_message(MBFCB *p_mbfcb, const void *msg, uint_t msgsz)
{
	TCB		*p_tcb;

	if (!queue_empty(&(p_mbfcb->rwait_queue))) {
		p_tcb = (TCB *) queue_delete_next(&(p_mbfcb->rwait_queue));
		memcpy(((WINFO_RMBF *)(p_tcb->p_winfo))->msg, msg, msgsz);
		wait_dequeue_tmevtb(p_tcb);
		p_tcb->p_winfo->wercd = (ER_UINT) msgsz;
		make_non_wait(p_tcb);
		return(true);
	}
	else if (queue_empty(&(p_mbfcb->swait_queue))
							&& enqueue_message(p_mbfcb, msg, msgsz)) {
		return(true);
	}
	else {
		return(false);
	}
}

#endif /* TOPPERS_mbfsnd */

/*
 *  メッセージバッファ送信待ちタスクのチェック
 *
 *  メッセージバッファ送信待ちキューの先頭のタスクの送信メッセージが，
 *  メッセージバッファ管理領域に格納することできれば格納し，待ち解除す
 *  る．この処理を，先頭のタスクの送信メッセージが格納できなくなるまで
 *  繰り返す．
 */
#ifdef TOPPERS_mbfsig

void
messagebuf_signal(MBFCB *p_mbfcb)
{
	TCB		*p_tcb;

	while (!queue_empty(&(p_mbfcb->swait_queue))) {
		p_tcb = (TCB *)(p_mbfcb->swait_queue.p_next);
		if (enqueue_message(p_mbfcb, ((WINFO_SMBF *)(p_tcb->p_winfo))->msg,
									((WINFO_SMBF *)(p_tcb->p_winfo))->msgsz)) {
			queue_delete(&(p_tcb->task_queue));
			wait_complete(p_tcb);
		}
		else {
			break;
		}
	}
}

#endif /* TOPPERS_mbfsig */

/*
 *  メッセージバッファからのメッセージ受信
 */
#ifdef TOPPERS_mbfrcv

uint_t
receive_message(MBFCB *p_mbfcb, void *msg)
{
	TCB		*p_tcb;
	uint_t	msgsz;

	if (p_mbfcb->smbfcnt > 0) {
		msgsz = dequeue_message(p_mbfcb, msg);
		messagebuf_signal(p_mbfcb);
		return(msgsz);
	}
	else if (!queue_empty(&(p_mbfcb->swait_queue))) {
		p_tcb = (TCB *) queue_delete_next(&(p_mbfcb->swait_queue));
		msgsz = ((WINFO_SMBF *)(p_tcb->p_winfo))->msgsz;
		memcpy(msg, ((WINFO_SMBF *)(p_tcb->p_winfo))->msg, msgsz);
		wait_complete(p_tcb);
		return(msgsz);
	}
	else {
		return(0U);
	}
}

#endif /* TOPPERS_mbfrcv */

/*
 *  メッセージバッファ送信待ちタスクの待ち解除時処理
 */
#ifdef TOPPERS_mbfwobj

void
messagebuf_dequeue_wobj(TCB *p_tcb)
{
	messagebuf_signal(((WINFO_SMBF *)(p_tcb->p_winfo))->p_mbfcb);
}

#endif /* TOPPERS_mbfwobj */

/*
 *  メッセージバッファへの送信
 */
#ifdef TOPPERS_snd_mbf

ER
snd_mbf(ID mbfid, const void *msg, uint_t msgsz)
{
	MBFCB		*p_mbfcb;
	WINFO_SMBF	winfo_smbf;
	ER			ercd;

	LOG_SND_MBF_ENTER(mbfid, msg, msgsz);
	CHECK_DISPATCH();
	CHECK_ID(VALID_MBFID(mbfid));
	p_mbfcb = get_mbfcb(mbfid);
	CHECK_PAR(0 < msgsz && msgsz <= p_mbfcb->p_mbfinib->maxmsz);

	lock_cpu_dsp();
	if (p_runtsk->raster) {
		ercd = E_RASTER;
	}
	else if (send_message(p_mbfcb, msg, msgsz)) {
		if (p_runtsk != p_schedtsk) {
			dispatch();
		}
		ercd = E_OK;
	}
	else {
		winfo_smbf.msg = msg;
		winfo_smbf.msgsz = msgsz;
		wobj_make_wait((WOBJCB *) p_mbfcb, TS_WAITING_SMBF,
											(WINFO_WOBJ *) &winfo_smbf);
		dispatch();
		ercd = winfo_smbf.winfo.wercd;
	}
	unlock_cpu_dsp();

  error_exit:
	LOG_SND_MBF_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_snd_mbf */

/*
 *  メッセージバッファへの送信（ポーリング）
 */
#ifdef TOPPERS_psnd_mbf

ER
psnd_mbf(ID mbfid, const void *msg, uint_t msgsz)
{
	MBFCB	*p_mbfcb;
	ER		ercd;

	LOG_PSND_MBF_ENTER(mbfid, msg, msgsz);
	CHECK_TSKCTX_UNL();
	CHECK_ID(VALID_MBFID(mbfid));
	p_mbfcb = get_mbfcb(mbfid);
	CHECK_PAR(0 < msgsz && msgsz <= p_mbfcb->p_mbfinib->maxmsz);

	lock_cpu();
	if (send_message(p_mbfcb, msg, msgsz)) {
		if (p_runtsk != p_schedtsk) {
			dispatch();
		}
		ercd = E_OK;
	}
	else {
		ercd = E_TMOUT;
	}
	unlock_cpu();

  error_exit:
	LOG_PSND_MBF_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_psnd_mbf */

/*
 *  メッセージバッファへの送信（タイムアウトあり）
 */
#ifdef TOPPERS_tsnd_mbf

ER
tsnd_mbf(ID mbfid, const void *msg, uint_t msgsz, TMO tmout)
{
	MBFCB		*p_mbfcb;
	WINFO_SMBF	winfo_smbf;
	TMEVTB		tmevtb;
	ER			ercd;

	LOG_TSND_MBF_ENTER(mbfid, msg, msgsz, tmout);
	CHECK_DISPATCH();
	CHECK_ID(VALID_MBFID(mbfid));
	CHECK_PAR(VALID_TMOUT(tmout));
	p_mbfcb = get_mbfcb(mbfid);
	CHECK_PAR(0 < msgsz && msgsz <= p_mbfcb->p_mbfinib->maxmsz);

	lock_cpu_dsp();
	if (p_runtsk->raster) {
		ercd = E_RASTER;
	}
	else if (send_message(p_mbfcb, msg, msgsz)) {
		if (p_runtsk != p_schedtsk) {
			dispatch();
		}
		ercd = E_OK;
	}
	else if (tmout == TMO_POL) {
		ercd = E_TMOUT;
	}
	else {
		winfo_smbf.msg = msg;
		winfo_smbf.msgsz = msgsz;
		wobj_make_wait_tmout((WOBJCB *) p_mbfcb, TS_WAITING_SMBF,
								(WINFO_WOBJ *) &winfo_smbf, &tmevtb, tmout);
		dispatch();
		ercd = winfo_smbf.winfo.wercd;
	}
	unlock_cpu_dsp();

  error_exit:
	LOG_TSND_MBF_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_tsnd_mbf */

/*
 *  メッセージバッファからの受信
 */
#ifdef TOPPERS_rcv_mbf

ER_UINT
rcv_mbf(ID mbfid, void *msg)
{
	MBFCB		*p_mbfcb;
	WINFO_RMBF	winfo_rmbf;
	uint_t		msgsz;
	ER_UINT		ercd;

	LOG_RCV_MBF_ENTER(mbfid, msg);
	CHECK_DISPATCH();
	CHECK_ID(VALID_MBFID(mbfid));
	p_mbfcb = get_mbfcb(mbfid);

	lock_cpu_dsp();
	if (p_runtsk->raster) {
		ercd = E_RASTER;
	}
	else if ((msgsz = receive_message(p_mbfcb, msg)) > 0U) {
		if (p_runtsk != p_schedtsk) {
			dispatch();
		}
		ercd = (ER_UINT) msgsz;
	}
	else {
		make_wait(TS_WAITING_RMBF, &(winfo_rmbf.winfo));
		queue_insert_prev(&(p_mbfcb->rwait_queue), &(p_runtsk->task_queue));
		winfo_rmbf.p_mbfcb = p_mbfcb;
		winfo_rmbf.msg = msg;
		LOG_TSKSTAT(p_runtsk);
		dispatch();
		ercd = winfo_rmbf.winfo.wercd;
	}
	unlock_cpu_dsp();

  error_exit:
	LOG_RCV_MBF_LEAVE(ercd, msg);
	return(ercd);
}

#endif /* TOPPERS_rcv_mbf */

/*
 *  メッセージバッファからの受信（ポーリング）
 */
#ifdef TOPPERS_prcv_mbf

ER
prcv_mbf(ID mbfid, void *msg)
{
	MBFCB	*p_mbfcb;
	uint_t	msgsz;
	ER_UINT	ercd;

	LOG_PRCV_MBF_ENTER(mbfid, msg);
	CHECK_TSKCTX_UNL();
	CHECK_ID(VALID_MBFID(mbfid));
	p_mbfcb = get_mbfcb(mbfid);

	lock_cpu();
	if ((msgsz = receive_message(p_mbfcb, msg)) > 0U) {
		if (p_runtsk != p_schedtsk) {
			dispatch();
		}
		ercd = (ER_UINT) msgsz;
	}
	else {
		ercd = E_TMOUT;
	}
	unlock_cpu();

  error_exit:
	LOG_PRCV_MBF_LEAVE(ercd, msg);
	return(ercd);
}

#endif /* TOPPERS_prcv_mbf */

/*
 *  メッセージバッファからの受信（タイムアウトあり）
 */
#ifdef TOPPERS_trcv_mbf

ER
trcv_mbf(ID mbfid, void *msg, TMO tmout)
{
	MBFCB		*p_mbfcb;
	WINFO_RMBF	winfo_rmbf;
	TMEVTB		tmevtb;
	uint_t		msgsz;
	ER_UINT		ercd;

	LOG_TRCV_MBF_ENTER(mbfid, msg, tmout);
	CHECK_DISPATCH();
	CHECK_ID(VALID_MBFID(mbfid));
	CHECK_PAR(VALID_TMOUT(tmout));
	p_mbfcb = get_mbfcb(mbfid);

	lock_cpu_dsp();
	if (p_runtsk->raster) {
		ercd = E_RASTER;
	}
	else if ((msgsz = receive_message(p_mbfcb, msg)) > 0U) {
		if (p_runtsk != p_schedtsk) {
			dispatch();
		}
		ercd = (ER_UINT) msgsz;
	}
	else if (tmout == TMO_POL) {
		ercd = E_TMOUT;
	}
	else {
		make_wait_tmout(TS_WAITING_RMBF, &(winfo_rmbf.winfo), &tmevtb, tmout);
		queue_insert_prev(&(p_mbfcb->rwait_queue), &(p_runtsk->task_queue));
		winfo_rmbf.p_mbfcb = p_mbfcb;
		winfo_rmbf.msg = msg;
		LOG_TSKSTAT(p_runtsk);
		dispatch();
		ercd = winfo_rmbf.winfo.wercd;
	}
	unlock_cpu_dsp();

  error_exit:
	LOG_TRCV_MBF_LEAVE(ercd, msg);
	return(ercd);
}

#endif /* TOPPERS_trcv_mbf */

/*
 *  メッセージバッファの再初期化
 */
#ifdef TOPPERS_ini_mbf

ER
ini_mbf(ID mbfid)
{
	MBFCB	*p_mbfcb;
	ER		ercd;
    
	LOG_INI_MBF_ENTER(mbfid);
	CHECK_TSKCTX_UNL();
	CHECK_ID(VALID_MBFID(mbfid));
	p_mbfcb = get_mbfcb(mbfid);

	lock_cpu();
	init_wait_queue(&(p_mbfcb->swait_queue));
	init_wait_queue(&(p_mbfcb->rwait_queue));
	p_mbfcb->fmbfsz = p_mbfcb->p_mbfinib->mbfsz;
	p_mbfcb->head = 0U;
	p_mbfcb->tail = 0U;
	p_mbfcb->smbfcnt = 0U;
	if (p_runtsk != p_schedtsk) {
		dispatch();
	}
	ercd = E_OK;
	unlock_cpu();

  error_exit:
	LOG_INI_MBF_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_ini_mbf */

/*
 *  メッセージバッファの状態参照
 */
#ifdef TOPPERS_ref_mbf

ER
ref_mbf(ID mbfid, T_RMBF *pk_rmbf)
{
	MBFCB	*p_mbfcb;
	ER		ercd;
    
	LOG_REF_MBF_ENTER(mbfid, pk_rmbf);
	CHECK_TSKCTX_UNL();
	CHECK_ID(VALID_MBFID(mbfid));
	p_mbfcb = get_mbfcb(mbfid);

	lock_cpu();
	pk_rmbf->stskid = wait_tskid(&(p_mbfcb->swait_queue));
	pk_rmbf->rtskid = wait_tskid(&(p_mbfcb->rwait_queue));
	pk_rmbf->smbfcnt = p_mbfcb->smbfcnt;
	pk_rmbf->fmbfsz = p_mbfcb->fmbfsz;
	ercd = E_OK;
	unlock_cpu();

  error_exit:
	LOG_REF_MBF_LEAVE(ercd, pk_rmbf);
	return(ercd);
}

#endif /* TOPPERS_ref_mbf */
