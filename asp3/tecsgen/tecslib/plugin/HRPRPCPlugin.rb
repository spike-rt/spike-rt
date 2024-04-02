# coding: utf-8
#
#  TECS Generator
#      Generator for TOPPERS Embedded Component System
#  
#   Copyright (C) 2008-2021 by TOPPERS Project
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
#  $Id: HRPRPCPlugin.rb 3241 2021-11-21 12:06:12Z okuma-top $
#++

require_tecsgen_lib "lib/GenOpaqueMarshaler.rb"
require_tecsgen_lib "lib/GenParamCopy.rb"

#= HRPRPCPlugin プラグイン
# スループラグイン (through)
# ・OpaqueMarshalerPlugin を使用してマーシャラセルタイプを生成する
# ・マーシャラ、TDR、チャンネル、メッセージバッファを生成する
#
class HRPRPCPlugin < ThroughPlugin

  include GenOpaqueMarshaler
  include GenParamCopy

  # RPCPlugin 専用のオプション
  HRPRPCPluginArgProc = RPCPluginArgProc.dup  # 複製を作って元を変更しないようにする
  HRPRPCPluginArgProc[ "noClientSemaphore"  ] = Proc.new { |obj,rhs| obj.set_noClientSemaphore rhs }
  HRPRPCPluginArgProc[ "semaphoreCelltype"  ] = Proc.new { |obj,rhs| obj.set_semaphoreCelltype rhs }
  @@isFirstInstance = true
  @@class_generated_region = {}     # { Region(parent) => region_name(generating) }

  #=== RPCPlugin の initialize
  #  説明は ThroughPlugin (plugin.rb) を参照
  def initialize( cell_name, plugin_arg, next_cell, next_cell_port_name, next_cell_port_subscript, signature, celltype, caller_cell )
    super
    @b_noClientSemaphore = false
    @semaphoreCelltype = "tSemaphore"
    initialize_opaque_marshaler

    # オプション：GenOpaqueMarshaler 参照
    @plugin_arg_check_proc_tab = HRPRPCPluginArgProc
    parse_plugin_arg

    @rpc_channel_celltype_name = "tRPCPlugin_#{@TDRCelltype}_#{@channelCelltype}_#{@signature.get_name}"
    @rpc_channel_celltype_file_name = "#{$gen}/#{@rpc_channel_celltype_name}.cdl"

    if @signature.get_context == "non-task" then
        cdl_error( "HRP9999 RPC cannot be applied to non-task context signature '$1'", @signature.get_name )
    elsif @signature.get_context == "any" then
        cdl_info( "HRP9999 RPC is applied to any context signature '$1'", @signature.get_name )
    end

    if @signature.need_PPAllocator?(true) then
      if @PPAllocatorSize == nil then
        cdl_error( "HRP9999 PPAllocatorSize must be speicified for pointer argments" )
        # @PPAllocatorSize = 0   # 仮に 0 としておく (cdl の構文エラーを避けるため)
      end
    elsif @PPAllocatorSize then
      cdl_warning( "HRP9999 PPAllocatorSize speicified in spite of PPAllocator unnecessary" )
      @PPAllocatorSize = nil
    end

#    @signature.each_param{ |func_decl, param_decl|
#      if func_decl.get_type.is_oneway? then
#        if ( param_decl.get_size || param_decl.get_count ) && param_decl.get_string then
#          cdl_error( "array of string not supported for oneway function in Transparent RPC" )  # mikan 文字列の配列
#        elsif param_decl.get_string == -1 then
#          cdl_error( "length unspecified string is not permited for oneway function in Transparent RPC" )  # mikan 長さ未指定文字列
#        end
#      end
#    }

    # 
    #  tecsgen/tecs/rpcにincludeパスを通す
    #  #include "tecs_rpc.h" を実現するために必要
    #    大山:削除 Makefile.tecsgen に vpath, INCLUDES を入れるのは、よくない考え
    #             TECSGEN.add_search_path で Makefile_templ に入れるのがよい
    # if @@isFirstInstance
    #     f = AppFile.open( "#{$gen}/Makefile.tecsgen" )
    #     f.puts "INCLUDES := $(INCLUDES) -I $(TECSPATH) -I $(TECSPATH)/rpc"
    #     f.puts "vpath %.c $(TECSPATH) $(TECSPATH)/rpc"
    #     f.close()
    #     @@isFirstInstance = false
    # end
  end

  #=== plugin の宣言コード (celltype の定義) 生成
  def gen_plugin_decl_code( file )

    ct_name = "#{@ct_name}_#{@channelCelltype}"

    # このセルタイプ（同じシグニチャ）は既に生成されているか？
    if @@generated_celltype[ ct_name ] == nil then
      @@generated_celltype[ ct_name ] = [ self ]
    else
      @@generated_celltype[ ct_name ] << self
      return
    end

    f = CFile.open( @rpc_channel_celltype_file_name, "w" )
    # 同じ内容を二度書く可能性あり (AppFile は不可)

    f.print <<EOT
