/* #[<PREAMBLE>]#
 * #[<...>]# から #[</...>]# で囲まれたコメントは編集しないでください
 * tecsmerge によるマージに使用されます
 *
 * 属性アクセスマクロ #_CAAM_#
 * NAME_LEN         int16_t          ATTR_NAME_LEN   
 * name             char_t*          VAR_name        
 * name2            char_t*          VAR_name2       
 * selectedCbp      int8_t*          VAR_selectedCbp 
 * selectedInibp    int8_t*          VAR_selectedInibp
 *
 * 呼び口関数 #_TCPF_#
 * call port: cTECSInfo signature: nTECSInfo_sTECSInfo context:task
 *   ER             cTECSInfo_findNamespace( const char_t* namespace_path, Descriptor( nTECSInfo_sNamespaceInfo )* NSdesc );
 *   ER             cTECSInfo_findRegion( const char_t* namespace_path, Descriptor( nTECSInfo_sRegionInfo )* RGNdesc );
 *   ER             cTECSInfo_findSignature( const char_t* namespace_path, Descriptor( nTECSInfo_sSignatureInfo )* SIGdesc );
 *   ER             cTECSInfo_findCelltype( const char_t* namespace_path, Descriptor( nTECSInfo_sCelltypeInfo )* CTdesc );
 *   ER             cTECSInfo_findCell( const char_t* namespace_path, Descriptor( nTECSInfo_sCellInfo )* CELLdesc );
 * call port: cNSInfo signature: nTECSInfo_sNamespaceInfo context:task optional:true
 *   bool_t     is_cNSInfo_joined()                     check if joined
 *   ER             cNSInfo_getName( char_t* name, int_t max_len );
 *   uint16_t       cNSInfo_getNameLength( );
 *   uint32_t       cNSInfo_getNNamespace( );
 *   ER             cNSInfo_getNamespaceInfo( uint32_t ith, Descriptor( nTECSInfo_sNamespaceInfo )* des );
 *   uint32_t       cNSInfo_getNSignature( );
 *   ER             cNSInfo_getSignatureInfo( uint32_t ith, Descriptor( nTECSInfo_sSignatureInfo )* des );
 *   uint32_t       cNSInfo_getNCelltype( );
 *   ER             cNSInfo_getCelltypeInfo( uint32_t ith, Descriptor( nTECSInfo_sCelltypeInfo )* des );
 *   [dynamic, optional]
 *      void           cNSInfo_set_descriptor( Descriptor( nTECSInfo_sNamespaceInfo ) desc );
 *      void           cNSInfo_unjoin(  );
 * call port: cCelltypeInfo signature: nTECSInfo_sCelltypeInfo context:task optional:true
 *   bool_t     is_cCelltypeInfo_joined()                     check if joined
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
 *   [dynamic, optional]
 *      void           cCelltypeInfo_set_descriptor( Descriptor( nTECSInfo_sCelltypeInfo ) desc );
 *      void           cCelltypeInfo_unjoin(  );
 * call port: cSignatureInfo signature: nTECSInfo_sSignatureInfo context:task optional:true
 *   bool_t     is_cSignatureInfo_joined()                     check if joined
 *   ER             cSignatureInfo_getName( char_t* name, int_t max_len );
 *   uint16_t       cSignatureInfo_getNameLength( );
 *   uint32_t       cSignatureInfo_getNFunction( );
 *   ER             cSignatureInfo_getFunctionInfo( uint32_t ith, Descriptor( nTECSInfo_sFunctionInfo )* desc );
 *   [dynamic, optional]
 *      void           cSignatureInfo_set_descriptor( Descriptor( nTECSInfo_sSignatureInfo ) desc );
 *      void           cSignatureInfo_unjoin(  );
 * call port: cFunctionInfo signature: nTECSInfo_sFunctionInfo context:task optional:true
 *   bool_t     is_cFunctionInfo_joined()                     check if joined
 *   ER             cFunctionInfo_getName( char_t* name, int_t max_len );
 *   uint16_t       cFunctionInfo_getNameLength( );
 *   void           cFunctionInfo_getReturnTypeInfo( Descriptor( nTECSInfo_sTypeInfo )* desc );
 *   uint32_t       cFunctionInfo_getNParam( );
 *   ER             cFunctionInfo_getParamInfo( uint32_t ith, Descriptor( nTECSInfo_sParamInfo )* param );
 *   [dynamic, optional]
 *      void           cFunctionInfo_set_descriptor( Descriptor( nTECSInfo_sFunctionInfo ) desc );
 *      void           cFunctionInfo_unjoin(  );
 * call port: cParamInfo signature: nTECSInfo_sParamInfo context:task optional:true
 *   bool_t     is_cParamInfo_joined()                     check if joined
 *   ER             cParamInfo_getName( char_t* name, int_t max_len );
 *   uint16_t       cParamInfo_getNameLength( );
 *   ER             cParamInfo_getTypeInfo( Descriptor( nTECSInfo_sTypeInfo )* desc );
 *   ER             cParamInfo_getDir( int8_t* dir );
 *   [dynamic, optional]
 *      void           cParamInfo_set_descriptor( Descriptor( nTECSInfo_sParamInfo ) desc );
 *      void           cParamInfo_unjoin(  );
 * call port: cCallInfo signature: nTECSInfo_sCallInfo context:task optional:true
 *   bool_t     is_cCallInfo_joined()                     check if joined
 *   ER             cCallInfo_getName( char_t* name, int_t max_len );
 *   uint16_t       cCallInfo_getNameLength( );
 *   void           cCallInfo_getSignatureInfo( Descriptor( nTECSInfo_sSignatureInfo )* desc );
 *   uint32_t       cCallInfo_getArraySize( );
 *   void           cCallInfo_getSpecifierInfo( bool_t* b_optional, bool_t* b_dynamic, bool_t* b_ref_desc, bool_t* b_omit );
 *   void           cCallInfo_getInternalInfo( bool_t* b_allocator_port, bool_t* b_require_port );
 *   void           cCallInfo_getLocationInfo( uint32_t* offset, int8_t* place );
 *   void           cCallInfo_getOptimizeInfo( bool_t* b_VMT_useless, bool_t* b_skelton_useless, bool_t* b_cell_unique );
 *   [dynamic, optional]
 *      void           cCallInfo_set_descriptor( Descriptor( nTECSInfo_sCallInfo ) desc );
 *      void           cCallInfo_unjoin(  );
 * call port: cEntryInfo signature: nTECSInfo_sEntryInfo context:task optional:true
 *   bool_t     is_cEntryInfo_joined()                     check if joined
 *   ER             cEntryInfo_getName( char_t* name, int_t max_len );
 *   uint16_t       cEntryInfo_getNameLength( );
 *   void           cEntryInfo_getSignatureInfo( Descriptor( nTECSInfo_sSignatureInfo )* desc );
 *   uint32_t       cEntryInfo_getArraySize( );
 *   bool_t         cEntryInfo_isInline( );
 *   [dynamic, optional]
 *      void           cEntryInfo_set_descriptor( Descriptor( nTECSInfo_sEntryInfo ) desc );
 *      void           cEntryInfo_unjoin(  );
 * call port: cAttrInfo signature: nTECSInfo_sVarDeclInfo context:task optional:true
 *   bool_t     is_cAttrInfo_joined()                     check if joined
 *   ER             cAttrInfo_getName( char_t* name, int_t max_len );
 *   uint16_t       cAttrInfo_getNameLength( );
 *   void           cAttrInfo_getLocationInfo( uint32_t* offset, int8_t* place );
 *   void           cAttrInfo_getTypeInfo( Descriptor( nTECSInfo_sTypeInfo )* desc );
 *   void           cAttrInfo_getSizeIsExpr( char_t* expr_str, int32_t max_len );
 *   ER             cAttrInfo_getSizeIs( uint32_t* size, const void* p_cb );
 *   [dynamic, optional]
 *      void           cAttrInfo_set_descriptor( Descriptor( nTECSInfo_sVarDeclInfo ) desc );
 *      void           cAttrInfo_unjoin(  );
 * call port: cVarInfo signature: nTECSInfo_sVarDeclInfo context:task optional:true
 *   bool_t     is_cVarInfo_joined()                     check if joined
 *   ER             cVarInfo_getName( char_t* name, int_t max_len );
 *   uint16_t       cVarInfo_getNameLength( );
 *   void           cVarInfo_getLocationInfo( uint32_t* offset, int8_t* place );
 *   void           cVarInfo_getTypeInfo( Descriptor( nTECSInfo_sTypeInfo )* desc );
 *   void           cVarInfo_getSizeIsExpr( char_t* expr_str, int32_t max_len );
 *   ER             cVarInfo_getSizeIs( uint32_t* size, const void* p_cb );
 *   [dynamic, optional]
 *      void           cVarInfo_set_descriptor( Descriptor( nTECSInfo_sVarDeclInfo ) desc );
 *      void           cVarInfo_unjoin(  );
 * call port: cTypeInfo signature: nTECSInfo_sTypeInfo context:task optional:true
 *   bool_t     is_cTypeInfo_joined()                     check if joined
 *   ER             cTypeInfo_getName( char_t* name, int_t max_len );
 *   uint16_t       cTypeInfo_getNameLength( );
 *   uint32_t       cTypeInfo_getSize( );
 *   int8_t         cTypeInfo_getKind( );
 *   uint32_t       cTypeInfo_getNType( );
 *   ER             cTypeInfo_getTypeInfo( Descriptor( nTECSInfo_sTypeInfo )* desc );
 *   uint32_t       cTypeInfo_getNMember( );
 *   ER             cTypeInfo_getMemberInfo( uint32_t ith, Descriptor( nTECSInfo_sVarDeclInfo )* desc );
 *   [dynamic, optional]
 *      void           cTypeInfo_set_descriptor( Descriptor( nTECSInfo_sTypeInfo ) desc );
 *      void           cTypeInfo_unjoin(  );
 * call port: cRegionInfo signature: nTECSInfo_sRegionInfo context:task optional:true
 *   bool_t     is_cRegionInfo_joined()                     check if joined
 *   ER             cRegionInfo_getName( char_t* name, int_t max_len );
 *   uint16_t       cRegionInfo_getNameLength( );
 *   uint32_t       cRegionInfo_getNCell( );
 *   ER             cRegionInfo_getCellInfo( uint32_t ith, Descriptor( nTECSInfo_sCellInfo )* des );
 *   uint32_t       cRegionInfo_getNRegion( );
 *   ER             cRegionInfo_getRegionInfo( uint32_t ith, Descriptor( nTECSInfo_sRegionInfo )* des );
 *   [dynamic, optional]
 *      void           cRegionInfo_set_descriptor( Descriptor( nTECSInfo_sRegionInfo ) desc );
 *      void           cRegionInfo_unjoin(  );
 * call port: cCellInfo signature: nTECSInfo_sCellInfo context:task optional:true
 *   bool_t     is_cCellInfo_joined()                     check if joined
 *   ER             cCellInfo_getName( char_t* name, int_t max_len );
 *   uint16_t       cCellInfo_getNameLength( );
 *   void           cCellInfo_getCelltypeInfo( Descriptor( nTECSInfo_sCelltypeInfo )* desc );
 *   void           cCellInfo_getCBP( void** cbp );
 *   void           cCellInfo_getINIBP( void** inibp );
 *   [dynamic, optional]
 *      void           cCellInfo_set_descriptor( Descriptor( nTECSInfo_sCellInfo ) desc );
 *      void           cCellInfo_unjoin(  );
 *
 * #[</PREAMBLE>]# */

