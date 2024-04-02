/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2007-2022 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: target_kernel_impl.c 1782 2023-01-08 14:50:43Z ertl-hiro $
 */

/*
 *		カーネルのターゲット依存部（CT11MPCore用）
 */

#include "kernel_impl.h"
#include <sil.h>
#include "arm.h"

/*
 *  カーネル動作時のメモリマップと関連する定義
 *
 *  0x00000000 - 0x00100000：ベクター領域（1MB）
 *  0x00100000 - 0x0FFFFFFF：DRAM（255MB）
 *  0x10000000 - 0x100FFFFF：Emulation Baseboard上のリソース（1MB）
 *  0x10100000 - 0x101FFFFF：Private Memory Region（1MB）(*)
 *  0x48000000 - 0x4BFFFFFF：SRAM（4MB）
 *
 *  (*) Private Memory Regionの先頭番地は，ボードの設定で変更できる．
 */

/*
 *  MMUへの設定属性（第1レベルディスクリプタ）
 */
#define MMU_ATTR_RAM	(ARM_MMU_DSCR1_SHARED|ARM_MMU_DSCR1_TEX001 \
							|ARMV6_MMU_DSCR1_AP011|ARM_MMU_DSCR1_CB11)
#define MMU_ATTR_IODEV	(ARM_MMU_DSCR1_SHARED|ARM_MMU_DSCR1_TEX000 \
							|ARMV6_MMU_DSCR1_AP011|ARM_MMU_DSCR1_CB01 \
							|ARMV6_MMU_DSCR1_NOEXEC)
#define MMU_ATTR_VECTOR	(ARM_MMU_DSCR1_TEX001|ARMV6_MMU_DSCR1_AP011 \
							|ARM_MMU_DSCR1_CB11)

/*
 *  メモリ領域の先頭番地とサイズ
 */
#define SDRAM_ADDR		0x00100000
#define SDRAM_SIZE		0x0ff00000		/* 255MB */
#define SDRAM_ATTR		MMU_ATTR_RAM

#define SRAM_ADDR		0x48000000
#define SRAM_SIZE		0x04000000		/* 16MB */
#define SRAM_ATTR		MMU_ATTR_RAM

/*
 *  デバイスレジスタ領域の先頭番地とサイズ
 */
#define EB_SYS_ADDR		EB_SYS_BASE
#define EB_SYS_SIZE		0x00100000		/* 1MB */
#define EB_SYS_ATTR		MMU_ATTR_IODEV

#define PMR_ADDR		MPCORE_PMR_BASE
#define PMR_SIZE		0x00100000		/* 1MB */
#define PMR_ATTR		MMU_ATTR_IODEV

/*
 *  ベクタテーブルを置くメモリ領域
 */
#define VECTOR_ADDR		0x01000000
#define VECTOR_SIZE		0x00100000		/* 1MB */
#define VECTOR_ATTR		MMU_ATTR_VECTOR

/*
 *  MMUの設定情報（メモリエリアの情報）
 */
__attribute__((weak))
const ARM_MMU_CONFIG arm_memory_area[] = {
	{ 0x00000000, VECTOR_ADDR, VECTOR_SIZE, VECTOR_ATTR },
	{ SRAM_ADDR, SRAM_ADDR, SRAM_SIZE, SRAM_ATTR },
	{ EB_SYS_ADDR, EB_SYS_ADDR, EB_SYS_SIZE, EB_SYS_ATTR },
	{ PMR_ADDR, PMR_ADDR, PMR_SIZE, PMR_ATTR },
	{ SDRAM_ADDR, SDRAM_ADDR, SDRAM_SIZE, SDRAM_ATTR }
};

/*
 *  MMUの設定情報の数（メモリエリアの数）
 */
__attribute__((weak))
const uint_t arm_tnum_memory_area
					= sizeof(arm_memory_area) / sizeof(ARM_MMU_CONFIG);

/*
 *  システムログの低レベル出力のための初期化
 */
#ifndef TOPPERS_OMIT_TECS

/*
 *  セルタイプtPutLogSIOPort内に実装されている関数を直接呼び出す．
 */
