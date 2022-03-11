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
 *   $Id: tNBOTDR_inline.h 2569 2016-03-27 12:58:00Z okuma-top $
 */

/* #[<PREAMBLE>]#
 * #[<...>]# から #[</...>]# で囲まれたコメントは編集しないでください
 * tecsmerge によるマージに使用されます
 *
 * 属性アクセスマクロ #_CAAM_#
 * tmo              TMO              VAR_tmo         
 *
 * 呼び口関数 #_TCPF_#
 * call port: cChannel signature: sChannel context:task 
 *   ER             cChannel_send( const int8_t* buf, int16_t size, TMO tmo );
 *   ER             cChannel_flush( TMO tmo );
 *   ER_UINT        cChannel_receive( int8_t* buf, int16_t size, TMO tmo );
 *   ER             cChannel_end_receive( );
 *   ER             cChannel_reset( );
 * require port: signature:sSysLog context:task
 *   ER             write( uint_t prio, const SYSLOG* p_syslog );
 *   ER_UINT        read( SYSLOG* p_syslog );
 *   ER             mask( uint_t logmask, uint_t lowmask );
 *   ER             refer( T_SYSLOG_RLOG* pk_rlog );
 *
 * #[</PREAMBLE>]# */

/*
 * marhaler, unmarshaler が使用する STRLEN 関数を定義する
 *  marshaler, unmarshaler は必ず TDR は に結合されるため
 */
#include	"rpc_string.h"

/*
 * Network Byte Order TECS Data Representation
 *
 *  NBOTDR is one of sTDR implementation and is default TDR for OpaqueRPC.
 *  NBOTDR always sends and receives data in Big Endian (Network Byte Order).
 *  So NBOTDR is useful with distributed hybrid systems.
 */

/*
 * Macro Symbols:
 * 1) endian
 *  NBOTDR_BIGENDIAN
 *  NBOTDR_LITTLEENDIAN
 *   if these macros are not specified, endian is automatically decided.
 * 2) size of each type in bits.
 *  NBOTDR_LONG_BIT (32)
 *  NBOTDR_SHORT_BIT (16)
 *  NBOTDR_INT_BIT (32)
 *  NBOTDR_INTPTR_BIT (32)
 *   default value in parenthesises
 *   normally use default value
 */

/*
 * 無調整型だが、いくらか暫定的な実装になっている
 * ・ntohX(), htonX() は int64_t 型に定義されている (32bitならオーバヘッド大)
 * ・int128_t, uint128_t は tecs.h で typedef された型である
 * ・BIGENDIAN 判定は動的になっている (恐らく最適化で定数判定にされることはない)
 */

#if defined( NBOTDR_BIGENDIAN )
#define	IS_BIGENDIAN()	(1)
# if defined( NBOTDR_LITTLEENDIAN )
#   error  "Don't specify both NBOTDR_LITTLEENDIAN and NBOTDR_BIGENDIAN"
# endif
#elif defined( NBOTDR_LITTLEENDIAN )
#define	IS_BIGENDIAN()	(0)
#else
/* BIGENDIAN かどうか自動判定 */
static const uint16_t	endian_check = 0xff00;
#define	IS_BIGENDIAN()	(*(uint8_t *)&endian_check)
#warning "it is better to specify -D NBOTDR_BIGENDIAN or -D NBOTDR_LITTLEENDIAN"
#endif

#if ! defined( NBOTDR_LONG_BIT )
#define	NBOTDR_LONG_BIT	32
#endif
#if NBOTDR_LONG_BIT == 32
#  define  NTOHL( d, sz )	ntoh32( d )
#  define  HTONL( d, sz )	hton32( d )
typedef		int32_t		TDR_LONG;
typedef		uint32_t	TDR_ULONG;
#elif NBOTDR_LONG_BIT == 64
#  define  NTOHL( d, sz )	ntoh64( d )
#  define  HTONL( d, sz )	hton64( d )
typedef		int64_t		TDR_LONG;
typedef		uint64_t	TDR_ULONG;
#elif NBOTDR_LONG_BIT == 128
#  define  NTOHL( d, sz )	ntoh128( d )
#  define  HTONL( d, sz )	hton128( d )
typedef		int128_t	TDR_LONG;
typedef		uint128_t	TDR_ULONG;
#elif ! defined( NBOTDR_LONG_BIT )
#  define  NTOHL	ntohX
#  define  HTONL	htonX
#  warning "it is better to specify -D NBOTDR_LONG_BIT=N  N: 32, 64, 128"
#else
#  error  "NBOTDR_LONG_BIT must be 32, 64 or 128"
#endif

#if ! defined( NBOTDR_SHORT_BIT )
#define	NBOTDR_SHORT_BIT	16
#endif
#if NBOTDR_SHORT_BIT == 16
#  define  NTOHS( d, sz )	ntoh16( d )
#  define  HTONS( d, sz )	hton16( d )
typedef		int16_t		TDR_SHORT;
typedef		uint16_t	TDR_USHORT;
#elif NBOTDR_SHORT_BIT == 32
#  define  NTOHS( d, sz )	ntoh32( d )
#  define  HTONS( d, sz )	hton32( d )
typedef		int32_t		TDR_SHORT;
typedef		uint32_t	TDR_USHORT;
#elif NBOTDR_SHORT_BIT == 64
#  define  NTOHS( d, sz )	ntoh64( d )
#  define  HTONS( d, sz )	hton64( d )
typedef		int64_t		TDR_SHORT;
typedef		uint64_t	TDR_USHORT;
#elif ! defined( NBOTDR_SHORT_BIT )
#  define  NTOHS( d, sz )	ntohX( d, sz )
#  define  HTONS( d, sz )	htonX( d, sz )
#  warning "it is better to specify -D NBOTDR_SHORT_BIT=N  N: 16, 32, 64"
#else
#  error  "NBOTDR_SHORT_BIT must be 16, 32 or 64"
#endif

