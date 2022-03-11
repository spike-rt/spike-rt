/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
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
 *  $Id: chip_serial.c 1365 2020-01-20 01:52:47Z ertl-komori $
 */

/*
 * シリアルインタフェースドライバのチップ依存部（LPC5500用）
 * （非TECS版専用）
 */

#include <kernel.h>
#include <sil.h>
#include <t_syslog.h>
#include "target_syssvc.h"
#include "target_serial.h"

struct sio_port_control_block {
    intptr_t exinf;
    uint32_t base;
};

static SIOPCB siopcb[TNUM_PORT];
static const uint32_t bases[TNUM_PORT] = {
    LPC5500_USART0_BASE, LPC5500_USART1_BASE, LPC5500_USART2_BASE, LPC5500_USART3_BASE
};
static const uint32_t flexcomms[TNUM_PORT] = {
    LPC5500_FLEXCOMM0_BASE, LPC5500_FLEXCOMM1_BASE, LPC5500_FLEXCOMM2_BASE, LPC5500_FLEXCOMM3_BASE
};

/*
 * SIOドライバの初期化
 */
void sio_initialize(intptr_t exinf)
{
    /* 100 MHz / (2 * (1 + 240/256)) ~ 25.8 MHz */
    sil_wrw_mem(LPC5500_SYSCON_PLL0CLKDIV, 2 - 1);
}

/*
 * SIOドライバの終了処理
 */
void sio_terminate(intptr_t exinf)
{
}

/*
 * SIOの割込みサービスルーチン
 */
void sio_isr(intptr_t exinf)
{
    const uint32_t stat = sil_rew_mem(LPC5500_USART_FIFOINTSTAT(siopcb[exinf].base));
    if (stat & LPC5500_USART_FIFOINTSTAT_TXLVL) {
        /*
         * 送信可能コールバックルーチンを呼び出す．
         */
        sio_irdy_snd(siopcb[exinf].exinf);
    }
    if (stat & LPC5500_USART_FIFOINTSTAT_RXLVL) {
        /*
         * 受信通知コールバックルーチンを呼び出す．
         */
        sio_irdy_rcv(siopcb[exinf].exinf);
    }
}

/*
 * SIOポートのオープン
 */
SIOPCB *sio_opn_por(ID siopid, intptr_t exinf)
{
	const uint32_t index = siopid - 1;
    siopcb[index].exinf = exinf;
    const uint32_t base = bases[index];
    if (siopcb[index].base) {
        /* It's already opened */
        return &siopcb[index];
    }
    siopcb[index].base = base;

    sil_wrw_mem(LPC5500_SYSCON_FLEXFRG0CTRL, LPC5500_SYSCON_FLEXFRGCTRL_DIV(0xFF) | LPC5500_SYSCON_FLEXFRGCTRL_MULT(240));
    sil_wrw_mem(LPC5500_SYSCON_FCCLKSEL(index), LPC5500_SYSCON_FCCLKSEL_SEL_PLL0DIV); /* Select PLL0_div as Flexconn clock */
    sil_wrw_mem(LPC5500_SYSCON_AHBCLKCTRLSET1, LPC5500_SYSCON_AHBCLKCTRL1_FC(index)); /* Enable Flexconn clock */
    sil_wrw_mem(LPC5500_FLEXCOMM_PSELID(flexcomms[index]), LPC5500_FLEXCOMM_PSELID_PERSEL_USART); /* Select USART */

    /* 25.8 MHz / (16 * 14) ~ 115207 bps */
    /* More accurate but slowing peripheral option below */
    /* 100 MHz / (16 * 43 * (1 + 67/256)) ~ 115199 bps */
    sil_wrw_mem(LPC5500_USART_BRG(base), 14 - 1);
    sil_wrw_mem(LPC5500_USART_FIFOCFG(base), LPC5500_USART_FIFOCFG_ENABLETX | LPC5500_USART_FIFOCFG_ENABLERX); /* Enable FIFO */
    sil_wrw_mem(LPC5500_USART_FIFOTRIG(base),
                LPC5500_USART_FIFOTRIG_RXLVLENA | LPC5500_USART_FIFOTRIG_TXLVLENA | LPC5500_USART_FIFOTRIG_TXLVL(15));
            /*  Let RX generate FIFO interrupt  | Let TX generate FIFO interrupt  | TX FIFO generates interrupt if there is a space */
    sil_wrw_mem(LPC5500_USART_CFG(base), LPC5500_USART_CFG_DATALEN_8BIT | LPC5500_USART_CFG_ENABLE);
                                      /*        8bit data length        |      Enable usart       */

    return &siopcb[index];
}

/*
 * SIOポートのクローズ
 */
void sio_cls_por(SIOPCB *p_siopcb)
{
    sil_wrw_mem(LPC5500_USART_CFG(p_siopcb->base), 0);
}

/*
 * SIOポートへの文字送信
 */
bool_t sio_snd_chr(SIOPCB *p_siopcb, char c)
{
	if (sil_rew_mem(LPC5500_USART_FIFOSTAT(p_siopcb->base)) & LPC5500_USART_FIFOSTAT_TXNOTFULL) {
        sil_wrw_mem(LPC5500_USART_FIFOWR(p_siopcb->base), c);
        return true;
    }
    return false;
}

/*
 * SIOポートからの文字受信
 */
int_t sio_rcv_chr(SIOPCB *p_siopcb)
{
    if (sil_rew_mem(LPC5500_USART_FIFOSTAT(p_siopcb->base)) & LPC5500_USART_FIFOSTAT_RXNOTEMPTY) {
        return sil_rew_mem(LPC5500_USART_FIFORD(p_siopcb->base));
    }
    return -1;
}

/*
 * SIOポートからのコールバックの許可
 */
void sio_ena_cbr(SIOPCB *p_siopcb, uint_t cbrtn)
{
    switch (cbrtn) {
        case SIO_RDY_SND:
            /* Enable TX FIFO interrupt */
            sil_wrw_mem(LPC5500_USART_FIFOINTENSET(p_siopcb->base), LPC5500_USART_FIFOINTEN_TXLVL);
            break;
        case SIO_RDY_RCV:
            /* Enable RX FIFO interrupt */
            sil_wrw_mem(LPC5500_USART_FIFOINTENSET(p_siopcb->base), LPC5500_USART_FIFOINTEN_RXLVL);
            break;
    }
}

/*
 * SIOポートからのコールバックの禁止
 */
void sio_dis_cbr(SIOPCB *p_siopcb, uint_t cbrtn)
{
    switch (cbrtn) {
        case SIO_RDY_SND:
            sil_wrw_mem(LPC5500_USART_FIFOINTENCLR(p_siopcb->base), LPC5500_USART_FIFOINTEN_TXLVL);
            break;
        case SIO_RDY_RCV:
            sil_wrw_mem(LPC5500_USART_FIFOINTENCLR(p_siopcb->base), LPC5500_USART_FIFOINTEN_RXLVL);
            break;
    }
}

/*
 * SIOポートへの文字出力
 */
void target_fput_log(char c)
{
    if (c == '\n') {
        while (! sio_snd_chr(&siopcb[SIOPID_FPUT - 1], '\r')) ;
    }
    while (! sio_snd_chr(&siopcb[SIOPID_FPUT - 1], c)) ;
}