/* プロトタイプ宣言や変数の定義をここに書きます #_PAC_# */
#include "tTECSInfoAccessor_tecsgen.h"

#ifndef E_OK
#define	E_OK	0		/* success */
#define	E_ID	(-18)	/* illegal ID */
#endif

static void
getSelectedTypeValue( CELLCB *p_cellcb, void *ptr, char_t *buf, int_t max_len );

/* 受け口関数 #_TEPF_# */
/* #[<ENTRY_PORT>]# eSelector
 * entry port: eSelector
 * signature:  sAccessor
 * context:    task
 * #[</ENTRY_PORT>]# */

/* #[<ENTRY_FUNC>]# eSelector_selectNamespaceInfoByName
 * name:         eSelector_selectNamespaceInfoByName
 * global_name:  tTECSInfoAccessor_eSelector_selectNamespaceInfoByName
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSelector_selectNamespaceInfoByName(CELLIDX idx, const char_t* namespacePath)
{
	ER		ercd = E_OK;
	CELLCB	*p_cellcb;
  Descriptor( nTECSInfo_sNamespaceInfo ) NSdesc;
  if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
  ercd = cTECSInfo_findNamespace( namespacePath, &NSdesc );
  if( ercd == E_OK ){
      cNSInfo_set_descriptor( NSdesc );
  }
  else {
      cNSInfo_unjoin(  );
  }

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eSelector_selectCelltypeInfoByName
 * name:         eSelector_selectCelltypeInfoByName
 * global_name:  tTECSInfoAccessor_eSelector_selectCelltypeInfoByName
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSelector_selectCelltypeInfoByName(CELLIDX idx, const char_t* namespacePath)
{
	ER		ercd = E_OK;
	CELLCB	*p_cellcb;
  Descriptor( nTECSInfo_sCelltypeInfo ) CTdesc;
  if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
  ercd = cTECSInfo_findCelltype( namespacePath, &CTdesc );
  if( ercd == E_OK ){
      cCelltypeInfo_set_descriptor( CTdesc );
  }
  else {
      cCelltypeInfo_unjoin(  );
  }
  
	return(ercd);
}

/* #[<ENTRY_FUNC>]# eSelector_selectSignatureInfoByName
 * name:         eSelector_selectSignatureInfoByName
 * global_name:  tTECSInfoAccessor_eSelector_selectSignatureInfoByName
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSelector_selectSignatureInfoByName(CELLIDX idx, const char_t* namespacePath)
{
	ER		ercd = E_OK;
	CELLCB	*p_cellcb;
  Descriptor( nTECSInfo_sSignatureInfo ) SIGdesc;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
  ercd = cTECSInfo_findSignature( namespacePath, &SIGdesc );
  if( ercd == E_OK ){
      cSignatureInfo_set_descriptor( SIGdesc );
  }
  else {
      cSignatureInfo_unjoin(  );
  }
	return(ercd);
}

/* #[<ENTRY_FUNC>]# eSelector_selectRegionInfoByName
 * name:         eSelector_selectRegionInfoByName
 * global_name:  tTECSInfoAccessor_eSelector_selectRegionInfoByName
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSelector_selectRegionInfoByName(CELLIDX idx, const char_t* regionPath)
{
	ER		ercd = E_OK;
	CELLCB	*p_cellcb;
  Descriptor( nTECSInfo_sRegionInfo ) RGNdesc;
  if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
  ercd = cTECSInfo_findRegion( regionPath, &RGNdesc );
  if( ercd == E_OK ){
      cRegionInfo_set_descriptor( RGNdesc );
  }
  else {
      cRegionInfo_unjoin(  );
  }
	return(ercd);
}

/* #[<ENTRY_FUNC>]# eSelector_selectCellInfoByName
 * name:         eSelector_selectCellInfoByName
 * global_name:  tTECSInfoAccessor_eSelector_selectCellInfoByName
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSelector_selectCellInfoByName(CELLIDX idx, const char_t* cellPath)
{
	ER		ercd = E_OK;
	CELLCB	*p_cellcb;
  Descriptor( nTECSInfo_sCellInfo ) desc;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
  ercd = cTECSInfo_findCell( cellPath, &desc );
  if( ercd == E_OK ){
      cCellInfo_set_descriptor( desc );
  }
  else {
      cCellInfo_unjoin(  );
  }

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eSelector_getSelectedNamespaceInfo
 * name:         eSelector_getSelectedNamespaceInfo
 * global_name:  tTECSInfoAccessor_eSelector_getSelectedNamespaceInfo
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSelector_getSelectedNamespaceInfo(CELLIDX idx, char_t* name, int_t max_len, int_t* num_namespace, int_t* num_celltype, int_t* num_signature)
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
  if( is_cNSInfo_joined() ){
      cNSInfo_getName( name, max_len );
      *num_namespace = cNSInfo_getNNamespace( );
      *num_signature = cNSInfo_getNSignature( );
      *num_celltype  = cNSInfo_getNCelltype( );
  }
  else {
      ercd = E_OBJ;      // cNSInfo not joined
  }

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eSelector_selectCelltypeInfo
 * name:         eSelector_selectCelltypeInfo
 * global_name:  tTECSInfoAccessor_eSelector_selectCelltypeInfo
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSelector_selectCelltypeInfo(CELLIDX idx, int_t ith)
{
	ER		ercd = E_OK;
	CELLCB	*p_cellcb;
  Descriptor( nTECSInfo_sCelltypeInfo ) desc;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
  cCelltypeInfo_unjoin(  );
  if( is_cNSInfo_joined() ){
      if( 0 <= ith && ith < cNSInfo_getNCelltype() ){
          cNSInfo_getCelltypeInfo( ith, &desc );
          cCelltypeInfo_set_descriptor( desc );
      }
      else{
          ercd = E_PAR;  // index out of range
      }
  }
  else{
      ercd = E_OBJ;      // cNSInfo not joined
  }


	return(ercd);
}

/* #[<ENTRY_FUNC>]# eSelector_selectSignatureInfo
 * name:         eSelector_selectSignatureInfo
 * global_name:  tTECSInfoAccessor_eSelector_selectSignatureInfo
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSelector_selectSignatureInfo(CELLIDX idx, int_t ith)
{
	ER		ercd = E_OK;
	CELLCB	*p_cellcb;
  Descriptor( nTECSInfo_sSignatureInfo ) desc;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
  cSignatureInfo_unjoin(  );
  if( is_cNSInfo_joined() ){
      if( 0 <= ith && ith < cNSInfo_getNSignature( ) ){
          cNSInfo_getSignatureInfo( ith, &desc );
          cSignatureInfo_set_descriptor( desc );
      }
      else{
          ercd = E_PAR;  // index out of range
      }
  }
  else{
      ercd = E_OBJ;      // cNSInfo not joined
  }

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eSelector_getSelectedCelltypeInfo
 * name:         eSelector_getSelectedCelltypeInfo
 * global_name:  tTECSInfoAccessor_eSelector_getSelectedCelltypeInfo
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSelector_getSelectedCelltypeInfo(CELLIDX idx, char_t* name, int_t max_len, int_t* num_attr, int_t* num_var, int_t* num_call, int_t* num_entry)
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
  if( is_cCelltypeInfo_joined() ){
      cCelltypeInfo_getName( name, max_len );
      *num_attr  = cCelltypeInfo_getNAttr( );
      *num_var   = cCelltypeInfo_getNVar( );
      *num_call  = cCelltypeInfo_getNCall( );
      *num_entry = cCelltypeInfo_getNEntry( );
  }
  else {
      ercd = E_OBJ;      // cCelltypeInfo not joined
  }

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eSelector_selectCallInfo
 * name:         eSelector_selectCallInfo
 * global_name:  tTECSInfoAccessor_eSelector_selectCallInfo
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSelector_selectCallInfo(CELLIDX idx, int_t ith)
{
	ER		ercd = E_OK;
	CELLCB	*p_cellcb;
  Descriptor( nTECSInfo_sCallInfo ) desc;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
  cCallInfo_unjoin(  );
  if( is_cCelltypeInfo_joined() ){
      if( 0 <= ith && ith < cCelltypeInfo_getNCall( ) ){
          cCelltypeInfo_getCallInfo( ith, &desc );
          cCallInfo_set_descriptor( desc );
      }
      else{
          ercd = E_PAR;  // index out of range
      }
  }
  else{
      ercd = E_OBJ;      // cCelltypeInfo not joined
  }

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eSelector_selectEntryInfo
 * name:         eSelector_selectEntryInfo
 * global_name:  tTECSInfoAccessor_eSelector_selectEntryInfo
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSelector_selectEntryInfo(CELLIDX idx, int_t ith)
{
	ER		ercd = E_OK;
	CELLCB	*p_cellcb;
  Descriptor( nTECSInfo_sEntryInfo ) desc;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
  cEntryInfo_unjoin(  );
  if( is_cCelltypeInfo_joined() ){
      if( 0 <= ith && ith < cCelltypeInfo_getNEntry( ) ){
          cCelltypeInfo_getEntryInfo( ith, &desc );
          cEntryInfo_set_descriptor( desc );
      }
      else{
          ercd = E_PAR;  // index out of range
      }
  }
  else{
      ercd = E_OBJ;      // cCelltypeInfo not joined
  }

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eSelector_selectAttrInfo
 * name:         eSelector_selectAttrInfo
 * global_name:  tTECSInfoAccessor_eSelector_selectAttrInfo
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSelector_selectAttrInfo(CELLIDX idx, int_t ith)
{
	ER		ercd = E_OK;
	CELLCB	*p_cellcb;
  Descriptor( nTECSInfo_sVarDeclInfo ) desc;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
  cAttrInfo_unjoin(  );
  if( is_cCelltypeInfo_joined() ){
      if( 0 <= ith && ith < cCelltypeInfo_getNAttr( ) ){
          cCelltypeInfo_getAttrInfo( ith, &desc );
          cAttrInfo_set_descriptor( desc );
      }
      else{
          ercd = E_PAR;  // index out of range
      }
  }
  else{
      ercd = E_OBJ;      // cCelltypeInfo not joined
  }

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eSelector_selectVarInfo
 * name:         eSelector_selectVarInfo
 * global_name:  tTECSInfoAccessor_eSelector_selectVarInfo
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSelector_selectVarInfo(CELLIDX idx, int_t ith)
{
	ER		ercd = E_OK;
	CELLCB	*p_cellcb;
  Descriptor( nTECSInfo_sVarDeclInfo ) desc;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
  cVarInfo_unjoin(  );
  if( is_cCelltypeInfo_joined() ){
      if( 0 <= ith && ith < cCelltypeInfo_getNVar( ) ){
          cCelltypeInfo_getVarInfo( ith, &desc );
          cVarInfo_set_descriptor( desc );
      }
      else{
          ercd = E_PAR;  // index out of range
      }
  }
  else{
      ercd = E_OBJ;      // cCelltypeInfo not joined
  }

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eSelector_getSelectedAttrInfo
 * name:         eSelector_getSelectedAttrInfo
 * global_name:  tTECSInfoAccessor_eSelector_getSelectedAttrInfo
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSelector_getSelectedAttrInfo(CELLIDX idx, char_t* name, int_t max_len)
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
  if( is_cAttrInfo_joined() ){
      cAttrInfo_getName( name, max_len );
  }
  else{
      ercd = E_OBJ;      // cAttrInfo not joined
  }
	return(ercd);
}

/* #[<ENTRY_FUNC>]# eSelector_getSizeIsExprOfAttr
 * name:         eSelector_getSizeIsExprOfAttr
 * global_name:  tTECSInfoAccessor_eSelector_getSizeIsExprOfAttr
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSelector_getSizeIsExprOfAttr(CELLIDX idx, char_t* expr_str, int32_t max_len)
{
	CELLCB	*p_cellcb;
  ER     ercd = E_OK;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		/* エラー処理コードをここに記述します */
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
  if( is_cAttrInfo_joined() ){
      cAttrInfo_getSizeIsExpr( expr_str, max_len );
  }
  else{
      ercd = E_OBJ;      // cAttrInfo not joined
  }
  return ercd;
}

