/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2013-2018 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: tSIOPortDummyMain.c 1095 2018-11-28 00:57:28Z ertl-hiro $
 */

/*
 *		SIOドライバ（ダミーターゲット用）
 */

#include "tSIOPortDummyMain_tecsgen.h"
 
/*
 *  SIOポートのオープン
 */
void
eSIOPort_open(CELLIDX idx)
{
	CELLCB	*p_cellcb = GET_CELLCB(idx);

	if (!VAR_opened) {
		/*
		 *  既にオープンしている場合は、二重にオープンしない．
		 */

		/* SIOのオープン処理 */

		VAR_opened = true;
	}
}

/*
 *  SIOポートのクローズ
 */
void
eSIOPort_close(CELLIDX idx)
{
	CELLCB	*p_cellcb = GET_CELLCB(idx);

	if (VAR_opened) {
		/* SIOのクローズ処理 */

		VAR_opened = false;
	}
}

/*
 *  SIOポートへの文字送信
 */
bool_t
eSIOPort_putChar(CELLIDX idx, char c)
{
	CELLCB	*p_cellcb = GET_CELLCB(idx);

	if (1/* 送信レジスタに空きがある場合 */) {
		/* 送信レジスタに文字cを入れる */
		return(true);
	}
	else {
		return(false);
	}
}

/*
 *  SIOポートからの文字受信
 */
int_t
eSIOPort_getChar(CELLIDX idx)
{
	CELLCB	*p_cellcb = GET_CELLCB(idx);
	char	c;

	if (1/* 受信レジスタに文字がある場合 */) {
		/* 受信レジスタの文字をcに入れる */
		c = '\0';
		return((int_t) c);
	}
	else {
		return(-1);
	}
}

/*
 *  SIOポートからのコールバックの許可
 */
void
eSIOPort_enableCBR(CELLIDX idx, uint_t cbrtn)
{
	CELLCB	*p_cellcb = GET_CELLCB(idx);

	switch (cbrtn) {
	case SIOSendReady:
		/* 送信完了割込みを許可 */
		break;
	case SIOReceiveReady:
		/* 受信完了割込みを許可 */
		break;
	}
}

/*
 *  SIOポートからのコールバックの禁止
 */
void
eSIOPort_disableCBR(CELLIDX idx, uint_t cbrtn)
{
	CELLCB	*p_cellcb = GET_CELLCB(idx);

	switch (cbrtn) {
	case SIOSendReady:
		/* 送信完了割込みを禁止 */
		break;
	case SIOReceiveReady:
		/* 受信完了割込みを禁止 */
		break;
	}
}

/*
 *  SIOの割込みサービスルーチン
 */
void
eiISR_main(CELLIDX idx)
{
	CELLCB	*p_cellcb = GET_CELLCB(idx);

	if (1/* 送信完了割込みの場合 */) {
		if (is_ciSIOCBR_joined()) {
			ciSIOCBR_readySend();
		}
	}
	if (1/* 受信完了割込みの場合 */) {
		if (is_ciSIOCBR_joined()) {
			ciSIOCBR_readyReceive();
		}
	}
}

/*
 *  SIOドライバの終了処理
 */
void
eTerminate_main(CELLIDX idx)
{
	eSIOPort_close(idx);
}
