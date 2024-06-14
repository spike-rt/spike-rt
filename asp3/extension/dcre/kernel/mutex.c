/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2005-2022 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: mutex.c 1782 2023-01-08 14:50:43Z ertl-hiro $
 */

/*
 *		ミューテックス機能
 */

#include "kernel_impl.h"
#include "check.h"
#include "task.h"
#include "taskhook.h"
#include "wait.h"
#include "mutex.h"

/*
 *  トレースログマクロのデフォルト定義
 */
#ifndef LOG_ACRE_MTX_ENTER
#define LOG_ACRE_MTX_ENTER(pk_cmtx)
#endif /* LOG_ACRE_MTX_ENTER */

#ifndef LOG_ACRE_MTX_LEAVE
#define LOG_ACRE_MTX_LEAVE(ercd)
#endif /* LOG_ACRE_MTX_LEAVE */

#ifndef LOG_DEL_MTX_ENTER
#define LOG_DEL_MTX_ENTER(mtxid)
#endif /* LOG_DEL_MTX_ENTER */

#ifndef LOG_DEL_MTX_LEAVE
#define LOG_DEL_MTX_LEAVE(ercd)
#endif /* LOG_DEL_MTX_LEAVE */

#ifndef LOG_LOC_MTX_ENTER
#define LOG_LOC_MTX_ENTER(mtxid)
#endif /* LOG_LOC_MTX_ENTER */

#ifndef LOG_LOC_MTX_LEAVE
#define LOG_LOC_MTX_LEAVE(ercd)
#endif /* LOG_LOC_MTX_LEAVE */

#ifndef LOG_PLOC_MTX_ENTER
#define LOG_PLOC_MTX_ENTER(mtxid)
#endif /* LOG_PLOC_MTX_ENTER */

#ifndef LOG_PLOC_MTX_LEAVE
#define LOG_PLOC_MTX_LEAVE(ercd)
#endif /* LOG_PLOC_MTX_LEAVE */

#ifndef LOG_TLOC_MTX_ENTER
#define LOG_TLOC_MTX_ENTER(mtxid, tmout)
#endif /* LOG_TLOC_MTX_ENTER */

#ifndef LOG_TLOC_MTX_LEAVE
#define LOG_TLOC_MTX_LEAVE(ercd)
#endif /* LOG_TLOC_MTX_LEAVE */

#ifndef LOG_UNL_MTX_ENTER
#define LOG_UNL_MTX_ENTER(mtxid)
#endif /* LOG_UNL_MTX_ENTER */

#ifndef LOG_UNL_MTX_LEAVE
#define LOG_UNL_MTX_LEAVE(ercd)
#endif /* LOG_UNL_MTX_LEAVE */

#ifndef LOG_INI_MTX_ENTER
#define LOG_INI_MTX_ENTER(mtxid)
#endif /* LOG_INI_MTX_ENTER */

#ifndef LOG_INI_MTX_LEAVE
#define LOG_INI_MTX_LEAVE(ercd)
#endif /* LOG_INI_MTX_LEAVE */

#ifndef LOG_REF_MTX_ENTER
#define LOG_REF_MTX_ENTER(mtxid, pk_rmtx)
#endif /* LOG_REF_MTX_ENTER */

#ifndef LOG_REF_MTX_LEAVE
#define LOG_REF_MTX_LEAVE(ercd, pk_rmtx)
#endif /* LOG_REF_MTX_LEAVE */

/*
 *  ミューテックスの数
 */
#define tnum_mtx	((uint_t)(tmax_mtxid - TMIN_MTXID + 1))
#define tnum_smtx	((uint_t)(tmax_smtxid - TMIN_MTXID + 1))

/*
 *  ミューテックスIDからミューテックス管理ブロックを取り出すためのマクロ
 */
#define INDEX_MTX(mtxid)	((uint_t)((mtxid) - TMIN_MTXID))
#define get_mtxcb(mtxid)	(&(mtxcb_table[INDEX_MTX(mtxid)]))

/*
 *  ミューテックスのプロトコルを判断するマクロ
 */
