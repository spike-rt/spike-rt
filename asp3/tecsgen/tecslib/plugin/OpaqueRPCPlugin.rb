# -*- coding: utf-8 -*-
#
#  TECS Generator
#      Generator for TOPPERS Embedded Component System
#  
#   Copyright (C) 2008-2020 by TOPPERS Project
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
#   $Id: OpaqueRPCPlugin.rb 3140 2020-03-29 09:21:42Z okuma-top $
#++

# 以下を仮定（制限事項）
#  呼び元、呼び先のエンディアン、char, short, int_t, long_t, intptr_t のサイズが同じ
#  有符号、無符号でサイズが同じ

require_tecsgen_lib "lib/GenOpaqueMarshaler.rb"
require_tecsgen_lib "lib/GenParamCopy.rb"

class OpaqueRPCPlugin < ThroughPlugin

  include GenOpaqueMarshaler
  include GenParamCopy

  # OpaqueRPCPlugin 専用のオプション
  OpaqueRPCPluginArgProc = RPCPluginArgProc.dup  # 複製を作って元を変更しないようにする
  OpaqueRPCPluginArgProc[ "noClientSemaphore"  ] = Proc.new { |obj,rhs| obj.set_noClientSemaphore rhs }

  #=== RPCPlugin の initialize
  #  説明は ThroughPlugin (plugin.rb) を参照
  def initialize( cell_name, plugin_arg, next_cell, next_cell_port_name, next_cell_port_subscript, signature, celltype, caller_cell )
    super
    @b_noClientSemaphore = false
    initialize_opaque_marshaler

    @plugin_arg_check_proc_tab = OpaqueRPCPluginArgProc
    parse_plugin_arg
    check_opener_code
    check_PPAllocator

    print "OpaqueRPCPlugin: #{@clientChannelCell}\n"

    @rpc_server_channel_celltype_name = "tOpaqueRPCPlugin_#{@TDRCelltype}_#{@serverChannelCelltype}_#{@signature.get_global_name}"
    @rpc_server_channel_celltype_file_name = "#{$gen}/#{@rpc_server_channel_celltype_name}.cdl"
    @rpc_client_channel_celltype_name = "tOpaqueRPCPlugin_#{@TDRCelltype}_#{@clientChannelCelltype}_#{@signature.get_global_name}"
    @rpc_client_channel_celltype_file_name = "#{$gen}/#{@rpc_client_channel_celltype_name}.cdl"
  end

  def gen_plugin_decl_code( file )

    gen_marshaler_celltype

    # チャンネル composite セルタイプの生成
    #  ここで生成された CDL ファイルは、tecsgen が直接 import するのではなく
    #  セルコードの CDL から import される

    f = CFile.open( @rpc_client_channel_celltype_file_name, "w" )
    # 同じ内容を二度書く可能性あり (AppFile は不可)

    f.print <<EOT
import( "#{@marshaler_celltype_file_name}" );

/* RPC Client side composite celltype */
composite #{@rpc_client_channel_celltype_name} {
  /* marshaler entry port */
  entry #{@signature.get_namespace_path} eThroughEntry;
  call  sChannel cChannel;
  [optional]
    call sRPCErrorHandler cErrorHandler;
  [optional]
    call sSemaphore cLockChannel;

  cell #{@TDRCelltype} TDR {
    cChannel => composite.cChannel;
  };
  cell #{@marshaler_celltype_name} Marshaler{
    cTDR = TDR.eTDR;
    cErrorHandler => composite.cErrorHandler;
    cLockChannel  => composite.cLockChannel;
  };

  composite.eThroughEntry => Marshaler.eClientEntry;
};
EOT
    f.close

    if @PPAllocatorSize then
      alloc_cell = "  cell tPPAllocator PPAllocator {\n    heapSize = #{@PPAllocatorSize};\n  };\n"
      alloc_call_port_join = "    cPPAllocator = PPAllocator.ePPAllocator;\n"
    else
      alloc_cell = ""
      alloc_call_port_join = ""
    end

    f = CFile.open( @rpc_server_channel_celltype_file_name, "w" )
    # 同じ内容を二度書く可能性あり (AppFile は不可)

    f.print <<EOT
import( "#{@marshaler_celltype_file_name}" );

/* RPC Server side composite celltype */
composite #{@rpc_server_channel_celltype_name} {
  /* Interface */
  call  #{@signature.get_namespace_path} #{@call_port_name};
  call  sChannel   cChannel;
  [optional]
    call sRPCErrorHandler cErrorHandler;
  entry sUnmarshalerMain  eService;

  /* Implementation */
  cell #{@TDRCelltype} TDR {
    cChannel => composite.cChannel;
  };
#{alloc_cell}  cell #{@unmarshaler_celltype_name} Unmarshaler{
    cTDR        = TDR.eTDR;
    cErrorHandler => composite.cErrorHandler;
    cServerCall => composite.#{@call_port_name};
#{alloc_call_port_join}  };
  composite.eService => Unmarshaler.eService;
};
EOT

    f.close
  end

  #===  through cell コードを生成
  #
  #
  def gen_through_cell_code( file )

    gen_plugin_decl_code( file )

    # セルを探す
    # path =["::",@next_cell.get_name]  # mikan namespace
    # cell = Namespace.find( path )
    cell = Namespace.find( @next_cell.get_namespace_path )

    file.print <<EOT
import( "#{@rpc_client_channel_celltype_file_name}" );
import( "#{@rpc_server_channel_celltype_file_name}" );

