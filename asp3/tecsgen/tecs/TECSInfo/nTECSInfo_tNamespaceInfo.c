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
 *  @(#) $Id: nTECSInfo_tNamespaceInfo.c 2656 2017-06-24 11:57:31Z okuma-top $
 */

/* #[<PREAMBLE>]#
 * #[<...>]# から #[</...>]# で囲まれたコメントは編集しないでください
 * tecsmerge によるマージに使用されます
 *
 * 属性アクセスマクロ #_CAAM_#
 * name             char*            ATTR_name       
 *
 * 呼び口関数 #_TCPF_#
 * call port: cSignatureInfo signature: nTECSInfo_sSignatureInfo context:task optional:true
 *   bool_t     is_cSignatureInfo_joined(int subscript)        check if joined
 *   ER             cSignatureInfo_getName( subscript, char_t* name, int_t max_len );
 *   uint16_t       cSignatureInfo_getNameLength( subscript );
 *   uint32_t       cSignatureInfo_getNFunction( subscript );
 *   ER             cSignatureInfo_getFunctionInfo( subscript, uint32_t ith, Descriptor( nTECSInfo_sFunctionInfo )* desc );
 *       subscript:  0...(NCP_cSignatureInfo-1)
 *   [ref_desc]
 *      Descriptor( nTECSInfo_sSignatureInfo ) cSignatureInfo_refer_to_descriptor( int_t subscript );
 *      Descriptor( nTECSInfo_sSignatureInfo ) cSignatureInfo_ref_desc( int_t subscript )      (same as above; abbreviated version);
 * call port: cCelltypeInfo signature: nTECSInfo_sCelltypeInfo context:task optional:true
 *   bool_t     is_cCelltypeInfo_joined(int subscript)        check if joined
 *   ER             cCelltypeInfo_getName( subscript, char_t* name, int_t max_len );
 *   uint16_t       cCelltypeInfo_getNameLength( subscript );
 *   uint32_t       cCelltypeInfo_getNAttr( subscript );
 *   ER             cCelltypeInfo_getAttrInfo( subscript, uint32_t ith, Descriptor( nTECSInfo_sVarDeclInfo )* desc );
 *   uint32_t       cCelltypeInfo_getNVar( subscript );
 *   ER             cCelltypeInfo_getVarInfo( subscript, uint32_t ith, Descriptor( nTECSInfo_sVarDeclInfo )* desc );
 *   uint32_t       cCelltypeInfo_getNCall( subscript );
 *   ER             cCelltypeInfo_getCallInfo( subscript, uint32_t ith, Descriptor( nTECSInfo_sCallInfo )* desc );
 *   uint32_t       cCelltypeInfo_getNEntry( subscript );
 *   ER             cCelltypeInfo_getEntryInfo( subscript, uint32_t ith, Descriptor( nTECSInfo_sEntryInfo )* desc );
 *   bool_t         cCelltypeInfo_isSingleton( subscript );
 *   bool_t         cCelltypeInfo_isIDX_is_ID( subscript );
 *   uint32_t       cCelltypeInfo_sizeOfCB( subscript );
 *   uint32_t       cCelltypeInfo_sizeOfINIB( subscript );
 *       subscript:  0...(NCP_cCelltypeInfo-1)
 *   [ref_desc]
 *      Descriptor( nTECSInfo_sCelltypeInfo ) cCelltypeInfo_refer_to_descriptor( int_t subscript );
 *      Descriptor( nTECSInfo_sCelltypeInfo ) cCelltypeInfo_ref_desc( int_t subscript )      (same as above; abbreviated version);
 * call port: cNamespaceInfo signature: nTECSInfo_sNamespaceInfo context:task optional:true
 *   bool_t     is_cNamespaceInfo_joined(int subscript)        check if joined
 *   ER             cNamespaceInfo_getName( subscript, char_t* name, int_t max_len );
 *   uint16_t       cNamespaceInfo_getNameLength( subscript );
 *   uint32_t       cNamespaceInfo_getNNamespace( subscript );
 *   ER             cNamespaceInfo_getNamespaceInfo( subscript, uint32_t ith, Descriptor( nTECSInfo_sNamespaceInfo )* des );
 *   uint32_t       cNamespaceInfo_getNSignature( subscript );
 *   ER             cNamespaceInfo_getSignatureInfo( subscript, uint32_t ith, Descriptor( nTECSInfo_sSignatureInfo )* des );
 *   uint32_t       cNamespaceInfo_getNCelltype( subscript );
 *   ER             cNamespaceInfo_getCelltypeInfo( subscript, uint32_t ith, Descriptor( nTECSInfo_sCelltypeInfo )* des );
 *       subscript:  0...(NCP_cNamespaceInfo-1)
 *   [ref_desc]
 *      Descriptor( nTECSInfo_sNamespaceInfo ) cNamespaceInfo_refer_to_descriptor( int_t subscript );
 *      Descriptor( nTECSInfo_sNamespaceInfo ) cNamespaceInfo_ref_desc( int_t subscript )      (same as above; abbreviated version);
 *
 * #[</PREAMBLE>]# */

/* Put prototype declaration and/or variale definition here #_PAC_# */
#include <string.h>
#include "nTECSInfo_tNamespaceInfo_tecsgen.h"

#ifndef E_OK
#define	E_OK	0		/* success */
#define	E_ID	(-18)	/* illegal ID */
#endif

/* entry port function #_TEPF_# */
/* #[<ENTRY_PORT>]# eNamespaceInfo
 * entry port: eNamespaceInfo
 * signature:  nTECSInfo_sNamespaceInfo
 * context:    task
 * #[</ENTRY_PORT>]# */

/* #[<ENTRY_FUNC>]# eNamespaceInfo_getName
 * name:         eNamespaceInfo_getName
 * global_name:  nTECSInfo_tNamespaceInfo_eNamespaceInfo_getName
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eNamespaceInfo_getName(CELLIDX idx, char_t* name, int_t max_len)
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

/* #[<ENTRY_FUNC>]# eNamespaceInfo_getNameLength
 * name:         eNamespaceInfo_getNameLength
 * global_name:  nTECSInfo_tNamespaceInfo_eNamespaceInfo_getNameLength
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
uint16_t
eNamespaceInfo_getNameLength(CELLIDX idx)
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

/* #[<ENTRY_FUNC>]# eNamespaceInfo_getNNamespace
 * name:         eNamespaceInfo_getNNamespace
 * global_name:  nTECSInfo_tNamespaceInfo_eNamespaceInfo_getNNamespace
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
uint32_t
eNamespaceInfo_getNNamespace(CELLIDX idx)
{
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		/* Write error processing code here */
	} /* end if VALID_IDX(idx) */

	/* Put statements here #_TEFB_# */
  return NCP_cNamespaceInfo;
}

