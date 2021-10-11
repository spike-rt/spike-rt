/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 *
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2006-2018 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: xuartps.c 1133 2018-12-29 02:08:25Z ertl-hiro $
 */

/*
 *		XUartPs用 簡易SIOドライバ（非TECS版専用）
 */

#include <kernel.h>
#include <t_syslog.h>
#include "target_syssvc.h"
#include "zynq7000.h"
#include "xuartps.h"

/*
 *  SIOポート初期化ブロックの定義
 */
typedef struct sio_port_initialization_block {
	uintptr_t	base;			/* UARTレジスタのベースアドレス */
	uint16_t	mode;			/* モードレジスタの設定値 */
	uint16_t	baudgen;		/* ボーレート生成レジスタの設定値 */
	uint8_t		bauddiv;		/* ボーレート分割レジスタの設定値 */
} SIOPINIB;

/*
 *  SIOポート管理ブロックの定義
 */
struct sio_port_control_block {
	const SIOPINIB *p_siopinib;		/* SIOポート初期化ブロック */
	intptr_t	exinf;				/* 拡張情報 */
	bool_t		opened;				/* オープン済み */
};

/*
 *  SIOポート初期化ブロック
 */
const SIOPINIB siopinib_table[TNUM_SIOP] = {
	{ SIO_XUARTPS_BASE, SIO_XUARTPS_MODE,
								SIO_XUARTPS_BAUDGEN, SIO_XUARTPS_BAUDDIV }
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
xuartps_initialize(void)
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
xuartps_terminate(void)
{
	uint_t	i;
	SIOPCB	*p_siopcb;

	for (i = 0; i < TNUM_SIOP; i++) {
		p_siopcb = &(siopcb_table[i]);
		if (p_siopcb->opened) {
			/*
			 *  送信FIFOが空になるまで待つ
			 */
			while ((sil_rew_mem(XUARTPS_SR(p_siopcb->p_siopinib->base))
											& XUARTPS_SR_TXEMPTY) == 0U);

			/*
			 *  オープンされているSIOポートのクローズ
			 */
			xuartps_cls_por(&(siopcb_table[i]));
		}
	}
}

/*
 *  SIOポートのオープン
 */
SIOPCB *
xuartps_opn_por(ID siopid, intptr_t exinf)
{
	SIOPCB		*p_siopcb;
	uintptr_t	base;

	p_siopcb = get_siopcb(siopid);

	if (!(p_siopcb->opened)) {
		/*
		 *  既にオープンしている場合は、二重にオープンしない．
		 */
		base = p_siopcb->p_siopinib->base;

		/*
		 *  全割込みをディスエーブル
		 */
		sil_wrw_mem(XUARTPS_IDR(base), XUARTPS_IXR_ALL);
		
		/*
		 *  ペンディングしている割込みをクリア
		 */
		sil_wrw_mem(XUARTPS_ISR(base), sil_rew_mem(XUARTPS_ISR(base)));

		/*
		 *  送受信のリセットとディスエーブル
		 */
		sil_wrw_mem(XUARTPS_CR(base),
							XUARTPS_CR_TXRST | XUARTPS_CR_RXRST
								| XUARTPS_CR_TX_DIS | XUARTPS_CR_RX_DIS);

		/*
		 *  ボーレートの設定
		 */
		sil_wrw_mem(XUARTPS_BAUDGEN(base), p_siopcb->p_siopinib->baudgen);
		sil_wrw_mem(XUARTPS_BAUDDIV(base), p_siopcb->p_siopinib->bauddiv);

		/*
		 *  データ長，ストップビット，パリティの設定
		 */
		sil_wrw_mem(XUARTPS_MR(base), p_siopcb->p_siopinib->mode);

		/*
		 *  受信トリガを1バイトに設定
		 */
		sil_wrw_mem(XUARTPS_RXWM(base), 1U);

		/*
		 *  タイムアウトを設定
		 */
		sil_wrw_mem(XUARTPS_RXTOUT(base), 10U);

		/*
		 *  送受信のイネーブル
		 */
		sil_wrw_mem(XUARTPS_CR(base),
					XUARTPS_CR_TX_EN | XUARTPS_CR_RX_EN | XUARTPS_CR_STOPBRK);

		p_siopcb->opened = true;
	}
	p_siopcb->exinf = exinf;
	return(p_siopcb);   
}

/*
 *  SIOポートのクローズ
 */
void
xuartps_cls_por(SIOPCB *p_siopcb)
{
	if (p_siopcb->opened) {
		/*
		 *  送受信のディスエーブル
		 */
		sil_wrw_mem(XUARTPS_CR(p_siopcb->p_siopinib->base),
				XUARTPS_CR_TX_DIS | XUARTPS_CR_RX_DIS | XUARTPS_CR_STOPBRK);

		/*
		 *  全割込みをディスエーブル
		 */
		sil_wrw_mem(XUARTPS_IDR(p_siopcb->p_siopinib->base), XUARTPS_IXR_ALL);

		p_siopcb->opened = false;
	}
}

/*
 *  SIOポートへの文字送信
 */
bool_t
xuartps_snd_chr(SIOPCB *p_siopcb, char c)
{
	if (xuartps_putready(p_siopcb->p_siopinib->base)){
		xuartps_putchar(p_siopcb->p_siopinib->base, c);
		return(true);
	}
	return(false);
}

/*
 *  SIOポートからの文字受信
 */
int_t
xuartps_rcv_chr(SIOPCB *p_siopcb)
{
	if (xuartps_getready(p_siopcb->p_siopinib->base)) {
		return((int_t) xuartps_getchar(p_siopcb->p_siopinib->base));
	}
	return(-1);
}

/*
 *  SIOポートからのコールバックの許可
 */
void
xuartps_ena_cbr(SIOPCB *p_siopcb, uint_t cbrtn)
{
	switch (cbrtn) {
	case SIO_RDY_SND:
		xuartps_enable_send(p_siopcb->p_siopinib->base);
		break;
	case SIO_RDY_RCV:
		xuartps_enable_receive(p_siopcb->p_siopinib->base);
		break;
	}
}

/*
 *  SIOポートからのコールバックの禁止
 */
void
xuartps_dis_cbr(SIOPCB *p_siopcb, uint_t cbrtn)
{
	switch (cbrtn) {
	case SIO_RDY_SND:
		xuartps_disable_send(p_siopcb->p_siopinib->base);
		break;
	case SIO_RDY_RCV:
		xuartps_disable_receive(p_siopcb->p_siopinib->base);
		break;
	}
}

/*
 *  SIOポートに対する割込み処理
 */
static void
xuartps_isr_siop(SIOPCB *p_siopcb)
{
	if (xuartps_getready(p_siopcb->p_siopinib->base)) {
		/*
		 *  受信通知コールバックルーチンを呼び出す．
		 */
		xuartps_irdy_rcv(p_siopcb->exinf);
	}
	if (xuartps_putready(p_siopcb->p_siopinib->base)) {
		/*
		 *  送信可能コールバックルーチンを呼び出す．
		 */
		xuartps_irdy_snd(p_siopcb->exinf);
	}

	/*
	 *  ペンディングしている割込みをクリア
	 */
	sil_wrw_mem(XUARTPS_ISR(p_siopcb->p_siopinib->base), 
					sil_rew_mem(XUARTPS_ISR(p_siopcb->p_siopinib->base)));
}

/*
 *  SIOの割込みサービスルーチン
 */
void
xuartps_isr(ID siopid)
{
	xuartps_isr_siop(get_siopcb(siopid));
}
