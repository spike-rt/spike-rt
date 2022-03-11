/*
 *   Copyright (C) 2008-2017 by TOPPERS Project
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
 *  @(#) $Id: nTECSInfo_tFunctionInfo.c 2665 2017-07-24 08:59:28Z okuma-top $
 */

/* #[<PREAMBLE>]#
 * #[<...>]# から #[</...>]# で囲まれたコメントは編集しないでください
 * tecsmerge によるマージに使用されます
 *
 * 属性アクセスマクロ #_CAAM_#
 * name             char_t*          ATTR_name       
 * bOneway          bool_t           ATTR_bOneway    
 *
 * 呼び口関数 #_TCPF_#
 * call port: cParamInfo signature: nTECSInfo_sParamInfo context:task optional:true
 *   bool_t     is_cParamInfo_joined(int subscript)        check if joined
 *   ER             cParamInfo_getName( subscript, char_t* name, int_t max_len );
 *   uint16_t       cParamInfo_getNameLength( subscript );
 *   ER             cParamInfo_getTypeInfo( subscript, Descriptor( nTECSInfo_sTypeInfo )* desc );
 *       subscript:  0...(NCP_cParamInfo-1)
 *   [ref_desc]
 *      Descriptor( nTECSInfo_sParamInfo ) cParamInfo_refer_to_descriptor( int_t subscript );
 *      Descriptor( nTECSInfo_sParamInfo ) cParamInfo_ref_desc( int_t subscript )      (same as above; abbreviated version);
 * call port: cReturnTypeInfo signature: nTECSInfo_sTypeInfo context:task
 *   ER             cReturnTypeInfo_getName( char_t* name, int_t max_len );
 *   uint16_t       cReturnTypeInfo_getNameLength( );
 *   uint32_t       cReturnTypeInfo_getSize( );
 *   int8_t         cReturnTypeInfo_getKind( );
 *   uint32_t       cReturnTypeInfo_getNType( );
 *   ER             cReturnTypeInfo_getTypeInfo( Descriptor( nTECSInfo_sTypeInfo )* desc );
 *   uint32_t       cReturnTypeInfo_getNMember( );
 *   ER             cReturnTypeInfo_getMemberInfo( uint32_t ith, Descriptor( nTECSInfo_sVarDeclInfo )* desc );
 *   [ref_desc]
 *      Descriptor( nTECSInfo_sTypeInfo ) cReturnTypeInfo_refer_to_descriptor();
 *      Descriptor( nTECSInfo_sTypeInfo ) cReturnTypeInfo_ref_desc()      (same as above; abbreviated version);
 *
 * #[</PREAMBLE>]# */

/* Put prototype declaration and/or variale definition here #_PAC_# */
#include <string.h>
#include "nTECSInfo_tFunctionInfo_tecsgen.h"

#ifndef E_OK
#define	E_OK	0		/* success */
#define	E_ID	(-18)	/* illegal ID */
#endif

/* entry port function #_TEPF_# */
/* #[<ENTRY_PORT>]# eFunctionInfo
 * entry port: eFunctionInfo
 * signature:  nTECSInfo_sFunctionInfo
 * context:    task
 * #[</ENTRY_PORT>]# */

/* #[<ENTRY_FUNC>]# eFunctionInfo_getName
 * name:         eFunctionInfo_getName
 * global_name:  nTECSInfo_tFunctionInfo_eFunctionInfo_getName
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eFunctionInfo_getName(CELLIDX idx, char_t* name, int_t max_len)
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
	name[ max_len - 1 ] = '\0';
	strncpy( name, ATTR_name, max_len );
  if( name[ max_len - 1 ] ){
      name[ max_len - 1 ] = '\0';
      ercd = E_NOMEM;
  }

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eFunctionInfo_getNameLength
 * name:         eFunctionInfo_getNameLength
 * global_name:  nTECSInfo_tFunctionInfo_eFunctionInfo_getNameLength
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
uint16_t
eFunctionInfo_getNameLength(CELLIDX idx)
{
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		/* Write error processing code here */
	} /* end if VALID_IDX(idx) */

	/* Put statements here #_TEFB_# */
	return strlen( ATTR_name ) + 1;
}

/* #[<ENTRY_FUNC>]# eFunctionInfo_getReturnTypeInfo
 * name:         eFunctionInfo_getReturnTypeInfo
 * global_name:  nTECSInfo_tFunctionInfo_eFunctionInfo_getReturnTypeInfo
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
void
eFunctionInfo_getReturnTypeInfo(CELLIDX idx, Descriptor( nTECSInfo_sTypeInfo )* desc)
{
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		/* Write error processing code here */
	} /* end if VALID_IDX(idx) */

	/* Put statements here #_TEFB_# */
  *desc = cReturnTypeInfo_refer_to_descriptor();
}

/* #[<ENTRY_FUNC>]# eFunctionInfo_getNParam
 * name:         eFunctionInfo_getNParam
 * global_name:  nTECSInfo_tFunctionInfo_eFunctionInfo_getNParam
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
uint32_t
eFunctionInfo_getNParam(CELLIDX idx)
{
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		/* Write error processing code here */
	} /* end if VALID_IDX(idx) */

	/* Put statements here #_TEFB_# */
  return NCP_cParamInfo;
}

/* #[<ENTRY_FUNC>]# eFunctionInfo_getParamInfo
 * name:         eFunctionInfo_getParamInfo
 * global_name:  nTECSInfo_tFunctionInfo_eFunctionInfo_getParamInfo
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eFunctionInfo_getParamInfo(CELLIDX idx, uint32_t ith, Descriptor( nTECSInfo_sParamInfo )* param)
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
  if( ith >= NCP_cParamInfo )
      ercd = E_NOEXS;
  else
      *param = cParamInfo_refer_to_descriptor( ith );
	return(ercd);
}

/* #[<POSTAMBLE>]#
 *   これより下に非受け口関数を書きます
 * #[</POSTAMBLE>]#*/
