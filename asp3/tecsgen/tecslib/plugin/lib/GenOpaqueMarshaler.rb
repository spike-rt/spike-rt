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
#   $Id: GenOpaqueMarshaler.rb 3176 2020-10-25 08:07:05Z okuma-top $
#++

#== GenOpaqueMarshaler
# OpaqueRPCPlugin, sharedOpaqueRPCPlugin 共通の要素を集めたモジュール
module GenOpaqueMarshaler

  # プラグイン引数名と Proc
  RPCPluginArgProc = {
    "clientChannelCelltype" => Proc.new { |obj,rhs| obj.set_clientChannelCelltype rhs },
    "serverChannelCelltype" => Proc.new { |obj,rhs| obj.set_serverChannelCelltype rhs },
    "clientChannelCell" => Proc.new { |obj,rhs| obj.set_clientChannelCell rhs },
    "serverChannelCell" => Proc.new { |obj,rhs| obj.set_serverChannelCell rhs },
    "clientChannelInitializer" => Proc.new { |obj,rhs| obj.set_clientChannelInitializer rhs },
    "serverChannelInitializer" => Proc.new { |obj,rhs| obj.set_serverChannelInitializer rhs },
    "clientSemaphoreCelltype" => Proc.new { |obj,rhs| obj.set_clientSemaphoreCelltype rhs },
    "clientSemaphoreInitializer" => Proc.new { |obj,rhs| obj.set_clientSemaphoreInitializer rhs },
    "clientErrorHandler" => Proc.new { |obj,rhs| obj.set_clientErrorHandler rhs },
    "serverErrorHandler" => Proc.new { |obj,rhs| obj.set_serverErrorHandler rhs },
    "TDRCelltype"     => Proc.new { |obj,rhs| obj.set_TDRCelltype rhs },
    "PPAllocatorSize" => Proc.new { |obj,rhs| obj.set_PPAllocatorSize rhs },
    "substituteAllocator"  => Proc.new { |obj,rhs| obj.set_substituteAllocator rhs },
    "noServerChannelOpenerCode"  => Proc.new { |obj,rhs| obj.set_noServerChannelOpenerCode rhs },
    "taskCelltype"    => Proc.new { |obj,rhs| obj.set_taskCelltype rhs },
    "taskPriority"    => Proc.new { |obj,rhs| obj.set_taskPriority rhs },
    "stackSize"      => Proc.new { |obj,rhs| obj.set_stackSize rhs },
  }

  ##### プラグイン引数チェック関数

  #=== プラグイン引数 taskPriority のチェック
  def set_taskPriority( rhs )
    @taskPriority = rhs
  end

  #=== プラグイン引数 serverChannelCelltype のチェック
  def set_serverChannelCelltype( rhs )
    @serverChannelCelltype = rhs.to_sym
    # path = [ "::", @serverChannelCelltype ]
    # obj = Namespace.find( path )
    nsp = NamespacePath.analyze( @serverChannelCelltype.to_s )
    obj = Namespace.find( nsp )
    if ! obj.instance_of?( Celltype ) && ! obj.instance_of?( CompositeCelltype ) then
      cdl_error( "RPCPlugin: serverChannelCelltype '#{rhs}' not celltype or not defined" )
    end
  end

  #=== プラグイン引数 clientChannelCelltype のチェック
  def set_clientChannelCelltype( rhs )
    @clientChannelCelltype = rhs.to_sym
    # path = [ "::", @clientChannelCelltype ]
    # obj = Namespace.find( path )
    nsp = NamespacePath.analyze( @clientChannelCelltype.to_s )
    obj = Namespace.find( nsp )
    if ! obj.instance_of?( Celltype ) && ! obj.instance_of?( CompositeCelltype ) then
      cdl_error( "RPCPlugin: clientChanneclCelltype '#{rhs}' not celltype or not defined" )
    end
  end

  #=== プラグイン引数 serverChannelCell のチェック
  def set_serverChannelCell( rhs )
    @serverChannelCell = rhs.to_sym
    # ChannelCell はプラグインで生成されるため、ここではチェックできない
    # path = [ "::", @serverChannelCell ]
    # obj = Namespace.find( path )
    # if ! obj.instance_of?( Cell ) then
    #   cdl_error( "RPCPlugin: serverChanneclCell '#{rhs}' not cell or not defined" )
    # end
  end

  #=== プラグイン引数 clientChannelCell のチェック
  def set_clientChannelCell( rhs )
    @clientChannelCell = rhs.to_sym
    # ChannelCell はプラグインで生成されるため、ここではチェックできない
    # path = [ "::", @clientChannelCell ]
    # obj = Namespace.find( path )
    # if ! obj.instance_of?( Cell ) then
    #   cdl_error( "RPCPlugin: clientChanneclCell '#{rhs}' not cell or not defined" )
    # end
  end

  #=== プラグイン引数 serverChannelInitializer のチェック
  def set_serverChannelInitializer( rhs )
    @serverChannelInitializer = rhs.to_sym
  end

  #=== プラグイン引数 clientChannelInitializer のチェック
  def set_clientChannelInitializer( rhs )
    @clientChannelInitializer = rhs.to_sym
  end

  #=== タスクタイプ taskCellype のチェック
  def set_taskCelltype( rhs )
    @taskCelltype = rhs.to_sym
    # path = [ "::", @taskCelltype ]
    # obj = Namespace.find( path )
    nsp = NamespacePath.analyze( @taskCelltype.to_s )
    obj = Namespace.find( nsp )
    if ! obj.instance_of?( Celltype ) && ! obj.instance_of?( CompositeCelltype ) then
      cdl_error( "RPCPlugin: taskCelltype '#{rhs}' not celltype or not defined" )
    end
  end

  #=== タスクタイプ stack\size のチェック
  def set_stackSize( rhs )
    @stackSize = rhs
  end

  #=== プラグイン引数 PPAllocatorSize のチェック
  def set_PPAllocatorSize( rhs )
    @PPAllocatorSize = rhs
  end

  #=== プラグイン引数 TDRCelltype のチェック
  def set_TDRCelltype( rhs )
    @TDRCelltype = rhs.to_sym
    # path = [ "::", @TDRCelltype ]
    # obj = Namespace.find( path )
    nsp = NamespacePath.analyze( @TDRCelltype.to_s )
    obj = Namespace.find( nsp )
    if ! obj.instance_of?( Celltype ) && ! obj.instance_of?( CompositeCelltype ) then
      cdl_error( "RPCPlugin: TDRCelltype '#{rhs}' not celltype or not found" )
    end
  end

  #=== プラグイン引数 substituteAllocator のチェック
  # オプション引数が、以下の形式であることをチェック
  #     substituteAllocator(Alloc.eAlloc=>Subst.eAlloc,Alloc2.eAlloc=>Subst2.eAlloc)
  def set_substituteAllocator( rhs )
    #str::String : 破壊される（マッチした残りになる）。str.empty? で空になったことをチェックできる
    #regexp::Regexp : 期待するトークンにマッチする正規表現。 "\A" 出始める
    #expected::String: 期待するトークン、regexp が出現しなかった場合にエラーメッセージとして表示
    def optparse (str,regexp,expected)
      str.strip!
      token = nil
      res = str.sub!( regexp ){ |matched|  token = matched;  "" }
      if ! token then
        cdl_error( "syntax error in substituteAllocator option near '#{str}', expected '#{expected}'" )
      end
      return token
    end

    opt = rhs.dup
    ident_rexpr = /\A(\w[\w\d]*)/

    # "Alloc.eAlloc=>CAlloc.eAlloc" の形式になっていることをチェック
    while true
      lhs_alloc_cell = optparse( opt, ident_rexpr, "allocator cell name" )
      break if ! lhs_alloc_cell

      res = optparse( opt, /\A\./, "." )
      break if ! res

      lhs_alloc_ent = optparse( opt, ident_rexpr, "allocator cell entry name" )
      break if ! lhs_alloc_ent

      res = optparse( opt, /\A\=\>/, "=>" )
      break if ! res

      rhs_alloc_cell = optparse( opt, ident_rexpr, "allocator cell name" )
      break if ! rhs_alloc_cell

      res = optparse( opt, /\A\./, "." )
      break if ! res

      rhs_alloc_ent = optparse( opt, ident_rexpr, "allocator cell entry name" )
      break if ! rhs_alloc_ent

