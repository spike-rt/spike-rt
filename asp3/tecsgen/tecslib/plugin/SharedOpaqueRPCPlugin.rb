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
#   $Id: SharedOpaqueRPCPlugin.rb 3140 2020-03-29 09:21:42Z okuma-top $
#++

require_tecsgen_lib "lib/GenOpaqueMarshaler.rb"
require_tecsgen_lib "lib/GenParamCopy.rb"

#= SharedOpaqueRPC プラグイン
# SharedOpaqueRPC チャンネルを生成する
# プラグイン引数は GenTransparentMarshaler 参照

# mikan through plugin: namespace が考慮されていない
# これを利用する場合、以下のように toppers_jsp.cdl sChannel.cdl を指定する必要がある
# tecsgen toppers_jsp.cdl sChannel.cdl your_description.cdl

# 以下を仮定（制限事項）
#  呼び元、呼び先のエンディアン、char, short, int_t, long_t, intptr_t のサイズが同じ
#  有符号、無符号でサイズが同じ

class SharedOpaqueRPCPlugin < ThroughPlugin

  # チャンネルを共有するプラグインオブジェクトへのハッシュリスト
  @@shared_channel_list = {}    # { chan_name => [ Plugin_obj0, Plugin_Obj1, ... ] }

  attr_reader :cell_name, :start_region, :end_region
  include GenOpaqueMarshaler
  include GenParamCopy

  # SharedOpaqueRPCPlugin 専用のオプション
  SharedOpaqueRPCPluginArgProc = RPCPluginArgProc.dup  # 複製を作って元を変更しないようにする
  SharedOpaqueRPCPluginArgProc[ "sharedChannelName"  ] = Proc.new { |obj,rhs| obj.set_sharedChannelName rhs }

  #=== RPCPlugin の initialize
  #  説明は ThroughPlugin (plugin.rb) を参照
  def initialize( cell_name, plugin_arg, next_cell, next_cell_port_name, next_cell_port_subscript, signature, celltype, caller_cell )
    super
    initialize_opaque_marshaler
    @entry_port_name = :"eClientEntry"   # Marshaler の受け口名 (through セルの入り口)

    # オプション：GenOpaqueMarshaler 参照
    @plugin_arg_check_proc_tab = SharedOpaqueRPCPluginArgProc
    @sharedChannelName = nil
    parse_plugin_arg
    check_opener_code
    check_PPAllocator

    @shared_channel_ct_name = :"tSharedOpaqueRPCPluginChannel_tTDR"
    @shared_channel_server_ct_name = :"#{@shared_channel_ct_name}_Server"
    @shared_channel_client_ct_name = :"#{@shared_channel_ct_name}_Client"
    @shared_channel_ct_file_name = "#{$gen}/#{@shared_channel_ct_name}.cdl"
    if @sharedChannelName == nil then
      cdl_error( "'sharedChannelName' option: mandatory")
    else
      @shared_channel_cell = @sharedChannelName
    end

    if @@shared_channel_list[ @shared_channel_cell ] == nil then
      @@shared_channel_list[ @shared_channel_cell ] = [ self ]
    else
      @@shared_channel_list[ @shared_channel_cell ] << self
    end
    @sub_channel_no = ( @@shared_channel_list[ @shared_channel_cell ].length ) -1

    prev_start = @@shared_channel_list[ @shared_channel_cell ][0].start_region
    if @start_region != prev_start then
      # 初出と start リージョン不一致 (初出は、自分自身とチェックされる。無駄だが小さいので放置)
      cdl_error( "SharedRPCPlugin: start region mismatch current: #{@region.get_name} previous: #{prev_start.get_name}")
    end

    prev_end = @@shared_channel_list[ @shared_channel_cell ][0].end_region
    if @end_region != prev_end then
      # 初出と end リージョン不一致  (初出は、自分自身とチェックされる。無駄だが小さいので放置)
      cdl_error( "SharedRPCPlugin: end region mismatch current: #{@region.get_name} previous: #{prev_end.get_name}")
    end

  end

  # def set_through_info( start_region, end_region, through_type )
  #   super
  # end

  #=== プラグイン引数 sharedChannelCell のチェック
  def set_sharedChannelName( rhs )
    @sharedChannelName = rhs
    # path = [ "::", rhs ]
    # obj = Namespace.find( path )
    # if ! obj.instance_of?( Cell ) then
    #   cdl_error( "SharedOpaqueRPCPlugin: sharedChannelName '#{rhs}' not cell" )
    # end
  end

  def gen_plugin_decl_code( file )

    # このセルタイプ（同じシグニチャ）は既に生成されているか？
    if @@generated_celltype[ @shared_channel_server_ct_name ] == nil then
      @@generated_celltype[ @shared_channel_server_ct_name ] = [ self ]
    else
      @@generated_celltype[ @shared_channel_server_ct_name ] << self
    end

    gen_marshaler_celltype

    # 同じ内容を二度書く可能性あり (AppFile は不可)

    if @PPAllocatorSize then
      alloc_call_port = "  call sPPAllocator cPPAllocator;\n"
      alloc_call_port_join = "  cPPAllocator => composite.cPPAllocator;\n"
    else
      alloc_call_port = ""
      alloc_call_port_join = ""
    end

    f = CFile.open( @shared_channel_ct_file_name, "w" )
    # 同じ内容を二度書く可能性あり (AppFile は不可)

    f.print <<EOT