#if ! defined( NBOTDR_INT_BIT )
#define	NBOTDR_INT_BIT	32
#endif
#if NBOTDR_INT_BIT == 16
#  define  NTOHI( d, sz )	ntoh16( d )
#  define  HTONI( d, sz )	hton16( d )
typedef		int16_t		TDR_INT;
typedef		uint16_t	TDR_UINT;
#elif NBOTDR_INT_BIT == 32
#  define  NTOHI( d, sz )	ntoh32( d )
#  define  HTONI( d, sz )	hton32( d )
typedef		int32_t		TDR_INT;
typedef		uint32_t	TDR_UINT;
#elif NBOTDR_INT_BIT == 64
#  define  NTOHI( d, sz )	ntoh64( d )
#  define  HTONI( d, sz )	hton64( d )
typedef		int64_t		TDR_INT;
typedef		uint64_t	TDR_UINT;
#elif NBOTDR_INT_BIT == 128
#  define  NTOHI( d, sz )	ntoh128( d )
#  define  HTONI( d, sz )	hton128( d )
typedef		int128_t	TDR_INT;
typedef		uint128_t	TDR_UINT;
#elif ! defined( NBOTDR_INT_BIT )
#  define  NTOHI( d, sz )	ntohX( d, sz )
#  define  HTONI( d, sz )	htonX( d, sz )
#  warning "it is better to specify -D NBOTDR_INT_BIT=N  N: 16, 32, 64, 128"
#else
#  error  "NBOTDR_INT_BIT must be 16, 32, 64 or 128"
#endif

#if ! defined( NBOTDR_INTPTR_BIT )
#define	NBOTDR_INTPTR_BIT	32
#endif
#if NBOTDR_INTPTR_BIT == 16
#  define  NTOHIP( d, sz )	ntoh16( d )
#  define  HTONIP( d, sz )	hton16( d )
typedef		int16_t		TDR_INTPTR;
#elif NBOTDR_INTPTR_BIT == 32
#  define  NTOHIP( d, sz )	ntoh32( d )
#  define  HTONIP( d, sz )	hton32( d )
typedef		int32_t		TDR_INTPTR;
#elif NBOTDR_INTPTR_BIT == 64
#  define  NTOHIP( d, sz )	ntoh64( d )
#  define  HTONIP( d, sz )	hton64( d )
typedef		int64_t		TDR_INTPTR;
#elif NBOTDR_INTPTR_BIT == 128
#  define  NTOHIP( d, sz )	ntoh128( d )
#  define  HTONIP( d, sz )	hton128( d )
typedef		int128_t	TDR_INTPTR;
#elif ! defined( NBOTDR_INTPTR_BIT )
#  define  NTOHIP( d, sz )		ntohX( d, sz )
#  define  HTONIP( d, sz )		htonX( d, sz )
#  warning "it is better to specify -D NBOTDR_INTPTR_BIT=N  N: 16, 32, 64, 128"
#else
#  error  "NBOTDR_INTPTR_BIT must be 16, 32, 64 or 128 "
#endif

Inline int16_t
hton16( int16_t d )
{
	if( IS_BIGENDIAN() )
		return d;		// big endian
	else{
		union  {
			int8_t	by[2];
			int16_t	d;
		} d0, d1;
		// syslog( LOG_INFO, "### little endian ###" );
		d0.d = d;
		d1.by[0] = d0.by[1];
		d1.by[1] = d0.by[0];
		return d1.d;
	}
}

Inline int32_t
hton32( int32_t d )
{
	if( IS_BIGENDIAN() )
		return d;
	else{
		union  {
			int8_t	by[4];
			int32_t	d;
		} d0, d1;
		d0.d = d;
		d1.by[0] = d0.by[3];
		d1.by[1] = d0.by[2];
		d1.by[2] = d0.by[1];
		d1.by[3] = d0.by[0];
		return d1.d;
	}
}

Inline int64_t
hton64( int64_t d )
{
	if( IS_BIGENDIAN() )
		return d;
	else{
		union  {
			int8_t	by[8];
			int64_t	d;
		} d0, d1;
		d0.d = d;
		d1.by[0] = d0.by[7];
		d1.by[1] = d0.by[6];
		d1.by[2] = d0.by[5];
		d1.by[3] = d0.by[4];
		d1.by[4] = d0.by[3];
		d1.by[5] = d0.by[2];
		d1.by[6] = d0.by[1];
		d1.by[7] = d0.by[0];
		return d1.d;
	}
}

Inline int128_t
hton128( int128_t d )
{
	if( IS_BIGENDIAN() )
		return d;
	else{
		/* この実装は tecs.h で typedef した int128_t に適合 */
		int128_t d1;
		d1.hi = hton64( d.lo );
		d1.lo = hton64( d.hi );
		return	d1;
	}
}