#  ここでは、右辺のチェックはできない。右辺のセルは前方参照となる
#      path = [ "::", rhs_alloc_cell.to_sym ]   # mikan namespace
#      obj = Namespace.find( path )
#      if ! obj.instance_of?( Cell ) || obj.get_region.get_path_string != @clientRegion then
#        cdl_error( "RPCPlugin: substituteAllocator: '#{rhs_alloc_cell}' not cell or not found in client region" )
#      else
#        ct = obj.get_celltype
#        if ct #  nil なら既にエラー
#          ent = ct.find rhs_alloc_ent
#          if ! ent.instance_of? Port || ent.get_port_type != :ENTRY || ent.get_signature == nil || ! ent.get_signature.is_allocator?
#            cdl_error( "RPCPlugin: substituteAllocator: '#{rhs_alloc_cell}.#{rhs_alloc_ent}' not entry port or not have alllocator signature" )
#          end
#        end
#      end

      @substituteAllocator[ "#{lhs_alloc_cell}.#{lhs_alloc_ent}".to_sym ] =
        [ lhs_alloc_cell, lhs_alloc_ent, rhs_alloc_cell, rhs_alloc_ent ]

# p "substituteAllocator: #{lhs_alloc_cell}.#{lhs_alloc_ent}=>#{rhs_alloc_cell}.#{rhs_alloc_ent}"

      break if opt.empty?

      res = optparse( opt, /\A\,/, "," )
      break if ! res
    end

  end

  #=== プラグイン引数 noServerChannelOpenerCode のチェック
  def set_noServerChannelOpenerCode( rhs )
    rhs = rhs.to_sym
    if rhs == :true
      @noServerChannelOpenerCode = true
    elsif rhs == :false then
      @noServerChannelOpenerCode = false
    else
      cdl_error( "RPCPlugin: specify true or false for noServerChannelOpenerCode" )
    end
  end

  #=== プラグイン引数 clientSemaphoreCelltype のチェック
  def set_clientSemaphoreCelltype rhs
    @semaphoreCelltype = rhs.to_sym
    nsp = NamespacePath.analyze( @semaphoreCelltype.to_s )
    obj = Namespace.find( nsp )
    if ! obj.instance_of?( Celltype ) && ! obj.instance_of?( CompositeCelltype ) then
      cdl_error( "RPCPlugin: clientSemaphoreCelltype '#{rhs}' not celltype or not defined" )
    end
  end

  #=== プラグイン引数 clientSemaphoreInitializer のチェック
  def set_clientSemaphoreInitializer rhs
    @semaphoreInitializer = rhs.to_sym
  end

  #=== プラグイン引数 clientErrorHandler のチェック
  def set_clientErrorHandler rhs
    @clientErrorHandler = rhs.to_sym
  end

  #=== プラグイン引数 serverErrorHandler のチェック
  def set_serverErrorHandler rhs
    @serverErrorHandler = rhs.to_sym
  end


  #===  セルの名前を得る
  # ThroughPlugin::get_cell_name plugin.rb をオーバーライド
  def get_cell_name
    @cell_name
    # @clientChannelCell
  end

  #=== marshaler のセルタイプ名を設定する
  def initialize_opaque_marshaler

    # オプション設定される変数のデフォルトを設定
    # @taskPriority = 11
    @taskPriority = 9
    @stackSize  = 4096
    @serverChannelCelltype = :"tSocketServer"
    @clientChannelCelltype = :"tSocketClient"
    @serverChannelCell = :"#{@cell_name}Server"
    @clientChannelCell = :"#{@cell_name}Client"
    @serverChannelInitializer = subst_name( "portNo=8931+$count$;" ).to_sym
    @clientChannelInitializer = subst_name( "portNo=8931+$count$; serverAddr=\"127.0.0.1\"; " ).to_sym
    @taskCelltype = :"tTask"
    @PPAllocatorSize = nil
    # @TDRCelltype  = :"tTDR"   # "tNBOTDR" に変更の予定
    @TDRCelltype  = :"tNBOTDR"
    @substituteAllocator = {}
    @noServerChannelOpenerCode = false
    @semaphoreCelltype = :"tSemaphore"
    @semaphoreInitializer = :"initialCount = 1; attribute = C_EXP( \"TA_NULL\" ); ";
    @clientErrorHandler = nil
    @serverErrorHandler = nil
    @b_genOpener = false
    @taskMainCelltype = :"tRPCDedicatedTaskMain"

    @marshaler_celltype_name = :"tOpaqueMarshaler_#{@signature.get_global_name}"
    @unmarshaler_celltype_name = :"tOpaqueUnmarshaler_#{@signature.get_global_name}"
    @marshaler_celltype_file_name = "#{$gen}/#{@marshaler_celltype_name}.cdl"

    if @signature.get_context != "task" then
      cdl_error( "OPQ9999 context of signature ($1) must be 'task'. $2 is not compatible with RPCPlugin", @signature.get_name, @signature.get_context )
    end

    # signature で対応できないものをチェック
    @signature.each_param{ |func_decl, param_decl|
      if param_decl.get_direction == :OUT then
        if param_decl.get_count && ! param_decl.get_size then
          cdl_error( "#{@signature.get_namespace_path}.#{func_decl.get_name}.#{param_decl.get_name}: size_is must be specified for out parameter of Opaque RPC" )
        end
        if param_decl.get_string == -1 then
          cdl_error( "#{@signature.get_namespace_path}.#{func_decl.get_name}.#{param_decl.get_name}: string length must be specified for out parameter of Opaque RPC" )
        end
      end
    }
  end

  #=== GenOpaqueMarshaler# Opener Code の生成時のチェック
  def check_opener_code
    # サーバーチャンネルセルタイプが entry sServerChannelOpener eOpener を持つかどうかをチェック
    # mikan entry か (call でないか) をチェックしていない
    # scct = Namespace.find ["::", @serverChannelCelltype] # mikan namespace
    nsp = NamespacePath.analyze( @serverChannelCelltype.to_s )
    scct = Namespace.find nsp
    if scct then
      obj = scct.find( :"eOpener" )
      if obj.instance_of? Port then
        if obj.get_signature.get_name.to_sym == :sServerChannelOpener then
          if @noServerChannelOpenerCode == false then
            @b_genOpener = true
            @taskMainCelltype = :"tRPCDedicatedTaskMainWithOpener"
          end
        end
      end
    end
    if @noServerChannelOpenerCode == false && @taskMainCelltype != :"tRPCDedicatedTaskMainWithOpener" then
      cdl_warning( "O9999 ServerChannelOpener code not generated, not found 'entry sServerChannelOpener eOpener'")
    end
  end

  #=== GenOpaqueMarshaler# PPAllocator の必要性をチェックする
  def check_PPAllocator
    if @signature.need_PPAllocator?(true) then
      if @PPAllocatorSize == nil then
        cdl_error( "PPAllocatorSize must be speicified for size_is array" )
      end
    end
  end

  ##### 

  def gen_marshaler_celltype
    f = CFile.open( @marshaler_celltype_file_name, "w" )
    # 同じ内容を二度書く可能性あり (AppFile は不可)

    if @PPAllocatorSize then
      alloc_call_port = "  call sPPAllocator cPPAllocator;\n"
    else
      alloc_call_port = ""
    end

    f.print <<EOT