/* Shared Channel Celltype for Client */
composite #{@shared_channel_client_ct_name} {
  entry  sTDR       eTDR;
  entry  sSemaphore eSemaphore[];
  call   sChannel   cClientChannel;

  cell tTDR TDR {
    cChannel => composite.cClientChannel;
  };
  cell tSemaphore Semaphore {
    initialCount = 1;
    attribute = C_EXP("TA_NULL");
  };
  cell tRPCSharedChannelMan SharedChannelMan{
    cSemaphore             = Semaphore.eSemaphore;
    cClientSideTDR         = TDR.eTDR;
  };
  composite.eSemaphore     => SharedChannelMan.eSemaphore;
  composite.eTDR           => TDR.eTDR;
};

/* Shared Channel Celltype for Server */
[active]
composite #{@shared_channel_server_ct_name} {
  entry  sTDR       eTDR;
  call   sChannel   cServerChannel;
  call   sUnmarshalerMain  cUnmarshalAndCallFunction[];
  call   sServerChannelOpener cOpener;
  attr {
    PRI    priority;
  };

  cell tTDR TDR {
    cChannel => composite.cServerChannel;
  };
  cell tRPCSharedTaskMainWithOpener RPCSharedTaskMain {
    cUnmarshalAndCallFunction => composite.cUnmarshalAndCallFunction;
    cServerSideTDR = TDR.eTDR;
    cOpener => composite.cOpener;
  };
  cell tTask Task {
    cTaskBody = RPCSharedTaskMain.eMain;
    attribute = C_EXP("TA_ACT");
    stackSize = 4096;
    priority = composite.priority;
  };
  composite.eTDR           => TDR.eTDR;
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

import( "#{@marshaler_celltype_file_name}" );
import( "#{@shared_channel_ct_file_name}" );
EOT

#    nest = @caller_cell.get_region.gen_region_str_pre file
    nest = @start_region.gen_region_str_pre file
    indent_str = "  " * nest
    nest_str = "  " * nest
    if @next_cell_port_subscript then
      subscript = '[' + @next_cell_port_subscript.to_s + ']'
    else
      subscript = ""
    end

    # セルを探す
    # path =["::",@next_cell.get_name]
    # cell = Namespace.find( path )
    cell = Namespace.find( @next_cell.get_namespace_path )

    # 共有される通信チャンネルの生成のプロトタイプ宣言
    file.print <<EOT
