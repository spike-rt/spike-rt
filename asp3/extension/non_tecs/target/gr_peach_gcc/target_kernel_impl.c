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
 *		カーネルのターゲット依存部（GR-PEACH用）
 */

#include "kernel_impl.h"
#include <sil.h>
#include "arm.h"
#include "rza1.h"
#include "scif.h"

/*
 *  カーネル動作時のメモリマップと関連する定義
 *
 *  0x18000000 - 0x1fffffff：シリアルフラッシュメモリ（128MB）
 *  0x20000000 - 0x209fffff：内蔵SRAM（10MB）
 *  0x3fe00000 - 0x3fffffff：I/O領域（2MB），予約領域を含む
 *  0xe8000000 - 0xffffffff：I/O領域（384MB），予約領域を含む
 */

/*
 *  MMUへの設定属性（第1レベルディスクリプタ）
 */
#define MMU_ATTR_RAM	(ARM_MMU_DSCR1_SHARED|ARM_MMU_DSCR1_TEX001 \
							|ARMV6_MMU_DSCR1_AP011|ARM_MMU_DSCR1_CB11)
#define MMU_ATTR_IODEV	(ARM_MMU_DSCR1_SHARED|ARM_MMU_DSCR1_TEX000 \
							|ARMV6_MMU_DSCR1_AP011|ARM_MMU_DSCR1_CB01 \
							|ARMV6_MMU_DSCR1_NOEXEC)

/*
 *  MMUの設定情報（メモリエリアの情報）
 */
__attribute__((weak))
const ARM_MMU_CONFIG arm_memory_area[] = {
	{ SPI_ADDR, SPI_ADDR, SPI_SIZE, MMU_ATTR_RAM },
	{ SRAM_ADDR, SRAM_ADDR, SRAM_SIZE, MMU_ATTR_RAM },
	{ IO1_ADDR, IO1_ADDR, IO1_SIZE, MMU_ATTR_IODEV },
	{ IO2_ADDR, IO2_ADDR, IO2_SIZE, MMU_ATTR_IODEV }
};

/*
 *  MMUの設定情報の数（メモリエリアの数）
 */
__attribute__((weak))
const uint_t arm_tnum_memory_area
					= sizeof(arm_memory_area) / sizeof(ARM_MMU_CONFIG);

/*
 *  低消費電力モードの初期化
 */
static void
lowpower_initialize(void)
{
	/* スタンバイモード時に端子状態を維持する．CoreSight動作 */
	sil_wrb_mem(RZA1_STBCR2, 0x6a);
	(void) sil_reb_mem(RZA1_STBCR2);		/* ダミーリード */

	/* IEBus, irDA, LIN0, LIN1, MTU2, RSCAN2, ASC, PWM動作 */
	sil_wrb_mem(RZA1_STBCR3, 0x00);
	(void) sil_reb_mem(RZA1_STBCR3);		/* ダミーリード */

	/* SCIF0, SCIF1, SCIF2, SCIF3, SCIF4, SCIF5, SCIF6, SCIF7動作 */
	sil_wrb_mem(RZA1_STBCR4, 0x00);
	(void) sil_reb_mem(RZA1_STBCR4);		/* ダミーリード */

	/* SCIM0, SCIM1, SDG0, SDG1, SDG2, SDG3, OSTM0, OSTM1動作 */
	sil_wrb_mem(RZA1_STBCR5, 0x00);
	(void) sil_reb_mem(RZA1_STBCR5);		/* ダミーリード */

	/* A/D, CEU, DISCOM0, DISCOM1, DRC0, DRC1, JCU, RTClock動作 */
	sil_wrb_mem(RZA1_STBCR6, 0x00);
	(void) sil_reb_mem(RZA1_STBCR6);		/* ダミーリード */

	/* DVDEC0, DVDEC1, ETHER, FLCTL, USB0, USB1動作 */
	sil_wrb_mem(RZA1_STBCR7, 0x24);
	(void) sil_reb_mem(RZA1_STBCR7);		/* ダミーリード */

	/* IMR-LS20, IMR-LS21, IMR-LSD, MMCIF, MOST50, SCUX動作 */
	sil_wrb_mem(RZA1_STBCR8, 0x05);
	(void) sil_reb_mem(RZA1_STBCR8);		/* ダミーリード */

	/* I2C0, I2C1, I2C2, I2C3, SPIBSC0, SPIBSC1, VDC50, VDC51動作 */
	sil_wrb_mem(RZA1_STBCR9, 0x00);
	(void) sil_reb_mem(RZA1_STBCR9);		/* ダミーリード */

	/* RSPI0, RSPI1, RSPI2, RSPI3, RSPI4, CD-ROMDEC, RSPDIF, RGPVG動作 */
	sil_wrb_mem(RZA1_STBCR10, 0x00);
	(void) sil_reb_mem(RZA1_STBCR10);		/* ダミーリード */

	/* SSIF0, SSIF1, SSIF2, SSIF3, SSIF4, SSIF5動作 */
	sil_wrb_mem(RZA1_STBCR11, 0xc0);
	(void) sil_reb_mem(RZA1_STBCR11);		/* ダミーリード */

	/* SDHI00, SDHI01, SDHI10, SDHI11動作 */
	sil_wrb_mem(RZA1_STBCR12, 0xf0);
	(void) sil_reb_mem(RZA1_STBCR12);		/* ダミーリード */
}

/*
 *  汎用入出力ポートの初期化（ポート／ペリフェラル兼用ピンのアサインの設定）
 */