// GenOpaqueMarshler0001
celltype #{@marshaler_celltype_name} {
  entry #{@signature.get_namespace_path} eClientEntry;
  call sTDR       cTDR;
  [optional]
    call sSemaphore cLockChannel;
  [optional]
    call sRPCErrorHandler cErrorHandler;
};
celltype #{@unmarshaler_celltype_name} {
  call #{@signature.get_namespace_path} cServerCall;
  call  sTDR       cTDR;
  [optional]
    call sRPCErrorHandler cErrorHandler;
  entry sUnmarshalerMain  eService;
#{alloc_call_port}};
EOT
    f.close
  end

  #===  受け口関数の本体コードを生成（頭部と末尾は別途出力）
  #ct_name:: Symbol    (through プラグインで生成された) セルタイプ名 ．Symbol として送られてくる（らしい）
  def gen_ep_func_body( file, b_singleton, ct_name, global_ct_name, sig_name, ep_name, func_name, func_global_name, func_type, params )

    # unmarshaler クラスか?
    if ct_name == @unmarshaler_celltype_name.to_sym then
      gen_ep_func_body_unmarshal( file, b_singleton, ct_name, global_ct_name, sig_name, ep_name, func_name, func_global_name, func_type, params )
    else
      gen_ep_func_body_marshal( file, b_singleton, ct_name, global_ct_name, sig_name, ep_name, func_name, func_global_name, func_type, params )
    end
  end

  #===  marshal コードの生成
  def gen_ep_func_body_marshal( file, b_singleton, ct_name, global_ct_name, sig_name, ep_name, func_name, func_global_name, func_type, params )

    b_void = false
    b_ret_er = false

    # 関数の戻り値の元の型を得る(typedef されている場合)
    type = func_type.get_type.get_original_type

    file.print( "\t// GenOpaqueMarshler0010\n" )
    # 戻り値記憶用の変数を出力（void 型の関数では出力しない）
    if ! type.is_void? then
      file.print( "\t#{func_type.get_type.get_type_str}\t\tretval_;\n" )
      if func_type.get_type.kind_of?( DefinedType ) && ( func_type.get_type.get_type_str == "ER" || func_type.get_type.get_type_str == "ER_INT" ) then
        b_ret_er = true
      end
    else
      b_void = true
    end

    file.print( "\tER\t\tercd_;\n" )
    file.print( "\tint16_t\tstate_;\n" )

    # 関数 ID （整数値）
    func_id = "FUNCID_#{@signature.get_global_name}_#{func_name}".upcase
    fid = @signature.get_id_from_func_name( func_name )
    file.print( "\tint16_t\tfunc_id_ = #{func_id};	/* (id of '#{func_name}') = #{fid}*/\n" )

    # シングルトンでないか？
    if ! b_singleton then

      # singleton でなければ p_cellcb 取得コードを出力
      file.print <<EOT
	#{ct_name}_CB *p_cellcb;

	if( VALID_IDX( idx ) ){
		p_cellcb = GET_CELLCB(idx);
EOT

      # エラーを返すか？
      if b_ret_er then
        file.print <<EOT
	}else{
		return ERCD( E_RPC, E_ID );
	}