#{indent_str}cell #{@shared_channel_client_ct_name} #{@shared_channel_cell};
EOT

    # マーシャラセルの生成（アロケータコードの生成から）
    # アロケータの指定があるか？
    if cell.get_allocator_list.length > 0 then

      file.print "#{indent_str}[allocator("

      delim = ""
      cell.get_allocator_list.each do |type, eport, subsc, func, buf, alloc|

        alloc_str = alloc.to_s
        subst = @substituteAllocator[alloc_str.to_sym]

        if subst then
          alloc_str = subst[2]+"."+subst[3]
        end

        file.print delim
        delim = ",\n#{indent_str}           "        # 最終行には出さない

        if subsc then        # 配列添数
          subsc_str = '[#{subsc}]'
        else
          subsc_str = ""
        end

        eport = @entry_port_name #RPCの受け口名に変更
        file.print  "#{eport}#{subsc_str}.#{func}.#{buf} = #{alloc_str}"
      end

      file.puts ")]"
    end

    # マーシャラセル本体生成
    file.print <<EOT
/* OpaqueRPC Marshaler Cell */
#{indent_str}cell #{@marshaler_celltype_name} #{@cell_name} {
#{indent_str}  cTDR         = #{@shared_channel_cell}.eTDR;
#{indent_str}  cLockChannel = #{@shared_channel_cell}.eSemaphore[#{@sub_channel_no}];
#{indent_str}};
EOT
    # @caller_cell.get_region.gen_region_str_post file
    @start_region.gen_region_str_post file

    # アンマーシャラセルの出力
    # セル本体の生成
    nest = @end_region.gen_region_str_pre file

    file.print <<EOT
/* Server Channel Cell prototype */
#{indent_str}cell #{@shared_channel_server_ct_name} #{@sharedChannelName}_Server;

EOT

    # PPAllocator が必要か?
    if @PPAllocatorSize then
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

    file.print <<EOT
#{indent_str}cell #{@unmarshaler_celltype_name} #{@cell_name}_Server {
#{indent_str}  cTDR         = #{@shared_channel_cell}_Server.eTDR;
#{indent_str}  cServerCall  = #{@next_cell.get_namespace_path.get_path_str}.#{@next_cell_port_name}#{subscript};
#{ppallocator_join}#{indent_str}};
EOT
    @end_region.gen_region_str_post file

  end

  #=== post コード(CDL) を生成
  # プラグインの後のコードを生成
  #file:: File: 
  def self.gen_post_code( file )

    file.print "/* '#{self.name}' post code */\n"

    @@shared_channel_list.each{ |chan_name,plugin_obj_array|
      file.print "/* '#{chan_name}' shared channel */\n"
      plugin_obj_array[0].gen_post_code( file, plugin_obj_array )
    }

  end

  #=== post コード(CDL) を生成
  # 共有チャンネルを生成する
  # このメソッドは、チャンネルを共有する最初のプラグインオブジェクトのみ呼び出される
  def gen_post_code( file, plugin_obj_array )

    # 共有されている通信チャンネルの生成
    # 各プラグインインスタンスでは @shared_channel_ct_name として記憶している
    # region = @caller_cell.get_region
    nest = @start_region.gen_region_str_pre file
    indent_str = "  " * nest
    file.print <<EOT
#{indent_str}cell #{@shared_channel_client_ct_name} #{@sharedChannelName} {
#{indent_str}    cClientChannel = #{@clientChannelCell}.eC0;
#{indent_str}};
EOT
    @start_region.gen_region_str_post file

    nest = @end_region.gen_region_str_pre file
    indent_str = "  " * nest
    file.print "#{indent_str}cell #{@shared_channel_server_ct_name} #{@sharedChannelName}_Server {\n"
      file.print <<EOT
#{indent_str}    cServerChannel = #{@serverChannelCell}.eC1;
#{indent_str}    cOpener        = #{@serverChannelCell}.eOpener;
EOT
    plugin_obj_array.each{ |po|
      file.print <<EOT
#{indent_str}    cUnmarshalAndCallFunction[] = #{po.cell_name}_Server.eService;
EOT
    }
    file.printf <<EOT
#{indent_str}    priority = #{@taskPriority};
#{indent_str}};
EOT
    @end_region.gen_region_str_post file

  end
end