Inline int16_t
ntoh16( int16_t d )
{
	if( IS_BIGENDIAN() )
		return d;
	else{
		union  {
			int8_t	by[2];
			int16_t	d;
		} d0, d1;
		d0.d = d;
		d1.by[0] = d0.by[1];
		d1.by[1] = d0.by[0];
		return d1.d;
	}
}

Inline int32_t
ntoh32( int32_t d )
{
	if( IS_BIGENDIAN() )
		return d;
	else{
		union  {
			int8_t	by[4];
			int32_t	d;
		} d0, d1;
		d0.d = d;
		d1.by[0] = d0.by[3];
		d1.by[1] = d0.by[2];
		d1.by[2] = d0.by[1];
		d1.by[3] = d0.by[0];
		return d1.d;
	}
}

Inline int64_t
ntoh64( int64_t d )
{
	if( IS_BIGENDIAN() )
		return d;
	else{
		union  {
			int8_t	by[8];
			int64_t	d;
		} d0, d1;
		d0.d = d;
		d1.by[0] = d0.by[7];
		d1.by[1] = d0.by[6];
		d1.by[2] = d0.by[5];
		d1.by[3] = d0.by[4];
		d1.by[4] = d0.by[3];
		d1.by[5] = d0.by[2];
		d1.by[6] = d0.by[1];
		d1.by[7] = d0.by[0];
		return d1.d;
	}
}

/*** unsigned 用の関数 (128 bit 版のみ用意) ***/
Inline int128_t
ntoh128( int128_t d )
{
	if( IS_BIGENDIAN() )
		return d;
	else{
		/* この実装は tecs.h で typedef した int128_t に適合 */
		int128_t d1;
		d1.hi = ntoh64( d.lo );
		d1.lo = ntoh64( d.hi );
		return	d1;
	}
}

Inline uint128_t
hton128u( uint128_t d )
{
	if( IS_BIGENDIAN() )
		return d;
	else{
		/* この実装は tecs.h で typedef した int128_t に適合 */
		uint128_t d1;
		d1.hi = hton64( d.lo );
		d1.lo = hton64( d.hi );
		return	d1;
	}
}

Inline uint128_t
ntoh128u( uint128_t d )
{
	if( IS_BIGENDIAN() )
		return d;
	else{
		/* この実装は tecs.h で typedef した int128_t に適合 */
		uint128_t d1;
		d1.hi = ntoh64( d.lo );
		d1.lo = ntoh64( d.hi );
		return	d1;
	}
}

/************************************************
 *  htonX, ntohX は 16, 32, 64 にのみ対応できる *
 ************************************************/
Inline int64_t
htonX( int64_t d, int_t sz )
{
	switch( sz ){
	case	2:	return	hton16( d );
	case	4:	return	hton32( d );
	case	8:	return	hton64( d );
	default:	return	0;			// # mikan
	};
}

Inline int64_t
ntohX( int64_t d, int_t sz )
{
	switch( sz ){
	case	2:	return	ntoh16( d );
	case	4:	return	ntoh32( d );
	case	8:	return	ntoh64( d );
	default:	return	0;			// # mikan
	};
}

/* 受け口関数 #_TEPF_# */
/* #[<ENTRY_PORT>]# eTDR
 * entry port: eTDR
 * signature:  sTDR
 * context:    task
 * #[</ENTRY_PORT>]# */

/* #[<ENTRY_FUNC>]# eTDR_reset
 * name:         eTDR_reset
 * global_name:  tNBOTDR_eTDR_reset
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_reset(CELLIDX idx)
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

	syslog( LOG_INFO, "TDR: resetting channel" );
	ercd = cChannel_reset();
	return(ercd);
}

/* #[<ENTRY_FUNC>]# eTDR_sendSOP
 * name:         eTDR_sendSOP
 * global_name:  tNBOTDR_eTDR_sendSOP
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_sendSOP(CELLIDX idx, bool_t b_client )
{
	ER		ercd = E_OK;
	CELLCB	*p_cellcb;
	uint16_t sopMagic;
	uint8_t	val;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	}

	/* ここに処理本体を記述します #_TEFB_# */
#ifdef RPC_DEBUG
	syslog(LOG_INFO, "eTDR_sendSOP(b_client=%d)", b_client);
#endif
    if( b_client )
		sopMagic = TDR_SOP_MAGIC1;
	else
		sopMagic = TDR_SOP_MAGIC2;

	/*
	 * SOP パケットの送信
	 * パケットシーケンスが多少崩れても回復できるように、バイト単位で送る
	 * 上位バイト、下位バイトの順に送る
	 */

	val = (uint8_t)(sopMagic>>8);
	ercd = eTDR_putUInt8( idx, val );		/* 上位8ビット */
	if( ercd != E_OK )
		return	ercd;

	val = (uint8_t)(sopMagic&0xff);			/* 下位8ビット */
	ercd = eTDR_putUInt8( idx, val );

	return	ercd;
}

/* #[<ENTRY_FUNC>]# eTDR_receiveSOP
 * name:         eTDR_receiveSOP
 * global_name:  tNBOTDR_eTDR_receiveSOP
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_receiveSOP(CELLIDX idx, bool_t b_client )
{
	ER		ercd = E_OK;
	CELLCB	*p_cellcb;
	uint16_t sopMagic;
	uint8_t val;

	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	}

	/* ここに処理本体を記述します #_TEFB_# */
#ifdef RPC_DEBUG
	syslog(LOG_INFO, "eTDR_receiveSOP(b_client=%d)", b_client);
