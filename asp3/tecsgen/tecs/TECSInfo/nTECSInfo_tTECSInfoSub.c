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
 *  @(#) $Id: nTECSInfo_tTECSInfoSub.c 2663 2017-07-08 23:09:53Z okuma-top $
 */

/* #[<PREAMBLE>]#
 * #[<...>]# から #[</...>]# で囲まれたコメントは編集しないでください
 * tecsmerge によるマージに使用されます
 *
 * 属性アクセスマクロ #_CAAM_#
 * name_len         int32_t          ATTR_name_len   
 * path_len         int32_t          ATTR_path_len   
 * name1            char_t*          VAR_name1       
 * name2            char_t*          VAR_name2       
 * path             char_t*          VAR_path        
 *
 * 呼び口関数 #_TCPF_#
 * call port: cNamespaceInfo signature: nTECSInfo_sNamespaceInfo context:task
 *   ER             cNamespaceInfo_getName( char_t* name, int_t max_len );
 *   uint16_t       cNamespaceInfo_getNameLength( );
 *   uint32_t       cNamespaceInfo_getNNamespace( );
 *   ER             cNamespaceInfo_getNamespaceInfo( uint32_t ith, Descriptor( nTECSInfo_sNamespaceInfo )* des );
 *   uint32_t       cNamespaceInfo_getNSignature( );
 *   ER             cNamespaceInfo_getSignatureInfo( uint32_t ith, Descriptor( nTECSInfo_sSignatureInfo )* des );
 *   uint32_t       cNamespaceInfo_getNCelltype( );
 *   ER             cNamespaceInfo_getCelltypeInfo( uint32_t ith, Descriptor( nTECSInfo_sCelltypeInfo )* des );
 *   [ref_desc]
 *      Descriptor( nTECSInfo_sNamespaceInfo ) cNamespaceInfo_refer_to_descriptor();
 *      Descriptor( nTECSInfo_sNamespaceInfo ) cNamespaceInfo_ref_desc()      (same as above; abbreviated version);
 * call port: cRegionInfo signature: nTECSInfo_sRegionInfo context:task
 *   ER             cRegionInfo_getName( char_t* name, int_t max_len );
 *   uint16_t       cRegionInfo_getNameLength( );
 *   uint32_t       cRegionInfo_getNCell( );
 *   ER             cRegionInfo_getCellInfo( uint32_t ith, Descriptor( nTECSInfo_sCellInfo )* des );
 *   uint32_t       cRegionInfo_getNRegion( );
 *   ER             cRegionInfo_getRegionInfo( uint32_t ith, Descriptor( nTECSInfo_sRegionInfo )* des );
 *   [ref_desc]
 *      Descriptor( nTECSInfo_sRegionInfo ) cRegionInfo_refer_to_descriptor();
 *      Descriptor( nTECSInfo_sRegionInfo ) cRegionInfo_ref_desc()      (same as above; abbreviated version);
 * call port: cNamespaceInfoTemp signature: nTECSInfo_sNamespaceInfo context:task optional:true
 *   bool_t     is_cNamespaceInfoTemp_joined()                     check if joined
 *   ER             cNamespaceInfoTemp_getName( char_t* name, int_t max_len );
 *   uint16_t       cNamespaceInfoTemp_getNameLength( );
 *   uint32_t       cNamespaceInfoTemp_getNNamespace( );
 *   ER             cNamespaceInfoTemp_getNamespaceInfo( uint32_t ith, Descriptor( nTECSInfo_sNamespaceInfo )* des );
 *   uint32_t       cNamespaceInfoTemp_getNSignature( );
 *   ER             cNamespaceInfoTemp_getSignatureInfo( uint32_t ith, Descriptor( nTECSInfo_sSignatureInfo )* des );
 *   uint32_t       cNamespaceInfoTemp_getNCelltype( );
 *   ER             cNamespaceInfoTemp_getCelltypeInfo( uint32_t ith, Descriptor( nTECSInfo_sCelltypeInfo )* des );
 *   [dynamic, optional]
 *      void           cNamespaceInfoTemp_set_descriptor( Descriptor( nTECSInfo_sNamespaceInfo ) desc );
 *      void           cNamespaceInfoTemp_unjoin(  );
 * call port: cRegionInfoTemp signature: nTECSInfo_sRegionInfo context:task optional:true
 *   bool_t     is_cRegionInfoTemp_joined()                     check if joined
 *   ER             cRegionInfoTemp_getName( char_t* name, int_t max_len );
 *   uint16_t       cRegionInfoTemp_getNameLength( );
 *   uint32_t       cRegionInfoTemp_getNCell( );
 *   ER             cRegionInfoTemp_getCellInfo( uint32_t ith, Descriptor( nTECSInfo_sCellInfo )* des );
 *   uint32_t       cRegionInfoTemp_getNRegion( );
 *   ER             cRegionInfoTemp_getRegionInfo( uint32_t ith, Descriptor( nTECSInfo_sRegionInfo )* des );
 *   [dynamic, optional]
 *      void           cRegionInfoTemp_set_descriptor( Descriptor( nTECSInfo_sRegionInfo ) desc );
 *      void           cRegionInfoTemp_unjoin(  );
 * call port: cCelltypeInfoTemp signature: nTECSInfo_sCelltypeInfo context:task optional:true
 *   bool_t     is_cCelltypeInfoTemp_joined()                     check if joined
 *   ER             cCelltypeInfoTemp_getName( char_t* name, int_t max_len );
 *   uint16_t       cCelltypeInfoTemp_getNameLength( );
 *   uint32_t       cCelltypeInfoTemp_getNAttr( );
 *   ER             cCelltypeInfoTemp_getAttrInfo( uint32_t ith, Descriptor( nTECSInfo_sVarDeclInfo )* desc );
 *   uint32_t       cCelltypeInfoTemp_getNVar( );
 *   ER             cCelltypeInfoTemp_getVarInfo( uint32_t ith, Descriptor( nTECSInfo_sVarDeclInfo )* desc );
 *   uint32_t       cCelltypeInfoTemp_getNCall( );
 *   ER             cCelltypeInfoTemp_getCallInfo( uint32_t ith, Descriptor( nTECSInfo_sCallInfo )* desc );
 *   uint32_t       cCelltypeInfoTemp_getNEntry( );
 *   ER             cCelltypeInfoTemp_getEntryInfo( uint32_t ith, Descriptor( nTECSInfo_sEntryInfo )* desc );
 *   bool_t         cCelltypeInfoTemp_isSingleton( );
 *   bool_t         cCelltypeInfoTemp_isIDX_is_ID( );
 *   uint32_t       cCelltypeInfoTemp_sizeOfCB( );
 *   uint32_t       cCelltypeInfoTemp_sizeOfINIB( );
 *   [dynamic, optional]
 *      void           cCelltypeInfoTemp_set_descriptor( Descriptor( nTECSInfo_sCelltypeInfo ) desc );
 *      void           cCelltypeInfoTemp_unjoin(  );
 * call port: cCellInfoTemp signature: nTECSInfo_sCellInfo context:task optional:true
 *   bool_t     is_cCellInfoTemp_joined()                     check if joined
 *   ER             cCellInfoTemp_getName( char_t* name, int_t max_len );
 *   uint16_t       cCellInfoTemp_getNameLength( );
 *   uint32_t       cCellInfoTemp_getNRawEntryDescriptorInfo( );
 *   ER             cCellInfoTemp_getRawEntryDescriptorInfo( int_t index, Descriptor( nTECSInfo_sRawEntryDescriptorInfo )* desc );
 *   void           cCellInfoTemp_getCelltypeInfo( Descriptor( nTECSInfo_sCelltypeInfo )* desc );
 *   void           cCellInfoTemp_getCBP( void** cbp );
 *   void           cCellInfoTemp_getINIBP( void** inibp );
 *   [dynamic, optional]
 *      void           cCellInfoTemp_set_descriptor( Descriptor( nTECSInfo_sCellInfo ) desc );
 *      void           cCellInfoTemp_unjoin(  );
 * call port: cEntryInfoTemp signature: nTECSInfo_sEntryInfo context:task optional:true
 *   bool_t     is_cEntryInfoTemp_joined()                     check if joined
 *   ER             cEntryInfoTemp_getName( char_t* name, int_t max_len );
 *   uint16_t       cEntryInfoTemp_getNameLength( );
 *   void           cEntryInfoTemp_getSignatureInfo( Descriptor( nTECSInfo_sSignatureInfo )* desc );
 *   uint32_t       cEntryInfoTemp_getArraySize( );
 *   bool_t         cEntryInfoTemp_isInline( );
 *   [dynamic, optional]
 *      void           cEntryInfoTemp_set_descriptor( Descriptor( nTECSInfo_sEntryInfo ) desc );
 *      void           cEntryInfoTemp_unjoin(  );
 *
 * #[</PREAMBLE>]# */

/* Put prototype declaration and/or variale definition here #_PAC_# */
#include <string.h>
#include "nTECSInfo_tTECSInfoSub_tecsgen.h"

#ifndef E_OK
#define	E_OK	0		/* success */
#define	E_ID	(-18)	/* illegal ID */
#endif

#ifndef DBG_SYSLOG
#define DBG_SYSLOG( x )
// #define DBG_SYSLOG( x ) syslog x
#endif

static ER
separate_basename( const char_t **namespace_path, char_t **basename );


/*
 * VAR_name1 … 引数として渡されたものをコピーする。ただし、途中の '::' を '\0' に書き換えることはある。
 * VAR_name2 … 都度上書きして使用する
 */

/* entry port function #_TEPF_# */
/* #[<ENTRY_PORT>]# eTECSInfo
 * entry port: eTECSInfo
 * signature:  nTECSInfo_sTECSInfo
 * context:    task
 * #[</ENTRY_PORT>]# */

/* #[<ENTRY_FUNC>]# eTECSInfo_findNamespace
 * name:         eTECSInfo_findNamespace
 * global_name:  nTECSInfo_tTECSInfoSub_eTECSInfo_findNamespace
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eTECSInfo_findNamespace(const char_t* namespace_path, Descriptor( nTECSInfo_sNamespaceInfo )* nsDesc)
{
    const char_t  *p = namespace_path;
    char_t  *p2;
    Descriptor( nTECSInfo_sNamespaceInfo ) desc, desc2;
    int     i;
    int     n;

    if( p[ 0 ] == ':' && p[ 1 ] == ':' ){
        p += 2;
    }
    desc = cNamespaceInfo_refer_to_descriptor(); // root name space
    while( *p != '\0' ){
        p2 = VAR_name1;
        for( i = 0; i < ATTR_name_len; i++ ){
            if( *p == '\0' ||  *p == ':' ){
                *p2 = '\0';
                break;
            }
            *p2++ = *p++;
        }
        if( i == ATTR_name_len )
            return E_NORES;

        desc2 = desc;
        cNamespaceInfoTemp_set_descriptor( desc2 );
        n = cNamespaceInfoTemp_getNNamespace( );
        for( i = 0; i < n; i++ ){
            cNamespaceInfoTemp_set_descriptor( desc2 );
            cNamespaceInfoTemp_getNamespaceInfo( i, &desc );
            cNamespaceInfoTemp_set_descriptor( desc );
            cNamespaceInfoTemp_getName( VAR_name2, ATTR_name_len );
            if( strcmp( VAR_name1, VAR_name2 ) == 0 )
                break;
        }
        if( i == n )
            return E_NOID;

        if( p[ 0 ] == ':' ){
            if( p[ 1 ] == ':' )
                p += 2;
            else
                return E_OBJ;
        }
    }
    *nsDesc = desc;
    return E_OK;
}

/* #[<ENTRY_FUNC>]# eTECSInfo_findRegion
 * name:         eTECSInfo_findRegion
 * global_name:  nTECSInfo_tTECSInfoSub_eTECSInfo_findRegion
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eTECSInfo_findRegion(const char_t* namespace_path, Descriptor( nTECSInfo_sRegionInfo )* regionDesc)
{
    const char_t  *p = namespace_path;
    char_t  *p2;
    Descriptor( nTECSInfo_sRegionInfo ) desc, desc2;
    int     i;
    int     n;

    if( p[ 0 ] == ':' && p[ 1 ] == ':' ){
        p += 2;
    }
    desc = cRegionInfo_refer_to_descriptor();
    while( *p != '\0' ){
        p2 = VAR_name1;
        for( i = 0; i < ATTR_name_len; i++ ){
            if( *p == '\0' ||  *p == ':' ){
                *p2 = '\0';
                break;
            }
            *p2++ = *p++;
        }
        if( i == ATTR_name_len )
            return E_NORES;

        desc2 = desc;
        cRegionInfoTemp_set_descriptor( desc2 );
        n = cRegionInfoTemp_getNRegion( );
        for( i = 0; i < n; i++ ){
            cRegionInfoTemp_set_descriptor( desc2 );
            cRegionInfoTemp_getRegionInfo( i, &desc );
            cRegionInfoTemp_set_descriptor( desc );
            cRegionInfoTemp_getName( VAR_name2, ATTR_name_len );
            if( strcmp( VAR_name1, VAR_name2 ) == 0 )
                break;
        }
        if( i == n )
            return E_NOID;

        if( p[ 0 ] == ':' ){
            if( p[ 1 ] == ':' )
                p += 2;
            else
                return E_OBJ;
        }
    }
    *regionDesc = desc;
    return E_OK;
}

/* #[<ENTRY_FUNC>]# eTECSInfo_findSignature
 * name:         eTECSInfo_findSignature
 * global_name:  nTECSInfo_tTECSInfoSub_eTECSInfo_findSignature
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eTECSInfo_findSignature(const char_t* namespace_path, Descriptor( nTECSInfo_sSignatureInfo )* signatureDesc)
{
    Descriptor( nTECSInfo_sNamespaceInfo ) nsDesc;
    Descriptor( nTECSInfo_sSignatureInfo ) desc;
    ER        ercd;
    char_t    *pname;
    int       i, n;

    ercd = separate_basename( &namespace_path, &pname );
    if( ercd != E_OK )
        return ercd;

    ercd = eTECSInfo_findNamespace( namespace_path, &nsDesc);
    if( ercd != E_OK )
        return ercd;

    cNamespaceInfoTemp_set_descriptor( nsDesc );
    n = cNamespaceInfoTemp_getNSignature( );
    for( i = 0; i < n; i++ ){
        cNamespaceInfoTemp_getSignatureInfo( i, &desc );
        cSignatureInfoTemp_set_descriptor( desc );
        cSignatureInfoTemp_getName( VAR_name1, ATTR_name_len );
        if( strcmp( pname, VAR_name1 ) == 0 )
            break;
    }
    if( i == n )
        return E_NOEXS;

    *signatureDesc = desc;
    return E_OK;
}

/* #[<ENTRY_FUNC>]# eTECSInfo_findCelltype
 * name:         eTECSInfo_findCelltype
 * global_name:  nTECSInfo_tTECSInfoSub_eTECSInfo_findCelltype
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eTECSInfo_findCelltype(const char_t* namespace_path, Descriptor( nTECSInfo_sCelltypeInfo )* celltypeDesc)
{
  return E_NOSPT;
}

/* #[<ENTRY_FUNC>]# eTECSInfo_findCell
 * name:         eTECSInfo_findCell
 * global_name:  nTECSInfo_tTECSInfoSub_eTECSInfo_findCell
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eTECSInfo_findCell(const char_t* namespace_path, Descriptor( nTECSInfo_sCellInfo )* cellDesc)
{
    char_t  *p, *pname;
    Descriptor( nTECSInfo_sRegionInfo ) RegionDesc;
    Descriptor( nTECSInfo_sCellInfo )   desc;
    int     i, n, len;
    ER      ercd;

    ercd = separate_basename( &namespace_path, &pname );
    if( ercd != E_OK )
        return ercd;

    ercd = eTECSInfo_findRegion( namespace_path, &RegionDesc);
    if( ercd != E_OK )
        return ercd;

    cRegionInfoTemp_set_descriptor( RegionDesc );
    n = cRegionInfoTemp_getNCell( );
    for( i = 0; i < n; i++ ){
        cRegionInfoTemp_getCellInfo( i, &desc );
        cCellInfoTemp_set_descriptor( desc );
        cCellInfoTemp_getName( VAR_name1, ATTR_name_len );
        if( strcmp( pname, VAR_name1 ) == 0 )
            break;
    }
    if( i == n )
        return E_NOEXS;

    *cellDesc = desc;
    return E_OK;
}

/* #[<ENTRY_FUNC>]# eTECSInfo_findRawEntryDescriptor
 * name:         eTECSInfo_findRawEntryDescriptor
 * global_name:  nTECSInfo_tTECSInfoSub_eTECSInfo_findRawEntryDescriptor
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eTECSInfo_findRawEntryDescriptor(const char_t* namespace_path, Descriptor( nTECSInfo_sRawEntryDescriptorInfo )* rawEntryDescDesc, Descriptor( nTECSInfo_sEntryInfo )* entryDesc)
{
    int_t  len;
    Descriptor( nTECSInfo_sCelltypeInfo )  celltypeDesc;
    Descriptor( nTECSInfo_sCellInfo )      cellDesc;
    Descriptor( nTECSInfo_sEntryInfo )     entDesc;
    char_t *p, *p2;
    int_t  i, n;
    ER        ercd;

    len = strnlen( namespace_path, ATTR_path_len );
    if( len >= ATTR_path_len ){
        return E_NOMEM;
    }
    strcpy( VAR_path, namespace_path );
    p = &VAR_path[len-1];
    while( *p != '.' && p != &VAR_path[0] )
        p--;
    if( *p != '.' )
        return E_PAR;    // '.' が見つからない
    *p = '\0';
    ercd = eTECSInfo_findCell( VAR_path, &cellDesc);
    if( ercd != E_OK )
        return ercd;
    cCellInfoTemp_set_descriptor( cellDesc );
    cCellInfoTemp_getCelltypeInfo( &celltypeDesc  );
    cCelltypeInfoTemp_set_descriptor( celltypeDesc );
    n = cCelltypeInfoTemp_getNEntry();
    p += 1;
    p2 = p;
    while( *p2 ){
        p2++;
        if( *p2 == '[' ){
            *p2 = '\0';
            p2++;
            break;
        }
    }

    for( i = 0; i < n; i++ ){
        ercd = cCelltypeInfoTemp_getEntryInfo( i, &entDesc ); // エラーはみない
        cEntryInfoTemp_set_descriptor( entDesc );
        ercd = cEntryInfoTemp_getName( VAR_name1, ATTR_name_len );
        if( ercd != E_OK )
            return ercd;
        if( strcmp( VAR_name1, p ) == 0 ){
            DBG_SYSLOG(( LOG_INFO, "tTECSInfoSub: cell name:%s entry:%s", namespace_path, VAR_name1 ));
            break;
        }
    }
    if( i != n ){
        ercd = cCellInfoTemp_getRawEntryDescriptorInfo( i, rawEntryDescDesc );
        *entryDesc = entDesc;
    }
    else{
        ercd = E_NOEXS;
    }
    return ercd;
}

/* #[<ENTRY_FUNC>]# eTECSInfo_findRawEntryDescriptor_unsafe
 * name:         eTECSInfo_findRawEntryDescriptor_unsafe
 * global_name:  nTECSInfo_tTECSInfoSub_eTECSInfo_findRawEntryDescriptor_unsafe
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eTECSInfo_findRawEntryDescriptor_unsafe(const char_t* namespace_path, uint32_t subsc, void** rawDesc)
{
    ER   ercd;
    Descriptor( nTECSInfo_sRawEntryDescriptorInfo ) rawEntryDescDesc;
    Descriptor( nTECSInfo_sEntryInfo ) entryDesc;
    ercd = eTECSInfo_findRawEntryDescriptor(namespace_path, &rawEntryDescDesc, &entryDesc);
    if( ercd != E_OK )
        return ercd;
    cRawEntryDescriptorInfoTemp_set_descriptor( rawEntryDescDesc );
    ercd = cRawEntryDescriptorInfoTemp_getRawDescriptor( subsc, rawDesc );
    return ercd;
}

/* #[<POSTAMBLE>]#
 *   これより下に非受け口関数を書きます
 * #[</POSTAMBLE>]#*/

/*
 * namespace_path を VAR_path へコピーし、ディレクトリパス部とオブジェクト名部に分割する
 * namespace_path  "::namespace::subnamespace::object_name"
 * VAR_path        "::namespace::subnamespace\0:object_name"   一番右の '::' を '\0:' に置き換える
 * basename        "object_name" へのポインタを返す
 */
static ER
separate_basename( const char_t **namespace_path, char_t **basename )
{
    int     i, len;
    char_t  *p;
    static const char_t *root_namespacepath = "::";

    len = strnlen( *namespace_path, ATTR_path_len );
    if( len >= ATTR_path_len ){
        return E_NOMEM;
    }
    if( VAR_path != *namespace_path )     // findRawEntryDescriptorInfo から呼ばれた時、VAR_path に入っているのでコピー不要
        strcpy( VAR_path, *namespace_path );
    p = &VAR_path[ len - 1 ];
    while( p > VAR_path ){
        if( *p == ':' )
            break;
        p--;
    }
    if( *p == ':' ){
        p--;
        if( p < VAR_path || *p != ':' )
            return E_PAR;    // :: ではなく、: であった
        if( p == VAR_path ){
            *namespace_path = root_namespacepath;
        }
        else{
            *p = '\0';
            *namespace_path = VAR_path;
        }
        *basename = &p[2];
    }
    else{
        *namespace_path = root_namespacepath;
        *basename = VAR_path;
    }
    return  E_OK;
}
