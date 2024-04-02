# coding: utf-8
#
#  TECS Generator
#      Generator for TOPPERS Embedded Component System
#  
#   Copyright (C) 2014-2020 by TOPPERS Project
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
#  $Id: HRPPlugin.rb 3191 2020-12-20 10:56:56Z okuma-top $
#++

require_tecsgen_lib "HRPKernelObjectManager.rb"
require_tecsgen_lib "lib/GenAttMod.rb"

#
# 各メソッドの役割りは、DomainPlugin.rb を参照のこと
# HRPカーネル用ドメインプラグイン
class HRPPlugin < DomainPlugin
  include GenAttMod
  include GenDefSvc
  
  def initialize( region, name, option )
    super
    dbgPrint "HRPPlugin: initialize: region=#{region.get_name}, domainName=#{name}, option=#{option}\n"
    @region = region
    @name   = name
    case option
    when "kernel", "user", "OutOfDomain"
      # OK
      @option = option
    else
      cdl_error( "HRPPlugin: '$1' is unacceptable domain kind, specify 'kernel', 'user' or 'OutOfDomain'", option )
      @option = "kernel"   # とりあえず kernel を設定しておく
    end
  end

  def add_through_plugin( join, current_region, next_region, through_type )
    # join.get_owner:Cell  左辺のセル
    # join.get_definition:Port 呼び口
    # join.get_subscript:Integer or nil 呼び口配列の添数 (Join::@subscript の説明参照)
    # join.get_cell:Cell 右辺のセル
    # join.get_port_name:Symbol 受け口
    # get_rhs_subscript:Integer or nil 受け口配列の添数 (Join::@rhs_subscript の説明参照)
    # return []
    dbgPrint "MyDomainPlugin: add_through_plugin: #{current_region.get_name}=>#{next_region.get_name}, #{join.get_owner.get_name}.#{join.get_definition.get_name}=>#{join.get_cell.get_name}.#{join.get_port_name}, #{through_type}\n"

    #puts "=====Join Check Start====="
    #puts "caller: #{join.get_owner.get_name.to_s}, callee: #{join.get_cell.get_name.to_s}"
    #puts "=====Join Check param====="
    current_domain = current_region.get_domain_root.get_domain_type
    next_domain = next_region.get_domain_root.get_domain_type
    #puts current_domain.get_option.to_s
    # if !next_domain.nil?
    #     puts next_domain.get_option.to_s
    # else
    #     puts "next domain is nil!"
    # end

    HRPPlugin.add_inter_domain_join_set join
    # p join.get_cell.get_name
    # p join.get_cell.get_real_celltype(join.get_rhs_port1).get_name
    # if HRPKernelObjectManager.include_celltype?(join.get_cell.get_celltype.get_global_name)   #oyama: get_name => get_global_name 
    if HRPKernelObjectManager.include_celltype?(join.
                                                 get_cell.
                                                 get_real_celltype(join.get_rhs_port1).
                                                 get_global_name)                #oyama: get_name => get_global_name 
        # 結合先がカーネルオブジェクトセル
        # @plugin_body = HRP2SVCPlugin.new(cell_name, plugin_arg, next_cell, next_cell_port_name, signature, celltype, caller_cell)
        # 何もしないthrough
        # puts "***** #{join.get_cell.get_celltype.get_name} is kernel object"
        return []
        # return [ :HRP2SVCPlugin, "channelCelltype=tMessagebufferChannel,noClientSemaphore=true" ]
    # elsif @end_region.is_root?
    # elsif next_region.get_option == OutOfDomain
    # elsif next_domain.nil?
    elsif next_domain.get_option.to_s == "OutOfDomain"
    #     # 結合先が無所属
    #     # 何もしないthrough
    #     # @plugin_body = HRP2SVCPlugin.new(cell_name, plugin_arg, next_cell, next_cell_port_name, signature, celltype, caller_cell)
        # puts "***** nil"
        return []
    #elsif @start_region.get_param != :KERNEL_DOMAIN && @end_region.get_param == :KERNEL_DOMAIN
    elsif current_domain.get_option.to_s != "kernel" && next_domain.get_option.to_s == "kernel"
        # ユーザドメインからカーネルドメインへの結合
        # @plugin_body = HRP2SVCPlugin.new(cell_name, plugin_arg, next_cell, next_cell_port_name, signature, celltype, caller_cell)
        # puts "***** svc"
        # return [ :HRPSVCPlugin, "" ]
        return [ :HRPSVCThroughPlugin, "" ]
      elsif current_domain != next_domain
        # 別のユーザドメインへの結合
        # @plugin_body = HRP2RPCPlugin.new(cell_name, plugin_arg, next_cell, next_cell_port_name, signature, celltype, caller_cell)
        # puts "***** rpc"
        # return [ :HRPRPCPlugin, "channelCelltype=tMessagebufferChannel,noClientSemaphore=true" ]
        # puts "HRPPlugin:RPC:PPAllocatorSize=256"
        return [ :HRPRPCPlugin, "noClientSemaphore=false,PPAllocatorSize=256" ]
    else
        # その他
        # 何もしないthrough
        # @plugin_body = HRP2SVCPlugin.new(cell_name, plugin_arg, next_cell, next_cell_port_name, signature, celltype, caller_cell)
        dbgPrint "warning: at HRP Join Check"
        return nil
    end

    # puts "=====Join Check End====="
  end

  def joinable?(current_region, next_region, through_type )
    dbgPrint "MyDomainPlugin: joinable? from #{current_region.get_name} to #{next_region.get_name} (#{through_type})\n"
    return true
  end

  #== ドメイン種別を返す
  #return::Symbol :kernel, :user, :OutOfDomain
  def get_kind
    return @option.to_sym
  end
  
  def self.gen_post_code file
  end

  # ATT_MODを生成済みかどうか                   # 2017.8.27
  @@generate_memory_module = {}

  @@include_extsvc_fncd = false  # 17.07.26 暫定
  # 
  #  ATT_MODの生成
  #  gen_factory実行時には，すべてのセルタイププラグインを生成済みのはずなので，
  #  カーネルAPIコードのメモリ保護を省略できる．

  #node_root::Region : ノードのルート
  def gen_factory node_root
    super
    dbgPrint "HRPPlugin: $generating_region=#{$generating_region.get_name} $gen=#{$gen}\n"
    if @@include_extsvc_fncd == false
      file = AppFile.open( "#{$gen}/tecsgen.cfg" )
      file.print "/* HRP001 */\n"
      file.print "#include \"extsvc_fncode.h\"\n"   ## 2017.7.26
      file.close
      @@include_extsvc_fncd = true
    end

    if @@generate_memory_module[ node_root ] == nil
      @@generate_memory_module[ node_root ] = true

      # INCLUDE の生成
      gen_include
      # ATT_MOD の生成
      gen_att_mod node_root
      # DEF_SVC の生成
      gen_def_svc node_root
    end
  end

  #----- Inter Domain Join Set -----#
  @@inter_domain_join_set = {}      #{entry_cell =>{domain_root=>count} }  Hash of inter domain join
  def self.add_inter_domain_join_set join
    rhs_cell = join.get_cell
    dbgPrint "--------- add_inter_domain:#{join.get_owner.get_namespace_path} => #{join.get_cell.get_namespace_path}-----\n"
    domain_root = join.get_owner.get_region.get_domain_root   # lhs cell's domain root
    if @@inter_domain_join_set[ rhs_cell ] == nil then
      @@inter_domain_join_set[ rhs_cell ] = []
    end
    # 左辺のドメインルートを記録
    @@inter_domain_join_set[ rhs_cell ] << join
  end
  def self.get_inter_domain_join_set rhs_cell
    if @@inter_domain_join_set[ rhs_cell ] then
      @@inter_domain_join_set[ rhs_cell ].uniq!
    else
      @@inter_domain_join_set[ rhs_cell ] = []
    end
    return @@inter_domain_join_set[ rhs_cell ]
  end
  def self.get_inter_domain_join_roots rhs_cell
    dbgPrint "--------- get_inter_domain #{rhs_cell.get_namespace_path} -----\n"
    domain_roots = []
    set = get_inter_domain_join_set rhs_cell
    set.each{ |join|
      dbgPrint "--------- get_inter_domain:#{join.get_owner.get_namespace_path} => #{join.get_cell.get_namespace_path}-----\n"
      # if join.get_owner.get_region.get_domain_root.get_domain_type.get_option == "user" then
        domain_roots << join.get_owner.get_region.get_domain_root
      # end
    }
    return domain_roots.uniq
  end

  def self.get_sac_str cell
    domain_roots = self.get_inter_domain_join_roots cell
    delim = ""
    acv = ""
    cell_domain_root = cell.get_region.get_domain_root
    if cell_domain_root.get_domain_type.get_kind != :OutOfDomain then
      domain_roots << cell.get_region.get_domain_root  # 結合先のドメインも含める
    end
    domain_roots.each{ |dr|
      case dr.get_domain_type.get_kind
      when :kernel
        acv += "#{delim}TACP_KERNEL"
        delim = "|"
      when :user
        acv += "#{delim}TACP(#{dr.get_name})"
        delim = "|"
        if cell_domain_root.get_domain_type.get_kind != :OutOfDomain &&
           dr.get_namespace_path != cell_domain_root.get_namespace_path then
          cdl_error( "HRP9999 '$1': kernel object joined from other user domain. kernel object joined from multi-user-domain must be placed out of domain", cell.get_name )
        end
      when :OutOfDomain
        if cell_domain_root.get_domain_type.get_kind == :OutOfDomain then
          acv += "#{delim}TACP_SHARED"
          delim = "|"
        end
      else
        raise "unkown domain kind"
      end
    }
    if acv == "" then
      # 呼び先セルが無所属かつ、呼び元も無所属のみ、または結合無しの場合
      acv = "TACP_SHARED"
    end
    sac_str = "{"
    delim = ""
    [:accessPattern1, :accessPattern2, :accessPattern3, :accessPattern4].each{ |acp|
      init = (cell.get_attr_initializer acp).to_s
      if init != "OMIT"
        sac_str += delim + init
      else
        sac_str += delim + acv
      end
      delim = ", "
    }
    return sac_str + '}'
  end
end
