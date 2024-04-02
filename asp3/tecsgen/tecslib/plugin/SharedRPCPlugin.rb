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
#   $Id: SharedRPCPlugin.rb 3140 2020-03-29 09:21:42Z okuma-top $
#++

require_tecsgen_lib "lib/GenTransparentMarshaler.rb"
require_tecsgen_lib "lib/GenParamCopy.rb"

#= SharedRPC プラグイン
# SharedRPC チャンネルを生成する
# プラグイン引数は GenTransparentMarshaler 参照

# mikan through plugin: namespace が考慮されていない
# これを利用する場合、以下のように toppers_jsp.cdl sChannel.cdl を指定する必要がある
# tecsgen toppers_jsp.cdl sChannel.cdl your_description.cdl

# 以下を仮定（制限事項）
#  呼び元、呼び先のエンディアン、char, short, int_t, long_t, intptr_t のサイズが同じ
#  有符号、無符号でサイズが同じ

class SharedRPCPlugin < ThroughPlugin

  attr_reader :region
  @@shared_channel_list = {}
  include GenTransparentMarshaler
  include GenParamCopy

  #=== RPCPlugin の initialize
  #  説明は ThroughPlugin (plugin.rb) を参照
  def initialize( cell_name, plugin_arg, next_cell, next_cell_port_name, next_cell_port_subscript, signature, celltype, caller_cell )

    # mikan プラグインオプション指定の不一致のチェック task_priority, 

    super
    initialize_transparent_marshaler cell_name

    # オプション：GenTransparentMarshaler 参照
    @plugin_arg_check_proc_tab = RPCPluginArgProc
    @channelCellName = ""     # nil にしておく
    parse_plugin_arg

    @shared_channel_ct_name = :"tSharedRPCPlugin_#{@channelCelltype}"
    @shared_channel_ct_file_name = "#{$gen}/#{@shared_channel_ct_name}.cdl"
    @rpc_channel_celltype_name = :"tSharedRPCPlugin_#{@TDRCelltype}_#{@channelCelltype}_#{@signature.get_global_name}"
    @rpc_channel_celltype_file_name = "#{$gen}/#{@rpc_channel_celltype_name}.cdl"

    @shared_channel_cell = @channelCellName
    if @shared_channel_cell == "" then
      cdl_error( "SharedRPCPlugin: need channelCellName option" )
    end

    if @@shared_channel_list[ @shared_channel_cell ] == nil then
      # 初出
      @@shared_channel_list[ @shared_channel_cell ] = [ self ]
    else
      # 二番目以降
      @@shared_channel_list[ @shared_channel_cell ] << self
    end
    @sub_channel_no = ( @@shared_channel_list[ @shared_channel_cell ].length ) -1

    if @region != @@shared_channel_list[ @shared_channel_cell ][0].region then
      # 初出とリージョン不一致 (初出は、自分自身とチェックされる。無駄だが小さいので放置)
      cdl_error( "SharedRPCPlugin: preferred region mismatch current: #{@region.get_name} previous: #{@@shared_channel_list[ @shared_channel_cell ][0].region.get_name}")
    else
      dbgPrint "SahredRPCPlugin: Region: #{@region.get_name}, #{@@shared_channel_list[ @shared_channel_cell ][0].region.get_name}\n"
    end

    if @signature.need_PPAllocator? then
      if @PPAllocatorSize == nil then
        cdl_error( "PPAllocatorSize must be speicified for oneway [in] array" )
      end
    end

  end

  # def set_through_info( start_region, end_region, through_type )
  #   super
  #  
  # end

  def gen_plugin_decl_code( file )

    # このセルタイプ（同じシグニチャ）は既に生成されているか？
    if @@generated_celltype[ @shared_channel_ct_name ] == nil then
      @@generated_celltype[ @shared_channel_ct_name ] = [ self ]
    else
      @@generated_celltype[ @shared_channel_ct_name ] << self
    end

    if @@generated_celltype[ @ct_name ] == nil then
      @@generated_celltype[ @ct_name ] = [ self ]
    else
      @@generated_celltype[ @ct_name ] << self
    end

    gen_marshaler_celltype
    # 同じ内容を二度書く可能性あり (AppFile は不可)

    if @signature.need_PPAllocator? then
      alloc_call_port = "  call sPPAllocator cPPAllocator;\n"
      alloc_call_port_join = "  cPPAllocator => composite.cPPAllocator;\n"
    else
      alloc_call_port = ""
      alloc_call_port_join = ""
    end

    f = CFile.open( @rpc_channel_celltype_file_name, "w" )
    # 同じ内容を二度書く可能性あり (AppFile は不可)

    f.print <<EOT
