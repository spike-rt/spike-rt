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
 *  $Id: scif.c 1437 2020-05-20 12:12:16Z ertl-hiro $
 */

/*
 *		FIFO内蔵シリアルコミュニケーションインタフェース用 簡易SIOドライバ
 *		（非TECS版専用）
 */

#include <sil.h>
#include <t_syslog.h>
#include "target_syssvc.h"
#include "rza1.h"
#include "scif.h"

/*
 *  SIOポート初期化ブロックの定義
 */
typedef struct sio_port_initialization_block {
	uintptr_t	base;			/* SCIFレジスタのベースアドレス */
	uint16_t	mode;			/* モードレジスタの設定値 */
	uint32_t	baudrate;		/* ボーレート */
} SIOPINIB;

/*
 *  SIOポート管理ブロックの定義
 */
struct sio_port_control_block {
	const SIOPINIB *p_siopinib;		/* SIOポート初期化ブロック */
	EXINF		exinf;				/* 拡張情報 */
	bool_t		opened;				/* オープン済み */
};

/*
 *  SIOポート初期化ブロック
 */
const SIOPINIB siopinib_table[TNUM_SIOP] = {
	{ SIO_SCIF_BASE, SIO_SCIF_MODE, SIO_SCIF_BAUDRATE }
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
scif_initialize(void)
{
	SIOPCB	*p_siopcb;
	uint_t	i;

	/*
	 *  SIOポート管理ブロックの初期化
	 */
	for (p_siopcb = siopcb_table, i = 0; i < TNUM_SIOP; p_siopcb++, i++) {
		p_siopcb->p_siopinib = &(siopinib_table[i]);
		p_siopcb->opened = false;
	}
}

/*
 *  SIOドライバの終了処理
 */
void
scif_terminate(void)
{
	uint_t	i;

	/*
	 *  オープンされているSIOポートのクローズ
	 */
	for (i = 0; i < TNUM_SIOP; i++) {
		scif_cls_por(&(siopcb_table[i]));
	}
}

/*
 *  SIOポートのオープン
 */
SIOPCB *
scif_opn_por(ID siopid, EXINF exinf)
{
	SIOPCB		*p_siopcb;
	uint_t		brr;
	uintptr_t	base;

	p_siopcb = get_siopcb(siopid);

	if (!(p_siopcb->opened)) {
		/*
		 *  既にオープンしている場合は、二重にオープンしない．
		 */
		brr = SCIF_CLK / (32 * p_siopcb->p_siopinib->baudrate) - 1;
		assert(brr <= 255);

		base = p_siopcb->p_siopinib->base;
		sil_wrh_mem(SCIF_SCSCR(base), 0U);
		sil_wrh_mem(SCIF_SCFCR(base),
							SCIF_SCFCR_TFRST|SCIF_SCFCR_RFRST);
		(void) sil_reh_mem(SCIF_SCFSR(base));
		(void) sil_reh_mem(SCIF_SCLSR(base));
		sil_wrh_mem(SCIF_SCFSR(base), 0U);
		sil_wrh_mem(SCIF_SCLSR(base), 0U);
		sil_wrh_mem(SCIF_SCSCR(base), SCIF_SCSCR_INTCLK);
		sil_wrh_mem(SCIF_SCSMR(base),
				SCIF_SCSMR_ASYNC|p_siopcb->p_siopinib->mode|SCIF_SCSMR_CKS1);
		sil_wrh_mem(SCIF_SCEMR(base), 0U);
		sil_wrb_mem(SCIF_SCBRR(base), (uint8_t) brr);
		sil_wrh_mem(SCIF_SCFCR(base),
					SCIF_SCFCR_RSTRG_15|SCIF_SCFCR_RTRG_1|SCIF_SCFCR_TTRG_8);
		sil_wrh_mem(SCIF_SCSCR(base),
					SCIF_SCSCR_TE|SCIF_SCSCR_RE|SCIF_SCSCR_INTCLK);

		while ((sil_reh_mem(SCIF_SCFSR(base)) & SCIF_SCFSR_RDF) != 0U) {
			(void) sil_reb_mem(SCIF_SCFRDR(base));
			sil_wrh_mem(SCIF_SCFSR(base), (uint16_t) ~(SCIF_SCFSR_RDF));
		}
		sil_wrh_mem(SCIF_SCFSR(base), 0U);

		p_siopcb->opened = true;
	}
	p_siopcb->exinf = exinf;
	return(p_siopcb);   
}

/*
 *  SIOポートのクローズ
 */
void
scif_cls_por(SIOPCB *p_siopcb)
{
	if (p_siopcb->opened) {
		sil_wrh_mem(SCIF_SCSCR(p_siopcb->p_siopinib->base), 0U);

		p_siopcb->opened = false;
	}
}

/*
 *  SIOポートへの文字送信
 */
bool_t
scif_snd_chr(SIOPCB *p_siopcb, char c)
{
	if (scif_putready(p_siopcb->p_siopinib->base)){
		scif_putchar(p_siopcb->p_siopinib->base, c);
		return(true);
	}
	return(false);
}

/*
 *  SIOポートからの文字受信
 */
int_t
scif_rcv_chr(SIOPCB *p_siopcb)
{
	char	c;

	if (scif_getready(p_siopcb->p_siopinib->base)) {
		if (scif_getchar(p_siopcb->p_siopinib->base, &c)) {
			return((int_t) c);
		}
	}
	return(-1);
}

/*
 *  SIOポートからのコールバックの許可
 */
void
scif_ena_cbr(SIOPCB *p_siopcb, uint_t cbrtn)
{
	uint16_t	scr;

	scr = sil_reh_mem(SCIF_SCSCR(p_siopcb->p_siopinib->base));
	switch (cbrtn) {
	case SIO_RDY_SND:
		scr |= SCIF_SCSCR_TIE;
		break;
	case SIO_RDY_RCV:
		scr |= SCIF_SCSCR_RIE;
		break;
	}
	sil_wrh_mem(SCIF_SCSCR(p_siopcb->p_siopinib->base), scr);
}

/*
 *  SIOポートからのコールバックの禁止
 */
void
scif_dis_cbr(SIOPCB *p_siopcb, uint_t cbrtn)
{
	uint16_t	scr;

	scr = sil_reh_mem(SCIF_SCSCR(p_siopcb->p_siopinib->base));
	switch (cbrtn) {
	case SIO_RDY_SND:
		scr &= ~(SCIF_SCSCR_TIE);
		break;
	case SIO_RDY_RCV:
		scr &= ~(SCIF_SCSCR_RIE);
		break;
	}
	sil_wrh_mem(SCIF_SCSCR(p_siopcb->p_siopinib->base), scr);
}

/*
 *  SIOポートに対する受信割込み処理
 */
static void
scif_isr_rx_siop(SIOPCB *p_siopcb)
{
	if (scif_getready(p_siopcb->p_siopinib->base)) {
		/*
		 *  受信通知コールバックルーチンを呼び出す．
		 */
		scif_irdy_rcv(p_siopcb->exinf);
	}
}

/*
 *  SIOポートに対する送信割込み処理
 */
static void
scif_isr_tx_siop(SIOPCB *p_siopcb)
{
	if (scif_putready(p_siopcb->p_siopinib->base)) {
		/*
		 *  送信可能コールバックルーチンを呼び出す．
		 */
		scif_irdy_snd(p_siopcb->exinf);
	}
}

/*
 *  SIOの受信割込みサービスルーチン
 */
void
scif_isr_rx(ID siopid)
{
	scif_isr_rx_siop(get_siopcb(siopid));
}

/*
 *  SIOの送信割込みサービスルーチン
 */
void
scif_isr_tx(ID siopid)
{
	scif_isr_tx_siop(get_siopcb(siopid));
}