#define MTXPROTO_MASK			0x03U
#define MTXPROTO(p_mtxcb)		((p_mtxcb)->p_mtxinib->mtxatr & MTXPROTO_MASK)
#define MTX_CEILING(p_mtxcb)	(MTXPROTO(p_mtxcb) == TA_CEILING)

#ifdef TOPPERS_mtxini

/*
 *  使用していないミューテックス管理ブロックのリスト
 */
QUEUE	free_mtxcb;

/*
 *  ミューテックス機能の初期化
 */
void
initialize_mutex(void)
{
	uint_t	i, j;
	MTXCB	*p_mtxcb;
	MTXINIB	*p_mtxinib;

	mtxhook_check_ceilpri = mutex_check_ceilpri;
	mtxhook_release_all = mutex_release_all;

	for (i = 0; i < tnum_smtx; i++) {
		p_mtxcb = &(mtxcb_table[i]);
		queue_initialize(&(p_mtxcb->wait_queue));
		p_mtxcb->p_mtxinib = &(mtxinib_table[i]);
		p_mtxcb->p_loctsk = NULL;
	}
	queue_initialize(&free_mtxcb);
	for (j = 0; i < tnum_mtx; i++, j++) {
		p_mtxcb = &(mtxcb_table[i]);
		p_mtxinib = &(amtxinib_table[j]);
		p_mtxinib->mtxatr = TA_NOEXS;
		p_mtxcb->p_mtxinib = ((const MTXINIB *) p_mtxinib);
		queue_insert_prev(&free_mtxcb, &(p_mtxcb->wait_queue));
	}
}

#endif /* TOPPERS_mtxini */

/*
 *  上限優先度違反のチェック
 */
#ifdef TOPPERS_mtxchk

bool_t
mutex_check_ceilpri(TCB *p_tcb, uint_t bpriority)
{
	MTXCB	*p_mtxcb;

	/*
	 *  タスクがロックしている優先度上限ミューテックスの中で，上限優先
	 *  度がbpriorityよりも低いものがあれば，falseを返す．
	 */
	p_mtxcb = p_tcb->p_lastmtx;
	while (p_mtxcb != NULL) {
		if (MTX_CEILING(p_mtxcb) && bpriority < p_mtxcb->p_mtxinib->ceilpri) {
			return(false);
		}
		p_mtxcb = p_mtxcb->p_prevmtx;
	}

	/*
	 *  タスクが優先度上限ミューテックスのロックを待っている場合に，そ
	 *  の上限優先度がbpriorityよりも低くければ，falseを返す．
	 */
	if (TSTAT_WAIT_MTX(p_tcb->tstat)) {
		p_mtxcb = ((WINFO_MTX *)(p_tcb->p_winfo))->p_mtxcb;
		if (MTX_CEILING(p_mtxcb) && bpriority < p_mtxcb->p_mtxinib->ceilpri) {
			return(false);
		}
	}

	/*
	 *  いずれの条件にも当てはまらなければtrueを返す．
	 */
	return(true);
}

#endif /* TOPPERS_mtxchk */

/*
 *  タスクの現在優先度の計算
 *
 *  p_tcbで指定されるタスクの現在優先度（に設定すべき値）を計算する．
 *  また，優先度上昇状態であるかを判定し，p_tcb->boostedに返す．
 */
Inline uint_t
mutex_calc_priority(TCB *p_tcb)
{
	uint_t	priority;
	MTXCB	*p_mtxcb;
	bool_t	boosted;

	priority = p_tcb->bpriority;
	p_mtxcb = p_tcb->p_lastmtx;
	boosted = false;
	while (p_mtxcb != NULL) {
		if (MTX_CEILING(p_mtxcb)) {
			if (p_mtxcb->p_mtxinib->ceilpri < priority) {
				priority = p_mtxcb->p_mtxinib->ceilpri;
			}
			boosted = true;
		}
		p_mtxcb = p_mtxcb->p_prevmtx;
	}
	p_tcb->boosted = boosted;
	return(priority);
}

/*
 *  ミューテックスのリストからの削除
 *
 *  p_tcbで指定されるタスクがロックしているミューテックスのリストから，
 *  p_mtxcbで指定されるミューテックスを削除する．
 */
