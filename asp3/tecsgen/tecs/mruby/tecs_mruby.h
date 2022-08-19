/*
 *  Copyright (C) 2008-2019 by TOPPERS Project
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
 *  @(#) $Id: tecs_mruby.h 2709 2017-11-06 11:09:57Z okuma-top $
 */

#ifndef tecs_mruby_h__
#define tecs_mruby_h__

// もし mruby 関連のヘッダ取り込みでエラーが出るようなら仮の定義で TECS ジェネレータを通す
#ifndef TECSGEN
// #if 1   // テスト中

// tecsgen doesn't include actual mruby.h
#include "mruby.h"
#include "mruby/class.h"
#include "mruby/data.h"
#include "mruby/string.h"
#include "mruby/irep.h"
#include "mruby/dump.h"
#include "mruby/proc.h"

#else /* TECSGEN */

/*
 * fake definition because tecsgen cannot accept actual mruby.h in case of below.
 *   types:   long long, long long int
 *   special keyword __attribute__(x), __extension__
 */
typedef int mrb_state;
typedef int mrb_irep;
typedef int mrb_context;
typedef int mrbc_context;
typedef int mrb_value;
typedef int mrb_int;
struct  RClass {uint32_t gcnext;};  // actual definition: struct RBasic *gcnext
struct  RProc  {uint32_t gcnext;};  // actual definition: struct RBasic *gcnext

typedef int CELLCB;

// これらのダミー定義は TECSPointer.h に移された (V1.7.0)
// #define GET_SET_BOOL( Type, type )
// #define GET_SET_CHAR( Type, type )
// #define GET_SET_INT( Type, type )
// #define GET_SET_FLOAT( Type, type )
// #define POINTER_CLASS( Type, type )
// #define CHECK_AND_GET_POINTER( Type, type )

#endif /* TECSGEN */

#include "TECSPointer.h"
#include "TECSStruct.h"

#endif /* tecs_mruby_h__ */
