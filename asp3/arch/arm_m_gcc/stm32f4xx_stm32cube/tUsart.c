/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2006-2016 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: tUsart.c 648 2016-02-20 00:50:56Z ertl-honda $
 */

/*
 *		ARM PrimCell UART（PL011）用 簡易SIOドライバ
 */

#include <sil.h>
#include "stm32f4xx_hal.h"
#include "tUsart_tecsgen.h"

/*
 * USARTレジスタ定義
 */
#define USART_SR(x)		(x)
#define USART_DR(x)		(x + 0x04)
#undef USART_BRR
#define USART_BRR(x)	(x + 0x08)
#define USART_CR1(x)	(x + 0x0C)
#define USART_CR2(x)	(x + 0x10)
#define USART_CR3(x)	(x + 0x14)
#define USART_GTPR(x)	(x + 0x18)


/*
 *  プリミティブな送信／受信関数
 */

/*
 *  受信バッファに文字があるか？
 */
Inline bool_t
usart_getready(CELLCB *p_cellcb)
{
	return (sil_rew_mem((void*)USART_SR(ATTR_baseAddress)) & USART_SR_RXNE) != 0;
}

/*
 *  送信バッファに空きがあるか？
 */
Inline bool_t
usart_putready(CELLCB *p_cellcb)
{
	return (sil_rew_mem((void*)USART_SR(ATTR_baseAddress)) & USART_SR_TXE) != 0;
}

/*
 *  受信した文字の取出し
 */
Inline char
usart_getchar(CELLCB *p_cellcb)
{
	return((char) sil_rew_mem((void*)USART_DR(ATTR_baseAddress)) & 0xFF);
}

/*
 *  送信する文字の書込み
 */
Inline void
usart_putchar(CELLCB *p_cellcb, char c)
{
	sil_wrw_mem((void*)USART_DR(ATTR_baseAddress), c);
}

/*
 *  シリアルI/Oポートのオープン
 */
void
eSIOPort_open(CELLIDX idx)
{
	uint32_t tmp, usartdiv, fraction;
	uint32_t src_clock;
	CELLCB	*p_cellcb = GET_CELLCB(idx);

	/* USARTの無効化 */
	sil_andw((void*)USART_CR1(ATTR_baseAddress), ~USART_CR1_UE);

	/* 1STOP BIT */
	sil_wrw_mem((void*)USART_CR2(ATTR_baseAddress), 0);

	/* 1START BIT, 8DATA bits, Parityなし */
	sil_wrw_mem((void*)USART_CR1(ATTR_baseAddress), 0);

	/* CR3初期化 */
	sil_wrw_mem((void*)USART_CR3(ATTR_baseAddress), 0);

	/* 通信速度設定 */
	if (ATTR_baseAddress == USART1_BASE ||
	    ATTR_baseAddress == USART6_BASE ||
	    ATTR_baseAddress == UART9_BASE ||
	    ATTR_baseAddress == UART10_BASE
     ) {
		/* USART1, USART6, UART9, UART10はPCLK2を使用する */
		src_clock = HAL_RCC_GetPCLK2Freq();
	} else {
		src_clock = HAL_RCC_GetPCLK1Freq();
	}

	tmp = (1000 * (src_clock / 100)) / ((ATTR_bps / 100) * 16);
	usartdiv = (tmp / 1000) << 4;
	fraction = tmp - ((usartdiv >> 4) * 1000);
	fraction = ((16 * fraction) + 500) / 1000;
	usartdiv |= (fraction & 0x0F);
	sil_wrw_mem((void*)USART_BRR(ATTR_baseAddress), usartdiv);

	/* 送受信の有効化、エラー割込みの有効化 */
	sil_orw((void*)USART_CR1(ATTR_baseAddress), USART_CR1_RE | USART_CR1_TE);
	sil_orw((void*)USART_CR3(ATTR_baseAddress), USART_CR3_EIE);

	/* USARTの有効化 */
	sil_orw((void*)USART_CR1(ATTR_baseAddress), USART_CR1_UE);
}

/*
 *  シリアルI/Oポートのクローズ
 */
void
eSIOPort_close(CELLIDX idx)
{
	CELLCB	*p_cellcb = GET_CELLCB(idx);

	/*
	 *  USARTをディスエーブル
	 */
	sil_andw((void*)USART_CR1(ATTR_baseAddress), ~USART_CR1_UE);
}

/*
 *  シリアルI/Oポートへの文字送信
 */
bool_t
eSIOPort_putChar(CELLIDX idx, char c)
{
	CELLCB	*p_cellcb = GET_CELLCB(idx);

	if (usart_putready(p_cellcb)){
		usart_putchar(p_cellcb, c);
		return(true);
	}
	return(false);
}

/*
 *  シリアルI/Oポートからの文字受信
 */
int_t
eSIOPort_getChar(CELLIDX idx)
{
	CELLCB	*p_cellcb = GET_CELLCB(idx);

	if (usart_getready(p_cellcb)) {
		return((int_t)(uint8_t) usart_getchar(p_cellcb));
	}
	return(-1);
}

/*
 *  シリアルI/Oポートからのコールバックの許可
 */
void
eSIOPort_enableCBR(CELLIDX idx, uint_t cbrtn)
{
	CELLCB		*p_cellcb = GET_CELLCB(idx);

	switch (cbrtn) {
	case SIOSendReady:
		sil_orw((void*)USART_CR1(ATTR_baseAddress), USART_CR1_TXEIE);
		break;
	case SIOReceiveReady:
		sil_orw((void*)USART_CR1(ATTR_baseAddress), USART_CR1_RXNEIE);
		break;
	}
}

/*
 *  シリアルI/Oポートからのコールバックの禁止
 */
void
eSIOPort_disableCBR(CELLIDX idx, uint_t cbrtn)
{
	CELLCB		*p_cellcb = GET_CELLCB(idx);

	switch (cbrtn) {
	case SIOSendReady:
		sil_andw((void*)USART_CR1(ATTR_baseAddress), ~USART_CR1_TXEIE);
		break;
	case SIOReceiveReady:
		sil_andw((void*)USART_CR1(ATTR_baseAddress), ~USART_CR1_RXNEIE);
		break;
	}
}

/*
 *  シリアルI/Oポートに対する割込み処理
 */
void
eiISR_main(CELLIDX idx)
{
	CELLCB	*p_cellcb = GET_CELLCB(idx);

	if (usart_getready(p_cellcb)) {
		/*
		 *  受信通知コールバックルーチンを呼び出す．
		 */
		ciSIOCBR_readyReceive();
	}
	if (usart_putready(p_cellcb)) {
		/*
		 *  送信可能コールバックルーチンを呼び出す．
		 */
		ciSIOCBR_readySend();
	}
}
