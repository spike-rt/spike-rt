/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
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
 *  $Id: uart_pl011.c 362 2015-07-26 11:29:15Z ertl-hiro $
 */

/*
 *		ARM PrimCell UART（PL011）用 簡易SIOドライバ（非TECS版専用）
 */

#include <sil.h>
#include "target_syssvc.h"
#include "uart_pl011.h"

/*
 *  SIOポート初期化ブロックの定義
 */
typedef struct sio_port_initialization_block {
	uintptr_t	base;			/* UARTレジスタのベースアドレス */
	uint8_t		ibrd;			/* ボーレート（整数部）レジスタの設定値 */
	uint8_t		fbrd;			/* ボーレート（小数部）レジスタの設定値 */
	uint8_t		lcr_h;			/* ライン制御レジスタの設定値 */
} SIOPINIB;

/*
 *  SIOポート管理ブロックの定義
 */
struct sio_port_control_block {
	const SIOPINIB *siopinib;	/* SIOポート初期化ブロック */
	EXINF		exinf;			/* 拡張情報 */
	bool_t		opened;			/* オープン済み */
};

/*
 *  SIOポート初期化ブロック
 */
const SIOPINIB siopinib_table[TNUM_SIOP] = {
	{ SIO_UART_BASE, SIO_UART_IBRD, SIO_UART_FBRD, SIO_UART_LCR_H }
};

/*
 *  SIOポート管理ブロックのエリア
 */
SIOPCB	siopcb_table[TNUM_SIOP];

/*
 *  SIOポートIDから管理ブロックを取り出すためのマクロ
 */
#define INDEX_SIOP(siopid)	((uint_t)((siopid) - 1))
#define get_siopcb(siopid)	(&(siopcb_table[INDEX_SIOP(siopid)]))

/*
 *  SIOドライバの初期化
 */
void
uart_pl011_initialize(void)
{
	SIOPCB	*p_siopcb;
	uint_t	i;

	/*
	 *  SIOポート管理ブロックの初期化
	 */
	for (p_siopcb = siopcb_table, i = 0; i < TNUM_SIOP; p_siopcb++, i++) {
		p_siopcb->siopinib = &(siopinib_table[i]);
		p_siopcb->opened = false;
	}
}

/*
 *  SIOドライバの終了処理
 */
void
uart_pl011_terminate(void)
{
	uint_t	i;

	/*
	 *  オープンされているSIOポートのクローズ
	 */
	for (i = 0; i < TNUM_SIOP; i++) {
		uart_pl011_cls_por(&(siopcb_table[i]));
	}
}

/*
 *  SIOポートのオープン
 */
SIOPCB *
uart_pl011_opn_por(ID siopid, EXINF exinf)
{
	SIOPCB	*p_siopcb;

	p_siopcb = get_siopcb(siopid);

	if (!(p_siopcb->opened)) {
		/*
		 *  既にオープンしている場合は、二重にオープンしない．
		 */

		/*
		 *  UARTをディスエーブル
		 */
		sil_wrw_mem(UART_CR(p_siopcb->siopinib->base), 0U);

		/*
		 *  エラーフラグをクリア
		 */
	    sil_wrw_mem(UART_ECR(p_siopcb->siopinib->base), 0U);

		/*
		 *  FIFOを空にする
		 */
		while (uart_pl011_getready(p_siopcb->siopinib->base)) {
			(void) uart_pl011_getchar(p_siopcb->siopinib->base);
		}

		/*
		 *  ボーレートと通信規格を設定
		 */
		sil_wrw_mem(UART_IBRD(p_siopcb->siopinib->base),
									p_siopcb->siopinib->ibrd);
		sil_wrw_mem(UART_FBRD(p_siopcb->siopinib->base),
									p_siopcb->siopinib->fbrd);
		sil_wrw_mem(UART_LCR_H(p_siopcb->siopinib->base),
									p_siopcb->siopinib->lcr_h);
		
		/*
		 *  UARTをイネーブル
		 */
		sil_wrw_mem(UART_CR(p_siopcb->siopinib->base),
						UART_CR_UARTEN|UART_CR_TXE|UART_CR_RXE);

		p_siopcb->opened = true;
	}
	p_siopcb->exinf = exinf;
	return(p_siopcb);   
}

/*
 *  SIOポートのクローズ
 */
void
uart_pl011_cls_por(SIOPCB *p_siopcb)
{
	if (p_siopcb->opened) {
		/*
		 *  UARTをディスエーブル
		 */
		sil_wrw_mem(UART_CR(p_siopcb->siopinib->base), 0U);

		p_siopcb->opened = false;
	}
}

/*
 *  SIOポートへの文字送信
 */
bool_t
uart_pl011_snd_chr(SIOPCB *p_siopcb, char c)
{
	if (uart_pl011_putready(p_siopcb->siopinib->base)){
		uart_pl011_putchar(p_siopcb->siopinib->base, c);
		return(true);
	}
	return(false);
}

/*
 *  SIOポートからの文字受信
 */
int_t
uart_pl011_rcv_chr(SIOPCB *p_siopcb)
{
	if (uart_pl011_getready(p_siopcb->siopinib->base)) {
		return((int_t)(uint8_t) uart_pl011_getchar(p_siopcb->siopinib->base));
	}
	return(-1);
}

/*
 *  SIOポートからのコールバックの許可
 */
void
uart_pl011_ena_cbr(SIOPCB *p_siopcb, uint_t cbrtn)
{
	uint32_t	reg;

	reg = sil_rew_mem(UART_IMSC(p_siopcb->siopinib->base));
	switch (cbrtn) {
	case SIO_RDY_SND:
		reg |= UART_IMSC_TXIM;
		break;
	case SIO_RDY_RCV:
		reg |= UART_IMSC_RXIM;
		break;
	}
	sil_wrw_mem(UART_IMSC(p_siopcb->siopinib->base), reg);
}

/*
 *  SIOポートからのコールバックの禁止
 */
void
uart_pl011_dis_cbr(SIOPCB *p_siopcb, uint_t cbrtn)
{
	uint32_t	reg;

	reg = sil_rew_mem(UART_IMSC(p_siopcb->siopinib->base));
	switch (cbrtn) {
	case SIO_RDY_SND:
		reg &= ~UART_IMSC_TXIM;
		break;
	case SIO_RDY_RCV:
		reg &= ~UART_IMSC_RXIM;
		break;
	}
	sil_wrw_mem(UART_IMSC(p_siopcb->siopinib->base), reg);
}

/*
 *  SIOポートに対する割込み処理
 */
static void
uart_pl011_isr_siop(SIOPCB *p_siopcb)
{
	if (uart_pl011_getready(p_siopcb->siopinib->base)) {
		/*
		 *  受信通知コールバックルーチンを呼び出す．
		 */
		uart_pl011_irdy_rcv(p_siopcb->exinf);
	}
	if (uart_pl011_putready(p_siopcb->siopinib->base)) {
		/*
		 *  送信可能コールバックルーチンを呼び出す．
		 */
		uart_pl011_irdy_snd(p_siopcb->exinf);
	}
}

/*
 *  SIOの割込みサービスルーチン
 */
void
uart_pl011_isr(ID siopid)
{
	uart_pl011_isr_siop(get_siopcb(siopid));
}