/* #[<ENTRY_FUNC>]# eSelector_selectTypeInfoOfAttr
 * name:         eSelector_selectTypeInfoOfAttr
 * global_name:  tTECSInfoAccessor_eSelector_selectTypeInfoOfAttr
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSelector_selectTypeInfoOfAttr(CELLIDX idx)
{
	ER		ercd = E_OK;
	CELLCB	*p_cellcb;
  Descriptor( nTECSInfo_sTypeInfo ) desc;
  if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
  if( is_cAttrInfo_joined() ){
      cAttrInfo_getTypeInfo( &desc );
      cTypeInfo_set_descriptor( desc );
  }
  else{
      ercd = E_OBJ;      // cAttrInfo not joined
  }

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eSelector_getSelectedVarInfo
 * name:         eSelector_getSelectedVarInfo
 * global_name:  tTECSInfoAccessor_eSelector_getSelectedVarInfo
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSelector_getSelectedVarInfo(CELLIDX idx, char_t* name, int_t max_len)
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
  if( is_cVarInfo_joined() ){
      cVarInfo_getName( name, max_len );
  }
  else{
      ercd = E_OBJ;      // cVarInfo not joined
  }

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eSelector_getSizeIsExprOfVar
 * name:         eSelector_getSizeIsExprOfVar
 * global_name:  tTECSInfoAccessor_eSelector_getSizeIsExprOfVar
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSelector_getSizeIsExprOfVar(CELLIDX idx, char_t* expr_str, int32_t max_len)
{
	CELLCB	*p_cellcb;
  ER     ercd = E_OK;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		/* エラー処理コードをここに記述します */
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
  printf( "eSelector_getSizeIsExprOfVar not supported\n" );

  return ercd;
}