import( "#{@marshaler_celltype_file_name}" );

composite #{@rpc_channel_celltype_name} {
  /* Interface */
  call  #{@signature.get_namespace_path} #{@call_port_name};
  entry #{@signature.get_namespace_path} eThroughEntry;
  call  sTDR       cTDR;
  call  sEventflag cEventflag;
  entry sUnmarshalerMain  eUnmarshalAndCallFunction;
#{alloc_call_port}
  [optional]
    call sSemaphore cLockChannel;

  /* Implementation */
  cell #{@marshaler_celltype_name} #{@signature.get_global_name}_marshaler{
    cTDR         => composite.cTDR;
    cEventflag   => composite.cEventflag;
    cLockChannel => composite.cLockChannel;
  };
  cell #{@unmarshaler_celltype_name} #{@signature.get_global_name}_unmarshaler{
    cTDR         => composite.cTDR;
    cEventflag   => composite.cEventflag;
    cServerCall  => composite.#{@call_port_name};
#{alloc_call_port_join}  };
  composite.eThroughEntry => #{@signature.get_global_name}_marshaler.eClientEntry;
  composite.eUnmarshalAndCallFunction => #{@signature.get_global_name}_unmarshaler.eUnmarshalAndCallFunction;
};
EOT
    # mikan stackSize option & 最新 tecs_package 対応

    f.close

    f = CFile.open( "#{$gen}/#{@shared_channel_ct_name}.cdl", "w" )
    # 同じ内容を二度書く可能性あり (AppFile は不可)

    f.print <<EOT
[active]
composite #{@shared_channel_ct_name} {
  /* Interface */
  entry  sSemaphore eSemaphore[];
  call   sUnmarshalerMain  cUnmarshalAndCallFunction[];

  entry  sDataqueue eDataqueue;
  entry  sTDR       eTDR;
  entry  sEventflag eEventflag;

  attr {
    PRI    priority;
  };

  /* Implementation */
  cell #{@channelCelltype} Channel{
  };
  cell tSemaphore Semaphore {initialCount = 1; attribute = C_EXP("TA_NULL");};
  cell tRPCSharedTaskMain RPCSharedTaskMain {
    cUnmarshalAndCallFunction => composite.cUnmarshalAndCallFunction;
    cServerSideTDR            = Channel.eTDR;
  };
  cell tRPCSharedChannelMan RPCChannelMan {
    cSemaphore                = Semaphore.eSemaphore;
    cClientSideTDR            = Channel.eTDR;
  };
  cell tTask RPCSharedTask {
    cTaskBody = RPCSharedTaskMain.eMain;
    attribute = C_EXP("TA_ACT");
    stackSize = 4096;
    priority = composite.priority;
  };
  composite.eTDR           => Channel.eTDR;
  composite.eEventflag     => Channel.eEventflag;
  composite.eSemaphore     => RPCChannelMan.eSemaphore;
  composite.eDataqueue     => Channel.eDataqueue;
};
EOT
    f.close

  end

  #===  through cell コードを生成
  #
  #
  def gen_through_cell_code( file )

    gen_plugin_decl_code( file )

    file.print <<EOT