#endif

    if( b_client )
		sopMagic = TDR_SOP_MAGIC2;
	else
		sopMagic = TDR_SOP_MAGIC1;

	if( (ercd=eTDR_getUInt8( idx, &val )) != E_OK )
		;
	else if( val != (uint8_t)(sopMagic>>8) ){
		ercd = E_MAGIC;
	}
	else if((ercd=eTDR_getUInt8( idx, &val )) != E_OK )
		;
	else if( val != (uint8_t)sopMagic ){
		ercd = E_MAGIC;
	}

	if( ercd != E_OK )
		syslog( LOG_INFO, "receiveSOP: ERCD=%d", ercd );

	return ercd;
}

/* #[<ENTRY_FUNC>]# eTDR_sendEOP
 * name:         eTDR_sendEOP
 * global_name:  tNBOTDR_eTDR_sendEOP
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_sendEOP(CELLIDX idx, bool_t b_continue )
{
	ER		ercd = E_OK;
	CELLCB	*p_cellcb;
	uint16_t magic;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	}

	/* ここに処理本体を記述します #_TEFB_# */
#ifdef RPC_DEBUG
	syslog(LOG_INFO, "eTDR_sendEOP(b_continue=%d)", b_continue);
#endif

	if( b_continue )
		magic = TDR_EOP_MAGIC1;
	else
		magic = TDR_EOP_MAGIC2;

	/* EOP magic の送信 */
	if( (ercd = eTDR_putUInt16( idx, magic )) != E_OK )
		return ercd;

	return cChannel_flush(VAR_tmo);
}

/* #[<ENTRY_FUNC>]# eTDR_receiveEOP
 * name:         eTDR_receiveEOP
 * global_name:  tNBOTDR_eTDR_receiveEOP
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_receiveEOP(CELLIDX idx, bool_t b_continue)
{
	ER		ercd = E_OK, er2;
	CELLCB	*p_cellcb;
	uint16_t magic;

	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	}

	/* ここに処理本体を記述します #_TEFB_# */
#ifdef RPC_DEBUG
	syslog(LOG_INFO, "eTDR_receiveEOP(b_continue=%d)",b_continue);
#endif
/* mikan cChannel EOF get_* */
/* mikan magic EOF */

	if( (ercd=eTDR_getUInt16( idx, &magic )) == E_OK ){
		if( (b_continue && magic == TDR_EOP_MAGIC1) || (!b_continue && magic == TDR_EOP_MAGIC2) ){
		} else {
			ercd = E_MAGIC;
		}
	}
	er2 = cChannel_end_receive();
	return ercd != E_OK ? ercd : er2;
}

/* #[<ENTRY_FUNC>]# eTDR_putInt8
 * name:         eTDR_putInt8
 * global_name:  tNBOTDR_eTDR_putInt8
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_putInt8(CELLIDX idx, int8_t in)
{
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	}

	/* ここに処理本体を記述します #_TEFB_# */
	return cChannel_send( (int8_t *)&in, (int16_t)sizeof( in ), VAR_tmo );
}

/* #[<ENTRY_FUNC>]# eTDR_putInt16
 * name:         eTDR_putInt16
 * global_name:  tNBOTDR_eTDR_putInt16
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_putInt16(CELLIDX idx, int16_t in)
{
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	}

	/* ここに処理本体を記述します #_TEFB_# */
	in = hton16( in );
	return cChannel_send( (int8_t *)&in, (int16_t)sizeof( in ), VAR_tmo );
}

/* #[<ENTRY_FUNC>]# eTDR_putInt32
 * name:         eTDR_putInt32
 * global_name:  tNBOTDR_eTDR_putInt32
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_putInt32(CELLIDX idx, int32_t in)
{
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	}

	/* ここに処理本体を記述します #_TEFB_# */
	in = hton32( in );
	return cChannel_send( (int8_t *)&in, (int16_t)sizeof( in ), VAR_tmo );
}

/* #[<ENTRY_FUNC>]# eTDR_putInt64
 * name:         eTDR_putInt64
 * global_name:  tNBOTDR_eTDR_putInt64
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_putInt64(CELLIDX idx, int64_t in)
{
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	}

	/* ここに処理本体を記述します #_TEFB_# */
	in = hton64( in );
	return cChannel_send( (int8_t *)&in, (int16_t)sizeof( in ), VAR_tmo );
}

/* #[<ENTRY_FUNC>]# eTDR_putInt128
 * name:         eTDR_putInt128
 * global_name:  tNBOTDR_eTDR_putInt128
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_putInt128(CELLIDX idx, int128_t in)
{
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	}

	/* ここに処理本体を記述します #_TEFB_# */
	in = hton128( in );
	return cChannel_send( (int8_t *)&in, (int16_t)sizeof( in ), VAR_tmo );
}

/* #[<ENTRY_FUNC>]# eTDR_getInt8
 * name:         eTDR_getInt8
 * global_name:  tNBOTDR_eTDR_getInt8
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_getInt8(CELLIDX idx, int8_t* out)
{
	ER_UINT	 er_sz;
	CELLCB	 *p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	}

	/* ここに処理本体を記述します #_TEFB_# */
	er_sz = cChannel_receive( (int8_t *)out, (int16_t)sizeof( *out ), VAR_tmo );
	return  er_sz < 0 ? er_sz : ( er_sz == sizeof( *out ) ? E_OK : E_BOVR );
}