EOT

    ##### クライアント側のセルの生成 #####
    nest = @start_region.gen_region_str_pre file
    nest_str = "  " * nest

    # セマフォの生成
    if @b_noClientSemaphore == false then
      file.print <<EOT

#{nest_str}  //  Semaphore for Multi-task use ("specify noClientSemaphore" option to delete this)
#{nest_str}  cell #{@semaphoreCelltype} #{@serverChannelCell}_Semaphore{
#{nest_str}    #{@semaphoreInitializer}
#{nest_str}  };
EOT
    end

    # クライアント側チャンネル (TINET, Socket など)の生成
    file.print <<EOT
#{nest_str}  //  Client Side Channel
#{nest_str}  cell #{@clientChannelCelltype} #{@clientChannelCell}{
#{nest_str}    #{@clientChannelInitializer}
#{nest_str}  };

#{nest_str}  //  Marshaler
EOT

    # セマフォの結合文
    if @b_noClientSemaphore == false then
      semaphore = "#{nest_str}    cLockChannel = #{@serverChannelCell}_Semaphore.eSemaphore;\n"
    else
      semaphore = ""
    end

    ### クライアント側チャンネル (マーシャラ+TDR)の生成 ###
    # アロケータの指定があるか？
    if cell.get_allocator_list.length > 0 then

      file.print nest_str
      file.print "[allocator("

      delim = ""
      cell.get_allocator_list.each do |type, eport, subsc, func, buf, alloc|

        alloc_str = alloc.to_s
        subst = @substituteAllocator[alloc_str.to_sym]
        if subst then
          alloc_str = subst[2]+"."+subst[3]
        end

        file.print delim
        delim = ",\n"        # 最終行には出さない

        if subsc then        # 配列添数
          subsc_str = '[#{subsc}]'
        else
          subsc_str = ""
        end

        eport = "eThroughEntry" #RPCの受け口名に変更
        file.print nest_str
        file.print  "#{eport}#{subsc_str}.#{func}.#{buf} = #{alloc_str}"
      end

      file.puts ")]"
    end

    if @clientErrorHandler then
      clientErrorHandler_str = "#{nest_str}    cErrorHandler = #{@clientErrorHandler};\n"
    else
      clientErrorHandler_str = ""
    end

    file.print <<EOT
#{nest_str}  cell #{@rpc_client_channel_celltype_name} #{@cell_name} {
#{nest_str}    cChannel = #{@clientChannelCell}.eC0;
#{clientErrorHandler_str}#{semaphore}#{nest_str}  };

EOT
    ### END: クライアント側チャンネル (マーシャラ+TDR)の生成 ###

    @start_region.gen_region_str_post file
    file.print "\n\n"

    ##### サーバー側のセルの生成 #####
    nest = @end_region.gen_region_str_pre file
    nest_str = "  " * nest
    if @next_cell_port_subscript then
      subscript = '[' + @next_cell_port_subscript.to_s + ']'
    else
      subscript = ""
    end

    if @serverErrorHandler then
      serverErrorHandler_str = "#{nest_str}    cErrorHandler = #{@serverErrorHandler};\n"
    else
      serverErrorHandler_str = ""
    end

    if @b_genOpener then
      opener = "#{nest_str}    cOpener       = #{@serverChannelCell}.eOpener;\n"
    else
      opener = ""
    end

    # サーバー側チャンネル (TINET, Socket など)
    file.print <<EOT

#{nest_str}  //  Server Side Channel
#{nest_str}  cell #{@serverChannelCelltype} #{@serverChannelCell}{
#{nest_str}    #{@serverChannelInitializer}
#{nest_str}  };
EOT

    # サーバー側チャンネル (アンマーシャラ+TDR)
    file.print <<EOT

#{nest_str}  //  Unmarshaler
#{nest_str}  cell #{@rpc_server_channel_celltype_name} #{@serverChannelCell}_Unmarshaler {
#{nest_str}    cChannel = #{@serverChannelCell}.eC1;
#{nest_str}    #{@call_port_name} = #{@next_cell.get_namespace_path.get_path_str}.#{@next_cell_port_name}#{subscript};
#{serverErrorHandler_str}#{nest_str}  };
EOT

    # サーバー側タスクメイン
    file.print <<EOT

#{nest_str}  //  Unmarshaler Task Main
#{nest_str}  cell #{@taskMainCelltype} #{@serverChannelCell}_TaskMain {
#{nest_str}    cMain         = #{@serverChannelCell}_Unmarshaler.eService;
#{opener}#{nest_str}  };
EOT

    # サーバー側タスク
    file.print <<EOT

#{nest_str}  //  Unmarshaler Task
#{nest_str}  cell #{@taskCelltype} #{@serverChannelCell}_Task {
#{nest_str}    cTaskBody         = #{@serverChannelCell}_TaskMain.eMain;
#{nest_str}    priority      = #{@taskPriority};
#{nest_str}    stackSize     = #{@stackSize};
#{nest_str}    attribute     = C_EXP( "TA_ACT" );  /* mikan : marshaler task starts at beginning */
#{nest_str}  };
EOT
    @end_region.gen_region_str_post file
  end

  #=== プラグイン引数 noClientSemaphore のチェック
  def set_noClientSemaphore rhs
    rhs = rhs.to_sym
    if rhs == :true then
      @b_noClientSemaphore = true
    elsif rhs == :false then
      @b_noClientSemaphore = false
    else
      cdl_error( "RPCPlugin: specify true or false for noClientSemaphore" )
    end
  end
end