static void
port_initialize(void)
{
	/* ポート6:ビット3（TxD2）の設定 */
	rza1_config_port(RZA1_PORT_PIBC(6),  3, 0);
	rza1_config_port(RZA1_PORT_PBDC(6),  3, 0);
	rza1_config_port(RZA1_PORT_PIPC(6),  3, 1);
	/* 第7兼用機能（TxD2），出力 */
	rza1_config_port(RZA1_PORT_PMC(6),   3, 1);
	rza1_config_port(RZA1_PORT_PFCAE(6), 3, 1);
	rza1_config_port(RZA1_PORT_PFCE(6),  3, 1);
	rza1_config_port(RZA1_PORT_PFC(6),   3, 0);
	rza1_config_port(RZA1_PORT_PM(6),    3, 0);

	/* ポート6:ビット2（RxD2）の設定 */
	rza1_config_port(RZA1_PORT_PIBC(6),  2, 0);
	rza1_config_port(RZA1_PORT_PBDC(6),  2, 0);
	rza1_config_port(RZA1_PORT_PIPC(6),  2, 1);
	/* 第7兼用機能（RxD2），入力 */
	rza1_config_port(RZA1_PORT_PMC(6),   2, 1);
	rza1_config_port(RZA1_PORT_PFCAE(6), 2, 1);
	rza1_config_port(RZA1_PORT_PFCE(6),  2, 1);
	rza1_config_port(RZA1_PORT_PFC(6),   2, 0);
	rza1_config_port(RZA1_PORT_PM(6),    2, 1);

	/* ポート6:ビット13（LED1／赤）の設定 */
	rza1_config_port(RZA1_PORT_PIBC(6),  13, 0);
	rza1_config_port(RZA1_PORT_PBDC(6),  13, 0);
	/* ポートモード，出力 */
	rza1_config_port(RZA1_PORT_PMC(6),   13, 0);
	rza1_config_port(RZA1_PORT_PM(6),    13, 0);

	/* ポート6:ビット14（LED2／緑）の設定 */
	rza1_config_port(RZA1_PORT_PIBC(6),  14, 0);
	rza1_config_port(RZA1_PORT_PBDC(6),  14, 0);
	/* ポートモード，出力 */
	rza1_config_port(RZA1_PORT_PMC(6),   14, 0);
	rza1_config_port(RZA1_PORT_PM(6),    14, 0);

	/* ポート6:ビット15（LED3／青）の設定 */
	rza1_config_port(RZA1_PORT_PIBC(6),  15, 0);
	rza1_config_port(RZA1_PORT_PBDC(6),  15, 0);
	/* ポートモード，出力 */
	rza1_config_port(RZA1_PORT_PMC(6),   15, 0);
	rza1_config_port(RZA1_PORT_PM(6),    15, 0);

	/* ポート6:ビット12（LED4／ユーザ）の設定 */
	rza1_config_port(RZA1_PORT_PIBC(6),  12, 0);
	rza1_config_port(RZA1_PORT_PBDC(6),  12, 0);
	/* ポートモード，出力 */
	rza1_config_port(RZA1_PORT_PMC(6),   12, 0);
	rza1_config_port(RZA1_PORT_PM(6),    12, 0);

	/* ポート6:ビット0（ユーザボタン）の設定 */
	rza1_config_port(RZA1_PORT_PIBC(6),  0, 1);
	rza1_config_port(RZA1_PORT_PBDC(6),  0, 0);
	/* 第6兼用機能（IRQ5），入力 */
	rza1_config_port(RZA1_PORT_PMC(6),   0, 1);
	rza1_config_port(RZA1_PORT_PFCAE(6), 0, 1);
	rza1_config_port(RZA1_PORT_PFCE(6),  0, 0);
	rza1_config_port(RZA1_PORT_PFC(6),   0, 1);
	rza1_config_port(RZA1_PORT_PM(6),    0, 1);
}

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
 *  ターゲット依存の初期化
 */
void
target_initialize(void)
{
	extern void	*vector_table;		/* ベクタテーブル */

	/*
	 *  Low exception vectorsを使用
	 */
	arm_set_low_vectors();

	/*
	 *  チップ依存の初期化
	 */
	chip_initialize();

	/*
	 *  低消費電力モードの初期化
	 */
	lowpower_initialize();

	/*
	 *  汎用入出力ポートの初期化（ポート／ペリフェラル兼用ピンのアサインの設定）
	 */
	port_initialize();

	/*
	 *  ベクタテーブルの設定
	 */
	CP15_WRITE_VBAR((uint32_t) &vector_table);

	/*
	 *  LEDを青色に点灯させる
	 */
	gr_peach_set_led(GR_PEACH_LED_BLUE, 1);

	/*
	 *  SIOを初期化
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
	 *  チップ依存の終了処理
	 */
	chip_terminate();

	/*
	 *  bkpt命令によりデバッガに制御を移す（パラメータが何が良いか未検討）
	 */
	Asm("bkpt #0");

	while (true) ;
}

/*
 *  LEDの点灯／消灯
 */
void
gr_peach_set_led(uint_t led, uint_t set)
{
	rza1_config_port(RZA1_PORT_P(6), led, set);
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
	p_siopcb_target_fput = sio_opn_por(SIOPID_FPUT, 0);
}

/*
 *  SIOポートへのポーリング出力
 */
Inline void
gr_peach_uart_fput(char c)
{
	/*
	 *  送信できるまでポーリング
	 */
	while (!(sio_snd_chr(p_siopcb_target_fput, c))) {
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
		gr_peach_uart_fput('\r');
	}
	gr_peach_uart_fput(c);
}

#endif /* TOPPERS_OMIT_TECS */
