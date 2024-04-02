/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2006-2020 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: uart_pl011.h 359 2015-07-26 10:27:20Z ertl-hiro $
 */

/*
 *		ARM PrimCell UART（PL011）に関する定義と簡易SIOドライバ
 */

#ifndef TOPPERS_UART_PL011_H
#define TOPPERS_UART_PL011_H

/*
 *		ARM PrimCell UART（PL011）に関する定義
 */

/*
 *  UARTレジスタの番地の定義
 */
#define UART_DR(base)		((uint32_t *)((base) + 0x00U))
#define UART_RSR(base)		((uint32_t *)((base) + 0x04U))
#define UART_ECR(base)		((uint32_t *)((base) + 0x04U))
#define UART_FR(base)		((uint32_t *)((base) + 0x18U))
#define UART_IBRD(base)		((uint32_t *)((base) + 0x24U))
#define UART_FBRD(base)		((uint32_t *)((base) + 0x28U))
#define UART_LCR_H(base)	((uint32_t *)((base) + 0x2cU))
#define UART_CR(base)		((uint32_t *)((base) + 0x30U))
#define UART_IFLS(base)		((uint32_t *)((base) + 0x34U))
#define UART_IMSC(base)		((uint32_t *)((base) + 0x38U))
#define UART_RIS(base)		((uint32_t *)((base) + 0x3cU))
#define UART_MIS(base)		((uint32_t *)((base) + 0x40U))
#define UART_ICR(base)		((uint32_t *)((base) + 0x44U))

/*
 *  フラグレジスタ（UART_FR）の参照値
 */
#define UART_FR_RXFE		UINT_C(0x10)	/* 受信バッファが空 */
#define UART_FR_TXFF		UINT_C(0x20)	/* 送信バッファがフル */

/*
 *  ライン制御レジスタ（UART_LCR_H）の設定値
 */
#define UART_LCR_H_PEN		UINT_C(0x02)	/* パリティを用いる */
#define UART_LCR_H_EPS		UINT_C(0x04)	/* 偶数パリティに */
#define UART_LCR_H_STP2		UINT_C(0x08)	/* ストップビットを2ビットに */
#define UART_LCR_H_FEN		UINT_C(0x10)	/* FIFOを有効に */
#define UART_LCR_H_WLEN8	UINT_C(0x60)	/* データ長を8ビットに */

/*
 *  制御レジスタ（UART_CR）の設定値
 */
#define UART_CR_UARTEN	UINT_C(0x0001)		/* UARTをイネーブルに */
#define UART_CR_TXE		UINT_C(0x0100)		/* 送信をイネーブルに */
#define UART_CR_RXE		UINT_C(0x0200)		/* 受信をイネーブルに */

/*
 *  割込みマスクセット／クリアレジスタ（UART_IMSC）の設定値
 */
#define UART_IMSC_RXIM	UINT_C(0x0010)		/* 受信割込みマスク */
#define UART_IMSC_TXIM	UINT_C(0x0020)		/* 送信割込みマスク */

#ifdef TOPPERS_OMIT_TECS
/*
 *		ARM PrimCell UART（PL011）用 簡易SIOドライバ
 */
#include <sil.h>

/*
 *  SIOポート数の定義
 */
#define TNUM_SIOP		1		/* サポートするSIOポートの数 */

/*
 *  コールバックルーチンの識別番号
 */
#define SIO_RDY_SND		1U		/* 送信可能コールバック */
#define SIO_RDY_RCV		2U		/* 受信通知コールバック */

#ifndef TOPPERS_MACRO_ONLY

/*
 *  SIOポート管理ブロックの定義
 */
typedef struct sio_port_control_block	SIOPCB;

/*
 *  プリミティブな送信／受信関数
 */

/*
 *  受信バッファに文字があるか？
 */
Inline bool_t
uart_pl011_getready(uintptr_t base)
{
	return((sil_rew_mem(UART_FR(base)) & UART_FR_RXFE) == 0U);
}

/*
 *  送信バッファに空きがあるか？
 */
Inline bool_t
uart_pl011_putready(uintptr_t base)
{
	return((sil_rew_mem(UART_FR(base)) & UART_FR_TXFF) == 0U);
}

/*
 *  受信した文字の取出し
 */
Inline char
uart_pl011_getchar(uintptr_t base)
{
	return((char) sil_rew_mem(UART_DR(base)));
}

/*
 *  送信する文字の書込み
 */
Inline void
uart_pl011_putchar(uintptr_t base, char c)
{
	sil_wrw_mem(UART_DR(base), (uint32_t) c);
}

/*
 *  シリアルインタフェースドライバに提供する機能
 */

/*
 *  SIOドライバの初期化
 */
extern void		uart_pl011_initialize(void);

/*
 *  SIOドライバの終了処理
 */
extern void		uart_pl011_terminate(void);

/*
 *  SIOの割込みサービスルーチン
 */
extern void		uart_pl011_isr(ID siopid);

/*
 *  SIOポートのオープン
 */
extern SIOPCB	*uart_pl011_opn_por(ID siopid, EXINF exinf);

/*
 *  SIOポートのクローズ
 */
extern void		uart_pl011_cls_por(SIOPCB *siopcb);

/*
 *  SIOポートへの文字送信
 */
extern bool_t	uart_pl011_snd_chr(SIOPCB *siopcb, char c);

/*
 *  SIOポートからの文字受信
 */
extern int_t	uart_pl011_rcv_chr(SIOPCB *siopcb);

/*
 *  SIOポートからのコールバックの許可
 */
extern void		uart_pl011_ena_cbr(SIOPCB *siopcb, uint_t cbrtn);

/*
 *  SIOポートからのコールバックの禁止
 */
extern void		uart_pl011_dis_cbr(SIOPCB *siopcb, uint_t cbrtn);

/*
 *  SIOポートからの送信可能コールバック
 */
extern void		uart_pl011_irdy_snd(EXINF exinf);

/*
 *  SIOポートからの受信通知コールバック
 */
extern void		uart_pl011_irdy_rcv(EXINF exinf);

#endif /* TOPPERS_MACRO_ONLY */
#endif /* TOPPERS_OMIT_TECS */
#endif /* TOPPERS_UART_PL011_H */
