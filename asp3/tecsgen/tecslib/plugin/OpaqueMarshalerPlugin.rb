# -*- coding: utf-8 -*-
#
#  TECS Generator
#      Generator for TOPPERS Embedded Component System
#  
#   Copyright (C) 2008-2014 by TOPPERS Project
#--
#   上記著作権者は，以下の(1)〜(4)の条件を満たす場合に限り，本ソフトウェ
#   ア（本ソフトウェアを改変したものを含む．以下同じ）を使用・複製・改
#   変・再配布（以下，利用と呼ぶ）することを無償で許諾する．
#   (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
#       権表示，この利用条件および下記の無保証規定が，そのままの形でソー
#       スコード中に含まれていること．
#   (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
#       用できる形で再配布する場合には，再配布に伴うドキュメント（利用
#       者マニュアルなど）に，上記の著作権表示，この利用条件および下記
#       の無保証規定を掲載すること．
#   (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
#       用できない形で再配布する場合には，次のいずれかの条件を満たすこ
#       と．
#     (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
#         作権表示，この利用条件および下記の無保証規定を掲載すること．
#     (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
#         報告すること．
#   (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
#       害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
#       また，本ソフトウェアのユーザまたはエンドユーザからのいかなる理
#       由に基づく請求からも，上記著作権者およびTOPPERSプロジェクトを
#       免責すること．
#  
#   本ソフトウェアは，無保証で提供されているものである．上記著作権者お
#   よびTOPPERSプロジェクトは，本ソフトウェアに関して，特定の使用目的
#   に対する適合性も含めて，いかなる保証も行わない．また，本ソフトウェ
#   アの利用により直接的または間接的に生じたいかなる損害に関しても，そ
#   の責任を負わない．
#  
#   $Id: OpaqueMarshalerPlugin.rb 3159 2020-07-05 10:25:24Z okuma-top $
#++

#== OpaqueMarshaler
# OpaqueRPC 用のマーシャラ、アンマーシャラセルタイプを生成するシグニチャプラグイン

require_tecsgen_lib "lib/GenOpaqueMarshaler.rb"
require_tecsgen_lib "lib/GenParamCopy.rb"

class OpaqueMarshalerPlugin < SignaturePlugin

  OpaqueMarshalerPluginArgProc = { }

  include GenOpaqueMarshaler
  include GenParamCopy

  def initialize( signature, option )
    super
    @b_noClientSemaphore = false
    @noServerChannelOpenerCode = true
    initialize_opaque_marshaler

    @plugin_arg_check_proc_tab = OpaqueMarshalerPluginArgProc
    parse_plugin_arg
    # check_PPAllocator
    if @signature.need_PPAllocator?(true) then
      @PPAllocatorSize = 1    # PPAllocatorの必要性有のために設定 (サイズは使われない)
      necessity = "Necessary"
    else
      necessity = "Unnecessary"
    end

    dbgPrint "OpaqueMarshalerPlugin: signature=#{signature.get_namespace_path}, PPAllocator=#{necessity}\n"
  end

  def gen_cdl_file file
    gen_marshaler_celltype
    file.print <<EOT
import( "#{@marshaler_celltype_file_name}" );
EOT
  end

  def subst_name( val )
    return :"_"
    # return nil
  end

end
