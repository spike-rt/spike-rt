/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
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
 *  $Id: task.c 1633 2022-09-23 09:27:28Z ertl-hiro $
 */

/*
 *		タスク管理モジュール
 */

#include "kernel_impl.h"
#include "task.h"
#include "taskhook.h"
#include "wait.h"

#ifdef TOPPERS_tskini

/*
 *  実行状態のタスク
 */
TCB		*p_runtsk;

/*
 *  実行すべきタスク
 */
TCB		*p_schedtsk;

/*
 *  ディスパッチ許可状態
 */
bool_t	enadsp;

/*
 *  タスクディスパッチ可能状態
 */
bool_t	dspflg;

/*
 *  レディキュー
 */
QUEUE	ready_queue[TNUM_TPRI];

/*
 *  レディキューサーチのためのビットマップ
 */
uint16_t	ready_primap;

/*
 *  タスク管理モジュールの初期化
 */
void
initialize_task(void)
{
	uint_t	i, j;
	TCB		*p_tcb;

	p_runtsk = NULL;
	p_schedtsk = NULL;
	enadsp = true;
	dspflg = true;

	for (i = 0; i < TNUM_TPRI; i++) {
		queue_initialize(&(ready_queue[i]));
	}
	ready_primap = 0U;

	for (i = 0; i < tnum_tsk; i++) {
		j = INDEX_TSK(torder_table[i]);
		p_tcb = &(tcb_table[j]);
		p_tcb->p_tinib = &(tinib_table[j]);
		p_tcb->actque = false;
		make_dormant(p_tcb);
		p_tcb->p_lastmtx = NULL;
		if ((p_tcb->p_tinib->tskatr & TA_ACT) != 0U) {
			make_active(p_tcb);
		}
	}
}

#endif /* TOPPERS_tskini */

/*
 *  ビットマップサーチ関数
 *
 *  bitmap内の1のビットの内，最も下位（右）のものをサーチし，そのビッ
 *  ト番号を返す．ビット番号は，最下位ビットを0とする．bitmapに0を指定
 *  してはならない．この関数では，bitmapが16ビットであることを仮定し，
 *  uint16_t型としている．
 *
 *  ビットサーチ命令を持つプロセッサでは，ビットサーチ命令を使うように
 *  書き直した方が効率が良い場合がある．このような場合には，ターゲット
 *  依存部でビットサーチ命令を使ったbitmap_searchを定義し，
 *  OMIT_BITMAP_SEARCHをマクロ定義すればよい．また，ビットサーチ命令の
 *  サーチ方向が逆などの理由で優先度とビットとの対応を変更したい場合に
 *  は，PRIMAP_BITをマクロ定義すればよい．
 *
 *  また，ライブラリにffsがあるなら，次のように定義してライブラリ関数を
 *  使った方が効率が良い可能性もある．
 *		#define	bitmap_search(bitmap) (ffs(bitmap) - 1)
 */
#ifndef PRIMAP_BIT
#define	PRIMAP_BIT(pri)		(1U << (pri))
#endif /* PRIMAP_BIT */

#ifndef OMIT_BITMAP_SEARCH

static const unsigned char bitmap_search_table[] = { 0, 1, 0, 2, 0, 1, 0,
												3, 0, 1, 0, 2, 0, 1, 0 };

Inline uint_t
bitmap_search(uint16_t bitmap)
{
	uint_t	n = 0U;

	assert(bitmap != 0U);
	if ((bitmap & 0x00ffU) == 0U) {
		bitmap >>= 8;
		n += 8;
	}
	if ((bitmap & 0x0fU) == 0U) {
		bitmap >>= 4;
		n += 4;
	}
	return(n + bitmap_search_table[(bitmap & 0x0fU) - 1]);
}

#endif /* OMIT_BITMAP_SEARCH */

/*
 *  優先度ビットマップが空かのチェック
 */
Inline bool_t
primap_empty(void)
{
	return(ready_primap == 0U);
}

/*
 *  優先度ビットマップのサーチ
 */
