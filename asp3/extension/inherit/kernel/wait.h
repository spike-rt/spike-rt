/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2000 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
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
 *  $Id: wait.h 1602 2022-09-12 17:00:18Z ertl-hiro $
 */

/*
 *		待ち状態管理モジュール
 */

#ifndef TOPPERS_WAIT_H
#define TOPPERS_WAIT_H

#include "kernel_impl.h"
#include <queue.h>
#include "task.h"
#include "taskhook.h"
#include "time_event.h"

/*
 *  タスクの優先度順の待ちキューへの挿入
 *
 *  p_tcbで指定されるタスクを，タスク優先度順のキューp_queueに挿入する．
 *  キューの中に同じ優先度のタスクがある場合には，その最後に挿入する．
 */
Inline void
queue_insert_tpri(QUEUE *p_queue, TCB *p_tcb)
{
	QUEUE	*p_entry;
	uint_t	pri = p_tcb->priority;

	for (p_entry = p_queue->p_next; p_entry != p_queue;
										p_entry = p_entry->p_next) {
		if (pri < ((TCB *) p_entry)->priority) {
			break;
		}
	}
	queue_insert_prev(p_entry, &(p_tcb->task_queue));
}

/*
 *  待ち状態への遷移
 *
 *  実行中のタスクを待ち状態に遷移させる．具体的には，実行中のタスクの
 *  タスク状態をtstatにしてレディキューから削除し，TCBのp_winfoフィー
 *  ルド，WINFOのp_tmevtbフィールドを設定する．
 */
Inline void
make_wait(uint_t tstat, WINFO *p_winfo)
{
	p_runtsk->tstat = tstat;
	make_non_runnable(p_runtsk);
	p_runtsk->p_winfo = p_winfo;
	p_winfo->p_tmevtb = NULL;
}

/*
 *  待ち状態への遷移（タイムアウト指定）
 *
 *  実行中のタスクを，タイムアウト指定付きで待ち状態に遷移させる．具体
 *  的には，実行中のタスクのタスク状態をtstatにしてレディキューから削
 *  除し，TCBのp_winfoフィールド，WINFOのp_tmevtbフィールドを設定する．
 *  また，タイムイベントブロックを登録する．
 */
extern void	make_wait_tmout(uint_t tstat, WINFO *p_winfo,
									TMEVTB *p_tmevtb, TMO tmout);

/*
 *  待ち解除のためのタスク状態の更新
 *
 *  p_tcbで指定されるタスクを，待ち解除するようタスク状態を更新する．
 *  待ち解除するタスクが実行できる状態になる場合は，レディキューにつな
 *  ぐ．
 */
Inline void
make_non_wait(TCB *p_tcb)
{
	assert(TSTAT_WAITING(p_tcb->tstat));

	if (!TSTAT_SUSPENDED(p_tcb->tstat)) {
		/*
		 *  待ち状態から実行できる状態への遷移
		 */
		p_tcb->tstat = TS_RUNNABLE;
		LOG_TSKSTAT(p_tcb);
		make_runnable(p_tcb);
	}
	else {
		/*
		 *  二重待ち状態から強制待ち状態への遷移
		 */
		p_tcb->tstat = TS_SUSPENDED;
		LOG_TSKSTAT(p_tcb);
	}
}

/*
 *  オブジェクト待ちキューからの削除
 *
 *  p_tcbで指定されるタスクが，同期・通信オブジェクトの待ちキューにつ
 *  ながれていれば，待ちキューから削除する．
 */
Inline void
wait_dequeue_wobj(TCB *p_tcb)
{
	if (TSTAT_WAIT_WOBJ(p_tcb->tstat)) {
		queue_delete(&(p_tcb->task_queue));
		if (TSTAT_WAIT_MTX(p_tcb->tstat)) {
			(*mtxhook_dequeue_wobj)(p_tcb);
		}
	}
}

/*
 *  時間待ちのためのタイムイベントブロックの登録解除
 *
 *  p_tcbで指定されるタスクに対して，時間待ちのためのタイムイベントブ
 *  ロックが登録されていれば，それを登録解除する．
 */
Inline void
wait_dequeue_tmevtb(TCB *p_tcb)
{
	if (p_tcb->p_winfo->p_tmevtb != NULL) {
		tmevtb_dequeue(p_tcb->p_winfo->p_tmevtb);
	}
}

/*
 *  待ち解除
 *
 *  p_tcbで指定されるタスクの待ち状態を解除する．具体的には，タイムイ
 *  ベントブロックが登録されていれば，それを登録解除する．また，タスク
 *  状態を更新し，待ち解除したタスクからの返値をE_OKとする．待ちキュー
 *  からの削除は行わない．
 */
