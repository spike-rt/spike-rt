/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2005-2015 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: core_support.S 1517 2021-12-10 13:07:12Z ertl-komori $
 */

/*
 *  プロセッサ依存モジュール アセンブリ言語部（ARM-M用）
 */

#define TOPPERS_MACRO_ONLY
#define UINT_C(val)		(val)		/* uint_t型の定数を作るマクロ */
#define ULONG_C(val)	(val)		/* ulong_t型の定数を作るマクロ */
#define CAST(type, val)	(val)		/* 型キャストを行うマクロ */

#include "kernel_impl.h"
#include "arm_m.h"
#include "offset.h"
#include "target_asm.inc"

/*
 *  CPU例外エントリ
 */
	AALIGN(2)
	ATEXT
	ATHUMB(core_exc_entry)
	AGLOBAL(core_exc_entry)
ALABEL(core_exc_entry)
	/*
	 *  例外/割込みが発生すると，発生時にアクティブなスタックにスクラ
	 *  ッチレジスタ等が保存される．
	 *  この内容に加えて，basepri の値と EXC_RETURN の情報を加えて保存する．
	 *  basepri は exc_sense_context() および exc_get_iipm() で利用する．
	 *
	 *   ---------------------
	 *  |  primask | basepri  |
	 *   ---------------------
	 *  |      EXC_RETURN     |
	 *   ---------------------
	 *  |          R0         |
	 *   ---------------------
	 *  |          R1         |
	 *   ---------------------
	 *  |          R2         |
	 *   ---------------------
	 *  |          R3         |
	 *   ---------------------
	 *  |         R12         |
	 *   ---------------------
	 *  |          LR         |
	 *   ---------------------
	 *  |          PC         |
	 *   ---------------------
	 *  |         xPSR        |
	 *   ---------------------
	 */
	mrs   r1, basepri
	mrs   r2, primask
	orr   r1, r1, r2, lsl #8
	tst   lr, #EXC_RETURN_PSP
	beq   core_exc_entry_1
	/* タスクコンテキストで発生した CPU 例外の場合 */
	mrs   r0, psp
	stmfd r0!, {r1, lr} /* 例外フレームの上に basepri と EXC_RETURN を積む */
	push  {r1, lr} /* リターン処理のため MSP 上にも積む */
	b     core_exc_entry_2
ALABEL(core_exc_entry_1)
	/* 非タスクコンテキストで発生した CPU 例外の場合 */
	push  {r1, lr}
	mov   r0, sp
ALABEL(core_exc_entry_2)
	/* CPU ロック状態を保存する */
	ldr   r2, =lock_flag
	ldr   r3, [r2]
	push  {r2, r3}
#ifdef TOPPERS_SUPPORT_OVRHDR
	push  {r0, r1}
	bl    ovrtimer_stop
	pop   {r0, r1}
#endif /* TOPPERS_SUPPORT_OVRHDR */
#ifdef LOG_EXC_ENTER
	push  {r0, r1}
	mrs   r0, ipsr /* 例外番号をパラメータに */
	bl    log_exc_enter /* log_exc_enterを呼び出す */
	pop   {r0, r1}