Inline uint_t
primap_search(void)
{
	return(bitmap_search(ready_primap));
}

/*
 *  優先度ビットマップのセット
 */
Inline void
primap_set(uint_t pri)
{
	ready_primap |= PRIMAP_BIT(pri);
}

/*
 *  優先度ビットマップのクリア
 */
Inline void
primap_clear(uint_t pri)
{
	ready_primap &= ~PRIMAP_BIT(pri);
}

/*
 *  最高優先順位タスクのサーチ
 */
#ifdef TOPPERS_tsksched

TCB *
search_schedtsk(void)
{
	uint_t	schedpri;

	schedpri = primap_search();
	return((TCB *)(ready_queue[schedpri].p_next));
}

#endif /* TOPPERS_tsksched */

/*
 *  実行できる状態への遷移
 *
 *  実行すべきタスクを更新するのは，実行できるタスクがなかった場合と，
 *  p_tcbで指定されるタスクの優先度が実行すべきタスクの優先度よりも高
 *  い場合である．
 */
#ifdef TOPPERS_tskrun

void
make_runnable(TCB *p_tcb)
{
	uint_t	pri = p_tcb->priority;

	queue_insert_prev(&(ready_queue[pri]), &(p_tcb->task_queue));
	primap_set(pri);

	if (dspflg) {
		if (p_schedtsk == (TCB *) NULL || pri < p_schedtsk->priority) {
			p_schedtsk = p_tcb;
		}
	}
}

#endif /* TOPPERS_tskrun */

/*
 *  実行できる状態から他の状態への遷移
 *
 *  実行すべきタスクを更新するのは，p_tcbで指定されるタスクが実行すべ
 *  きタスクであった場合である．p_tcbで指定されるタスクと同じ優先度の
 *  タスクが他にある場合は，そのタスクの次のタスクが実行すべきタスクに
 *  なる．そうでない場合は，レディキューをサーチする必要がある．
 *
 *  自タスクに対してこの関数が呼ばれるのは，タスクディスパッチ可能状態
 *  に限られる．またこの関数は，非タスクコンテキストから呼ばれることは
 *  ないため，p_runtskとp_schedtskは必ず一致している．そのため，p_tcb
 *  とp_schedtskが一致するときは，必ずタスクディスパッチ可能状態である．
 */
#ifdef TOPPERS_tsknrun

void
make_non_runnable(TCB *p_tcb)
{
	uint_t	pri = p_tcb->priority;
	QUEUE	*p_queue = &(ready_queue[pri]);

	queue_delete(&(p_tcb->task_queue));
	if (queue_empty(p_queue)) {
		primap_clear(pri);
		if (p_schedtsk == p_tcb) {
			assert(dspflg);
			p_schedtsk = primap_empty() ? (TCB *) NULL : search_schedtsk();
		}
	}
	else {
		if (p_schedtsk == p_tcb) {
			assert(dspflg);
			p_schedtsk = (TCB *)(p_queue->p_next);
		}
	}
}

#endif /* TOPPERS_tsknrun */

/*
 *  休止状態への遷移
 */
#ifdef TOPPERS_tskdmt

void
make_dormant(TCB *p_tcb)
{
	p_tcb->tstat = TS_DORMANT;
	p_tcb->bpriority = p_tcb->p_tinib->ipriority;
	p_tcb->priority = p_tcb->p_tinib->ipriority;
	p_tcb->wupque = false;
	p_tcb->raster = false;
	p_tcb->enater = true;
	p_tcb->boosted = false;
#ifdef TOPPERS_SUPPORT_OVRHDR
	p_tcb->staovr = false;
#endif /* TOPPERS_SUPPORT_OVRHDR */
	LOG_TSKSTAT(p_tcb);
}

#endif /* TOPPERS_tskdmt */

/*
 *  休止状態から実行できる状態への遷移
 */
#ifdef TOPPERS_tskact

void
make_active(TCB *p_tcb)
{
	activate_context(p_tcb);
	p_tcb->tstat = TS_RUNNABLE;
	LOG_TSKSTAT(p_tcb);
	make_runnable(p_tcb);
}

