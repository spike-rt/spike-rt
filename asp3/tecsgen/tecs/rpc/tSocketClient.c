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
 *   $Id: tSocketClient.c 2027 2014-01-20 08:36:17Z okuma-top $
 */

/* #[<PREAMBLE>]#
 * Don't edit the comments between #[<...>]# and #[</...>]#
 * These comment are used by tecsmerege when merging.
 *
 * attr access macro #_CAAM_#
 * portNo           int16_t          ATTR_portNo    
 * serverAddr       char_t*          ATTR_serverAddr
 * sd               int_t            VAR_sd          
 *
 * #[</PREAMBLE>]# */

#include "tSocketClient_tecsgen.h"

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
/* #[<ENTRY_PORT>]# eC0
 * entry port: eC0
 * signature:  sChannel
 * context:    task
 * #[</ENTRY_PORT>]# */

/* #[<ENTRY_FUNC>]# eC0_send
 * name:         eC0_send
 * global_name:  tSocketClient_eC0_send
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eC0_send(CELLIDX idx, const int8_t* buf, int16_t size, TMO tmo)
{
	ER		ercd = E_OK;
	CELLCB	*p_cellcb;
	ssize_t sz;

	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	}

	/* ここに処理本体を記述します */
	// mikan BUG  buf の位置を更新していない
	while(size>0){
		if((sz=write( VAR_sd, buf, (ssize_t)size )) < 0 ){
#ifdef RPC_DEBUG
			perror( "eC0_send" );
			syslog( LOG_INFO, "eC0_send: sd %d, writing %d bytes, ercd=%d", VAR_sd, size, ercd );
#endif
			ercd = errno2ER( errno );
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
 * global_name:  tSocketClient_eC0_flush
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
 * global_name:  tSocketClient_eC0_receive
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER_UINT
eC0_receive(CELLIDX idx, int8_t* buf, int16_t size, TMO tmo)
{
	ER		ercd = E_OK;
	CELLCB	*p_cellcb;
	ssize_t  sz;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	}

	/* ここに処理本体を記述します */
	sz = read( VAR_sd, buf, (ssize_t)size );
	ercd = errno2ER( errno );
#ifdef DEBUG
	{
		int i;
		for( i = 0; i < sz; i++ ){
			printf( "%02X ", (unsigned char)buf[i] );
			if( i % 8 == 7 )
				putchar( '\n' );
		}
		putchar( '\n' );
	}
#endif
	/* mikan 一発で全て読めると仮定 */
	if( sz >= 0 )
		ercd = sz;

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eC0_end_receive
 * name:         eC0_end_receive
 * global_name:  tSocketClient_eC0_end_receive
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
 * global_name:  tSocketClient_eC0_reset
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
 * signature:  sSocketClientOpener
 * context:    task
 * #[</ENTRY_PORT>]# */

/* #[<ENTRY_FUNC>]# eOpener_open
 * name:         eOpener_open
 * global_name:  tSocketClient_eOpener_open
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eOpener_open(CELLIDX idx, const char_t* serverAddr, uint16_t portNo, TMO tmo)
{
	ER		ercd = E_OK;
	CELLCB	*p_cellcb;
	int		soc;
	struct sockaddr_in addr={AF_INET};

	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	}

	/* ここに処理本体を記述します #_TEFB_# */
	soc=socket(AF_INET, SOCK_STREAM, 0);
	if(soc<0){
		ercd = errno2ER( errno );
		syslog( LOG_INFO, "SocketClient: socket failed");
		return ercd;
	}

#if 0
/* ノンブロックに変更 */
	int flag=fcntl(soc, F_GETFL, 0);
	if(flag<0) perror("fcntl(GET) error");
	if(fcntl(soc, F_SETFL, flag|O_NONBLOCK)<0) perror("fcntl(NONBLOCK) error");
#endif /* 0 */

	puts( "connecting" );
	inet_aton( serverAddr, &addr.sin_addr );
	addr.sin_port=htons(portNo);
	if(connect(soc, (struct sockaddr*)&addr, sizeof(addr))<0){
		ercd = errno2ER( errno );
		close( soc );
		printf( "connect fail errno=%d, ercd=%d\n", errno, ercd );
		syslog( LOG_INFO, "SocketClient: connect" );
		return ercd;
	}
	VAR_sd = soc;

#if 0
	if(errno!=EINPROGRESS)
		perror("connect error");
	//EINPROGRESS:コネクション要求は始まったが、まだ完了していない

	fd_set rmask,wmask;FD_ZERO(&rmask);FD_SET(soc,&rmask);wmask=rmask;
	struct timeval tv={ タイムアウト時間[秒],[μ秒] };
	int rc=select(soc+1, &rmask, &wmask, NULL, &tv);
	if(rc<0) perror("connect-select error");
	if(rc==0){ タイムアウト処理 }
	if(rc==2){ //読み書きが同時に出来る場合
#if Solaris
		int val;
		socklen_t len=sizeof(val);
		if(getsockopt(soc,SOL_SOCKET,SO_ERROR, &val,&len)>=0) {
#elif Linux
			struct sockaddr_in name;
			socklen_t len=sizeof(name);
			if(getpeername(soc,(struct sockaddr*)&name,&len)>=0) {
#endif
				// 既にデータが来ている
			}else{
				// コネクト失敗
			}
		}
	}
//フラグを元に戻す
	if(fcntl(soc, F_SETFL, flag)<0) perror("fcntl(END) error");
#endif /* 0 */

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eOpener_simpleOpen
 * name:         eOpener_simpleOpen
 * global_name:  tSocketClient_eOpener_simpleOpen
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eOpener_simpleOpen(CELLIDX idx, TMO tmo)
{
	ER		ercd = E_OK;
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */

	/* Put statements here #_TEFB_# */
	ercd = eOpener_open(idx, ATTR_serverAddr, ATTR_portNo, tmo);

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eOpener_close
 * name:         eOpener_close
 * global_name:  tSocketClient_eOpener_close
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
	close( VAR_sd );       /* mikan エラー処理 */

	return(ercd);
}

/* #[<POSTAMBLE>]#
 *   Put non-entry functions below.
 * #[</POSTAMBLE>]#*/
