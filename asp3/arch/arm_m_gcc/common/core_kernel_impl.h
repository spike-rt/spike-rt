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
 *  @(#) $Id: core_kernel_impl.h 1500 2021-07-28 12:35:13Z ertl-komori $
 */

/*
 *		プロセッサ依存モジュール（ARM-M用）
 *
 *  このインクルードファイルは，target_config.h（または，そこからインク
 *  ルードされるファイル）のみからインクルードされる．他のファイルから
 *  直接インクルードしてはならない．
 */

#ifndef TOPPERS_CORE_KERNEL_IMPL_H
#define TOPPERS_CORE_KERNEL_IMPL_H

/*
 *  型キャストを行うマクロの定義
 */
#ifndef CAST
#define CAST(type, val)     ((type)(val))
#endif /* CAST */

#ifndef TOPPERS_MACRO_ONLY

/*
 *  プロセッサの特殊命令のインライン関数定義
 */
#include <core_insn.h>

/*
 *  ターゲット依存のオブジェクト属性
 */
#define TA_DIRECT     UINT_C(0x01) /* CPU 例外ハンドラの直接呼び出し */
#define TARGET_EXCATR (TA_DIRECT)  /* ターゲット定義のCPU例外ハンドラ属性 */
#define TARGET_INHATR TA_NONKERNEL /* ターゲット定義の割込みハンドラ属性 */

/*
 *  エラーチェック方法の指定
 */
#define CHECK_STKSZ_ALIGN	8	/* スタックサイズのアライン単位 */
#define CHECK_FUNC_ALIGN	1	/* 関数のアライン単位 */
#define CHECK_FUNC_NONNULL		/* 関数の非NULLチェック */
#define CHECK_STACK_ALIGN	8	/* スタック領域のアライン単位 */
#define CHECK_STACK_NONNULL		/* スタック領域の非NULLチェック */
#define CHECK_MPF_ALIGN		4	/* 固定長メモリプール領域のアライン単位 */
#define CHECK_MPF_NONNULL		/* 固定長メモリプール領域の非NULLチェック */
#define CHECK_MB_ALIGN		4	/* 管理領域のアライン単位 */

/*
 *  非タスクコンテキスト用のスタック初期値
 */
#define TOPPERS_ISTKPT(istk, istksz) ((STK_T *)((char *)(istk) + (istksz)))

/*
 *  タスクコンテキストブロックの定義
 */
typedef struct task_context_block {
	uint32_t    *sp;      /* スタックポインタ */
	intptr_t    pc;       /* プログラムカウンタ */
#if __TARGET_ARCH_THUMB >= 4
#ifndef TOPPERS_FPU_CONTEXT
	union {
#endif /* TOPPERS_FPU_CONTEXT */
	uint32_t    *stk_top; /* アクセス高速化のための TINIB のコピー */
	uint32_t    fpu_flag; /* FPU レジスタを復帰するかのフラグ */
#ifndef TOPPERS_FPU_CONTEXT
	};
#endif /* TOPPERS_FPU_CONTEXT */
#endif /* __TARGET_ARCH_THUMB >= 4 */
} TSKCTXB;

/*
 *	タスク初期化コンテキストブロック
 */
#define USE_TSKINICTXB	/*  TSKINICTXBを使用する  */

typedef struct task_initialization_context_block {
	uint32_t    *stk_top;    /* スタック領域の末尾番地 */
	uint32_t    *stk_bottom; /* スタック領域の先頭番地 */
} TSKINICTXB;

#endif /* TOPPERS_MACRO_ONLY */

#ifndef TOPPERS_MACRO_ONLY
/*
 *  コンテキストの参照
 *
 */
Inline bool_t
sense_context(void)
{
	/*
	 *  PSPが有効ならタスクコンテキスト，MSPが有効なら非タスクコンテキスト
	 *  とする．
	 */
	if ((get_control() & CONTROL_PSP) == CONTROL_PSP){
		return false;
	}
	else {
		return true;
	}
}

#endif /* TOPPERS_MACRO_ONLY */

/*
 *  ARMv7,8-MとARMv6-Mで異なる処理
 *  ARMv6-Mの処理はcore_kernel_impl_v6m.hに記述する
 */
#if __TARGET_ARCH_THUMB >= 4

/*
 *  TOPPERS標準割込み処理モデルの実現
 *
 *  割込み優先度マスクとしては，BASEPRIを用いる．全割込みを禁止する
 *  機能として，FAULTMASKやPRIMASKがあるが，カーネル管理外の割込みを
 *  サポートするため，これらはCPUロックのために用いない．
 *  そのため，BASEPRIを用いて擬似的にCPUロックフラグを実現する．
 *
 *  まず，CPUロック状態を管理すための変数(lock_flag)を用意する．
 *
 *  CPUロックフラグがクリアされている間は，BASEPRIをモデル上の割込み
 *  優先度マスクの値に設定する．この間は，モデル上の割込み優先度マス
 *  クは，BASEPRIを用いる．
 *
 *  それに対してCPUロックフラグがセットされいる間は，BASEPRIを，カーネ
 *  ル管理外のものを除くすべての割込み要求をマスクする値(TIPM_LOCK)と，
 *  モデル上の割込み優先度マスクとの高い方に設定する．この間のモデル上
 *  の割込み優先度マスクは，そのための変数(saved_iipm, 内部表現で保持)
 *  を用意して保持する．
 */

/*
 *  割込み優先度マスクの外部表現と内部表現の変換
 *
 *  アセンブリ言語のソースファイルからインクルードする場合のために，
 *  CASTを使用
 *  割込み優先度のビット幅(TBITW_IPRI)が 8 の場合は，内部優先度 255
 *  は，外部優先度 -1 に対応する．
 */
#define EXT_IPM(iipm)   (CAST(PRI,((iipm >> (8 - TBITW_IPRI)) - (1 << TBITW_IPRI))))       /* 内部表現を外部表現に */
#define INT_IPM(ipm)    (((1 << TBITW_IPRI) - CAST(uint8_t, -(ipm)))  << (8 - TBITW_IPRI)) /* 外部表現を内部表現に */

/*
 *  CPUロック状態での割込み優先度マスク
 */
#define TIPM_LOCK    TMIN_INTPRI

/*
 *  CPUロック状態での割込み優先度マスクの内部表現
 *
 *  TIPM_LOCKは，CPUロック状態でのBASEPRIの値．カーネル管理外のものを
 *  除くすべての割込みをマスクする値に定義する．
 */
#define IIPM_LOCK    INT_IPM(TIPM_LOCK)

/*
 *  TIPM_ENAALL（割込み優先度マスク全解除）の内部表現
 *
 *  BASEPRIに '0' を設定することで，全割込みを許可する．
 */
#define IIPM_ENAALL  (0)


#ifndef TOPPERS_MACRO_ONLY

/*
 *  CPUロックフラグ実現のための変数
 *
 *  これらの変数は，CPUロック状態の時のみ書き換えてもよいとする．
 *  インライン関数中で，アクセスの順序が変化しないよう，volatile を指定．
 */
extern volatile bool_t   lock_flag;   /* CPUロックフラグの値を保持する変数 */
extern volatile uint32_t saved_iipm;  /* 割込み優先度をマスクする変数 */

/*
 *  CPUロック状態への移行
 *
 *  BASEPRI（ハードウェアの割込み優先度マスク）を，saved_iipmに保存し，
 *  カーネル管理外のものを除くすべての割込みをマスクする値（TIPM_LOCK）
 *  に設定する．また，lock_flagをtrueにする．
 *
 *  BASEPRIが，最初からTIPM_LOCKと同じかそれより高い場合には，それを
 *  saved_iipmに保存するのみで，TIPM_LOCKには設定しない．これは，モデル
 *  上の割込み優先度マスクが，TIPM_LOCKと同じかそれより高いレベルに設定
 *  されている状態にあたる．
 *
 *  この関数は，CPUロック状態（lock_flagがtrueの状態）で呼ばれることは
 *  ないものと想定している．
 */
Inline void
lock_cpu(void)
{
	uint32_t iipm;

	/* 
	 * クリティカルセクションに入る前に全てのメモリアクセスが
	 * 終了していることを保証する．
	 */
	data_synchronization_barrier();
	/*
	 *  get_basepri()の返り値を直接saved_iipmに保存せず，一時変数iipm
	 *  を用いているのは，get_baespri()を呼んだ直後に割込みが発生し，
	 *  起動された割込み処理でsaved_iipmが変更される可能性があるためで
	 *  ある．
	 */
	iipm = get_basepri();
	set_basepri_max(IIPM_LOCK);
	saved_iipm = iipm;
	lock_flag = true;
	/* クリティカルセクションの前後でメモリが書き換わる可能性がある */
	ARM_MEMORY_CHANGED;
}

/*
 *  CPUロック状態の解除
 *
 *  lock_flagをfalseにし，IPM（ハードウェアの割込み優先度マスク）を，
 *  saved_iipmに保存した値に戻す．
 *
 *  この関数は，CPUロック状態（lock_flagがtrueの状態）でのみ呼ばれるも
 *  のと想定している．
 */
Inline void
unlock_cpu(void)
{
	/* クリティカルセクションの前後でメモリが書き換わる可能性がある */
	ARM_MEMORY_CHANGED;
	lock_flag = false;
	/* 
	 * クリティカルセクションを抜ける前に全てのメモリアクセスが
	 * 終了していることを保証する．
	 */
	data_synchronization_barrier();
	set_basepri(saved_iipm);
}


/*
 * タスクディスパッチできる状態において，CPUロック解除状態から，CPUロック
 * 状態へ遷移させる関数．
 *
 * この関数は，ディスパッチできる状態でのみ呼び出される．ディスパッチでき
 * る状態においては，割込み優先度マスク全解除状態である
 */
Inline void
lock_cpu_dsp(void)
{
	/* 
	 * クリティカルセクションに入る前に全てのメモリアクセスが
	 * 終了していることを保証する．
	 */
	data_synchronization_barrier();
	/*
	 * この時点では必ず割込み優先度全解除状態であるため，
	 * ・割込み優先度マスクをCPUロック状態
	 * ・割込み優先度マスクの保存値をIIPM_ENAALL
	 * を必ず実行する
	 */
	set_basepri(IIPM_LOCK);
	saved_iipm = IIPM_ENAALL;
	lock_flag = true;
	/* クリティカルセクションの前後でメモリが書き換わる可能性がある */
	ARM_MEMORY_CHANGED;
}

/*
 * タスクディスパッチできる状態に戻る状況において，CPUロック状態から，CPU
 * ロック解除状態へ遷移させる関数．
 *
 * この関数は，CPUロック解除後は，ディスパッチできる状態に戻る状況でのみ呼
 * び出される．ディスパッチできる状態においては，割込み優先度マスク全解除
 * 状態である
 */
Inline void
unlock_cpu_dsp(void)
{
	/* クリティカルセクションの前後でメモリが書き換わる可能性がある */
	ARM_MEMORY_CHANGED;
	lock_flag = false;
	/* 
	 * クリティカルセクションを抜ける前に全てのメモリアクセスが
	 * 終了していることを保証する．
	 */
	data_synchronization_barrier();
	set_basepri(IIPM_ENAALL);
}

/*
 *  CPUロック状態の参照
 */
Inline bool_t
sense_lock(void)
{
	return(lock_flag);
}

/*
 *  chg_ipmで有効な割込み優先度の範囲の判定
 *
 *  TMIN_INTPRIの値によらず，chg_ipmでは，-(1 << TBITW_IPRI)〜TIPM_ENAALL（＝0）
 *  の範囲に設定できることとする（ターゲット定義の拡張）．
 *  割込み優先度のビット幅(TBITW_IPRI)が 8 の場合は，-256 〜 0 が指定可能である．
 *
 */
#define VALID_INTPRI_CHGIPM(intpri) \
				((-((1 << TBITW_IPRI) - 1) <= (intpri) && (intpri) <= TIPM_ENAALL))

/*
 * （モデル上の）割込み優先度マスクの設定
 *
 *  この関数はタスクコンテキスト，CPUロック状態でのみ呼び出される．
 *  saved_iipmを設定し，さらに，ハードウェアの割込み優先度マスクを，
 *  設定しようとした（モデル上の）割込み優先度マスクとTIPM_LOCKの
 *  高い方に設定する．
 */
Inline void
t_set_ipm(PRI intpri)
{
	if (intpri == TIPM_ENAALL) {
		saved_iipm = IIPM_ENAALL;
	} else {
		const uint32_t iipm = INT_IPM(intpri);
		saved_iipm = iipm;
		set_basepri_max(iipm);
	}
}

/*
 * （モデル上の）割込み優先度マスクの参照
 *
 *  この関数はタスクコンテキスト，CPUロック状態でのみ呼び出される．
 */
Inline PRI
t_get_ipm(void)
{
	const uint32_t iipm = saved_iipm;
	if (iipm == IIPM_ENAALL) {
		return TIPM_ENAALL;
	} else {
		return EXT_IPM(iipm);
	}
}

/*
 *  割込み要求禁止フラグ
 */

/*
 *  割込み属性が設定されているかを判別するための変数（kernel_cfg.c）
 */
extern const uint32_t	bitpat_cfgint[];

/*
 *  割込み属性の設定のチェック
 */
Inline bool_t
check_intno_cfg(INTNO intno)
{
	if ((bitpat_cfgint[intno >> 5] & (1 << (intno & 0x1f))) == 0x00) {
		return(false);
	}
	return(true);
}

/*
 *  割込み要求禁止フラグのセット
 *
 */
Inline void
disable_int(INTNO intno)
{
	uint32_t tmp;

	if (intno == IRQNO_SYSTICK) {
		tmp = sil_rew_mem((void *)SYSTIC_CONTROL_STATUS);
		tmp &= ~SYSTIC_TICINT;
		sil_wrw_mem((void *)SYSTIC_CONTROL_STATUS, tmp);
	}else {
		tmp = intno - 16;
		sil_wrw_mem((void *)((uint32_t *)NVIC_CLRENA0 + (tmp >> 5)),
					(1 << (tmp & 0x1f)));
	}
}

/*
 *  割込み要求禁止フラグの解除
 *
 */
Inline void
enable_int(INTNO intno)
{
	uint32_t tmp;

	if (intno == IRQNO_SYSTICK) {
		tmp = sil_rew_mem((void *)SYSTIC_CONTROL_STATUS);
		tmp |= SYSTIC_TICINT;
		sil_wrw_mem((void *)SYSTIC_CONTROL_STATUS, tmp);
	}else {
		tmp = intno - 16;
		sil_wrw_mem((void *)((uint32_t *)NVIC_SETENA0 + (tmp >> 5)),
					(1 << (tmp & 0x1f)));
	}
}

/*
 *  割込み要求がクリアできる割込み番号の範囲の判定
 */
#define VALID_INTNO_CLRINT(intno) \
				(IRQNO_SYSTICK <= (intno) && (intno) <= TMAX_INTNO)

/*
 *  割込み要求がクリアできる状態か？
 */
Inline bool_t
check_intno_clear(INTNO intno)
{
	return(true);
}

/*
 *  割込み要求のクリア
 */
Inline void
clear_int(INTNO intno)
{
	uint32_t tmp;

	if (intno == IRQNO_SYSTICK) {
		tmp = sil_rew_mem((void*)NVIC_ICSR);
		tmp &= ~NVIC_PENDSTSET;
		sil_wrw_mem((void*)NVIC_ICSR, tmp);
	}else {
		tmp = intno - 16;
		sil_wrw_mem((void *)((uint32_t *)NVIC_ICPR0 + (tmp >> 5)),
					(1 << (tmp & 0x1f)));
	}
}

/*
 *  割込みが要求できる状態か？
 */
Inline bool_t
check_intno_raise(INTNO intno)
{
	return(true);
}

/*
 *  割込みの要求
 */
Inline void
raise_int(INTNO intno)
{
	uint32_t tmp;

	if (intno == IRQNO_SYSTICK) {
		tmp = sil_rew_mem((void*)NVIC_ICSR);
		tmp |= NVIC_PENDSTSET;
		sil_wrw_mem((void*)NVIC_ICSR, tmp);
	}else {
		tmp = intno - 16;
		sil_wrw_mem((void *)((uint32_t *)NVIC_ISPR0 + (tmp >> 5)),
					(1 << (tmp & 0x1f)));
	}
}

/*
 *  割込み要求のチェック
 */
Inline bool_t
probe_int(INTNO intno)
{
	uint32_t tmp;

	if (intno == IRQNO_SYSTICK) {
		return ((sil_rew_mem((void*)NVIC_ICSR) & NVIC_PENDSTSET) == NVIC_PENDSTSET);
	}else {
		tmp = intno - 16;
		return ((sil_rew_mem((void *)NVIC_ISPR0 + (tmp >> 5)) & (1 << (tmp & 0x1f)))
		  == (1 << (tmp & 0x1f)));
	}
}

#endif /* TOPPERS_MACRO_ONLY */

#else /* __TARGET_ARCH_THUMB == 3 */

/*
 *  ARMv6-Mに関する処理 
 */
#include "core_kernel_impl_v6m.h"

#endif /* __TARGET_ARCH_THUMB >= 4 */

#ifndef TOPPERS_MACRO_ONLY

/*
 * スタートアップルーチン（start.S）
 */
extern void _kernel_start(void);

/*
 * PendSV 例外ハンドラ (core_support.S)
 */
extern void pendsv_handler(void);

/*
 * Supervisor Call 例外ハンドラ (core_support.S)
 */
extern void svc_handler(void);

/*
 * ディスパッチャの実体 (core_support.S)
 */
extern void do_dispatch(void *, void *, void *);

/*
 * dispatch() をコールする直前には，多くの場合で
 * p_runtsk と p_schedtsk を参照している．
 * これを引数として渡すことで，効率化が期待できる．
 */
#ifdef TOPPERS_SUPPORT_OVRHDR
#define _kernel_dispatch() \
	ovrtimer_stop(); \
	do_dispatch(p_runtsk, p_schedtsk, &p_runtsk); \
	ovrtimer_start()
#else
#define _kernel_dispatch() \
	do_dispatch(p_runtsk, p_schedtsk, &p_runtsk)
#endif

Inline void
request_dispatch_retint(void)
{
	/*
	 * issue PendSV
	 * ICSR.PENDSVSET([28]) = 1
	 */
	sil_wrw_mem((void *)0xE000ED04, (uint32_t)(0x10000000U));
}

/*
 *  ディスパッチャの動作開始（core_support.S）
 *
 *  start_dispatchは，カーネル起動時に呼び出すべきもので，すべての割込
 *  みを禁止した状態（割込みロック状態と同等の状態）で呼び出さなければ
 *  ならない．
 */
extern void start_dispatch(void) NoReturn;

/*
 *  現在のコンテキストを捨ててディスパッチ（core_support.S）
 *
 *  exit_and_dispatchは，ext_tskから呼び出すべきもので，タスクコンテキ
 *  スト・CPUロック状態・ディスパッチ許可状態・（モデル上の）割込み優先
 *  度マスク全解除状態で呼び出さなければならない．
 */
extern void exit_and_dispatch(void) NoReturn;

/*
 *  カーネルの終了処理の呼出し（core_support.S）
 *
 *  call_exit_kernelは，カーネルの終了時に呼び出すべきもので，非タスク
 *  コンテキストに切り換えて，カーネルの終了処理（exit_kernel）を呼び出
 *  す．
 */
extern void call_exit_kernel(void) NoReturn;

/*
 *  タスクコンテキストの初期化
 *
 *  タスクが休止状態から実行できる状態に移行する時に呼ばれる．この時点
 *  でスタック領域を使ってはならない．
 *
 *  activate_contextを，インライン関数ではなくマクロ定義としているのは，
 *  この時点ではTCBが定義されていないためである．
 */
extern void start_r(void);

#ifdef TOPPERS_FPU_CONTEXT
#define activate_context(p_tcb) \
{ \
	(p_tcb)->tskctxb.sp = (p_tcb)->p_tinib->tskinictxb.stk_bottom - 8; \
	(p_tcb)->tskctxb.pc = (intptr_t)start_r; \
	(p_tcb)->tskctxb.fpu_flag = 0; \
}
#else
#define activate_context(p_tcb) \
{ \
	(p_tcb)->tskctxb.sp = (p_tcb)->p_tinib->tskinictxb.stk_bottom - 8; \
	(p_tcb)->tskctxb.pc = (intptr_t)start_r; \
}
#endif /* TOPPERS_FPU_CONTEXT */

/*
 *  割込み番号・割込みハンドラ番号
 *
 *  割込みハンドラ番号(inhno)と割込み番号(intno)は，割り込み発生時に
 *  EPSRに設定される例外番号とする．
 */

/*
 *  割込み番号の範囲の判定
 */
#define VALID_INTNO(intno)           ((TMIN_INTNO <= (intno)) && ((intno) <= TMAX_INTNO))
#define VALID_INTNO_DISINT(intno)    VALID_INTNO(intno)
#define VALID_INTNO_CFGINT(intno)    VALID_INTNO(intno)

/*
 *  割込みハンドラの設定
 *
 *  ベクトル番号inhnoの割込みハンドラの起動番地int_entryに設定する．割込み
 *  ハンドラテーブル
 */
Inline void
define_inh(INHNO inhno, FP int_entry)
{

}

/*
 *  割込みハンドラの出入口処理の生成マクロ
 *
 */
#define INT_ENTRY(inhno, inthdr)    inthdr
#define INTHDR_ENTRY(inhno, inhno_num, inthdr) extern void inthdr(void);

/*
 *  割込み要求ラインの属性の設定
 */
extern void config_int(INTNO intno, ATR intatr, PRI intpri);

/*
 *  割込みハンドラ入口で必要なIRC操作
 */
Inline void
i_begin_int(INTNO intno)
{
}

/*
 *  割込みハンドラの出口で必要なIRC操作
 */
Inline void
i_end_int(INTNO intno)
{
}

/*
 *  CPU例外ハンドラ関係
 */

/*
 *  CPU例外ハンドラ番号
 */
#define VALID_EXCNO_DEFEXC(excno)    (TMIN_EXCNO <= (excno) && (excno) <= TMAX_EXCNO)

/*
 *  CPU例外ハンドラの許可
 */
extern void enable_exc(EXCNO excno);

/*
 *  CPU例外ハンドラの禁止
 */
extern void disable_exc(EXCNO excno);

/*
 *  CPU例外ハンドラの設定
 */
Inline void
define_exc(EXCNO excno, FP exc_entry)
{
	/*
	 *  一部の例外は許可を行う必要がある
	 */
	enable_exc(excno);
}

/*
 *  CPU例外ハンドラの入口処理の生成マクロ
 */
#define EXC_ENTRY(excno, exchdr)    exchdr
#define EXCHDR_ENTRY(excno, excno_num, exchdr) extern void exchdr(void *p_excinf);

/*
 *  CPU例外の発生した時のコンテキストと割込みのマスク状態の参照
 *
 *  CPU例外の発生した時のシステム状態が，カーネル実行中でなく，タスクコ
 *  ンテキストであり，割込みロック状態でなく，CPUロック状態でなく，（モ
 *  デル上の）割込み優先度マスク全解除状態である時にtrue，そうでない時
 *  にfalseを返す（CPU例外がカーネル管理外の割込み処理中で発生した場合
 *  にもfalseを返す）．
 *
 *  v7,v8ではCPU例外の発生した時のBASEPRI（ハードウェアの割込み優先度マスク）
 *  がすべての割込みを許可する状態であることをチェックすることで，カー
 *  ネル実行中でないこと，CPUロック状態でないこと，（モデル上の）割込み
 *  優先度マスク全解除状態であることの3つの条件をチェックすることができる
 * （CPU例外が発生した時のlock_flagを参照する必要はない）．
 *  v6ではBASEPRIがないため例外発生時のlock_flagを参照する必要がある．
 */
Inline bool_t
exc_sense_intmask(void *p_excinf)
{
#if __TARGET_ARCH_THUMB == 3
	const uint32_t lockFlag = ((uint32_t *)p_excinf)[P_EXCINF_OFFSET_BASEPRI] & (1 << 9);
	if (lockFlag) {
		/*
		 * (1) カーネル内のクリティカルセクションの実行中でない
		 * (3) CPUロック状態でない
		 */
		return false;
	}
#endif /* __TARGET_ARCH_THUMB == 3 */
	const uint32_t basepri = ((uint32_t *)p_excinf)[P_EXCINF_OFFSET_BASEPRI] & 0xFF;
	if (basepri != IIPM_ENAALL) {
		/*
		 * (1) カーネル内のクリティカルセクションの実行中でない
		 * (3) CPUロック状態でない
		 * (7) 割込み優先度マスクが全解除
		 */
		return false;
	}
	const int primask = ((uint32_t *)p_excinf)[P_EXCINF_OFFSET_BASEPRI] & (1 << 8);
	if (primask) {
		/* (2) 全割込みロック状態でない */
		return false;
	}
	const uint32_t lr = ((uint32_t *)p_excinf)[P_EXCINF_OFFSET_EXC_RETURN];
	if ((lr & EXC_RETURN_PSP) == 0) {
		/*
		 * (4) カーネル管理外の割込みハンドラ実行中でない
		 * (5) カーネル管理外のCPU例外ハンドラ実行中でない
		 * (6) タスクコンテキスト
		 */
		return false;
	}
	return true;
}

/*
 *  CPU例外エントリ（core_support.S）
 */
extern void core_exc_entry(void);

/*
 *  割込みエントリ
 */
extern void core_int_entry(void);

/*
 *  プロセッサ依存の初期化
 */
extern void core_initialize(void);

/*
 *  プロセッサ依存の終了時処理
 */
extern void core_terminate(void);

/*
 * 登録されていない例外が発生すると呼び出される
 */
extern void default_exc_handler(void *p_excinf);

/*
 * 未登録の割込みが発生した場合に呼び出される
 */
extern void default_int_handler(void);

#endif /* TOPPERS_MACRO_ONLY */
#endif /* TOPPERS_CORE_KERNEL_IMPL_H */