/* HRPRPCPlugin:000 */
import( <rpc.cdl> );
import( <tMessageBufferCEP.cdl> );
generate( OpaqueMarshalerPlugin, #{@signature.get_namespace_path}, "" );

composite tOpaqueMarshaler_#{@signature.get_global_name}_through {
  entry #{@signature.get_namespace_path} eThroughEntry;
  call sTDR       cTDR;
  [optional]
    call sSemaphore cLockChannel;
  [optional]
    call sRPCErrorHandler cErrorHandler;

  cell tOpaqueMarshaler_#{@signature.get_global_name} Marshaler{
    cTDR          => composite.cTDR;
    cLockChannel  => composite.cLockChannel;
    cErrorHandler => composite.cErrorHandler;
  };
  composite.eThroughEntry => Marshaler.eClientEntry;
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
/* HRPRPCPlugin:000
 * caller_cell:#{@caller_cell.get_namespace_path}
 * next_cell:#{@next_cell.get_namespace_path}
 */ 
import( <#{@rpc_channel_celltype_file_name}> );
EOT

    # MessageBuffer チャンネルの生成 (OutOfDomain に生成)
    # アクセス許可ベクタの作成 (呼び元、呼び先のドメインからアクセス可能)
    case @start_region.get_domain_root.get_domain_type.get_kind
    when :kernel
      tacp_str = "\"TACP_KERNEL"
    when :user
      tacp_str = "\"TACP(#{@start_region.get_domain_root.get_name})"
    when :OutOfDomain
      tacp_str = "\"TACP_SHARED"
    end
    # p "end_region=#{@end_region.get_name} kind=#{@end_region.get_domain_root.get_domain_type.get_kind}"
    case @end_region.get_domain_root.get_domain_type.get_kind
    when :kernel
      tacp_str += "|TACP_KERNEL\""
    when :user
      tacp_str += "|TACP(#{@end_region.get_domain_root.get_name})\""
    when :OutOfDomain
      tacp_str += "|TACP_SHARED\""
    end

    node_root = @end_region.get_node_root
    nest = node_root.gen_region_str_pre file
    nest = print_class_region_str_pre node_root, file, nest
    nest_str = "    " * nest

    # MessageBuffer Client->Server
    file.print <<EOT
#{nest_str}/* HRPRPCPlugin001 */
#{nest_str}//  MessageBuffer client=>server
#{nest_str}cell tMessageBuffer #{@clientChannelCell}Body0{
#{nest_str}    maxMessageSize = 64;      /* This value must be same as MessageBufferCEP's buffer size */
#{nest_str}    bufferSize     = 128;
#{nest_str}    accessPattern1 = C_EXP( #{tacp_str} );
#{nest_str}    accessPattern2 = C_EXP( #{tacp_str} );
#{nest_str}    accessPattern3 = C_EXP( #{tacp_str} );
#{nest_str}    accessPattern4 = C_EXP( #{tacp_str} );
#{nest_str}};
EOT
    # MessageBuffer Server->Client
    file.print <<EOT

#{nest_str}/* HRPRPCPlugin002 */
#{nest_str}//  MessageBuffer server=>client
#{nest_str}cell tMessageBuffer #{@clientChannelCell}Body1{
#{nest_str}    maxMessageSize = 64;      /* This value must be same as MessageBufferCEP's buffer size */
#{nest_str}    bufferSize     = 128;
#{nest_str}    accessPattern1 = C_EXP( #{tacp_str} );
#{nest_str}    accessPattern2 = C_EXP( #{tacp_str} );
#{nest_str}    accessPattern3 = C_EXP( #{tacp_str} );
#{nest_str}    accessPattern4 = C_EXP( #{tacp_str} );
#{nest_str}};
EOT
    nest -= 1
    print_class_region_str_post node_root, file, nest
    node_root.gen_region_str_post file

    ##### クライアント側のセルの生成 #####
    nest = @start_region.gen_region_str_pre file
    nest = print_class_region_str_pre @start_region, file, nest
    nest_str = "    " * nest

    # セマフォの生成
    if @b_noClientSemaphore == false then
      file.print <<EOT

/* HRPRPCPlugin003 */
#{nest_str}//  Semaphore for Multi-task use ("specify noClientSemaphore" option to delete this)
#{nest_str}cell #{@semaphoreCelltype} #{@serverChannelCell}_Semaphore{
#{nest_str}    initialCount = 1;
#{nest_str}};
EOT
    end

    # クライアント側チャンネル (tMessageBufferCEP)の生成
    # チャンネルは必ずリージョン下にあるので、 '::' でつなぐ (でなければ、ルートリージョンにないかチェックが必要）

    # node_root が OutOfDomain でない場合、恐らく問題がある ***
    class_path = get_default_class_region_path @end_region.get_node_root
    file.print <<EOT
/* HRPRPCPlugin004 */
#{nest_str}//  Client Side Channel
#{nest_str}cell tMessageBufferCEP #{@clientChannelCell}_CEP{
#{nest_str}    cMessageBuffer0 = #{class_path}#{@clientChannelCell}Body0.eMessageBuffer;
#{nest_str}    cMessageBuffer1 = #{class_path}#{@clientChannelCell}Body1.eMessageBuffer;
#{nest_str}};

#{nest_str}//  Client Side TDR
#{nest_str}cell tTDR #{@clientChannelCell}_TDR{
#{nest_str}    cChannel = #{@clientChannelCell}_CEP.eChannel;
#{nest_str}};

#{nest_str}  //  Marshaler
EOT

    # セマフォの結合文
    if @b_noClientSemaphore == false then
      semaphore = "#{nest_str}    cLockChannel = #{@serverChannelCell}_Semaphore.eSemaphore;\n"
    else
      semaphore = ""
    end

    ### クライアント側チャンネル (マーシャラ+TDR)の生成 ###
    cell = @next_cell
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
/* HRPRPCPlugin005 */
#{nest_str}cell tOpaqueMarshaler_#{@signature.get_global_name}_through #{@cell_name} {
#{nest_str}    cTDR = #{@clientChannelCell}_TDR.eTDR;
#{clientErrorHandler_str}#{semaphore}#{nest_str}};
EOT

    ### END: クライアント側チャンネル (マーシャラ+TDR)の生成 ###
    nest -= 1
    print_class_region_str_post @start_region, file, nest
    @start_region.gen_region_str_post file
    file.print "\n\n"

    ##### サーバー側のセルの生成 #####
    nest = @end_region.gen_region_str_pre file
    nest = print_class_region_str_pre @end_region, file, nest
    nest_str = "    " * nest

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

    # サーバー側チャンネル (tMessageBufferCEP)
    if @PPAllocatorSize then
      alloc_cell =<<EOT

#{nest_str}cell tPPAllocator #{@serverChannelCell}_PPAllocator {
#{nest_str}    heapSize = #{@PPAllocatorSize};
#{nest_str}};
EOT
      alloc_call_port_join = "#{nest_str}    cPPAllocator = #{@serverChannelCell}_PPAllocator.ePPAllocator;\n"
    else
      alloc_cell = ""
      alloc_call_port_join = ""
    end

    file.print <<EOT
/* HRPRPCPlugin006 */
#{nest_str}//  Server Side Channel
#{nest_str}cell tMessageBufferCEP #{@serverChannelCell}_CEP{
#{nest_str}    cMessageBuffer0 = #{class_path}#{@clientChannelCell}Body1.eMessageBuffer;
#{nest_str}    cMessageBuffer1 = #{class_path}#{@clientChannelCell}Body0.eMessageBuffer;
#{nest_str}};
EOT

    # サーバー側TDR
    file.print <<EOT

/* HRPRPCPlugin007 */
#{nest_str}//  Server Side TDR
#{nest_str}cell tTDR #{@serverChannelCell}_TDR{
#{nest_str}    cChannel = #{@serverChannelCell}_CEP.eChannel;
#{nest_str}};

EOT

    if @next_cell_port_subscript then
      subscript = '[' + @next_cell_port_subscript.to_s + ']'
    else
      subscript = ""
    end

    # サーバー側チャンネル (アンマーシャラ)
    file.print <<EOT
/* HRPRPCPlugin008 */#{alloc_cell}
#{nest_str}//  Unmarshaler
#{nest_str}cell tOpaqueUnmarshaler_#{@signature.get_global_name} #{@serverChannelCell}_Unmarshaler {
#{nest_str}    cTDR        = #{@serverChannelCell}_TDR.eTDR;
#{nest_str}    cServerCall = #{@next_cell.get_namespace_path.get_path_str}.#{@next_cell_port_name}#{subscript};
#{alloc_call_port_join}#{serverErrorHandler_str}#{nest_str}};
EOT

    # サーバー側タスクメイン
    file.print <<EOT

/* HRPRPCPlugin009 */
#{nest_str}//  Unmarshaler Task Main
#{nest_str}cell #{@taskMainCelltype} #{@serverChannelCell}_TaskMain {
#{nest_str}    cMain         = #{@serverChannelCell}_Unmarshaler.eService;
#{opener}#{nest_str}};
EOT

    # サーバー側タスク
    file.print <<EOT

/* HRPRPCPlugin010 */
#{nest_str}//  Unmarshaler Task
#{nest_str}cell #{@taskCelltype} #{@serverChannelCell}_Task {
#{nest_str}    cTaskBody = #{@serverChannelCell}_TaskMain.eMain;
#{nest_str}    priority  = #{@taskPriority};
#{nest_str}    stackSize = #{@stackSize};
#{nest_str}    attribute = C_EXP( "TA_ACT" );  /* mikan : marshaler task starts at beginning */
#{nest_str}};
EOT
    nest -= 1
    print_class_region_str_post @end_region, file, nest
    @end_region.gen_region_str_post file
  end

  #=== デフォルトのクラス名、クラスリージョン名を返す
  # デフォルトのクラス名は "CLS_PRC1"
  # デフォルトのクラスリージョン名は "__CLS_P1_GLOBAL"
  def get_default_class_and_region region
    class_type = region.get_class_root.get_class_type
    if class_type == nil then
      return "", ""
    else
      return "CLS_PRC1", "__CLS_P1_GLOBAL"
    end
  end

  #=== OutofDomain なクラスへのパス
  # region
  def get_default_class_region_path region
    class_name, class_region = get_default_class_and_region region
    if class_region != "" then
      if region.is_root? then
        return "::" + class_region + "::"
      else
        return region.get_namespace_path.to_s + "::" + class_region + "::"
      end
    else
      return ""
    end
  end

  #=== ファイルにクラスリージョンの CDL での定義を出力
  def print_class_region_str_pre region, file, nest
    class_type = region.get_class_root.get_class_type
    if class_type == nil then
      return nest
    end
    klass = class_type.get_option
    dbgPrint "HRPRPCPlugin#print_class_region_str_pre class_type=#{class_type.get_name} class=#{klass}\n"
    print  "HRPRPCPlugin#print_class_region_str_pre class_type=#{class_type.get_name} class=#{klass}\n"

    nest_str = "    " * nest
    if klass == :root then
      klass, region_name = get_default_class_and_region region
    else
      return nest
    end
    region_name = region_name.to_s
    if region.is_root? then
      nsp = "::" + region_name
    else
      nsp = region.get_namespace_path.to_s + "::" + region_name
    end
    file.print "#{nest_str}/* HRPRPCPlugin011 */\n"
    region_namespacepath = NamespacePath.analyze( nsp )
    first = false
    if Namespace.find( region_namespacepath ) == nil then
      file. print "#{nest_str}[class(#{class_type.get_name},\"#{klass}\")]\n"
      first = true
    end
    dbgPrint "HRPRPCPlugin: In class class_type=#{class_type.class.name} class=#{klass} region=#{region.get_class_root.get_name} nsp=#{nsp} first=#{first}\n"
    file.print "#{nest_str}region #{region_name}{\n"
    return nest + 1
  end

  def print_class_region_str_post region, file, nest
    class_type = region.get_class_root.get_class_type
    if class_type == nil then
      return
    end
    klass = class_type.get_option
    if klass != :root then
      return
    end
    nest_str = "    " * nest
    file.print "#{nest_str}};\n"
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

  #=== プラグイン引数 semaphoreCelltype のチェック
  def set_semaphoreCelltype rhs
    @semaphoreCelltype = rhs.to_sym
    nsp = NamespacePath.analyze( @semaphoreCelltype.to_s )
    obj = Namespace.find( nsp )
    if ! obj.instance_of?( Celltype ) && ! obj.instance_of?( CompositeCelltype ) then
      cdl_error( "RPCPlugin: semaphoreCelltype '#{rhs}' not celltype or not defined" )
    end
  end

  #=== NamespacePath を得る
  # 生成するセルの namespace path を生成する
  def get_cell_namespace_path
#    nsp = @region.get_namespace.get_namespace_path
    nsp = @start_region.get_namespace_path
    class_path, class_region = get_default_class_and_region @start_region
    if class_region != "" then
      nsp = nsp.append( class_region.to_sym )
    end
    return nsp.append( @cell_name )
  end

end