#endif /* LOG_EXC_ENTER */
	/*
	 *  CPU例外ハンドラの呼び出し
	 */
	mrs   r3, ipsr
	ldr   r1, =_kernel_exc_tbl
	ldr   r2, [r1, r3, lsl #2]
	/* 引数 p_excinf は r0 に設定されている */
	blx   r2
#ifdef LOG_EXC_ENTER
	mrs   r0, ipsr /* 例外番号をパラメータに */
	bl    log_exc_leave /* log_exc_leaveを呼び出す */
#endif /* LOG_EXC_ENTER */
#ifdef TOPPERS_SUPPORT_OVRHDR
	bl    ovrtimer_start
#endif /* TOPPERS_SUPPORT_OVRHDR */
	/* CPU ロック状態と割り込み優先度を元に戻す */
	pop   {r0-r3}
	str   r1, [r0]
	msr   basepri, r2
	bx    r3 /* リターン */

/*
 * 遅延ディスパッチ
 */
	ATHUMB(pendsv_handler)
	AGLOBAL(pendsv_handler)
ALABEL(pendsv_handler)
	/* カーネル管理の割り込みを禁止 */
	mov    r0, #IIPM_LOCK
	msr    basepri, r0
	ldr    r1, =p_runtsk
	ldr    r0, [r1]
	/* p_runtsk == NULL <=> アイドル処理から -> コンテキスト保存なし */
	cbz    r0, pendsv_handler_1
	/* コンテキストを保存 */
	mrs    r2, psp
#ifdef TOPPERS_FPU_CONTEXT
	tst    lr, #EXC_RETURN_FP
	bne    pendsv_handler_0
	vstmdb r2!, {s16-s31}
	/* vstmdb により，Lazy Stacking の際もスクラッチが保存される． */
ALABEL(pendsv_handler_0)
#endif /* TOPPERS_FPU_CONTEXT */
	stmfd  r2!, {r4-r11}
	str    r2, [r0, #TCB_sp]
	str    lr, [r0, #TCB_pc]
ALABEL(pendsv_handler_1)
	/*
	 * この時点でのレジスタ
	 * r0 : p_runtsk
	 * r1 : &p_runtsk
	 */
#ifdef LOG_DSP_ENTER
	push   {r0, r1}
	bl     log_dsp_enter
	pop    {r0, r1}
#endif /* LOG_DSP_ENTER */
	ldr    r0, =p_schedtsk
	ldr    r0, [r0]
	str    r0, [r1] /* p_runtsk = p_schedtsk */
	cbz    r0, return_to_idle /* p_runtsk == NULL なら idle_loop へ */
#ifdef LOG_DSP_LEAVE
	push   {r0, r1}
	bl     log_dsp_leave
	pop    {r0, r1}
#endif /* LOG_DSP_LEAVE */
#if __TARGET_ARCH_THUMB >= 5
	ldr    r1, [r0, #TCB_stk_top]
	msr    psplim, r1
#endif /* __TARGET_ARCH_THUMB >= 5 */
	/* 戻り先が EXC_RETURN でないなら Thread モードへ */
	ldr    r1, [r0, #TCB_pc]
	and    r2, r1, #EXC_RETURN_PREFIX
	cmp    r2, #EXC_RETURN_PREFIX
	bne    return_to_thread
	ldr    r2, [r0, #TCB_sp]
	ldmfd  r2!, {r4-r11}
#ifdef TOPPERS_FPU_CONTEXT
	tst    r1, #EXC_RETURN_FP
	bne    pendsv_handler_2
	vldmia r2!, {s16-s31}
ALABEL(pendsv_handler_2)
#endif /* TOPPERS_FPU_CONTEXT */
	msr    psp, r2
	/*
	 * 割り込み優先度マスク全解除状態へ
	 * この区間でカーネル管理の割り込みが発生しないよう faultmask をセットする．
	 * 例外リターンにより，faultmask は自動的に解除される．
	 */
	mov    r0, #IIPM_ENAALL
	cpsid  f
	msr    basepri, r0
	bx     r1
ALABEL(return_to_idle)
	mrs    r0, psp
	ldr    r1, =idle_loop
	ldr    r2, =#EPSR_T
	stmfd  r0!, {r1, r2}
	sub    r0, #(4 * 6)
	msr    psp, r0
	/*
	 * GNU AS はバージョンにより ldr pc, =#EXC_RETURN を
	 * mvn pc, #2 のように変換するが，mvn 命令により pc に
	 * EXC_RETURN を代入して例外復帰することはできない．
	 * そのため以下のように ldr 命令を直接記述している．
	 */
AALIGN(2)
	ldr    pc, [pc]
	.word  EXC_RETURN
ALABEL(return_to_thread)
	/*
	 * この時点でのレジスタ
	 * r0 : p_runtsk
	 * r1 : p_runtsk->tskctxb.pc
	 */
#ifdef TOPPERS_FPU_CONTEXT
	ldr    r2, [r0, #TCB_fpu_flag]
	cbz    r2, return_to_thread_no_fp
	ldr    r2, =return_to_thread_fp
	ldr    r3, =#EPSR_T
	ldr    r0, [r0, #TCB_sp]
	ldmfd  r0!, {r4-r11}
	stmfd  r0!, {r1-r3}
	sub    r0, #(4 * 5)
	b      return_to_thread_0
ALABEL(return_to_thread_no_fp)
#endif /* TOPPERS_FPU_CONTEXT */
	ldr    r0, [r0, #TCB_sp]
	ldmfd  r0!, {r4-r11}
	ldr    r2, =#EPSR_T
	stmfd  r0!, {r1, r2}
	sub    r0, #(4 * 6)
#ifdef TOPPERS_FPU_CONTEXT
ALABEL(return_to_thread_0)
#endif /* TOPPERS_FPU_CONTEXT */
	msr    psp, r0
	/* CPU ロック状態へ */
	ldr    r0, =lock_flag
	mov    r1, #1
	str    r1, [r0]
	dsb /* lock_cpu_dsp() のコメント参照 */
AALIGN(2)
	ldr    pc, [pc] /* return_to_idle のコメント参照 */
	.word  EXC_RETURN
	/*
	 * この時点でのレジスタ
	 * lr  : p_runtsk->tskctxb.pc
	 */
#ifdef TOPPERS_FPU_CONTEXT
ALABEL(return_to_thread_fp)
	/* 例外リターンにより CONTROL.FPCA がクリアされるが，ここでまたセットされる． */
	vpop   {s16-s31}
	bx     lr
#endif /* TOPPERS_FPU_CONTEXT */

/*
 * Thread モードから Handler モードへの復帰
 * dispatch -> do_dispatch から呼ばれる
 * この時点でのレジスタ
 * r4 : p_runtsk
 * r5 : p_runtsk->tskctxb.pc
 */
	ATHUMB(svc_handler)
	AGLOBAL(svc_handler)
ALABEL(svc_handler)
#ifdef LOG_DSP_LEAVE
	mov    r0, r4
	bl     log_dsp_leave
#endif /* LOG_DSP_LEAVE */
	/* CPU ロック解除状態へ */
	ldr    r0, =lock_flag
	mov    r1, #0
	str    r1, [r0]
	ldr    r0, =#IIPM_ENAALL
	msr    basepri, r0
#if __TARGET_ARCH_THUMB >= 5
	ldr    r0, [r4, #TCB_stk_top]
	msr    psplim, r0
#endif
	ldr    r0, [r4, #TCB_sp]
	mov    r1, r5
	ldmfd  r0!, {r4-r11}
#ifdef TOPPERS_FPU_CONTEXT
	tst    r1, #EXC_RETURN_FP
	bne    svc_handler_0
	vldmia r0!, {s16-s31}
ALABEL(svc_handler_0)
#endif /* TOPPERS_FPU_CONTEXT */
	msr    psp, r0
	bx     r1

/*
 *  ディスパッチャの動作開始
 */
	ATHUMB(start_dispatch)
	AGLOBAL(start_dispatch)
ALABEL(start_dispatch)
	/*
	 *  このルーチンは，カーネル起動時に，すべての割込みを禁止した状態
	 * （割込みロック状態と同等）で呼び出される．また，割込みモード（非
	 *  タスクコンテキストと同等）で呼び出されることを想定している．
	 *
	 *  core_initializeで，lock_flagをtrueに，saved_iipmをIIPM_ENAALLに
	 *  初期化しているため，カーネル管理外の割込みを許可することで，
	 *  CPUロック状態・（モデル上の）割込み優先度マスク全解除状態になる．
	 *  また，task_initializeでdisdspをfalseに初期化しているため，ディ
	 *  スパッチ許可状態になっている．
	 */
#if __TARGET_ARCH_THUMB >= 5
	ldr   r0, =istk
	ldr   r0, [r0]
	msr   msplim, r0
#endif /* __TARGET_ARCH_THUMB >= 5 */
	ldr   r0, =istkpt             /* MSPを初期化   */
	ldr   r1, [r0]                /* start_dispatch呼び出し時に呼び出し用に */
	msr   msp, r1                 /* 使用しているため初期化する             */
	/* 
	 * スタックをIDが1のタスクのスタック領域に切り換える
	 */
	mov   r0, #CONTROL_PSP        /* PSPを有効に  */
	msr   control, r0
	isb                           /* control の操作後に必要 */
	ldr   r1, =tinib_table
	ldr   sp, [r1, #TINIB_stk_bottom]

	/* dispatcher_0 の引数を設定 */
	ldr   r1, =p_schedtsk
	ldr   r1, [r1]
	ldr   r2, =p_runtsk

	cpsie f                       /* カーネル管理外の割込みを許可 */
	b     dispatcher_0

/*
 *  現在のコンテキストを捨ててディスパッチ
 */
	ATHUMB(exit_and_dispatch)
	AGLOBAL(exit_and_dispatch)
ALABEL(exit_and_dispatch)
	/* dispatcher_0 の引数を設定 */
	ldr   r1, =p_schedtsk
	ldr   r1, [r1]
	ldr   r2, =p_runtsk
	b     dispatcher_0

/*
 * ディスパッチャ本体
 * 引数
 * r0: p_runtsk
 * r1: p_schedtsk
 * r2: &p_runtsk
 */
	ATHUMB(do_dispatch)
	AGLOBAL(do_dispatch)
ALABEL(do_dispatch)
#ifdef TOPPERS_FPU_CONTEXT
	mrs   r3, control
	and   r3, #CONTROL_FPCA
	cbz   r3, dispatch_1 /* FPU の使用がなければ保存をスキップ */
	vpush {s16-s31}
	mov   r3, #CONTROL_PSP
	msr   control, r3 /* FPCA をクリア */
	isb
ALABEL(dispatch_1)
	/* fpu_flag はゼロもしくは CONTROL_PSP と同じ値（非ゼロ） */
	str   r3, [r0, #TCB_fpu_flag]
#endif /* TOPPERS_FPU_CONTEXT */
	push  {r4-r11}
	str   lr, [r0, #TCB_pc]
	str   sp, [r0, #TCB_sp]
ALABEL(dispatcher_0)
#ifdef LOG_DSP_ENTER
	push  {r1, r2}
	bl    log_dsp_enter
	pop   {r1, r2}
#endif /* LOG_DSP_ENTER */
	str   r1, [r2] /* p_runtsk = p_schedtsk */
	cbz   r1, idle_loop /* p_runtsk == NULL ならアイドル処理へ */
	ldr   r2, [r1, #TCB_pc]
	/* 実行再開番地が EXC_RETURN なら Handler モードへ */
	and   r0, r2, #EXC_RETURN_PREFIX
	cmp   r0, #EXC_RETURN_PREFIX
	bne   dispatcher_1
	/* r1, r2 は例外ハンドラの先頭で未定義となるため退避 */
	mov   r4, r1
	mov   r5, r2
	svc   #0

/*
 * Thread モードでのコンテキスト復帰
 * この時点でのレジスタ
 * r1 : p_runtsk
 * r2 : p_runtsk->tskctxb.pc
 */
ALABEL(dispatcher_1)
#ifdef LOG_DSP_LEAVE
	push  {r1, r2}
	mov   r0, r1
	bl    log_dsp_leave
	pop   {r1, r2}
#endif /* LOG_DSP_LEAVE */
#if __TARGET_ARCH_THUMB >= 5
	/*
	 * psplim の設定中にカーネル管理外の割り込み/例外が発生して
	 * 例外フレームが積まれてもスタックオーバーフローとならないようにする．
	 * ただし，本当にスタックオーバーフローした場合も検知できない．
	 */
	mov   r0, #0
	msr   psplim, r0
	ldr   sp, [r1, #TCB_sp]
	ldr   r0, [r1, #TCB_stk_top]
	msr   psplim, r0
ALABEL(dispatcher_1_after_sp)
#else
	ldr   sp, [r1, #TCB_sp]
#endif /* __TARGET_ARCH_THUMB >= 5 */
	pop   {r4-r11}
#ifdef TOPPERS_FPU_CONTEXT
	ldr   r3, [r1, #TCB_fpu_flag]
	cbz   r3, dispatcher_2
	vpop  {s16-s31} /* CONTROL.FPCA もセットされる */
ALABEL(dispatcher_2)
#endif /* TOPPERS_FPU_CONTEXT */
	bx    r2

ALABEL(idle_loop)
#ifdef TOPPERS_CUSTOM_IDLE
	toppers_asm_custom_idle
#else
	/* CPU ロック解除状態へ */
	ldr r0, =lock_flag
	mov r1, #0
	str r1, [r0]
	dsb /* unlock_cpu_dsp() のコメント参照 */
	ldr r0, =#IIPM_ENAALL
	msr basepri, r0
ALABEL(_idle_loop_1)
	/*
	 *  basepriを全割込み許可に設定してからwfiが発行されるまでに
	 *  割込みが入った場合，そこでディスパッチが必要な状態になれば
	 *  割込みの出口処理で遅延ディスパッチし，ここには戻ってこない．
	 *  遅延ディスパッチしなかった場合はここに戻ってきて次の割込みを
	 *  wfiで待つため，想定どおりの動作となる
	 */
	dsb /* スリープ前に全てのメモリアクセスが完了していることを保証する */
	wfi
	b     _idle_loop_1
	nop
#endif /* TOPPERS_CUSTOM_IDLE */

/*
 * タスクの起動
 */
	ATHUMB(start_r)
	AGLOBAL(start_r)
ALABEL(start_r)
#ifdef TOPPERS_SUPPORT_OVRHDR
	bl  ovrtimer_start
#endif /* TOPPERS_SUPPORT_OVRHDR */
	/* CPU ロック解除状態へ */
	ldr r0, =lock_flag
	mov r1, #0
	str r1, [r0]
	dsb /* unlock_cpu_dsp() のコメント参照 */
	ldr r0, =#IIPM_ENAALL
	msr basepri, r0
	ldr r0, =p_runtsk
	ldr r1, [r0]
	ldr r2, [r1, #TCB_p_tinib]
	ldr r0, [r2, #TINIB_exinf]
	ldr lr, =ext_tsk
	ldr pc, [r2, #TINIB_task]

/*
 *  カーネルの終了処理の呼出し
 *
 *  スタックを非タスクコンテキスト用に切り替え．
 *  
 */
	ATHUMB(call_exit_kernel)
	AGLOBAL(call_exit_kernel)
ALABEL(call_exit_kernel)
	mov   r1, #CONTROL_MSP
	msr   control, r1  /* MSPを有効に  */
	isb                /* control の操作後に必要 */
	b     exit_kernel  /* カーネルの終了処理を呼ぶ */

/*
 *  微少時間待ち
 */
	AALIGN(2)
	ATEXT
	ATHUMB(sil_dly_nse)
	AGLOBAL(sil_dly_nse)
ALABEL(sil_dly_nse)
	sub   r0, r0, #SIL_DLY_TIM1
	cmp   r0, #0
	bgt   sil_dly_nse1
	bx    lr
ALABEL(sil_dly_nse1)
	sub   r0, r0, #SIL_DLY_TIM2
	cmp   r0, #0
	bgt   sil_dly_nse1
	bx    lr
