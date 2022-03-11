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
 *  @(#) $Id: nTECSInfo_tCellInfo.c 2656 2017-06-24 11:57:31Z okuma-top $
 */

/* #[<PREAMBLE>]#
 * #[<...>]# から #[</...>]# で囲まれたコメントは編集しないでください
 * tecsmerge によるマージに使用されます
 *
 * 属性アクセスマクロ #_CAAM_#
 * name             char_t*          ATTR_name       
 * cbp              void*            ATTR_cbp        
 * inibp            void*            ATTR_inibp      
 *
 * 呼び口関数 #_TCPF_#
 * call port: cCelltypeInfo signature: nTECSInfo_sCelltypeInfo context:task
 *   ER             cCelltypeInfo_getName( char_t* name, int_t max_len );
 *   uint16_t       cCelltypeInfo_getNameLength( );
 *   uint32_t       cCelltypeInfo_getNAttr( );
 *   ER             cCelltypeInfo_getAttrInfo( uint32_t ith, Descriptor( nTECSInfo_sVarDeclInfo )* desc );
 *   uint32_t       cCelltypeInfo_getNVar( );
 *   ER             cCelltypeInfo_getVarInfo( uint32_t ith, Descriptor( nTECSInfo_sVarDeclInfo )* desc );
 *   uint32_t       cCelltypeInfo_getNCall( );
 *   ER             cCelltypeInfo_getCallInfo( uint32_t ith, Descriptor( nTECSInfo_sCallInfo )* desc );
 *   uint32_t       cCelltypeInfo_getNEntry( );
 *   ER             cCelltypeInfo_getEntryInfo( uint32_t ith, Descriptor( nTECSInfo_sEntryInfo )* desc );
 *   bool_t         cCelltypeInfo_isSingleton( );
 *   bool_t         cCelltypeInfo_isIDX_is_ID( );
 *   uint32_t       cCelltypeInfo_sizeOfCB( );
 *   uint32_t       cCelltypeInfo_sizeOfINIB( );
 *   [ref_desc]
 *      Descriptor( nTECSInfo_sCelltypeInfo ) cCelltypeInfo_refer_to_descriptor();
 *      Descriptor( nTECSInfo_sCelltypeInfo ) cCelltypeInfo_ref_desc()      (same as above; abbreviated version);
 * call port: cRawEntryDescriptor signature: nTECSInfo_sRawEntryDescriptorInfo context:task optional:true
 *   bool_t     is_cRawEntryDescriptor_joined(int subscript)        check if joined
 *   uint16_t       cRawEntryDescriptor_getNRawEntryDescriptorInfo( subscript );
 *   ER             cRawEntryDescriptor_getRawDescriptor( subscript, const char_t* signatureName, int_t ith, void** rawDesc );
 *       subscript:  0...(NCP_cRawEntryDescriptor-1)
 *   [ref_desc]
 *      Descriptor( nTECSInfo_sRawEntryDescriptorInfo ) cRawEntryDescriptor_refer_to_descriptor( int_t subscript );
 *      Descriptor( nTECSInfo_sRawEntryDescriptorInfo ) cRawEntryDescriptor_ref_desc( int_t subscript )      (same as above; abbreviated version);
 *
 * #[</PREAMBLE>]# */

/* Put prototype declaration and/or variale definition here #_PAC_# */
#include <string.h>
#include "nTECSInfo_tCellInfo_tecsgen.h"

#ifndef E_OK
#define	E_OK	0		/* success */
#define	E_ID	(-18)	/* illegal ID */
#endif

#ifndef DBG_SYSLOG
#define DBG_SYSLOG( x )
// #define DBG_SYSLOG( x ) syslog x
#endif

/* entry port function #_TEPF_# */
/* #[<ENTRY_PORT>]# eCellInfo
 * entry port: eCellInfo
 * signature:  nTECSInfo_sCellInfo
 * context:    task
 * #[</ENTRY_PORT>]# */

/* #[<ENTRY_FUNC>]# eCellInfo_getName
 * name:         eCellInfo_getName
 * global_name:  nTECSInfo_tCellInfo_eCellInfo_getName
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eCellInfo_getName(CELLIDX idx, char_t* name, int_t max_len)
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

/* #[<ENTRY_FUNC>]# eCellInfo_getNameLength
 * name:         eCellInfo_getNameLength
 * global_name:  nTECSInfo_tCellInfo_eCellInfo_getNameLength
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
uint16_t
eCellInfo_getNameLength(CELLIDX idx)
{
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		/* エラー処理コードをここに記述します */
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
	return strlen( ATTR_name ) + 1;
}

