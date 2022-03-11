/*
 *  TECS Generator
 *      Generator for TOPPERS Embedded Component System
 *  
 *   Copyright (C) 2008-2013 by TOPPERS Project
 *--
 *   上記著作権者は，以下の(1)(4)の条件を満たす場合に限り，本ソフトウェ
 *   ア（本ソフトウェアを改変したものを含む．以下同じ）を使用・複製・改
 *   変・再配布（以下，利用と呼ぶ）することを無償で許諾する．
 *   (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *       権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *       スコード中に含まれていること．
 *   (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
 *       用できる形で再配布する場合には，再配布に伴うドキュメント（利用
 *       者マニュアルなど）に，上記の著作権表示，この利用条件および下記
 *       の無保証規定を掲載すること．
 *   (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
 *       用できない形で再配布する場合には，次のいずれかの条件を満たすこ
 *       と．
 *     (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
 *         作権表示，この利用条件および下記の無保証規定を掲載すること．
 *     (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
 *         報告すること．
 *   (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *       害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
 *       また，本ソフトウェアのユーザまたはエンドユーザからのいかなる理
 *       由に基づく請求からも，上記著作権者およびTOPPERSプロジェクトを
 *       免責すること．
 *  
 *   本ソフトウェアは，無保証で提供されているものである．上記著作権者お
 *   よびTOPPERSプロジェクトは，本ソフトウェアに関して，特定の使用目的
 *   に対する適合性も含めて，いかなる保証も行わない．また，本ソフトウェ
 *   アの利用により直接的または間接的に生じたいかなる損害に関しても，そ
 *   の責任を負わない．
 *  
 *   $Id: tTINETClient.c 2027 2014-01-20 08:36:17Z okuma-top $
 */

/* #[<PREAMBLE>]#
 * #[<...>]# から #[</...>]# で囲まれたコメントは編集しないでください
 * tecsmerge によるマージに使用されます
 *
 * 属性アクセスマクロ #_CAAM_#
 * cep_id           ID               ATTR_cep_id     
 *
 * #[</PREAMBLE>]# */

#include "tTINETClient_tecsgen.h"
#include "tTINETClient_factory.h"

#include <string.h>

#include <netinet/in.h>
#include <netinet/in_itron.h>
#include <netinet/tcp.h>

#ifndef E_OK
#define	E_OK	0		/* 正常終了 */
#define	E_ID	(-18)	/* 不正ID番号 */
#endif

/* 受け口関数 #_TEPF_# */
/* #[<ENTRY_PORT>]# eC0
 * entry port: eC0
 * signature:  sChannel
 * context:    task
 * #[</ENTRY_PORT>]# */

/* #[<ENTRY_FUNC>]# eC0_send
 * name:         eC0_send
 * global_name:  tTINETClient_eC0_send
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eC0_send(CELLIDX idx, const int8_t* buf, int16_t size, TMO tmo)
{
	ER		ercd = E_OK;
	CELLCB	*p_cellcb;
	int     sz;

	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	}

	/* ここに処理本体を記述します */
#ifdef DEBUG
	syslog( LOG_INFO, "tTINETServer: send" );
	{
		int i;
		for( i = 0; i < size; i++ ){
			printf( "%02X ", (unsigned char)buf[i] );
			if( i % 8 == 7 )
				putchar( '\n' );
		}
		putchar( '\n' );
	}
#endif
	while(size>0){
		if((sz=tcp_snd_dat( ATTR_cep_id, (intptr_t *)buf, (size_t)size, tmo )) < 0 ){
			ercd = sz;
			break;
		}
		else{
			size -= sz;
		}
	}

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eC0_flush
 * name:         eC0_flush
 * global_name:  tTINETClient_eC0_flush
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eC0_flush(CELLIDX idx, TMO tmo)
{
	ER		ercd = E_OK;
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	}

	/* ここに処理本体を記述します */
	/* write はバッファリングなしなので flush しない */

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eC0_receive
 * name:         eC0_receive
 * global_name:  tTINETClient_eC0_receive
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER_UINT
eC0_receive(CELLIDX idx, int8_t* buf, int16_t size, TMO tmo)
{
	ER		ercd = E_OK;
	CELLCB	*p_cellcb;

	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	}

	/* ここに処理本体を記述します */
	ercd = tcp_rcv_dat( ATTR_cep_id, buf, (size_t)size, tmo );

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eC0_end_receive
 * name:         eC0_end_receive
 * global_name:  tTINETClient_eC0_end_receive
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eC0_end_receive(CELLIDX idx)
{
	ER		ercd = E_OK;
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	}

	/* ここに処理本体を記述します #_TEFB_# */

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eC0_reset
 * name:         eC0_reset
 * global_name:  tTINETClient_eC0_reset
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eC0_reset(CELLIDX idx)
{
	ER		ercd = E_OK;
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	}

	/* ここに処理本体を記述します */

	return(ercd);
}

/* #[<ENTRY_PORT>]# eOpener
 * entry port: eOpener
 * signature:  sTINETClientOpenerIPV4
 * context:    task
 * #[</ENTRY_PORT>]# */

/* #[<ENTRY_FUNC>]# eOpener_open
 * name:         eOpener_open
 * global_name:  tTINETClient_eOpener_open
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eOpener_open(CELLIDX idx, const T_IPV4EP* clientAddr, TMO tmo)
{
	ER		ercd = E_OK;
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	}

	/* ここに処理本体を記述します #_TEFB_# */
	puts("opening");
	ercd=tcp_con_cep(ATTR_cep_id, NADR, clientAddr, tmo);
	if(ercd<0){
		puts("failed");
		syslog( LOG_INFO, "TINETClient: connect failed");
	}

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eOpener_close
 * name:         eOpener_close
 * global_name:  tTINETClient_eOpener_close
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eOpener_close(CELLIDX idx, TMO tmo)
{
	ER		ercd = E_OK;
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	}

	/* ここに処理本体を記述します #_TEFB_# */
	ercd = tcp_cls_cep( ATTR_cep_id, 0 );       /* mikan エラー処理 */

	return(ercd);
}

/* #[<POSTAMBLE>]#
 *   これより下に非受け口関数を書きます
 * #[</POSTAMBLE>]#*/
