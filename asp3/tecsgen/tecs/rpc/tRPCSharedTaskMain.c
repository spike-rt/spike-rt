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
 *   $Id: tRPCSharedTaskMain.c 2027 2014-01-20 08:36:17Z okuma-top $
 */

/* #[<PREAMBLE>]#
 * #[<...>]# から #[</...>]# で囲まれたコメントは編集しないでください
 * tecsmerge によるマージに使用されます
 *
 * 属性アクセスマクロ #_CAAM_#
 * channelNo        int16_t          VAR_channelNo   
 *
 * 呼び口関数 #_TCPF_#
 * call port : cUnmarshalAndCallFunction  signature: sTaskBody context: task
 *   void           cUnmarshalAndCallFunction_main( subscript );
 * call port : cServerSideTDR  signature: sTDR context: task
 *   ER             cServerSideTDR_reset( );
 *   ER             cServerSideTDR_sendSOP( bool_t b_client );
 *   ER             cServerSideTDR_receiveSOP( bool_t b_client );
 *   ER             cServerSideTDR_sendEOP( bool_t b_continue );
 *   ER             cServerSideTDR_receiveEOP( bool_t b_continue );
 *   ER             cServerSideTDR_putInt8( int8_t in );
 *   ER             cServerSideTDR_putInt16( int16_t in );
 *   ER             cServerSideTDR_putInt32( int32_t in );
 *   ER             cServerSideTDR_putInt64( int64_t in );
 *   ER             cServerSideTDR_putInt128( int128_t in );
 *   ER             cServerSideTDR_getInt8( int8_t* out );
 *   ER             cServerSideTDR_getInt16( int16_t* out );
 *   ER             cServerSideTDR_getInt32( int32_t* out );
 *   ER             cServerSideTDR_getInt64( int64_t* out );
 *   ER             cServerSideTDR_getInt128( int128_t* out );
 *   ER             cServerSideTDR_putFloat32( float32_t in );
 *   ER             cServerSideTDR_putDouble64( double64_t in );
 *   ER             cServerSideTDR_getFloat32( float32_t* out );
 *   ER             cServerSideTDR_getDouble64( double64_t* out );
 *   ER             cServerSideTDR_putChar( char_t in );
 *   ER             cServerSideTDR_putShort( short in );
 *   ER             cServerSideTDR_putInt( int_t in );
 *   ER             cServerSideTDR_putLong( long_t in );
 *   ER             cServerSideTDR_getChar( char_t* out );
 *   ER             cServerSideTDR_getShort( short* out );
 *   ER             cServerSideTDR_getInt( int_t* out );
 *   ER             cServerSideTDR_getLong( long_t* out );
 *   ER             cServerSideTDR_putIntptr( const intptr_t ptr );
 *   ER             cServerSideTDR_getIntptr( intptr_t* ptr );
 * require port : signature: sSysLog context: task
 *   ER             write( uint_t prio, const SYSLOG* p_syslog );
 *   ER_UINT        read( SYSLOG* p_syslog );
 *   ER             mask( uint_t logmask, uint_t lowmask );
 *   ER             refer( T_SYSLOG_RLOG* pk_rlog );
 *
 * #[</PREAMBLE>]# */

#include "tRPCSharedTaskMain_tecsgen.h"
#include "t_syslog.h"

#ifndef E_OK
#define	E_OK	0		/* success */
#define	E_ID	(-18)	/* illegal ID */
#endif

/* 受け口関数 #_TEPF_# */
/* #[<ENTRY_PORT>]# eMain
 * entry port: eMain
 * signature:  sTaskBody
 * context:    task
 * #[</ENTRY_PORT>]# */

/* #[<ENTRY_FUNC>]# eMain_main
 * name:         eMain_main
 * global_name:  tRPCSharedTaskMain_eMain_main
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
void
eMain_main(CELLIDX idx)
{
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		/* エラー処理コードをここに記述します */
	}

	/* ここに処理本体を記述します #_TEFB_# */
	syslog( LOG_INFO, "Shared RPC server task started." );
	while(1){
		int16_t magic;
		ER      er;

		er = cServerSideTDR_getInt16( &magic );
		if( er != E_OK ){
			syslog( LOG_INFO, "err: getInt16 for SOP_MAGIC(%X)", er );
			continue;
		}
		if( magic != RPC_CHANNEL_MAN_SOP_MAGIC ){
			syslog( LOG_INFO, "illegal SOP magic(%X): retrying", (uint16_t)magic );
			continue;
		}
		er = cServerSideTDR_getInt16( &VAR_channelNo );
		if( er != E_OK ){
			syslog( LOG_INFO, "err: getInt16 for channelNo(%X)", er );
			continue;
		}
		if( VAR_channelNo < 0 || VAR_channelNo >= N_CP_cUnmarshalAndCallFunction(p_cellcb) ){
			syslog( LOG_INFO, "illegal channel number(%X): retrying", (uint16_t)VAR_channelNo );
			continue;
		}
		syslog( LOG_INFO, "Shared Channel: servicing channel No. %d", VAR_channelNo );
		
		cUnmarshalAndCallFunction_main( VAR_channelNo );

		er = cServerSideTDR_getInt16( &magic );
		if( er != E_OK ){
			syslog( LOG_INFO, "err: getInt16 for EOP_MAGIC(%X)", er );
			continue;
		}
		if( magic != RPC_CHANNEL_MAN_EOP_MAGIC ){
			syslog( LOG_INFO, "illegal EOP magic(%X): retrying", (uint16_t)magic );
			continue;
		}
	}
}

/* #[<POSTAMBLE>]#
 *   これより下に非受け口関数を書きます
 * #[</POSTAMBLE>]#*/