Inline bool_t
remove_mutex(TCB *p_tcb, MTXCB *p_mtxcb)
{
	MTXCB	**pp_prevmtx;

	pp_prevmtx = &(p_tcb->p_lastmtx);
	while (*pp_prevmtx != NULL) {
		if (*pp_prevmtx == p_mtxcb) {
			*pp_prevmtx = p_mtxcb->p_prevmtx;
			return(true);
		}
		pp_prevmtx = &((*pp_prevmtx)->p_prevmtx);
	}
	return(false);
}

/*
 *  要素優先度が上がる（または増える）場合の現在優先度変更処理
 *
 *  p_tcbで指定されるタスクに対して，newpriで指定される要素優先度が上
 *  がった（または増えた）際の現在優先度変更処理を行う．
 */
Inline void
mutex_raise_priority(TCB *p_tcb, uint_t newpri)
{
	p_tcb->boosted = true;
	if (newpri < p_tcb->priority) {
		change_priority(p_tcb, newpri, true);
	}
}

/*
 *  要素優先度が下がる（または減る）場合の現在優先度変更処理
 *
 *  p_tcbで指定されるタスクに対して，oldpriで指定される要素優先度が下
 *  がった（または減った）際の現在優先度変更処理を行う．
 */
Inline void
mutex_drop_priority(TCB *p_tcb, uint_t oldpri)
{
	uint_t	newpri;

	if (oldpri == p_tcb->priority) {
		newpri = mutex_calc_priority(p_tcb);
		if (newpri != p_tcb->priority) {
			change_priority(p_tcb, newpri, true);
		}
	}
}

/*
 *  ミューテックスのロック
 */
#ifdef TOPPERS_mtxacq

void
mutex_acquire(TCB *p_tcb, MTXCB *p_mtxcb)
{
	p_mtxcb->p_loctsk = p_tcb;
	p_mtxcb->p_prevmtx = p_tcb->p_lastmtx;
	p_tcb->p_lastmtx = p_mtxcb;
	if (MTX_CEILING(p_mtxcb)) {
		mutex_raise_priority(p_tcb, p_mtxcb->p_mtxinib->ceilpri);
	}
}

#endif /* TOPPERS_mtxacq */

/*
 *  ミューテックスのロック解除
 */
#ifdef TOPPERS_mtxrel

void
mutex_release(MTXCB *p_mtxcb)
{
	TCB		*p_tcb;

	if (queue_empty(&(p_mtxcb->wait_queue))) {
		p_mtxcb->p_loctsk = NULL;
	}
	else {
		/*
		 *  ミューテックス待ちキューの先頭タスク（p_tcb）に，ミューテッ
		 *  クスをロックさせる．
		 */
		p_tcb = (TCB *) queue_delete_next(&(p_mtxcb->wait_queue));
		wait_dequeue_tmevtb(p_tcb);
		p_tcb->p_winfo->wercd = E_OK;

		p_mtxcb->p_loctsk = p_tcb;
		p_mtxcb->p_prevmtx = p_tcb->p_lastmtx;
		p_tcb->p_lastmtx = p_mtxcb;
		if (MTX_CEILING(p_mtxcb)) {
			if (p_mtxcb->p_mtxinib->ceilpri < p_tcb->priority) {
				p_tcb->priority = p_mtxcb->p_mtxinib->ceilpri;
			}
			p_tcb->boosted = true;
		}
		make_non_wait(p_tcb);
	}
}

#endif /* TOPPERS_mtxrel */

/*
 *  タスクがロックしているすべてのミューテックスのロック解除
 */
#ifdef TOPPERS_mtxrela

void
mutex_release_all(TCB *p_tcb)
{
	MTXCB	*p_mtxcb;

	while ((p_mtxcb = p_tcb->p_lastmtx) != NULL) {
		p_tcb->p_lastmtx = p_mtxcb->p_prevmtx;
		mutex_release(p_mtxcb);
	}
}

#endif /* TOPPERS_mtxrela */

/*
 *  ミューテックスの生成［NGKI2022］
 */
#ifdef TOPPERS_acre_mtx

