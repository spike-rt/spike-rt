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
 *   $Id: tRPCSharedChannelMan.c 2027 2014-01-20 08:36:17Z okuma-top $
 */

/* #[<PREAMBLE>]#
 * #[<...>]# から #[</...>]# で囲まれたコメントは編集しないでください
 * tecsmerge によるマージに使用されます
 *
 * 呼び口関数 #_TCPF_#
 * call port : cSemaphore  signature: sSemaphore context: task
 *   ER             cSemaphore_signal( );
 *   ER             cSemaphore_wait( );
 *   ER             cSemaphore_waitPolling( );
 *   ER             cSemaphore_waitTimeout( TMO timeout );
 *   ER             cSemaphore_initialize( );
 *   ER             cSemaphore_refer( T_RSEM* pk_semaphore_status );
 * call port : cClientSideTDR  signature: sTDR context: task
 *   ER             cClientSideTDR_open( const int8_t* arg, int16_t size, TMO tmo );
 *   ER             cClientSideTDR_close( );
 *   ER             cClientSideTDR_reset( );
 *   ER             cClientSideTDR_sop( int_t no );
 *   ER             cClientSideTDR_is_sop( int_t no );
 *   ER             cClientSideTDR_eop( int_t no );
 *   ER             cClientSideTDR_is_eop( int_t no );
 *   ER             cClientSideTDR_set_tmo( TMO tmo );
 *   ER             cClientSideTDR_put_int8( int8_t in );
 *   ER             cClientSideTDR_put_int16( int16_t in );
 *   ER             cClientSideTDR_put_int32( int32_t in );
 *   ER             cClientSideTDR_put_int64( int64_t in );
 *   ER             cClientSideTDR_put_int128( int128_t in );
 *   ER             cClientSideTDR_get_int8( int8_t* out );
 *   ER             cClientSideTDR_get_int16( int16_t* out );
 *   ER             cClientSideTDR_get_int32( int32_t* out );
 *   ER             cClientSideTDR_get_int64( int64_t* out );
 *   ER             cClientSideTDR_get_int128( int128_t* out );
 *   ER             cClientSideTDR_put_char( char_t in );
 *   ER             cClientSideTDR_put_short( short in );
 *   ER             cClientSideTDR_put_int( int_t in );
 *   ER             cClientSideTDR_put_long( long_t in );
 *   ER             cClientSideTDR_get_char( char_t* out );
 *   ER             cClientSideTDR_get_short( short* out );
 *   ER             cClientSideTDR_get_int( int_t* out );
 *   ER             cClientSideTDR_get_long( long_t* out );
 *   ER             cClientSideTDR_put_intptr( const void* ptr );
 *   ER             cClientSideTDR_get_intptr( void** ptr );
 *
 * #[</PREAMBLE>]# */

#include "tRPCSharedChannelMan_tecsgen.h"

#ifndef E_OK
#define	E_OK	0		/* success */
#define	E_ID	(-18)	/* illegal ID */
#endif


/* 受け口関数 #_TEPF_# */
/* #[<ENTRY_PORT>]# eSemaphore
 * entry port: eSemaphore
 * signature:  sSemaphore
 * context:    task
 * #[</ENTRY_PORT>]# */

/* #[<ENTRY_FUNC>]# eSemaphore_signal
 * name:         eSemaphore_signal
 * global_name:  tRPCSharedTaskMain_eSemaphore_signal
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSemaphore_signal(CELLIDX idx, int_t subscript)
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
	cClientSideTDR_putInt16( RPC_CHANNEL_MAN_EOP_MAGIC );
	ercd = cSemaphore_signal();
	// syslog( LOG_INFO, "tRPCSharedTaskMain: cSemaphore_signal" );

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eSemaphore_wait
 * name:         eSemaphore_wait
 * global_name:  tRPCSharedTaskMain_eSemaphore_wait
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSemaphore_wait(CELLIDX idx, int_t subscript)
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
	ercd = cSemaphore_wait();
	if( ercd == E_OK ){
		// syslog( LOG_INFO, "tRPCSharedTaskMain: cSemaphore_wait" );

		cClientSideTDR_putInt16( RPC_CHANNEL_MAN_SOP_MAGIC );
		cClientSideTDR_putInt16( subscript );
	}

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eSemaphore_waitPolling
 * name:         eSemaphore_waitPolling
 * global_name:  tRPCSharedTaskMain_eSemaphore_waitPolling
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSemaphore_waitPolling(CELLIDX idx, int_t subscript)
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
	ercd = cSemaphore_waitPolling();
	if( ercd == E_OK ){
		cClientSideTDR_putInt16( RPC_CHANNEL_MAN_SOP_MAGIC );
		cClientSideTDR_putInt16( subscript );
	}

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eSemaphore_waitTimeout
 * name:         eSemaphore_waitTimeout
 * global_name:  tRPCSharedTaskMain_eSemaphore_waitTimeout
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSemaphore_waitTimeout(CELLIDX idx, int_t subscript, TMO timeout)
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
	ercd = cSemaphore_waitTimeout(timeout);
	if( ercd == E_OK ){
		// syslog( LOG_INFO, "tRPCSharedTaskMain: cSemaphore_waitTimeout" );

		cClientSideTDR_putInt16( RPC_CHANNEL_MAN_SOP_MAGIC );
		cClientSideTDR_putInt16( subscript );
	}

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eSemaphore_initialize
 * name:         eSemaphore_initialize
 * global_name:  tRPCSharedTaskMain_eSemaphore_initialize
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSemaphore_initialize(CELLIDX idx, int_t subscript)
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

/* #[<ENTRY_FUNC>]# eSemaphore_refer
 * name:         eSemaphore_refer
 * global_name:  tRPCSharedTaskMain_eSemaphore_refer
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSemaphore_refer(CELLIDX idx, int_t subscript, T_RSEM* pk_semaphore_status)
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

/* #[<POSTAMBLE>]#
 *   これより下に非受け口関数を書きます
 * #[</POSTAMBLE>]#*/