/* #[<ENTRY_FUNC>]# eTDR_getInt16
 * name:         eTDR_getInt16
 * global_name:  tNBOTDR_eTDR_getInt16
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_getInt16(CELLIDX idx, int16_t* out)
{
	ER_UINT	 er_sz;
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	}

	/* ここに処理本体を記述します #_TEFB_# */
	er_sz = cChannel_receive( (int8_t *)out, (int16_t)sizeof( *out ), VAR_tmo );
	*out = ntoh16( *out );
	return  er_sz < 0 ? er_sz : ( er_sz == sizeof( *out ) ? E_OK : E_BOVR );
}

/* #[<ENTRY_FUNC>]# eTDR_getInt32
 * name:         eTDR_getInt32
 * global_name:  tNBOTDR_eTDR_getInt32
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_getInt32(CELLIDX idx, int32_t* out)
{
	ER_UINT	 er_sz;
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	}

	/* ここに処理本体を記述します #_TEFB_# */
	er_sz = cChannel_receive( (int8_t *)out, (int16_t)sizeof( *out ), VAR_tmo );
	*out = ntoh32( *out );
	return  er_sz < 0 ? er_sz : ( er_sz == sizeof( *out ) ? E_OK : E_BOVR );
}

/* #[<ENTRY_FUNC>]# eTDR_getInt64
 * name:         eTDR_getInt64
 * global_name:  tNBOTDR_eTDR_getInt64
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_getInt64(CELLIDX idx, int64_t* out)
{
	ER_UINT	 er_sz;
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	}

	/* ここに処理本体を記述します #_TEFB_# */
	er_sz = cChannel_receive( (int8_t *)out, (int16_t)sizeof( *out ), VAR_tmo );
	*out = ntoh64( *out );
	return  er_sz < 0 ? er_sz : ( er_sz == sizeof( *out ) ? E_OK : E_BOVR );
}

/* #[<ENTRY_FUNC>]# eTDR_getInt128
 * name:         eTDR_getInt128
 * global_name:  tNBOTDR_eTDR_getInt128
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_getInt128(CELLIDX idx, int128_t* out)
{
	ER_UINT	 er_sz;
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	}

	/* ここに処理本体を記述します #_TEFB_# */
	er_sz = cChannel_receive( (int8_t *)out, (int16_t)sizeof( *out ), VAR_tmo );
	*out = ntoh128( *out );
	return  er_sz < 0 ? er_sz : ( er_sz == sizeof( *out ) ? E_OK : E_BOVR );
}

/* #[<ENTRY_FUNC>]# eTDR_putUInt8
 * name:         eTDR_putUInt8
 * global_name:  tNBOTDR_eTDR_putUInt8
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_putUInt8(CELLIDX idx, uint8_t in)
{
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
	return cChannel_send( (int8_t *)&in, (int16_t)sizeof( in ), VAR_tmo );
}

/* #[<ENTRY_FUNC>]# eTDR_putUInt16
 * name:         eTDR_putUInt16
 * global_name:  tNBOTDR_eTDR_putUInt16
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_putUInt16(CELLIDX idx, uint16_t in)
{
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
	in = hton16( in );
	return cChannel_send( (int8_t *)&in, (int16_t)sizeof( in ), VAR_tmo );
}

/* #[<ENTRY_FUNC>]# eTDR_putUInt32
 * name:         eTDR_putUInt32
 * global_name:  tNBOTDR_eTDR_putUInt32
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_putUInt32(CELLIDX idx, uint32_t in)
{
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
	in = hton32( in );
	return cChannel_send( (int8_t *)&in, (int16_t)sizeof( in ), VAR_tmo );
}

/* #[<ENTRY_FUNC>]# eTDR_putUInt64
 * name:         eTDR_putUInt64
 * global_name:  tNBOTDR_eTDR_putUInt64
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_putUInt64(CELLIDX idx, uint64_t in)
{
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
	in = hton64( in );
	return cChannel_send( (int8_t *)&in, (int16_t)sizeof( in ), VAR_tmo );
}

/* #[<ENTRY_FUNC>]# eTDR_putUInt128
 * name:         eTDR_putUInt128
 * global_name:  tNBOTDR_eTDR_putUInt128
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_putUInt128(CELLIDX idx, uint128_t in)
{
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
	in = hton128u( in );
	return cChannel_send( (int8_t *)&in, (int16_t)sizeof( in ), VAR_tmo );
}

/* #[<ENTRY_FUNC>]# eTDR_getUInt8
 * name:         eTDR_getUInt8
 * global_name:  tNBOTDR_eTDR_getUInt8
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_getUInt8(CELLIDX idx, uint8_t* out)
{
	ER_UINT	 er_sz;
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
	er_sz = cChannel_receive( (int8_t *)out, (int16_t)sizeof( *out ), VAR_tmo );
	return  er_sz < 0 ? er_sz : ( er_sz == sizeof( *out ) ? E_OK : E_BOVR );
}

/* #[<ENTRY_FUNC>]# eTDR_getUInt16
 * name:         eTDR_getUInt16
 * global_name:  tNBOTDR_eTDR_getUInt16
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_getUInt16(CELLIDX idx, uint16_t* out)
{
	ER_UINT	 er_sz;
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
	er_sz = cChannel_receive( (int8_t *)out, (int16_t)sizeof( *out ), VAR_tmo );
	*out = ntoh16( *out );
	return  er_sz < 0 ? er_sz : ( er_sz == sizeof( *out ) ? E_OK : E_BOVR );
}

/* #[<ENTRY_FUNC>]# eTDR_getUInt32
 * name:         eTDR_getUInt32
 * global_name:  tNBOTDR_eTDR_getUInt32
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_getUInt32(CELLIDX idx, uint32_t* out)
{
	ER_UINT	 er_sz;
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
	er_sz = cChannel_receive( (int8_t *)out, (int16_t)sizeof( *out ), VAR_tmo );
	*out = ntoh32( *out );
	return  er_sz < 0 ? er_sz : ( er_sz == sizeof( *out ) ? E_OK : E_BOVR );
}

/* #[<ENTRY_FUNC>]# eTDR_getUInt64
 * name:         eTDR_getUInt64
 * global_name:  tNBOTDR_eTDR_getUInt64
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_getUInt64(CELLIDX idx, uint64_t* out)
{
	ER_UINT	 er_sz;
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
	er_sz = cChannel_receive( (int8_t *)out, (int16_t)sizeof( *out ), VAR_tmo );
	*out = ntoh64( *out );
	return  er_sz < 0 ? er_sz : ( er_sz == sizeof( *out ) ? E_OK : E_BOVR );
}

/* #[<ENTRY_FUNC>]# eTDR_getUInt128
 * name:         eTDR_getUInt128
 * global_name:  tNBOTDR_eTDR_getUInt128
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_getUInt128(CELLIDX idx, uint128_t* out)
{
	ER_UINT	 er_sz;
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
	er_sz = cChannel_receive( (int8_t *)out, (int16_t)sizeof( *out ), VAR_tmo );
	*out = ntoh128u( *out );
	return  er_sz < 0 ? er_sz : ( er_sz == sizeof( *out ) ? E_OK : E_BOVR );
}

/* #[<ENTRY_FUNC>]# eTDR_putChar
 * name:         eTDR_putChar
 * global_name:  tNBOTDR_eTDR_putChar
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_putChar(CELLIDX idx, char_t in)
{
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	}

	/* ここに処理本体を記述します #_TEFB_# */
	return cChannel_send( (int8_t *)&in, (int16_t)sizeof( in ), VAR_tmo );
}