EOT
      else
        file.print <<EOT
	}else{
		/* エラー処理コードをここに記述 */
	}

EOT
      end
    end

    if func_type.has_receive? then
      file.print "	/* initialize receive parameters */\n"
      params.each{ |param|
        if param.get_direction == :RECEIVE then
          file.print "	*#{param.get_name} = 0;\n"
        end
      }
    end

    # channel lock コード
    file.print <<EOT
	/* Channel Lock (GenOpaqueMarshler0012) */
	SET_RPC_STATE( state_, RPCSTATE_CLIENT_GET_SEM );
	if( is_cLockChannel_joined() ){
		if( (ercd_=cLockChannel_wait()) != E_OK )
			goto error_reset;
	}
EOT

    # SOP を送信
    file.print "	/* SOPの送出 (GenOpaqueMarshler0013) */\n"
    file.print "	SET_RPC_STATE( state_, RPCSTATE_CLIENT_SEND_SOP );\n"
    file.print "	if( ( ercd_ = cTDR_sendSOP( true ) ) != E_OK )\n"
    file.print "		goto error_reset;\n"

    # func_id を送信
    file.print "	/* 関数 id の送出 (GenOpaqueMarshler0014) */\n"
    file.print "	if( ( ercd_ = cTDR_putInt16( func_id_ ) ) != E_OK )\n"
    file.print "		goto error_reset;\n"

    # p "celltype_name, sig_name, func_name, func_global_name"
    # p "#{ct_name}, #{sig_name}, #{func_name}, #{func_global_name}"

    b_get = false    # marshal なら put
    b_marshal = true  # marshal

    # in 方向の入出力を出力
    if func_type.has_inward? then
      file.print "	/* 入力引数送出 (GenOpaqueMarshler0015) */\n"
      file.print "	SET_RPC_STATE( state_, RPCSTATE_CLIENT_SEND_BODY );\n"
      print_params( params, file, 1, b_marshal, b_get, true, "eClientEntry", func_name )
      print_params( params, file, 1, b_marshal, b_get, false, "eClientEntry", func_name )
    end
    print_out_nullable( params, file, 1, b_marshal );

    if ! func_type.is_oneway? then
      b_continue = "true"
    else
      b_continue = "false"
    end
    file.print "	/* EOPの送出（パケットの掃きだし）(GenOpaqueMarshler0016) */\n"
    file.print "	SET_RPC_STATE( state_, RPCSTATE_CLIENT_SEND_EOP );\n"
    file.print "	if( (ercd_=cTDR_sendEOP(#{b_continue})) != E_OK )\n"
    file.print "		goto error_reset;\n\n"

    # send のメモリをデアロケート
    if func_type.has_send? then
      file.print "	/* dealloc send parameter while executing (GenOpaqueMarshler0017) */\n"
      file.print "	SET_RPC_STATE( state_, RPCSTATE_CLIENT_EXEC );\n"
      dir = :SEND; nest = 1; dealloc_cp = "eClientEntry_#{func_name}"
      dealloc_for_params( params, file, nest, dir, dealloc_cp )
      file.print "\n"
    end

    if ! func_type.is_oneway? then

      file.print "	/* パケットの始まりをチェック (GenOpaqueMarshler0018) */\n"
      file.print "	SET_RPC_STATE( state_, RPCSTATE_CLIENT_RECV_SOP );\n"
      file.print "	if( (ercd_=cTDR_receiveSOP( true )) != E_OK )\n"
      file.print "		goto error_reset;\n"

      b_get = true     # marshaler は get
      file.print "	/* 戻り値の受け取り (GenOpaqueMarshler0019) */\n"
      print_param( "retval_", func_type.get_type, file, 1, :RETURN, nil, nil, b_marshal, b_get )

      if func_type.has_outward? then
        if b_ret_er then
          indent_level = 2
          file.print "	if( MERCD( retval_ ) != E_RPC ){\n"
        else
          indent_level = 1
        end
        indent = "	" * indent_level

        file.print "#{indent}/* 出力値の受け取り (GenOpaqueMarshler0020) */\n"
        file.print "#{indent}SET_RPC_STATE( state_, RPCSTATE_CLIENT_RECV_BODY );\n"
        print_params( params, file, indent_level, b_marshal, b_get, true, "eClientEntry", func_name )
        print_params( params, file, indent_level, b_marshal, b_get, false, "eClientEntry", func_name )

        if b_ret_er then
          file.print "	}\n"
        end
      end

      file.print "\n	/* パケットの終わりをチェック (GenOpaqueMarshler0021) */\n"
      file.print "	SET_RPC_STATE( state_, RPCSTATE_CLIENT_RECV_EOP );\n"
      file.print "	if( (ercd_=cTDR_receiveEOP(false)) != E_OK )\n"  # b_continue = false
      file.print "		goto error_reset;\n"

    end # ! func_type.is_oneway?

    # channel unlock コード
    file.print <<EOT
	/* Channel Unlock (GenOpaqueMarshler0022) */
	SET_RPC_STATE( state_, RPCSTATE_CLIENT_RELEASE_SEM );
	if( is_cLockChannel_joined() ){
		if( (ercd_=cLockChannel_signal()) != E_OK )
			goto error_reset;
	}