/* #[<ENTRY_FUNC>]# eSelector_selectTypeInfoOfVar
 * name:         eSelector_selectTypeInfoOfVar
 * global_name:  tTECSInfoAccessor_eSelector_selectTypeInfoOfVar
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSelector_selectTypeInfoOfVar(CELLIDX idx)
{
	ER		ercd = E_OK;
	CELLCB	*p_cellcb;
  Descriptor( nTECSInfo_sTypeInfo ) desc;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
  if( is_cVarInfo_joined() ){
      cVarInfo_getTypeInfo( &desc );
      cTypeInfo_set_descriptor( desc );
  }
  else{
      ercd = E_OBJ;      // cVarInfo not joined
  }

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eSelector_getSelectedCallInfo
 * name:         eSelector_getSelectedCallInfo
 * global_name:  tTECSInfoAccessor_eSelector_getSelectedCallInfo
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSelector_getSelectedCallInfo(CELLIDX idx, char_t* name, int_t max_len, int_t* array_size)
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
  if( is_cCallInfo_joined() ){
      cCallInfo_getName( name, max_len );
      *array_size = cCallInfo_getArraySize();
  }
  else{
      ercd = E_OBJ;      // cCallInfo not joined
  }
  return(ercd);
}

/* #[<ENTRY_FUNC>]# eSelector_selectSignatureOfCall
 * name:         eSelector_selectSignatureOfCall
 * global_name:  tTECSInfoAccessor_eSelector_selectSignatureOfCall
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSelector_selectSignatureOfCall(CELLIDX idx)
{
	ER		ercd = E_OK;
	CELLCB	*p_cellcb;
  Descriptor( nTECSInfo_sSignatureInfo ) desc;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
  if( is_cCallInfo_joined() ){
      cCallInfo_getSignatureInfo( &desc );
      cSignatureInfo_set_descriptor( desc );
  }
  else {
      ercd = E_OBJ;      // cCallInfo not joined
  }

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eSelector_getSelectedCallSpecifierInfo
 * name:         eSelector_getSelectedCallSpecifierInfo
 * global_name:  tTECSInfoAccessor_eSelector_getSelectedCallSpecifierInfo
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSelector_getSelectedCallSpecifierInfo(CELLIDX idx, bool_t* b_optional, bool_t* b_dynamic, bool_t* b_ref_desc, bool_t* b_omit)
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
  if( is_cCallInfo_joined() ){
      cCallInfo_getSpecifierInfo( b_optional, b_dynamic, b_ref_desc, b_omit );
  }
  else {
      ercd = E_OBJ;      // cCallInfo not joined
  }
	return(ercd);
}

/* #[<ENTRY_FUNC>]# eSelector_getSelectedCallInternalInfo
 * name:         eSelector_getSelectedCallInternalInfo
 * global_name:  tTECSInfoAccessor_eSelector_getSelectedCallInternalInfo
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSelector_getSelectedCallInternalInfo(CELLIDX idx, bool_t* b_allocator_port, bool_t* b_require_port)
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
  if( is_cCallInfo_joined() ){
      cCallInfo_getInternalInfo( b_allocator_port, b_require_port );
  }
  else {
      ercd = E_OBJ;      // cCallInfo not joined
  }
	return(ercd);
}

/* #[<ENTRY_FUNC>]# eSelector_getSelectedCallLocationInfo
 * name:         eSelector_getSelectedCallLocationInfo
 * global_name:  tTECSInfoAccessor_eSelector_getSelectedCallLocationInfo
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSelector_getSelectedCallLocationInfo(CELLIDX idx, uint32_t* offset, int8_t* place)
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
  if( is_cCallInfo_joined() ){
      cCallInfo_getLocationInfo( offset, place );
  }
  else {
      ercd = E_OBJ;      // cCallInfo not joined
  }
	return(ercd);
}

/* #[<ENTRY_FUNC>]# eSelector_getSelectedCallOptimizeInfo
 * name:         eSelector_getSelectedCallOptimizeInfo
 * global_name:  tTECSInfoAccessor_eSelector_getSelectedCallOptimizeInfo
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSelector_getSelectedCallOptimizeInfo(CELLIDX idx, bool_t* b_VMT_useless, bool_t* b_skelton_useless, bool_t* b_cell_unique)
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
  if( is_cCallInfo_joined() ){
      cCallInfo_getOptimizeInfo( b_VMT_useless, b_skelton_useless, b_cell_unique );
  }
  else {
      ercd = E_OBJ;      // cCallInfo not joined
  }
	return(ercd);
}

/* #[<ENTRY_FUNC>]# eSelector_getSelectedEntryInfo
 * name:         eSelector_getSelectedEntryInfo
 * global_name:  tTECSInfoAccessor_eSelector_getSelectedEntryInfo
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSelector_getSelectedEntryInfo(CELLIDX idx, char_t* name, int_t max_len, int_t* array_size)
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
  if( is_cEntryInfo_joined() ){
      cEntryInfo_getName( name, max_len );
      *array_size = cEntryInfo_getArraySize();
  }
  else{
      ercd = E_OBJ;      // cEntryInfo not joined
  }

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eSelector_selectSignatureOfEntry
 * name:         eSelector_selectSignatureOfEntry
 * global_name:  tTECSInfoAccessor_eSelector_selectSignatureOfEntry
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSelector_selectSignatureOfEntry(CELLIDX idx)
{
	ER		ercd = E_OK;
	CELLCB	*p_cellcb;
  Descriptor( nTECSInfo_sSignatureInfo ) desc;
  if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
  if( is_cEntryInfo_joined() ){
      cEntryInfo_getSignatureInfo( &desc );
      cSignatureInfo_set_descriptor( desc );
  }
  else {
      ercd = E_OBJ;      // cEntryInfo not joined
  }

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eSelector_getSelectedEntryInlineInfo
 * name:         eSelector_getSelectedEntryInlineInfo
 * global_name:  tTECSInfoAccessor_eSelector_getSelectedEntryInlineInfo
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSelector_getSelectedEntryInlineInfo(CELLIDX idx, bool_t* b_inline)
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
  if( is_cEntryInfo_joined() ){
      *b_inline = cEntryInfo_isInline( );
  }
  else {
      ercd = E_OBJ;      // cEntryInfo not joined
  }

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eSelector_getSelectedSignatureInfo
 * name:         eSelector_getSelectedSignatureInfo
 * global_name:  tTECSInfoAccessor_eSelector_getSelectedSignatureInfo
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSelector_getSelectedSignatureInfo(CELLIDX idx, char_t* name, int_t max_len, int_t* num_function)
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
  if( is_cSignatureInfo_joined() ){
      cSignatureInfo_getName( name, max_len );
      *num_function = cSignatureInfo_getNFunction( );
  }
  else {
      ercd = E_OBJ;      // cSignatureInfo not joined
  }

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eSelector_selectFunctionInfoByIndex
 * name:         eSelector_selectFunctionInfoByIndex
 * global_name:  tTECSInfoAccessor_eSelector_selectFunctionInfoByIndex
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSelector_selectFunctionInfoByIndex(CELLIDX idx, int_t ith)
{
	ER		ercd = E_OK;
	CELLCB	*p_cellcb;
  Descriptor( nTECSInfo_sFunctionInfo ) desc;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
  if( is_cSignatureInfo_joined() ){
      if( 0 <= ith && ith < cSignatureInfo_getNFunction( ) ){
          cSignatureInfo_getFunctionInfo( ith, &desc );
          cFunctionInfo_set_descriptor( desc );
      }
      else {
      }
  }
  else {
      ercd = E_OBJ;      // cSignatureInfo not joined
  }

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eSelector_getSelectedFunctionInfo
 * name:         eSelector_getSelectedFunctionInfo
 * global_name:  tTECSInfoAccessor_eSelector_getSelectedFunctionInfo
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSelector_getSelectedFunctionInfo(CELLIDX idx, char_t* name, int_t max_len, int_t* num_args)
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
  if( is_cFunctionInfo_joined() ){
      cFunctionInfo_getName( name, max_len );
      *num_args = cFunctionInfo_getNParam( );
  }
  else {
      ercd = E_OBJ;      // cFunctionInfo not joined
  }
	return(ercd);
}

/* #[<ENTRY_FUNC>]# eSelector_selectTypeInfoOfReturn
 * name:         eSelector_selectTypeInfoOfReturn
 * global_name:  tTECSInfoAccessor_eSelector_selectTypeInfoOfReturn
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSelector_selectTypeInfoOfReturn(CELLIDX idx)
{
	ER		ercd = E_OK;
	CELLCB	*p_cellcb;
  Descriptor( nTECSInfo_sTypeInfo ) desc;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
  if( is_cFunctionInfo_joined() ){
      cFunctionInfo_getReturnTypeInfo( &desc );
      cTypeInfo_set_descriptor( desc );
  }
  else {
      ercd = E_OBJ;      // cFunctionInfo not joined
  }

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eSelector_getSelectedParamInfo
 * name:         eSelector_getSelectedParamInfo
 * global_name:  tTECSInfoAccessor_eSelector_getSelectedParamInfo
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSelector_getSelectedParamInfo(CELLIDX idx, char_t* name, int_t max_len, int8_t* dir)
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
  if( is_cParamInfo_joined() ){
      cParamInfo_getName( name, max_len );
      cParamInfo_getDir( dir );
  }
  else {
      ercd = E_OBJ;      // cParamInfo not joined
  }

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eSelector_selectParamInfo
 * name:         eSelector_selectParamInfo
 * global_name:  tTECSInfoAccessor_eSelector_selectParamInfo
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSelector_selectParamInfo(CELLIDX idx, int_t ith)
{
	ER		ercd = E_OK;
	CELLCB	*p_cellcb;
  Descriptor( nTECSInfo_sParamInfo ) desc;
  if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
  if( is_cFunctionInfo_joined() ){
      if( 0 <= ith && ith < cFunctionInfo_getNParam() ){
          cFunctionInfo_getParamInfo( ith, &desc );
          cParamInfo_set_descriptor( desc );
      }
      else{
          ercd = E_PAR;  // index out of range
      }
  }
  else {
      ercd = E_OBJ;      // cFunctionInfo not joined
  }

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eSelector_selectTypeInfoOfParam
 * name:         eSelector_selectTypeInfoOfParam
 * global_name:  tTECSInfoAccessor_eSelector_selectTypeInfoOfParam
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSelector_selectTypeInfoOfParam(CELLIDX idx)
{
	ER		ercd = E_OK;
	CELLCB	*p_cellcb;
  Descriptor( nTECSInfo_sTypeInfo ) desc;
  if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
  if( is_cParamInfo_joined() ){
      cParamInfo_getTypeInfo( &desc );
      cTypeInfo_set_descriptor( desc );
  }
  else {
      ercd = E_OBJ;      // cFunctionInfo not joined
  }

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eSelector_getSelectedTypeInfo
 * name:         eSelector_getSelectedTypeInfo
 * global_name:  tTECSInfoAccessor_eSelector_getSelectedTypeInfo
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSelector_getSelectedTypeInfo(CELLIDX idx, char_t* name, int_t max_len, int8_t* kind)
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
  if( is_cTypeInfo_joined() ){
      cTypeInfo_getName( name, max_len );
      *kind = cTypeInfo_getKind( );
  }
  else {
      ercd = E_OBJ;      // cTypeInfo not joined
  }
  return(ercd);
}

/* #[<ENTRY_FUNC>]# eSelector_selectTypeInfoOfType
 * name:         eSelector_selectTypeInfoOfType
 * global_name:  tTECSInfoAccessor_eSelector_selectTypeInfoOfType
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSelector_selectTypeInfoOfType(CELLIDX idx)
{
	ER		ercd = E_OK;
	CELLCB	*p_cellcb;
  Descriptor( nTECSInfo_sTypeInfo ) desc;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
  if( is_cTypeInfo_joined() ){
      cTypeInfo_getTypeInfo( &desc );
      cTypeInfo_set_descriptor( desc );
  }
  else {
      ercd = E_OBJ;      // cTypeInfo not joined
  }

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eSelector_getSelectedRegionInfo
 * name:         eSelector_getSelectedRegionInfo
 * global_name:  tTECSInfoAccessor_eSelector_getSelectedRegionInfo
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSelector_getSelectedRegionInfo(CELLIDX idx, char_t* name, int_t max_len, int_t* num_cell)
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
  if( is_cRegionInfo_joined() ){
      cRegionInfo_getName( name, max_len );
      *num_cell = cRegionInfo_getNCell();
  }
  else {
      ercd = E_OBJ;      // cNSInfo not joined
  }

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eSelector_selectCellInfo
 * name:         eSelector_selectCellInfo
 * global_name:  tTECSInfoAccessor_eSelector_selectCellInfo
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSelector_selectCellInfo(CELLIDX idx, int_t ith)
{
	ER		ercd = E_OK;
	CELLCB	*p_cellcb;
  Descriptor( nTECSInfo_sCellInfo ) desc;
  if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
  if( is_cRegionInfo_joined() ){
      if( 0 <= ith && ith < cRegionInfo_getNCell() ){
          cRegionInfo_getCellInfo( ith, &desc );
          cCellInfo_set_descriptor( desc );
      }
      else {
          ercd = E_PAR;
      }
  }
  else {
      ercd = E_OBJ;      // cNSInfo not joined
  }

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eSelector_getSelectedCellInfo
 * name:         eSelector_getSelectedCellInfo
 * global_name:  tTECSInfoAccessor_eSelector_getSelectedCellInfo
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSelector_getSelectedCellInfo(CELLIDX idx, char_t* name, int_t max_len)
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
  if( is_cCellInfo_joined() ){
      cCellInfo_getName( name, max_len );
  }
  else {
      ercd = E_OBJ;      // cNSInfo not joined
  }

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eSelector_selectCelltypeInfoOfCell
 * name:         eSelector_selectCelltypeInfoOfCell
 * global_name:  tTECSInfoAccessor_eSelector_selectCelltypeInfoOfCell
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSelector_selectCelltypeInfoOfCell(CELLIDX idx)
{
	ER		ercd = E_OK;
	CELLCB	*p_cellcb;
  Descriptor( nTECSInfo_sCelltypeInfo ) desc;
  if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
  if( is_cCellInfo_joined() ){
      cCellInfo_getCelltypeInfo( &desc );
      cCelltypeInfo_set_descriptor( desc );
  }
  else {
      ercd = E_OBJ;      // cNSInfo not joined
  }
	return(ercd);
}

/* #[<ENTRY_FUNC>]# eSelector_getAttrValueInStr
 * name:         eSelector_getAttrValueInStr
 * global_name:  tTECSInfoAccessor_eSelector_getAttrValueInStr
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSelector_getAttrValueInStr(CELLIDX idx, char_t* buf, int_t max_len)
{
	ER		  ercd = E_OK;
	CELLCB	*p_cellcb;
  uint32_t offset;
  void     *base, *ptr;
  int8_t   place;

  Descriptor( nTECSInfo_sCelltypeInfo ) CTdesc;
  if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
  if( is_cCellInfo_joined() && is_cAttrInfo_joined() ){
      cCellInfo_getCelltypeInfo( &CTdesc );
      cCelltypeInfo_set_descriptor( CTdesc );
      printf( "eSelector_getAttrValueInStr not supported\n" );
      cAttrInfo_getLocationInfo( &offset, &place );
      switch( place ){
      case VARDECL_PLACE_CB:
          cCellInfo_getCBP( &base );
          break;
      case VARDECL_PLACE_INIB:
          cCellInfo_getINIBP( &base );
          break;
      default:
          base = (void *)0;
      };
      if( base ){
          ptr = base + offset;
          eSelector_selectTypeInfoOfAttr(idx);
          getSelectedTypeValue( p_cellcb, ptr, buf, max_len );
      }
      else {
          strncpy( buf, "(OMITTED)", max_len );
      }
  }
  else{
      ercd = E_OBJ;
  }

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eSelector_getAttrSizeIsValue
 * name:         eSelector_getAttrSizeIsValue
 * global_name:  tTECSInfoAccessor_eSelector_getAttrSizeIsValue
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSelector_getAttrSizeIsValue(CELLIDX idx)
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
  printf( "eSelector_getAttrSizeIsValue not supported\n" );

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eSelector_getVarValueInStr
 * name:         eSelector_getVarValueInStr
 * global_name:  tTECSInfoAccessor_eSelector_getVarValueInStr
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSelector_getVarValueInStr(CELLIDX idx, char_t* buf, int_t max_len)
{
	ER        ercd = E_OK;
	CELLCB    *p_cellcb;
  int8_t    place;
  void      *base, *ptr;
  uint32_t  offset;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
  cVarInfo_getLocationInfo( &offset, &place );
  switch( place ){
  case VARDECL_PLACE_CB:
      cCellInfo_getCBP( &base );
      break;
  case VARDECL_PLACE_INIB:
      cCellInfo_getINIBP( &base );
      break;
  default:
      base = (void *)0;
  };
  if( base ){
      ptr = base + offset;
      eSelector_selectTypeInfoOfVar(idx);
      getSelectedTypeValue( p_cellcb, ptr, buf, max_len );
  }
  else {
      strncpy( buf, "(OMITTED)", max_len );
  }
  printf( "eSelector_getVarValueInStr not supported\n" );
	return(ercd);
}

/* #[<ENTRY_FUNC>]# eSelector_getVarSizeIsValue
 * name:         eSelector_getVarSizeIsValue
 * global_name:  tTECSInfoAccessor_eSelector_getVarSizeIsValue
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSelector_getVarSizeIsValue(CELLIDX idx)
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
  printf( "eSelector_getVarSizeIsValue not supported\n" );

	return(ercd);
}

/* #[<POSTAMBLE>]#
 *   これより下に非受け口関数を書きます
 * #[</POSTAMBLE>]#*/