/* #[<ENTRY_FUNC>]# eTDR_getChar
 * name:         eTDR_getChar
 * global_name:  tNBOTDR_eTDR_getChar
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_getChar(CELLIDX idx, char_t* out)
{
	ER_UINT	 er_sz;
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	}

	/* ここに処理本体を記述します #_TEFB_# */
	er_sz = cChannel_receive( (int8_t *)out, (int16_t)sizeof( *out ), VAR_tmo );
	return  er_sz < 0 ? er_sz : ( er_sz == sizeof( *out ) ? E_OK : E_BOVR );
}

/* #[<ENTRY_FUNC>]# eTDR_putBool
 * name:         eTDR_putBool
 * global_name:  tNBOTDR_eTDR_putBool
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_putBool(CELLIDX idx, bool_t in)
{
	ER		ercd = E_OK;
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
	eTDR_putUInt8(idx, (uint8_t)(in != 0) );

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eTDR_getBool
 * name:         eTDR_getBool
 * global_name:  tNBOTDR_eTDR_getBool
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_getBool(CELLIDX idx, bool_t* out)
{
	ER		ercd = E_OK;
	CELLCB	*p_cellcb;
	uint8_t	val;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
	ercd = eTDR_getUInt8(idx, &val );
	if( ercd != E_OK )
		return ercd;
	*out = (val != 0);

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eTDR_putFloat32
 * name:         eTDR_putFloat32
 * global_name:  tNBOTDR_eTDR_putFloat32
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_putFloat32(CELLIDX idx, float32_t in)
{
	CELLCB	*p_cellcb;
    int32_t val;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	}

	/* ここに処理本体を記述します #_TEFB_# */
	val = hton32( *(int32_t*)&in );
	return cChannel_send( (int8_t *)&val, (int16_t)sizeof( val ), VAR_tmo );
}

/* #[<ENTRY_FUNC>]# eTDR_putDouble64
 * name:         eTDR_putDouble64
 * global_name:  tNBOTDR_eTDR_putDouble64
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_putDouble64(CELLIDX idx, double64_t in)
{
    int64_t val;
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	}

	/* ここに処理本体を記述します #_TEFB_# */
    val = *(int64_t *)&in;
	val = hton64( val );
	return cChannel_send( (int8_t *)&val, (int16_t)sizeof( val ), VAR_tmo );
}

/* #[<ENTRY_FUNC>]# eTDR_getFloat32
 * name:         eTDR_getFloat32
 * global_name:  tNBOTDR_eTDR_getFloat32
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_getFloat32(CELLIDX idx, float32_t* out)
{
	ER_UINT		er_sz = E_OK;
    int32_t   val;
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	}

	/* ここに処理本体を記述します #_TEFB_# */
	er_sz = cChannel_receive( (int8_t *)&val, (int16_t)sizeof( val ), VAR_tmo );
	val = ntoh32( val );
    *out = *(float32_t *)&val;
	return  er_sz < 0 ? er_sz : ( er_sz == sizeof( *out ) ? E_OK : E_BOVR );
}