EOT

    file.print <<EOT
	/* state_ is not used in normal case (GenOpaqueMarshler0023) */
  /* below is to avoid 'set but not used' warnning */
	(void)state_;
EOT

    if( b_void == false )then
      # 呼び元に戻り値をリターン
      file.print( "	return retval_;\n" )
    else
      file.print( "	return;\n" )
    end

    file.print <<EOT

error_reset:
EOT
    # send のメモリをデアロケート
    if func_type.has_send? then
      file.print "	/* dealloc send parameter (GenOpaqueMarshler0024) */\n"
      file.print "	if( state_ < RPCSTATE_CLIENT_EXEC ){\n"
      dir = :SEND; nest = 2; dealloc_cp = "eClientEntry_#{func_name}"
      dealloc_for_params( params, file, nest, dir, dealloc_cp )
      file.print "	}\n"
    end

    # receive のメモリをデアロケート
    if func_type.has_receive? then
      file.print( "	/* receive parameter (GenOpaqueMarshler0025) */\n" )
      dir = :RECEIVE; nest = 1; dealloc_cp = "eClientEntry_#{func_name}"
      dealloc_for_params( params, file, nest, dir, dealloc_cp, true )
    end

    file.print <<EOT
	if( MERCD( ercd_ ) != E_RESET )
		(void)cTDR_reset();
EOT

    # channel lock コード
    file.print <<EOT
	/* Channel Unlock (GenOpaqueMarshler0026) */
	if( is_cLockChannel_joined() )
		cLockChannel_signal();

	if( ercd_ != E_OK && is_cErrorHandler_joined() )
		cErrorHandler_errorOccured( func_id_, ercd_, state_ );
EOT

    if( b_ret_er != false )then
      # 呼び元に戻り値をリターン
      file.print( "	return ERCD( E_RPC, MERCD( ercd_ ) ); /* (GenOpaqueMarshler0027) */\n" )
    else
      file.print( "	return;/* (GenOpaqueMarshler0028) */\n" )
    end

  end

  #===  unmarshal コードの生成
  def gen_ep_func_body_unmarshal( file, b_singleton, ct_name, global_ct_name, sig_name, ep_name, func_name, func_global_name, func_type, params )

    b_ret_er = true

    # func_id を得るコードを生成
    file.print <<EOT
  /* (GenOpaqueMarshler0101) */
	int16_t	func_id_;
	ER		ercd_ = E_OK;
	int16_t	state_;

	#{ct_name}_CB *p_cellcb;

	if( VALID_IDX( idx ) ){
		p_cellcb = GET_CELLCB(idx);
EOT

    if b_ret_er then
        file.print <<EOT
	}else{
		return E_ID;
	}
EOT
    else
        file.print <<EOT
	}else{
		/* エラー処理コードをここに記述 */
	}
EOT
    end

    file.print <<EOT

  /* (GenOpaqueMarshler0102) */
#ifdef RPC_DEBUG
	syslog(LOG_INFO, "Entering RPC service loop" );
#endif

	/* SOPのチェック */
	SET_RPC_STATE( state_, RPCSTATE_SERVER_RECV_SOP );
	if( (ercd_=cTDR_receiveSOP( false )) != E_OK )
		goto error_reset;
	/* func_id の取得 */
	if( (ercd_=cTDR_getInt16( &func_id_ )) != E_OK )
		goto error_reset;

#ifdef RPC_DEBUG
	syslog(LOG_INFO, "unmarshaler task: func_id: %d", func_id_ );
#endif
  /* (GenOpaqueMarshler0103) */
	switch( func_id_ ){
EOT

    # signature に含まれる すべての関数について
    @signature.get_function_head_array.each { |f|
      f_name = f.get_name
      f_type = f.get_declarator.get_type
      func_id = "FUNCID_#{@signature.get_global_name}_#{f_name}".upcase
      fid = @signature.get_id_from_func_name( f_name )

      file.print "	case #{func_id}:		/* (id of '#{f_name}') = #{fid} */ \n"
      file.print "		ercd_ = tOpaqueUnmarshaler_#{@signature.get_global_name}_#{f_name}( p_cellcb, &state_ );\n"
      file.print "		break;\n"

    } #

    if @PPAllocatorSize then
      ppallocator_dealloc_str = "	/* PPAllocator のすべてを解放 */\n	cPPAllocator_dealloc_all();"
    else
      ppallocator_dealloc_str = ""
    end

    file.print <<EOT
  /* (GenOpaqueMarshler0104) */
	default:
		syslog(LOG_INFO, "unmarshaler task: ERROR: unknown func_id: %d", func_id_ );
		ercd_ = E_ID;
	};