extern void	tPutLogSIOPort_initialize(void);

#else /* TOPPERS_OMIT_TECS */

extern void	sio_initialize(EXINF exinf);
extern void	target_fput_initialize(void);

#endif /* TOPPERS_OMIT_TECS */

/*
 *  ハードウェアの初期化
 */
void
hardware_init_hook(void)
{
	uint32_t	reg;

	/* 
	 *  キャッシュとMMUのディスエーブル処理
	 */
	CP15_READ_SCTLR(reg);
	if (reg & CP15_SCTLR_DCACHE) {
		/* データキャッシュがイネーブルの場合 */
		arm_clean_dcache();
		arm_clean_outer_cache();
		arm_disable_dcache();
	}
	/* MMUのディスエーブル */
	arm_disable_mmu();
	/* L2キャッシュのディスエーブル */
	arm_disable_outer_cache();
	/* 命令キャッシュのディスエーブル */
	arm_disable_icache();
}

/*
 *  ターゲット依存の初期化
 */
void
target_initialize(void)
{
	uint32_t	reg;

	/*
	 *  MPCore依存の初期化
	 */
	mpcore_initialize();
	
	/*
	 *  Emulation Baseboardの割込みモードの設定
	 */
	sil_wrw_mem(EB_SYS_LOCK, EB_SYS_LOCK_UNLOCK);	/* ロック解除 */

	reg = sil_rew_mem(EB_SYS_PLD_CTRL1);
	reg &= ~EB_SYS_PLD_CTRL1_INTMODE_MASK;
	reg |= EB_SYS_PLD_CTRL1_INTMODE_NEW_NODCC;
	sil_wrw_mem(EB_SYS_PLD_CTRL1, reg);

	sil_wrw_mem(EB_SYS_LOCK, EB_SYS_LOCK_LOCK);		/* ロック */
	
	/*
	 *  UARTを初期化
	 */
#ifndef TOPPERS_OMIT_TECS
	tPutLogSIOPort_initialize();
#else /* TOPPERS_OMIT_TECS */
	sio_initialize(0);
	target_fput_initialize();
#endif /* TOPPERS_OMIT_TECS */
}

/*
 *  デフォルトのsoftware_term_hook（weak定義）
 */
__attribute__((weak))
void software_term_hook(void)
{
}

/*
 *  ターゲット依存の終了処理
 */
void
target_exit(void)
{
	/*
	 *  software_term_hookの呼出し
	 */
	software_term_hook();

	/*
	 *  MPCore依存の終了処理
	 */
	mpcore_terminate();

	/*
	 *  ターゲット依存の終了処理
	 */
#if defined(TOPPERS_USE_QEMU) && !defined(TOPPERS_OMIT_QEMU_SEMIHOSTING)
	/*
	 *  QEMUを終了させる．
	 */
	Asm("mov r0, #24\n\t"
		"svc 0x00123456");
#endif
	while (true) ;
}

#ifdef TOPPERS_OMIT_TECS
/*
 *		システムログの低レベル出力（本来は別のファイルにすべき）
 */

#include "target_syssvc.h"
#include "target_serial.h"

/*
 *  低レベル出力用のSIOポート管理ブロック
 */
static SIOPCB	*p_siopcb_target_fput;

/*
 *  SIOポートの初期化
 */
void
target_fput_initialize(void)
{
	p_siopcb_target_fput = uart_pl011_opn_por(SIOPID_FPUT, 0);
}

/*
 *  SIOポートへのポーリング出力
 */
static void
ct11mpcore_uart_fput(char c)
{
	/*
	 *  送信できるまでポーリング
	 */
	while (!(uart_pl011_snd_chr(p_siopcb_target_fput, c))) {
		sil_dly_nse(100);
	}
}

/*
 *  SIOポートへの文字出力
 */
void
target_fput_log(char c)
{
	if (c == '\n') {
		ct11mpcore_uart_fput('\r');
	}
	ct11mpcore_uart_fput(c);
}

#endif /* TOPPERS_OMIT_TECS */