ER_UINT
acre_mtx(const T_CMTX *pk_cmtx)
{
	MTXCB	*p_mtxcb;
	MTXINIB	*p_mtxinib;
	ATR		mtxatr;
	PRI		ceilpri;
	ER		ercd;

	LOG_ACRE_MTX_ENTER(pk_cmtx);
	CHECK_TSKCTX_UNL();							/*［NGKI2023］［NGKI2024］*/

	mtxatr = pk_cmtx->mtxatr;
	ceilpri = pk_cmtx->ceilpri;

	if (mtxatr == TA_CEILING) {
		CHECK_PAR(VALID_TPRI(ceilpri));			/*［NGKI2037］*/
	}
	else {
		CHECK_VALIDATR(mtxatr, TA_TPRI);		/*［NGKI2025］*/
	}

	lock_cpu();
	if (tnum_mtx == 0 || queue_empty(&free_mtxcb)) {
		ercd = E_NOID;							/*［NGKI2031］*/
	}
	else {
		p_mtxcb = ((MTXCB *) queue_delete_next(&free_mtxcb));
		p_mtxinib = (MTXINIB *)(p_mtxcb->p_mtxinib);
		p_mtxinib->mtxatr = mtxatr;
		p_mtxinib->ceilpri = INT_PRIORITY(ceilpri);

		queue_initialize(&(p_mtxcb->wait_queue));
		p_mtxcb->p_loctsk = NULL;				/*［NGKI2033］*/
		ercd = MTXID(p_mtxcb);
	}
	unlock_cpu();

  error_exit:
	LOG_ACRE_MTX_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_acre_mtx */

/*
 *  ミューテックスの削除［NGKI2056］
 */
#ifdef TOPPERS_del_mtx

ER
del_mtx(ID mtxid)
{
	MTXCB	*p_mtxcb;
	MTXINIB	*p_mtxinib;
	TCB		*p_loctsk;
	ER		ercd;

	LOG_DEL_MTX_ENTER(mtxid);
	CHECK_TSKCTX_UNL();							/*［NGKI2057］［NGKI2058］*/
	CHECK_ID(VALID_MTXID(mtxid));				/*［NGKI2059］*/
	p_mtxcb = get_mtxcb(mtxid);

	lock_cpu();
	if (p_mtxcb->p_mtxinib->mtxatr == TA_NOEXS) {
		ercd = E_NOEXS;							/*［NGKI2060］*/
	}
	else if (mtxid <= tmax_smtxid) {
		ercd = E_OBJ;							/*［NGKI2062］*/
	}
	else {
		init_wait_queue(&(p_mtxcb->wait_queue));	/*［NGKI2065］*/
		p_loctsk = p_mtxcb->p_loctsk;
		if (p_loctsk != NULL) {
			/*
			 *  p_loctskがロックしているミューテックスのリストから対象
			 *  ミュークスを削除し，必要な場合にはp_loctskの現在優先度
			 *  を変更する［NGKI2064］．
			 */
			(void) remove_mutex(p_loctsk, p_mtxcb);
			if (MTX_CEILING(p_mtxcb)) {
				mutex_drop_priority(p_loctsk, p_mtxcb->p_mtxinib->ceilpri);
			}
		}
		p_mtxinib = (MTXINIB *)(p_mtxcb->p_mtxinib);
		p_mtxinib->mtxatr = TA_NOEXS;						/*［NGKI2063］*/
		queue_insert_prev(&free_mtxcb, &(p_mtxcb->wait_queue));
		if (p_runtsk != p_schedtsk) {
			dispatch();
		}
		ercd = E_OK;
	}
	unlock_cpu();

  error_exit:
	LOG_DEL_MTX_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_del_mtx */

/*
 *  ミューテックスのロック
 */
#ifdef TOPPERS_loc_mtx

ER
loc_mtx(ID mtxid)
{
	MTXCB		*p_mtxcb;
	WINFO_MTX	winfo_mtx;
	ER			ercd;

	LOG_LOC_MTX_ENTER(mtxid);
	CHECK_DISPATCH();
	CHECK_ID(VALID_MTXID(mtxid));
	p_mtxcb = get_mtxcb(mtxid);

	lock_cpu_dsp();
	if (p_mtxcb->p_mtxinib->mtxatr == TA_NOEXS) {
		ercd = E_NOEXS;
	}
	else if (p_runtsk->raster) {
		ercd = E_RASTER;
	}
	else if (MTX_CEILING(p_mtxcb)
				&& p_runtsk->bpriority < p_mtxcb->p_mtxinib->ceilpri) {
		ercd = E_ILUSE;
	}
	else if (p_mtxcb->p_loctsk == NULL) {
		mutex_acquire(p_runtsk, p_mtxcb);
		/*
		 *  優先度上限ミューテックスをロックした場合，p_runtskの優先度
		 *  が上がる可能性があるが，ディスパッチが必要になることはない．
		 */
		assert(p_runtsk == p_schedtsk);
		ercd = E_OK;
	}
	else if (p_mtxcb->p_loctsk == p_runtsk) {
		ercd = E_OBJ;
	}
	else {
		wobj_make_wait((WOBJCB *) p_mtxcb, TS_WAITING_MTX,
											(WINFO_WOBJ *) &winfo_mtx);
		dispatch();
		ercd = winfo_mtx.winfo.wercd;
	}
	unlock_cpu_dsp();

  error_exit:
	LOG_LOC_MTX_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_loc_mtx */

/*
 *  ミューテックスのロック（ポーリング）
 */
#ifdef TOPPERS_ploc_mtx

ER
ploc_mtx(ID mtxid)
{
	MTXCB	*p_mtxcb;
	ER		ercd;

	LOG_PLOC_MTX_ENTER(mtxid);
	CHECK_TSKCTX_UNL();
	CHECK_ID(VALID_MTXID(mtxid));
	p_mtxcb = get_mtxcb(mtxid);

	lock_cpu();
	if (p_mtxcb->p_mtxinib->mtxatr == TA_NOEXS) {
		ercd = E_NOEXS;
	}
	else if (MTX_CEILING(p_mtxcb)
				&& p_runtsk->bpriority < p_mtxcb->p_mtxinib->ceilpri) {
		ercd = E_ILUSE;
	}
	else if (p_mtxcb->p_loctsk == NULL) {
		mutex_acquire(p_runtsk, p_mtxcb);
		/*
		 *  優先度上限ミューテックスをロックした場合，p_runtskの優先度
		 *  が上がる可能性があるが，ディスパッチが必要になることはない．
		 */
		assert(p_runtsk == p_schedtsk);
		ercd = E_OK;
	}
	else if (p_mtxcb->p_loctsk == p_runtsk) {
		ercd = E_OBJ;
	}
	else {
		ercd = E_TMOUT;
	}
	unlock_cpu();

  error_exit:
	LOG_PLOC_MTX_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_ploc_mtx */

/*
 *  ミューテックスのロック（タイムアウトあり）
 */
#ifdef TOPPERS_tloc_mtx

ER
tloc_mtx(ID mtxid, TMO tmout)
{
	MTXCB		*p_mtxcb;
	WINFO_MTX	winfo_mtx;
	TMEVTB		tmevtb;
	ER			ercd;

	LOG_TLOC_MTX_ENTER(mtxid, tmout);
	CHECK_DISPATCH();
	CHECK_ID(VALID_MTXID(mtxid));
	CHECK_PAR(VALID_TMOUT(tmout));
	p_mtxcb = get_mtxcb(mtxid);

	lock_cpu_dsp();
	if (p_mtxcb->p_mtxinib->mtxatr == TA_NOEXS) {
		ercd = E_NOEXS;
	}
	else if (p_runtsk->raster) {
		ercd = E_RASTER;
	}
	else if (MTX_CEILING(p_mtxcb)
				&& p_runtsk->bpriority < p_mtxcb->p_mtxinib->ceilpri) {
		ercd = E_ILUSE;
	}
	else if (p_mtxcb->p_loctsk == NULL) {
		mutex_acquire(p_runtsk, p_mtxcb);
		/*
		 *  優先度上限ミューテックスをロックした場合，p_runtskの優先度
		 *  が上がる可能性があるが，ディスパッチが必要になることはない．
		 */
		assert(p_runtsk == p_schedtsk);
		ercd = E_OK;
	}
	else if (p_mtxcb->p_loctsk == p_runtsk) {
		ercd = E_OBJ;
	}
	else if (tmout == TMO_POL) {
		ercd = E_TMOUT;
	}
	else {
		wobj_make_wait_tmout((WOBJCB *) p_mtxcb, TS_WAITING_MTX,
								(WINFO_WOBJ *) &winfo_mtx, &tmevtb, tmout);
		dispatch();
		ercd = winfo_mtx.winfo.wercd;
	}
	unlock_cpu_dsp();

  error_exit:
	LOG_TLOC_MTX_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_tloc_mtx */

/*
 *  ミューテックスのロック解除
 */
#ifdef TOPPERS_unl_mtx

ER
unl_mtx(ID mtxid)
{
	MTXCB	*p_mtxcb;
	ER		ercd;

	LOG_UNL_MTX_ENTER(mtxid);
	CHECK_TSKCTX_UNL();
	CHECK_ID(VALID_MTXID(mtxid));
	p_mtxcb = get_mtxcb(mtxid);

	lock_cpu();
	if (p_mtxcb->p_mtxinib->mtxatr == TA_NOEXS) {
		ercd = E_NOEXS;
	}
	else if (p_mtxcb != p_runtsk->p_lastmtx) {
		ercd = E_OBJ;
	}
	else {
		p_runtsk->p_lastmtx = p_mtxcb->p_prevmtx;
		if (MTX_CEILING(p_mtxcb)) {
			mutex_drop_priority(p_runtsk, p_mtxcb->p_mtxinib->ceilpri);
		}
		mutex_release(p_mtxcb);
		if (p_runtsk != p_schedtsk) {
			dispatch();
		}
		ercd = E_OK;
	}
	unlock_cpu();

  error_exit:
	LOG_UNL_MTX_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_unl_mtx */

/*
 *  ミューテックスの再初期化
 */
#ifdef TOPPERS_ini_mtx

ER
ini_mtx(ID mtxid)
{
	MTXCB	*p_mtxcb;
	TCB		*p_loctsk;
	ER		ercd;

	LOG_INI_MTX_ENTER(mtxid);
	CHECK_TSKCTX_UNL();
	CHECK_ID(VALID_MTXID(mtxid));
	p_mtxcb = get_mtxcb(mtxid);

	lock_cpu();
	if (p_mtxcb->p_mtxinib->mtxatr == TA_NOEXS) {
		ercd = E_NOEXS;
	}
	else {
		init_wait_queue(&(p_mtxcb->wait_queue));
		p_loctsk = p_mtxcb->p_loctsk;
		if (p_loctsk != NULL) {
			p_mtxcb->p_loctsk = NULL;
			(void) remove_mutex(p_loctsk, p_mtxcb);
			if (MTX_CEILING(p_mtxcb)) {
				mutex_drop_priority(p_loctsk, p_mtxcb->p_mtxinib->ceilpri);
			}
		}
		if (p_runtsk != p_schedtsk) {
			dispatch();
		}
		ercd = E_OK;
	}
	unlock_cpu();

  error_exit:
	LOG_INI_MTX_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_ini_mtx */

/*
 *  ミューテックスの状態参照
 */
#ifdef TOPPERS_ref_mtx

ER
ref_mtx(ID mtxid, T_RMTX *pk_rmtx)
{
	MTXCB	*p_mtxcb;
	ER		ercd;

	LOG_REF_MTX_ENTER(mtxid, pk_rmtx);
	CHECK_TSKCTX_UNL();
	CHECK_ID(VALID_MTXID(mtxid));
	p_mtxcb = get_mtxcb(mtxid);

	lock_cpu();
	if (p_mtxcb->p_mtxinib->mtxatr == TA_NOEXS) {
		ercd = E_NOEXS;
	}
	else {
		pk_rmtx->htskid = (p_mtxcb->p_loctsk != NULL) ? TSKID(p_mtxcb->p_loctsk)
														: TSK_NONE;
		pk_rmtx->wtskid = wait_tskid(&(p_mtxcb->wait_queue));
		ercd = E_OK;
	}
	unlock_cpu();

  error_exit:
	LOG_REF_MTX_LEAVE(ercd, pk_rmtx);
	return(ercd);
}

#endif /* TOPPERS_ref_mtx */