/* #[<ENTRY_FUNC>]# eNamespaceInfo_getNamespaceInfo
 * name:         eNamespaceInfo_getNamespaceInfo
 * global_name:  nTECSInfo_tNamespaceInfo_eNamespaceInfo_getNamespaceInfo
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eNamespaceInfo_getNamespaceInfo(CELLIDX idx, uint32_t ith, Descriptor( nTECSInfo_sNamespaceInfo )* des)
{
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		/* Write error processing code here */
	} /* end if VALID_IDX(idx) */

	/* Put statements here #_TEFB_# */
  *des = cNamespaceInfo_refer_to_descriptor( ith );
  return E_OK;
}

/* #[<ENTRY_FUNC>]# eNamespaceInfo_getNSignature
 * name:         eNamespaceInfo_getNSignature
 * global_name:  nTECSInfo_tNamespaceInfo_eNamespaceInfo_getNSignature
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
uint32_t
eNamespaceInfo_getNSignature(CELLIDX idx)
{
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		/* Write error processing code here */
	} /* end if VALID_IDX(idx) */

	/* Put statements here #_TEFB_# */
  return NCP_cSignatureInfo;
}

/* #[<ENTRY_FUNC>]# eNamespaceInfo_getSignatureInfo
 * name:         eNamespaceInfo_getSignatureInfo
 * global_name:  nTECSInfo_tNamespaceInfo_eNamespaceInfo_getSignatureInfo
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eNamespaceInfo_getSignatureInfo(CELLIDX idx, uint32_t ith, Descriptor( nTECSInfo_sSignatureInfo )* des)
{
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		/* Write error processing code here */
	} /* end if VALID_IDX(idx) */

	/* Put statements here #_TEFB_# */
	*des = cSignatureInfo_refer_to_descriptor( ith );
  return E_OK;
}

/* #[<ENTRY_FUNC>]# eNamespaceInfo_getNCelltype
 * name:         eNamespaceInfo_getNCelltype
 * global_name:  nTECSInfo_tNamespaceInfo_eNamespaceInfo_getNCelltype
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
uint32_t
eNamespaceInfo_getNCelltype(CELLIDX idx)
{
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		/* Write error processing code here */
	} /* end if VALID_IDX(idx) */

	/* Put statements here #_TEFB_# */
  return NCP_cCelltypeInfo;
}

/* #[<ENTRY_FUNC>]# eNamespaceInfo_getCelltypeInfo
 * name:         eNamespaceInfo_getCelltypeInfo
 * global_name:  nTECSInfo_tNamespaceInfo_eNamespaceInfo_getCelltypeInfo
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eNamespaceInfo_getCelltypeInfo(CELLIDX idx, uint32_t ith, Descriptor( nTECSInfo_sCelltypeInfo )* des)
{
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		/* Write error processing code here */
	} /* end if VALID_IDX(idx) */

	/* Put statements here #_TEFB_# */
	*des = cCelltypeInfo_refer_to_descriptor( ith );

  return E_OK;
}

/* #[<POSTAMBLE>]#
 *   これより下に非受け口関数を書きます
 * #[</POSTAMBLE>]#*/