static void
getSelectedTypeValue( CELLCB *p_cellcb, void *ptr, char_t *buf, int_t max_len )
{
    switch( cTypeInfo_getKind() ){
    case TECSTypeKind_BoolType:
        snprintf( buf, max_len, "%d", *(bool_t *)ptr );
        break;
    case TECSTypeKind_IntType:
        switch( cTypeInfo_getSize() ){
        case 8:
            snprintf( buf, max_len, "%d", *(int8_t *)ptr );
            break;
        case 16:
            snprintf( buf, max_len, "%d", *(int16_t *)ptr );
            break;
        case 32:
            snprintf( buf, max_len, "%d", *(int32_t *)ptr );
            break;
        case 64:
            snprintf( buf, max_len, "%lld", *(int64_t *)ptr );
            break;
        default:
            snprintf( buf, max_len, "unknown int size(%d)", cTypeInfo_getSize() );
        };
    case TECSTypeKind_FloatType:
        switch( cTypeInfo_getSize() ){
        case 32:
            snprintf( buf, max_len, "%g", *(float32_t *)ptr );
            break;
        case 64:
            snprintf( buf, max_len, "%g", *(double64_t *)ptr );
            break;
        default:
            snprintf( buf, max_len, "unknown double size(%d)", cTypeInfo_getSize() );
        };
    case TECSTypeKind_PtrType:
        snprintf( buf, max_len, "%X", *(double64_t *)ptr );
        break;
    case TECSTypeKind_ArrayType:        break;
    case TECSTypeKind_DefinedType:      break;
    case TECSTypeKind_StructType:       break;
    case TECSTypeKind_EnumType:         break;
    case TECSTypeKind_VoidType:         break;
    case TECSTypeKind_DescriptorType:   break;
    default:                            break;
    };
}
