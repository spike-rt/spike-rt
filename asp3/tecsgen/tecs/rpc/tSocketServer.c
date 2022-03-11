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
 *   $Id: tSocketServer.c 2027 2014-01-20 08:36:17Z okuma-top $
 */

/* #[<PREAMBLE>]#
 * #[<...>]# から #[</...>]# で囲まれたコメントは編集しないでください
 * tecsmerge によるマージに使用されます
 *
 * 属性アクセスマクロ #_CAAM_#
 * portNo           int16_t          ATTR_portNo    
 * sd               int_t            VAR_sd          
 *
 * #[</PREAMBLE>]# */

#include "tSocketServer_tecsgen.h"

#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "errno2ER.h"

#ifndef E_OK
#define	E_OK	0		/* 正常終了 */
#define	E_ID	(-18)	/* 不正ID番号 */
#endif


/* 受け口関数 #_TEPF_# */
/* #[<ENTRY_PORT>]# eC1
 * entry port: eC1
 * signature:  sChannel
 * context:    task
 * #[</ENTRY_PORT>]# */

/* #[<ENTRY_FUNC>]# eC1_send
 * name:         eC1_send
 * global_name:  tSocketServer_eC1_send
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eC1_send(CELLIDX idx, const int8_t* buf, int16_t size, TMO tmo)
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
	syslog( LOG_INFO, "tSocketServer: send" );
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
		if((sz=write( VAR_sd, buf, (size_t)size )) < 0 ){
			ercd = errno2ER( errno );
			break;
		}
		else{
			size -= sz;
		}
	}

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eC1_flush
 * name:         eC1_flush
 * global_name:  tSocketServer_eC1_flush
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eC1_flush(CELLIDX idx, TMO tmo)
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

/* #[<ENTRY_FUNC>]# eC1_receive
 * name:         eC1_receive
 * global_name:  tSocketServer_eC1_receive
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER_UINT
eC1_receive(CELLIDX idx, int8_t* buf, int16_t size, TMO tmo)
{
	ER		ercd = E_OK;
	ER_UINT sz;
	CELLCB	*p_cellcb;

	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	}

	/* ここに処理本体を記述します */
	if( (sz=read( VAR_sd, buf, (size_t)size )) < 0 )
		ercd = errno2ER( errno );
	else
		ercd = sz;

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eC1_end_receive
 * name:         eC1_end_receive
 * global_name:  tSocketServer_eC1_end_receive
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eC1_end_receive(CELLIDX idx)
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

/* #[<ENTRY_FUNC>]# eC1_reset
 * name:         eC1_reset
 * global_name:  tSocketServer_eC1_reset
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eC1_reset(CELLIDX idx)
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
	syslog( LOG_INFO, "tSocketServer: reset" );

	return(ercd);
}

/* #[<ENTRY_PORT>]# eOpener
 * entry port: eOpener
 * signature:  sServerChannelOpener
 * context:    task
 * #[</ENTRY_PORT>]# */

/* #[<ENTRY_FUNC>]# eOpener_open
 * name:         eOpener_open
 * global_name:  tSocketServer_eOpener_open
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eOpener_open(CELLIDX idx)
{
	ER		ercd = E_OK;
	CELLCB	*p_cellcb;
	int		ssoc, soc;
	struct sockaddr_in addr={ AF_INET };

	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	}

	/* ここに処理本体を記述します #_TEFB_# */
	syslog( LOG_INFO, "tSocketServer: open" );

	ssoc=socket(AF_INET,SOCK_STREAM,0);
	if(ssoc<0){
		ercd = errno2ER( errno );
		syslog( LOG_INFO, "tSocketServer: socket failed" );
		return ercd;
	}

	addr.sin_port = htons(ATTR_portNo);
	addr.sin_addr.s_addr = INADDR_ANY;
	if(bind(ssoc,(struct sockaddr*)&addr,sizeof(addr))<0){
		ercd = errno2ER( errno );
		syslog( LOG_INFO, "tSocketServer: bind failed" );
		return ercd;
	}

	if(listen(ssoc, 1 )<0){
		ercd = errno2ER( errno );
		syslog( LOG_INFO, "tSocketServer: listen failed" );
		return ercd;
	}

    soc=accept(ssoc, NULL, NULL);
    if(soc<0){
		ercd = errno2ER( errno );
		syslog( LOG_INFO, "tSocketServer: accept failed" );
		return ercd;
	}
	syslog( LOG_INFO, "tSocketServer: accepted" );

	close( ssoc );
	VAR_sd = soc;

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eOpener_close
 * name:         eOpener_close
 * global_name:  tSocketServer_eOpener_close
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eOpener_close(CELLIDX idx)
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
	syslog( LOG_INFO, "tSocketServer: close" );
	if( close( VAR_sd ) < 0 )
		ercd = errno2ER( errno );

	return(ercd);
}

/* #[<POSTAMBLE>]#
 *   これより下に非受け口関数を書きます
 * #[</POSTAMBLE>]#*/