#endif /* TOPPERS_tskact */

/*
 *  タスクの優先度の変更
 *
 *  p_tcbで指定されるタスクが実行できる状態の場合には，レディキューの
 *  中での位置を変更する．オブジェクトの待ちキューの中で待ち状態になっ
 *  ている場合には，待ちキューの中での位置を変更する．
 *
 *  実行すべきタスクを更新するのは，(1) p_tcbで指定されるタスクが実行
 *  すべきタスクであった場合には，優先度を下げた（または優先度が変わら
 *  なかった）時，(2) p_tcbで指定されるタスクが実行すべきタスクでなかっ
 *  た場合には，変更後の優先度が実行すべきタスクの優先度よりも高いか同
 *  じ時（同じ場合に更新が必要になるのは，実際には，mtxmodeがtrueの場
 *  合のみ）である．(1)の場合には，レディキューをサーチする必要がある．
 */
#ifdef TOPPERS_tskpri

void
change_priority(TCB *p_tcb, uint_t newpri, bool_t mtxmode)
{
	uint_t	oldpri;

	oldpri = p_tcb->priority;
	p_tcb->priority = newpri;

	if (TSTAT_RUNNABLE(p_tcb->tstat)) {
		/*
		 *  タスクが実行できる状態の場合
		 */
		queue_delete(&(p_tcb->task_queue));
		if (queue_empty(&(ready_queue[oldpri]))) {
			primap_clear(oldpri);
		}
		if (mtxmode) {
			queue_insert_next(&(ready_queue[newpri]), &(p_tcb->task_queue));
		}
		else {
			queue_insert_prev(&(ready_queue[newpri]), &(p_tcb->task_queue));
		}
		primap_set(newpri);

		if (dspflg) {
			if (p_schedtsk == p_tcb) {
				if (newpri >= oldpri) {
					p_schedtsk = search_schedtsk();
				}
			}
			else {
				if (newpri <= p_schedtsk->priority) {
					p_schedtsk = (TCB *)(ready_queue[newpri].p_next);
				}
			}
		}
	}
	else {
		if (TSTAT_WAIT_WOBJCB(p_tcb->tstat)) {
			/*
			 *  タスクが，同期・通信オブジェクトの管理ブロックの共通部
			 *  分（WOBJCB）の待ちキューにつながれている場合
			 */
			wobj_change_priority(((WINFO_WOBJ *)(p_tcb->p_winfo))->p_wobjcb,
																	p_tcb);
		}
	}
}

#endif /* TOPPERS_tskpri */

/*
 *  レディキューの回転
 *
 *  実行すべきタスクを更新するのは，実行すべきタスクがタスクキューの末
 *  尾に移動した場合である．
 */
#ifdef TOPPERS_tskrot

void
rotate_ready_queue(QUEUE *p_queue)
{
	QUEUE	*p_entry;

	if (!queue_empty(p_queue) && p_queue->p_next->p_next != p_queue) {
		p_entry = queue_delete_next(p_queue);
		queue_insert_prev(p_queue, p_entry);
		if (dspflg) {
			if (p_schedtsk == (TCB *) p_entry) {
				p_schedtsk = (TCB *)(p_queue->p_next);
			}
		}
	}
}

#endif /* TOPPERS_tskrot */

/*
 *  タスクの終了処理
 */
#ifdef TOPPERS_tskterm

void
task_terminate(TCB *p_tcb)
{
	if (TSTAT_RUNNABLE(p_tcb->tstat)) {
		make_non_runnable(p_tcb);
	}
	else if (TSTAT_WAITING(p_tcb->tstat)) {
		wait_dequeue_wobj(p_tcb);
		wait_dequeue_tmevtb(p_tcb);
	}
	if (p_tcb->p_lastmtx != NULL) {
		(*mtxhook_release_all)(p_tcb);
	}
	make_dormant(p_tcb);
	if (p_tcb->actque) {
		p_tcb->actque = false;
		make_active(p_tcb);
	}
}

#endif /* TOPPERS_tskterm */