/* #[<ENTRY_FUNC>]# eCellInfo_getNRawEntryDescriptorInfo
 * name:         eCellInfo_getNRawEntryDescriptorInfo
 * global_name:  nTECSInfo_tCellInfo_eCellInfo_getNRawEntryDescriptorInfo
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
uint32_t
eCellInfo_getNRawEntryDescriptorInfo(CELLIDX idx)
{
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		/* エラー処理コードをここに記述します */
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
  return NCP_cRawEntryDescriptor;
}

/* #[<ENTRY_FUNC>]# eCellInfo_getRawEntryDescriptorInfo
 * name:         eCellInfo_getRawEntryDescriptorInfo
 * global_name:  nTECSInfo_tCellInfo_eCellInfo_getRawEntryDescriptorInfo
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eCellInfo_getRawEntryDescriptorInfo(CELLIDX idx, int_t index, Descriptor( nTECSInfo_sRawEntryDescriptorInfo )* desc)
{
	CELLCB	*p_cellcb;
  ER      ercd = E_OK;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		/* エラー処理コードをここに記述します */
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
  if( 0 <= index && index < NCP_cRawEntryDescriptor ){
      DBG_SYSLOG(( LOG_INFO, "tCellInfo: index=%d", index ));
      if( is_cRawEntryDescriptor_joined( index ) )
          *desc = cRawEntryDescriptor_refer_to_descriptor( index );
      else
          ercd = E_NOEXS;
  }
  else
      ercd = E_ID;

  return ercd;
}

/* #[<ENTRY_FUNC>]# eCellInfo_getCelltypeInfo
 * name:         eCellInfo_getCelltypeInfo
 * global_name:  nTECSInfo_tCellInfo_eCellInfo_getCelltypeInfo
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
void
eCellInfo_getCelltypeInfo(CELLIDX idx, Descriptor( nTECSInfo_sCelltypeInfo )* desc)
{
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		/* Write error processing code here */
	} /* end if VALID_IDX(idx) */

	/* Put statements here #_TEFB_# */
  *desc = cCelltypeInfo_refer_to_descriptor();
}

/* #[<ENTRY_FUNC>]# eCellInfo_getCBP
 * name:         eCellInfo_getCBP
 * global_name:  nTECSInfo_tCellInfo_eCellInfo_getCBP
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
void
eCellInfo_getCBP(CELLIDX idx, void** cbp)
{
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		/* Write error processing code here */
	} /* end if VALID_IDX(idx) */

	/* Put statements here #_TEFB_# */
  *cbp = ATTR_cbp;
}

/* #[<ENTRY_FUNC>]# eCellInfo_getINIBP
 * name:         eCellInfo_getINIBP
 * global_name:  nTECSInfo_tCellInfo_eCellInfo_getINIBP
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
void
eCellInfo_getINIBP(CELLIDX idx, void** inibp)
{
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		/* Write error processing code here */
	} /* end if VALID_IDX(idx) */

	/* Put statements here #_TEFB_# */
  *inibp = ATTR_inibp;
}

/* #[<POSTAMBLE>]#
 *   これより下に非受け口関数を書きます
 * #[</POSTAMBLE>]#*/