import( "#{@rpc_channel_celltype_file_name}" );
import( "#{$gen}/#{@shared_channel_ct_name}.cdl" );
EOT

    # nest = @next_cell.get_region.gen_region_str_pre file
    nest = @region.gen_region_str_pre file
    indent_str = "  " * nest
    if @next_cell_port_subscript then
      subscript = '[' + @next_cell_port_subscript.to_s + ']'
    else
      subscript = ""
    end

    # セルを探す
    # path =["::",@next_cell.get_name]
    # cell = Namespace.find( path )
    cell = Namespace.find( @next_cell.get_namespace_path )

    # PPAllocator が必要か?
    if @signature.need_PPAllocator? then
      if @sub_channel_no == 0 then
        file.print <<EOT
#{indent_str}cell tPPAllocator PPAllocator_#{@shared_channel_cell}{
#{indent_str}  heapSize = #{@PPAllocatorSize};
#{indent_str}};
EOT
      end

      ppallocator_join = "#{indent_str}  cPPAllocator = PPAllocator_#{@shared_channel_cell}.ePPAllocator;\n"
    else
      ppallocator_join = ""
    end

    # 共有される通信チャンネルの生成のプロトタイプ宣言
    file.print <<EOT
#{indent_str}cell #{@shared_channel_ct_name} #{@shared_channel_cell};
EOT

    # ここから各結合ごとのセルを生成

    # アロケータの指定があるか？
    if cell.get_allocator_list.length > 0 then

      file.print "#{indent_str}[allocator("

      delim = ""
      cell.get_allocator_list.each do |type, eport, subsc, func, buf, alloc|

        file.print delim
        delim = ",\n#{indent_str}           "        # 最終行には出さない

        if subsc then        # 配列添数
          subsc_str = '[#{subsc}]'
        else
          subsc_str = ""
        end

        eport = "eThroughEntry" #RPCの受け口名に変更
        file.print  "#{eport}#{subsc_str}.#{func}.#{buf} = #{alloc}"
      end

      file.puts ")]"
    end

    # セル本体の生成
    file.print <<EOT
#{indent_str}cell #{@rpc_channel_celltype_name} #{@cell_name} {
#{indent_str}  #{@call_port_name} = #{@next_cell.get_name}.#{@next_cell_port_name}#{subscript};
#{indent_str}  cTDR         = #{@shared_channel_cell}.eTDR;
#{indent_str}  cEventflag   = #{@shared_channel_cell}.eEventflag;
#{indent_str}  cLockChannel =  #{@shared_channel_cell}.eSemaphore[#{@sub_channel_no}];
#{ppallocator_join}#{indent_str}};
EOT
    @region.gen_region_str_post file
  end

  #=== 後ろのコードを生成
  #プラグインの後ろのコードを生成
  #file:: File: 
  def self.gen_post_code( file )
    file.print "/* '#{self.name}' post code */\n"
    @@shared_channel_list.each{ |chan_name,plugin_obj|
      plugin_obj[0].gen_post_code( file, plugin_obj )
    }
  end

  #=== 後ろのコードを生成
  # plugin_obj[0] が代表して出力する（インスタンス変数にアクセスしたいため）
  def gen_post_code( file, plugin_obj )

    chan_name = @shared_channel_cell

    nest = @region.gen_region_str_pre file
    indent_str = "  " * nest

    # 共有されている通信チャンネルの生成
    # 各プラグインインスタンスでは @shared_channel_ct_name として記憶している
    file.print "#{indent_str}cell tSharedRPCPlugin_#{@channelCelltype} #{chan_name} {\n"
    plugin_obj.each{ |po|
      file.print <<EOT
#{indent_str}    cUnmarshalAndCallFunction[] = #{@cell_name}.eUnmarshalAndCallFunction;
EOT
    }
    file.printf "#{indent_str}    priority = #{@task_priority};\n#{indent_str}};\n"
    @region.gen_region_str_post file

  end

end