extern void	wait_complete(TCB *p_tcb);

/*
 *  タイムアウトに伴う待ち解除
 *
 *  p_tcbで指定されるタスクが，待ちキューにつながれていれば待ちキュー
 *  から削除し，タスク状態を更新する．また，待ち解除したタスクからの返
 *  値を，wait_tmoutではE_TMOUT，wait_tmout_okではE_OKとする．
 *
 *  wait_tmout_okは，dly_tskで使うためのもので，待ちキューから削除する
 *  処理を行わない．
 *
 *  いずれの関数も，タイムイベントのコールバック関数として用いるための
 *  もので，割込みハンドラから呼び出されることを想定している．
 */
extern void	wait_tmout(TCB *p_tcb);
extern void	wait_tmout_ok(TCB *p_tcb);

/*
 *  待ちキューの先頭のタスクID
 *
 *  p_wait_queueで指定した待ちキューの先頭のタスクIDを返す．待ちキュー
 *  が空の場合には，TSK_NONEを返す．
 */
Inline ID
wait_tskid(QUEUE *p_wait_queue)
{
	if (!queue_empty(p_wait_queue)) {
		return(TSKID((TCB *) p_wait_queue->p_next));
	}
	else {
		return(TSK_NONE);
	}
}

/*
 *  同期・通信オブジェクトの管理ブロックの共通部分操作ルーチン
 *
 *  同期・通信オブジェクトの初期化ブロックと管理ブロックの先頭部分は共
 *  通になっている．以下は，その共通部分を扱うための型およびルーチン群
 *  である．
 *
 *  複数の待ちキューを持つ同期・通信オブジェクトの場合，先頭以外の待ち
 *  キューを操作する場合には，これらのルーチンは使えない．また，オブジェ
 *  クト属性のTA_TPRIビットを参照するので，このビットを他の目的に使って
 *  いる場合も，これらのルーチンは使えない．
 */

/*
 *  同期・通信オブジェクトの初期化ブロックの共通部分
 */
typedef struct wait_object_initialization_block {
	ATR			wobjatr;		/* オブジェクト属性 */
} WOBJINIB;

/*
 *  同期・通信オブジェクトの管理ブロックの共通部分
 */
typedef struct wait_object_control_block {
	QUEUE		wait_queue;		/* 待ちキュー */
	const WOBJINIB *p_wobjinib;	/* 初期化ブロックへのポインタ */
} WOBJCB;

/*
 *  同期・通信オブジェクトの待ち情報ブロックの共通部分
 *
 *  この構造体は，待ち情報ブロック（WINFO）を拡張（オブジェクト指向言
 *  語の継承に相当）したものであるが，WINFOが共用体で定義されているた
 *  めに，1つのフィールドとして含めている．
 */
typedef struct wait_object_waiting_information {
	WINFO	winfo;			/* 標準の待ち情報ブロック */
	WOBJCB	*p_wobjcb;		/* 待ちオブジェクトの管理ブロック */
} WINFO_WOBJ;

/*
 *  同期・通信オブジェクトに対する待ち状態への遷移
 *  
 *  実行中のタスクを待ち状態に遷移させ，同期・通信オブジェクトの待ちキュー
 *  につなぐ．また，待ち情報ブロック（WINFO）のp_wobjcbを設定する．
 *  wobj_make_wait_tmoutは，タイムイベントブロックの登録も行う．
 */
extern void	wobj_make_wait(WOBJCB *p_wobjcb, uint_t tstat,
				 							WINFO_WOBJ *p_winfo_wobj);
extern void	wobj_make_wait_tmout(WOBJCB *p_wobjcb, uint_t tstat,
						WINFO_WOBJ *p_winfo_wobj, TMEVTB *p_tmevtb, TMO tmout);

/*
 *  タスク優先度変更時の処理
 *
 *  同期・通信オブジェクトに対する待ち状態にあるタスクの優先度が変更さ
 *  れた場合に，待ちキューの中でのタスクの位置を修正する．
 */
Inline void
wobj_change_priority(WOBJCB *p_wobjcb, TCB *p_tcb)
{
	if ((p_wobjcb->p_wobjinib->wobjatr & TA_TPRI) != 0U) {
		queue_delete(&(p_tcb->task_queue));
		queue_insert_tpri(&(p_wobjcb->wait_queue), p_tcb);
	}
}

/*
 *  待ちキューの初期化
 *
 *  待ちキューにつながれているタスクをすべて待ち解除する．待ち解除した
 *  タスクからの返値は，E_DLTとする．
 */
extern void	init_wait_queue(QUEUE *p_wait_queue);

#endif /* TOPPERS_WAIT_H */