error_reset:  /* OK cases also come here */
#{ppallocator_dealloc_str}
	if( ercd_ == E_OK )
		return ercd_;
	if( is_cErrorHandler_joined() )
		ercd_ = cErrorHandler_errorOccured( func_id_, ercd_, state_ );
	if( MERCD( ercd_ ) != E_RESET )
		(void)cTDR_reset();
	return ercd_;
EOT

  end

  #=== PREAMBLE 部のコード生成
  # アンマーシャラセルタイプの場合、アンマーシャラ関数のプロトタイプ宣言を生成
  def gen_preamble file, b_singleton, ct_name, global_name
    if ct_name != @unmarshaler_celltype_name.to_sym then
      return
    end

    # string.h の include (memset, strlen のため)
    file.print "/* header file (strlen, memset) (GenOpaqueMarshler0201) */\n"
    file.print "#include\t<string.h>\n\n"

    file.print "/* アンマーシャラ関数のプロトタイプ宣言 */\n"
    # signature に含まれる すべての関数について
    @signature.get_function_head_array.each { |f|
      f_name = f.get_name
      f_type = f.get_declarator.get_type
      id = @signature.get_id_from_func_name( f_name )
      file.print "static ER  tOpaqueUnmarshaler_#{@signature.get_global_name}_#{f_name}(CELLCB *p_cellcb, int16_t *state);\t/* func_id: #{id} */\n"
    }
    file.print "\n"
  end

  #=== POSTAMBLE 部のコード生成
  # アンマーシャラセルタイプの場合、個々のアンマーシャラ関数の生成
  def gen_postamble file, b_singleton, ct_name, global_name
    if ct_name != @unmarshaler_celltype_name.to_sym then
      return
    end

    file.print "\n/*** アンマーシャラ関数 (GenOpaqueMarshler0301) ***/\n\n"
    @signature.get_function_head_array.each { |f|
      f_name = f.get_name
      f_type = f.get_declarator.get_type
      id = @signature.get_id_from_func_name( f_name )

      # 関数は返り値を持つか?
      b_ret_er = false
      init_retval = ""
      if f_type.get_type.is_void? then
        b_void = true
      else
        b_void = false
        if f_type.get_type.get_type_str == "ER" || f_type.get_type.get_type_str == "ER_INT" then
          b_ret_er = true
          init_retval = " = E_OK"
        end
      end

      file.print <<EOT
/* (GenOpaqueMarshler0302)  */
/*
 * name:    #{f_name}
 * func_id: #{id} 
 */
EOT
      file.print "static ER\n"
      file.print "tOpaqueUnmarshaler_#{@signature.get_global_name}_#{f_name}(CELLCB *p_cellcb, int16_t *state_)\t\t\n"
      file.print "{\n"
      file.print "	ER      ercd_;\n"

      # 引数を受取る変数の定義
      params = f.get_declarator.get_type.get_paramlist.get_items
           # FuncHead->  Decl->    FuncType->ParamList
      params.each{ |par|
        name = par.get_name
        type = par.get_type.get_original_type

        dir = par.get_direction
        if( dir == :RECEIVE )then
          # type は PtrType で、それを取り除いた型
          type = type.get_type
        end
        if( dir == :SEND || dir == :RECEIVE )then
          init = " = 0"
        else
          init = ""
        end

        if type.kind_of? ArrayType then
          type = type.get_type
          aster = "(*"
          aster2 = ")"
        else
          aster = ""
          aster2 = ""
        end

        type_str = type.get_type_str.gsub( /\bconst\b */, "" ) # "const" を外す
        file.printf( "	%-12s %s%s%s%s%s;\n", type_str, aster, name, aster2, type.get_type_str_post, init )

        if dir == :OUT && type.is_nullable? then
          file.print( "	int8_t\tb_#{name}_null_;\n" )
        end
      }

      if ! b_void then
        file.printf( "	%-12s retval_%s%s;\n", f_type.get_type.get_type_str, f_type.get_type.get_type_str_post, init_retval )
      end

      # in 方向の入出力を入力
      file.print "\n	/* 入力引数受取 (GenOpaqueMarshler0303) */\n"
      file.print "	SET_RPC_STATE( *state_, RPCSTATE_SERVER_RECV_BODY );\n"
      b_get = true    # unmarshal では get
      b_marshal  = false
      print_params( params, file, 1, b_marshal, b_get, true, "cServerCall", f_name )
      print_params( params, file, 1, b_marshal, b_get, false, "cServerCall", f_name )
      print_out_nullable( params, file, 1, b_marshal );


      # パケットの受信完了
      file.print "	/* パケット終わりをチェック (GenOpaqueMarshler0304) */\n"
      file.print "	SET_RPC_STATE( *state_, RPCSTATE_SERVER_RECV_EOP );\n"
      if ! f_type.is_oneway? then
        b_continue = "true"
      else
        b_continue = "false"
      end
      file.print "	if( (ercd_=cTDR_receiveEOP(#{b_continue})) != E_OK )\n"
      file.print "		goto error_reset;\n\n"

      # out のメモリをアロケート
      dir = :OUT; alloc_cp = "cPPAllocator_alloc"; alloc_cp_extra = nil; nest = 1
      alloc_for_out_params( params, file, nest, dir, alloc_cp, alloc_cp_extra )

      # 対象関数を呼出す
      file.print "	/* 対象関数の呼出し (GenOpaqueMarshler0305) */\n"
      file.print "	SET_RPC_STATE( *state_, RPCSTATE_SERVER_EXEC );\n"
      if b_void then
        file.print( "	cServerCall_#{f_name}(" )
      else
        file.print( "	retval_ = cServerCall_#{f_name}(" )
      end

      delim = " "
      params.each{ |par|
        file.print delim
        delim = ", "
        if par.get_direction == :RECEIVE then
          file.print "&"
        end
        file.print par.get_name
      }
      file.print( " );\n" )

      # 戻り値、出力引数の受取コードの生成

      # oneway の場合出力、戻り値が無く、受取を待たない（非同期な呼出し）
      if ! f.is_oneway? then

        file.print "\n	/* SOPの送出 (GenOpaqueMarshler0306) */\n"
        file.print "	SET_RPC_STATE( *state_, RPCSTATE_SERVER_SEND_SOP );\n"

        file.print "	if( ( ercd_ = cTDR_sendSOP( false ) ) != E_OK )\n"
        file.print "		goto error_reset;\n"

        b_get = false     # unmarshaler は put
        if( ! b_void )then
          file.print "	/* 戻り値の送出 */\n"
          print_param( "retval_", f_type.get_type, file, 1, :RETURN, nil, nil, b_marshal, b_get )
        end

        if f_type.has_outward? then
          if b_ret_er then
            indent_level = 2
            file.print "	if( MERCD( retval_ ) != E_RPC ){\n"
          else
            indent_level = 1
          end
          indent = "	" * indent_level

          file.print "#{indent}/* 出力値の送出 (GenOpaqueMarshler0307) */\n"
          file.print "#{indent}SET_RPC_STATE( *state_, RPCSTATE_SERVER_SEND_BODY );\n"
          print_params( params, file, indent_level, b_marshal, b_get, true, "cServerCall", f_name )
          print_params( params, file, indent_level, b_marshal, b_get, false, "cServerCall", f_name )

          # receive のメモリをデアロケート
          if f_type.has_receive? then
            file.print "#{indent}/* dealloc receive parameter */\n"
            dir = :RECEIVE; dealloc_cp = "cServerCall_#{f_name}"
            dealloc_for_params( params, file, indent_level, dir, dealloc_cp )
          end

          if b_ret_er then
            file.print "	}\n"
          end
        end

        file.print "	/* パケットの終わり（掃きだし） (GenOpaqueMarshler0308) */\n"
        file.print "	SET_RPC_STATE( *state_, RPCSTATE_SERVER_SEND_EOP );\n"
        file.print "	if( (ercd_=cTDR_sendEOP(false)) != E_OK )\n"  # b_continue = false
        file.print "		goto error_reset;\n"
      end  # ! f.is_oneway?

      file.print "	return E_OK;\n"
      file.print <<EOT

error_reset:
EOT
      # send のリセット用デアロケート
      if f_type.has_send? then
        file.print "	/* dealloc send parameter (GenOpaqueMarshler0309) */\n"
        file.print "	if( *state_ < RPCSTATE_SERVER_EXEC ){\n"
        dir = :SEND; indent_level = 2; dealloc_cp = "cServerCall_#{f_name}"
        dealloc_for_params( params, file, indent_level, dir, dealloc_cp, true )
        file.print "	}\n"
      end

      # receive のメモリをデアロケート
      if f_type.has_receive? && b_ret_er then
        file.print "	/* dealloc receive parameter (GenOpaqueMarshler0310) */\n"
        file.print "	if( MERCD( retval_ ) != E_RPC ){\n"
        dir = :RECEIVE; indent_level = 2; dealloc_cp = "cServerCall_#{f_name}"
        dealloc_for_params( params, file, indent_level, dir, dealloc_cp )
        file.print "	}\n"
      end

      file.print "	return ERCD( E_RPC, MERCD( ercd_ ) );\n"
      file.print "}\n\n"

      # ここ（個々の関数）ではエラーハンドラーは呼び出さない。呼び元（サーバーのメイン関数）で呼び出す。
    }
  end
 

  #b_marshal:: bool
  #b_get:: bool
  #  b_marshal = true  && b_get == false   :  マーシャラで入力引数送出
  #  b_marshal = true  && b_get == true    :  マーシャラで出力引数受取
  #  b_marshal = false && b_get == false   :  アンマーシャラで入力引数受取
  #  b_marshal = false && b_get == true    :  アンマーシャラで出力引数送出
  #b_referenced:: size_is, count_is, string で参照されているものを出力
  def print_params( params, file, nest, b_marshal, b_get, b_referenced, port_name, func_name )
    params.each{ |param|
# p "#{param.get_name}:  b_marshal: #{b_marshal} b_get: #{b_get}"
      if ! ( b_referenced == param.is_referenced? ) then
        next
      end

      dir = param.get_direction
      if( b_get == false && b_marshal == true || b_get == true && b_marshal == false  )then
        case dir
        when :IN, :INOUT
          alloc_cp = "cPPAllocator_alloc"
          alloc_cp_extra = nil
          print_param( param.get_name, param.get_type, file, nest, dir, nil, nil, b_marshal, b_get, alloc_cp, alloc_cp_extra )
        when :SEND
          alloc_cp = "#{port_name}_#{func_name}_#{param.get_name}_alloc"
          alloc_cp_extra = nil
          print_param( param.get_name, param.get_type, file, nest, dir, nil, nil, b_marshal, b_get, alloc_cp, alloc_cp_extra )
        end
      else
        case dir
        when :OUT, :INOUT
          alloc_cp = nil        # inout の b_get==true&&b_marsha==true のときアロケータコードは不用
          alloc_cp_extra = nil
          print_param( param.get_name, param.get_type, file, nest, dir, nil, nil, b_marshal, b_get, alloc_cp, alloc_cp_extra )
        when :RECEIVE
          alloc_cp = "#{port_name}_#{func_name}_#{param.get_name}_alloc"
          alloc_cp_extra = nil
          if b_get then
            outer = "(*"         # マーシャラ側では、ポインタが (send と比べ) 一つ多い
            outer2 = ")"
          else
            outer = nil          # アンマーシャラ側では、ポインタが一つ外されている
            outer2 = nil
          end
          type = param.get_type.get_referto
          print_param( param.get_name, type, file, nest, dir, outer, outer2, b_marshal, b_get, alloc_cp, alloc_cp_extra )
        end
      end
    }
  end

  #=== アロケータコードを生成 (out のアンマーシャラ用)
  def alloc_for_out_params( params, file, nest, dir, alloc_cp, alloc_cp_extra )
    params.each{ |param|
      dir = param.get_direction
      if dir == :OUT then
        alloc_for_out_param( param.get_name, param.get_type, file, nest, nil, nil, alloc_cp, alloc_cp_extra )
      end
    }
  end

  #=== アロケータコードを生成 (out のアンマーシャラ用個別パラメータの生成)
  def alloc_for_out_param( name, type, file, nest, outer, outer2, alloc_cp, alloc_cp_extra )
    org_type = type.get_original_type
    if org_type.is_nullable? then
      indent = "\t" * nest
      file.print "#{indent}if( ! b_#{name}_null_  ){\n"
      nest += 1
    end
    case org_type
    when PtrType
      indent = "\t" * nest
      count = type.get_count; size = type.get_size; string = type.get_string
      if count || size || string then
        loop_counter_type = IntType.new(16)   # mikan 方を size_is, count_is の引数の型とする
        if count then
          len = type.get_count.to_s
        elsif size then
          len = type.get_size.to_s
        elsif string then
          if  type.get_string.instance_of? Expression then
            len = type.get_string.to_s
          else
            raise "unsuscripted string used for out parameter #{name}"
          end
        end

        # size_is に max 指定がある場合、length が max を超えているかチェックするコードを生成
        if org_type.get_max != nil && string == nil then
          file.print "#{indent}/* (GenOpaqueMarshler0401) */\n"
          file.print "#{indent}if( #{len} > #{type.get_max} ){\t/* GenOpaqueMarshaler max check 2 */\n"
          file.print "#{indent}	ercd_ = E_PAR;\n"
          file.print "#{indent}	goto error_reset;\n"
          file.print "#{indent}}\n"
        end

        file.print <<EOT