/* #[<ENTRY_FUNC>]# eTDR_getDouble64
 * name:         eTDR_getDouble64
 * global_name:  tNBOTDR_eTDR_getDouble64
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_getDouble64(CELLIDX idx, double64_t* out)
{
	ER_UINT		er_sz = E_OK;
    int64_t     val;
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	}

	/* ここに処理本体を記述します #_TEFB_# */
	er_sz = cChannel_receive( (int8_t *)&val, (int16_t)sizeof( val ), VAR_tmo );
	val = ntoh64( val );
    *out = *(double64_t *)&val;
	return  er_sz < 0 ? er_sz : ( er_sz == sizeof( *out ) ? E_OK : E_BOVR );
}

/* #[<ENTRY_FUNC>]# eTDR_putSChar
 * name:         eTDR_putSChar
 * global_name:  tNBOTDR_eTDR_putSChar
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_putSChar(CELLIDX idx, signed char in)
{
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
	return cChannel_send( (int8_t *)&in, (int16_t)sizeof( in ), VAR_tmo );
}

/* #[<ENTRY_FUNC>]# eTDR_putShort
 * name:         eTDR_putShort
 * global_name:  tNBOTDR_eTDR_putShort
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_putShort(CELLIDX idx, short_t in)
{
	CELLCB	*p_cellcb;
	TDR_SHORT	IN = in;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	}

	/* ここに処理本体を記述します #_TEFB_# */
	IN = HTONS( IN, sizeof IN );
	return cChannel_send( (int8_t *)&IN, (int16_t)sizeof( IN ), VAR_tmo );
}

/* #[<ENTRY_FUNC>]# eTDR_putInt
 * name:         eTDR_putInt
 * global_name:  tNBOTDR_eTDR_putInt
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_putInt(CELLIDX idx, int_t in)
{
	CELLCB	*p_cellcb;
	TDR_INT	IN = in;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	}

	/* ここに処理本体を記述します #_TEFB_# */
	IN = HTONI( IN, sizeof IN );
	return cChannel_send( (int8_t *)&IN, (int16_t)sizeof( IN ), VAR_tmo );
}

/* #[<ENTRY_FUNC>]# eTDR_putLong
 * name:         eTDR_putLong
 * global_name:  tNBOTDR_eTDR_putLong
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_putLong(CELLIDX idx, long_t in)
{
	CELLCB	*p_cellcb;
	TDR_LONG	IN = in;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	}

	/* ここに処理本体を記述します #_TEFB_# */
	IN = HTONL( IN, sizeof IN );
	return cChannel_send( (int8_t *)&IN, (int16_t)sizeof( IN ), VAR_tmo );
}

/* #[<ENTRY_FUNC>]# eTDR_getSChar
 * name:         eTDR_getSChar
 * global_name:  tNBOTDR_eTDR_getSChar
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_getSChar(CELLIDX idx, signed char* out)
{
	ER_UINT	 er_sz;
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
	er_sz = cChannel_receive( (int8_t *)out, (int16_t)sizeof( *out ), VAR_tmo );
	return  er_sz < 0 ? er_sz : ( er_sz == sizeof( *out ) ? E_OK : E_BOVR );
}

/* #[<ENTRY_FUNC>]# eTDR_getShort
 * name:         eTDR_getShort
 * global_name:  tNBOTDR_eTDR_getShort
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_getShort(CELLIDX idx, short* out)
{
	ER_UINT	 er_sz;
	CELLCB	*p_cellcb;
	TDR_SHORT	OUT;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	}

	/* ここに処理本体を記述します #_TEFB_# */
	er_sz = cChannel_receive( (int8_t *)&OUT, (int16_t)sizeof( OUT ), VAR_tmo );
	*out = NTOHS( OUT, sizeof OUT );
	return  er_sz < 0 ? er_sz : ( er_sz == sizeof( OUT ) ? E_OK : E_BOVR );
}

/* #[<ENTRY_FUNC>]# eTDR_getInt
 * name:         eTDR_getInt
 * global_name:  tNBOTDR_eTDR_getInt
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_getInt(CELLIDX idx, int_t* out)
{
	ER_UINT		 er_sz;
	CELLCB		*p_cellcb;
	TDR_INT	OUT;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	}

	/* ここに処理本体を記述します #_TEFB_# */
	er_sz = cChannel_receive( (int8_t *)&OUT, (int16_t)sizeof( OUT ), VAR_tmo );
	*out = NTOHI( OUT, sizeof OUT );
	return  er_sz < 0 ? er_sz : ( er_sz == sizeof( OUT ) ? E_OK : E_BOVR );
}

/* #[<ENTRY_FUNC>]# eTDR_getLong
 * name:         eTDR_getLong
 * global_name:  tNBOTDR_eTDR_getLong
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_getLong(CELLIDX idx, long_t* out)
{
	ER_UINT	 er_sz;
	CELLCB	*p_cellcb;
	TDR_LONG	OUT;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	}

	/* ここに処理本体を記述します #_TEFB_# */
	er_sz = cChannel_receive( (int8_t *)&OUT, (int16_t)sizeof( OUT ), VAR_tmo );
	*out = NTOHL( OUT, sizeof OUT );
	return  er_sz < 0 ? er_sz : ( er_sz == sizeof( OUT ) ? E_OK : E_BOVR );
}

/* #[<ENTRY_FUNC>]# eTDR_putUChar
 * name:         eTDR_putUChar
 * global_name:  tNBOTDR_eTDR_putUChar
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_putUChar(CELLIDX idx, unsigned char in)
{
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
	return cChannel_send( (int8_t *)&in, (int16_t)sizeof( in ), VAR_tmo );
}

/* #[<ENTRY_FUNC>]# eTDR_putUShort
 * name:         eTDR_putUShort
 * global_name:  tNBOTDR_eTDR_putUShort
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_putUShort(CELLIDX idx, ushort_t in)
{
	CELLCB	*p_cellcb;
	TDR_USHORT	IN = in;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
	IN = HTONS( IN, sizeof IN );
	return cChannel_send( (int8_t *)&IN, (int16_t)sizeof( IN ), VAR_tmo );
}

/* #[<ENTRY_FUNC>]# eTDR_putUInt
 * name:         eTDR_putUInt
 * global_name:  tNBOTDR_eTDR_putUInt
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_putUInt(CELLIDX idx, uint_t in)
{
	CELLCB		*p_cellcb;
	TDR_UINT	IN = in;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
	IN = HTONI( IN, sizeof IN );
	return cChannel_send( (int8_t *)&IN, (int16_t)sizeof( IN ), VAR_tmo );
}

/* #[<ENTRY_FUNC>]# eTDR_putULong
 * name:         eTDR_putULong
 * global_name:  tNBOTDR_eTDR_putULong
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_putULong(CELLIDX idx, ulong_t in)
{
	CELLCB	*p_cellcb;
	TDR_ULONG	IN = in;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
	IN = HTONL( IN, sizeof IN );
	return cChannel_send( (int8_t *)&IN, (int16_t)sizeof( IN ), VAR_tmo );
}

/* #[<ENTRY_FUNC>]# eTDR_getUChar
 * name:         eTDR_getUChar
 * global_name:  tNBOTDR_eTDR_getUChar
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_getUChar(CELLIDX idx, unsigned char* out)
{
	ER_UINT	 er_sz;
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
	er_sz = cChannel_receive( (int8_t *)out, (int16_t)sizeof( *out ), VAR_tmo );
	return  er_sz < 0 ? er_sz : ( er_sz == sizeof( *out ) ? E_OK : E_BOVR );
}

/* #[<ENTRY_FUNC>]# eTDR_getUShort
 * name:         eTDR_getUShort
 * global_name:  tNBOTDR_eTDR_getUShort
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_getUShort(CELLIDX idx, unsigned short* out)
{
	ER_UINT	 er_sz;
	CELLCB	*p_cellcb;
	TDR_USHORT	OUT;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
	er_sz = cChannel_receive( (int8_t *)&OUT, (int16_t)sizeof( OUT ), VAR_tmo );
	*out = NTOHS( OUT, sizeof OUT );
	return  er_sz < 0 ? er_sz : ( er_sz == sizeof( OUT ) ? E_OK : E_BOVR );
}

/* #[<ENTRY_FUNC>]# eTDR_getUInt
 * name:         eTDR_getUInt
 * global_name:  tNBOTDR_eTDR_getUInt
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_getUInt(CELLIDX idx, uint_t* out)
{
	ER_UINT	 er_sz;
	CELLCB	*p_cellcb;
	TDR_UINT	OUT;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
	er_sz = cChannel_receive( (int8_t *)&OUT, (int16_t)sizeof( OUT ), VAR_tmo );
	*out = NTOHI( OUT, sizeof OUT );
	return  er_sz < 0 ? er_sz : ( er_sz == sizeof( OUT ) ? E_OK : E_BOVR );
}

/* #[<ENTRY_FUNC>]# eTDR_getULong
 * name:         eTDR_getULong
 * global_name:  tNBOTDR_eTDR_getULong
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_getULong(CELLIDX idx, ulong_t* out)
{
	ER_UINT	 er_sz;
	CELLCB	*p_cellcb;
	TDR_ULONG	OUT;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
	er_sz = cChannel_receive( (int8_t *)&OUT, (int16_t)sizeof( OUT ), VAR_tmo );
	*out = NTOHL( OUT, sizeof OUT );
	return  er_sz < 0 ? er_sz : ( er_sz == sizeof( OUT ) ? E_OK : E_BOVR );
}

/* #[<ENTRY_FUNC>]# eTDR_putIntptr
 * name:         eTDR_putIntptr
 * global_name:  tNBOTDR_eTDR_putIntptr
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_putIntptr(CELLIDX idx, const intptr_t ptr)
{
	CELLCB	*p_cellcb;
	TDR_INTPTR	IN = ptr;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	}

	/* ここに処理本体を記述します #_TEFB_# */
	IN = HTONIP( IN, sizeof IN );
	return cChannel_send( (int8_t *)&IN, (int16_t)sizeof( IN ), VAR_tmo );
}

/* #[<ENTRY_FUNC>]# eTDR_getIntptr
 * name:         eTDR_getIntptr
 * global_name:  tNBOTDR_eTDR_getIntptr
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eTDR_getIntptr(CELLIDX idx, intptr_t* ptr)
{
	ER_UINT	 er_sz;
	CELLCB	*p_cellcb;
	TDR_INTPTR	OUT;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	}

	/* ここに処理本体を記述します #_TEFB_# */
	er_sz = cChannel_receive( (int8_t *)&OUT, (int16_t)sizeof( OUT ), VAR_tmo );
	*ptr = NTOHIP( OUT, sizeof OUT );
	return  er_sz < 0 ? er_sz : ( er_sz == sizeof( OUT ) ? E_OK : E_BOVR );
}

/* #[<POSTAMBLE>]#
 *   これより下に非受け口関数を書きます
 * #[</POSTAMBLE>]#*/