#{indent}/* (GenOpaqueMarshler0402) */
#{indent}if((ercd_=#{alloc_cp}(sizeof(#{type.get_type.get_type_str}#{type.get_type.get_type_str_post})*#{len},(void **)&#{outer}#{name}#{outer2}#{alloc_cp_extra}))!=E_OK)\t/* GenOpaqueMarshaler 1 */
#{indent}	goto error_reset;
EOT

        if type.get_type.kind_of? PtrType then
          file.print "#{indent}{  /* (GenOpaqueMarshler0403) */\n"
          file.print "#{indent}	#{loop_counter_type.get_type_str}  i__#{nest}, length__#{nest} = #{len};\n"
          file.print "#{indent}	for( i__#{nest} = 0; i__#{nest} < length__#{nest}; i__#{nest}++ ){\n"
          alloc_for_out_param( name, type.get_type, file, nest + 2, outer, "#{outer2}[i__#{nest}]", alloc_cp, alloc_cp_extra )
          file.print "#{indent}	}\n"
          file.print "#{indent}}\n"
        end

      else
        file.print <<EOT
#{indent}/* (GenOpaqueMarshler0404) */
#{indent}if((ercd_=#{alloc_cp}(sizeof(#{type.get_type.get_type_str}#{type.get_type.get_type_str_post}),(void **)&#{outer}#{name}#{outer2}#{alloc_cp_extra}))!=E_OK)\t/* GenOpaqueMarshaler 2 */
#{indent}	goto error_reset;
EOT
      end
    end
    if org_type.is_nullable? then
      nest -= 1
      indent = "\t" * nest
      file.print "#{indent}} else {\n"
      file.print "#{indent}	#{name} = NULL;\n"
      file.print "#{indent}}\n"
      nest += 1
    end
  end

  #=== 引数の一括デアロケートコードの生成
  # send：マーシャラの最後、receive：アンマーシャラの最後で一括して引数をデアロケートする
  def dealloc_for_params( params, file, nest, dir, dealloc_cp, b_reset = false )
    if b_reset then
      reset_str = "_reset"
    else
      reset_str = ""
    end

    params.each{ |param|
      if dir == param.get_direction then
        indent = "\t" * nest
        type = param.get_type.get_original_type
        aster = ""
        if dir == :RECEIVE then
          type = type.get_type.get_original_type   # ポインタを一つ外す
          if b_reset then
            aster = "*"
          end
        end
        count = type.get_count; size = type.get_size
        if (size || count) && type.get_type.has_pointer? then
          if count then
            len = ", #{type.get_count.to_s}"
          elsif size then
            len = ", #{type.get_size.to_s}"
          end
        else
          len = ""
        end
        cp = "#{dealloc_cp}_#{param.get_name}_dealloc#{reset_str}".upcase
        file.print "#{indent}#{cp}(#{aster}#{param.get_name}#{len});\n"
      end
    }
  end

  #== out で nullable な引数の情報を渡す
  # out nullable の場合、in, send, receive のように、値を渡す直前ではなく、呼出し時に渡す
  def  print_out_nullable( params, file, nest, b_marshal );
    indent = "\t" * nest
    params.each{ |param|
      next if param.get_direction != :OUT
      next if ! param.is_nullable?
      if b_marshal then
        file.print "#{indent}if( (ercd_=cTDR_putInt8( (int8_t)(#{param.get_name} == NULL) )) != E_OK )\n"
        file.print "#{indent}\tgoto error_reset;\n"
      else
        # 呼び先は alloc_for_out_param で nullable の対応する
        file.print "#{indent}if( (ercd_=cTDR_getInt8( &b_#{param.get_name}_null_)) != E_OK )\n"
        file.print "#{indent}\tgoto error_reset;\n"
      end
    }
  end
end
