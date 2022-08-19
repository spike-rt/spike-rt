# -*- coding: utf-8 -*-
#
#  TECS Generator
#      Generator for TOPPERS Embedded Component System
#  
#   Copyright (C) 2019-2021 by TOPPERS Project
#
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
#   $Id: HRMPPlugin.rb 3225 2021-09-26 05:08:38Z okuma-top $
#

#== HRMPPlugin クラス
class HRMPPlugin < MultiPlugin
  def self.get_plugin superClass
    # case when (つまりは ===) では、期待したように一致しない模様
    if superClass == DomainPlugin then
      dbgPrint "HRMPPlugin: DomainPlugin"
      require_tecsgen_lib 'tecslib/plugin/HRMPDomainPlugin.rb'
      return HRMPDomainPlugin
    elsif superClass == ClassPlugin
      dbgPrint "HRMPPlugin: ClassPlugin"
      require_tecsgen_lib 'tecslib/plugin/HRMPClassPlugin.rb'
      return HRMPClassPlugin
    else
      dbgPrint "HRMPPlugin: unsupported"
      return nil
    end
  end
end

module HRMPJoinManager
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

    if current_domain != next_domain then
      HRMPDomainPlugin.add_inter_domain_join_set join
      # p join.get_cell.get_name
      # p join.get_cell.get_real_celltype(join.get_rhs_port1).get_name
      if HRPKernelObjectManager.include_celltype?(join.
                                                  get_cell.
                                                  get_real_celltype(join.get_rhs_port1).
                                                  get_global_name)                #oyama: get_name => get_global_name 
          # 結合先がカーネルオブジェクトセル
          # @plugin_body = HRP2SVCPlugin.new(cell_name, plugin_arg, next_cell, next_cell_port_name, signature, celltype, caller_cell)
          # 何もしないthrough
          # puts "***** #{join.get_cell.get_celltype.get_name} is kernel object"
          # return []
          return add_through_plugin_class( join, current_region, next_region, through_type )
      elsif next_domain.get_option.to_s == "OutOfDomain"
      #     # 結合先が無所属
      #     # 何もしないthrough
      #     # @plugin_body = HRP2SVCPlugin.new(cell_name, plugin_arg, next_cell, next_cell_port_name, signature, celltype, caller_cell)
          # puts "***** nil"
          return add_through_plugin_class( join, current_region, next_region, through_type )
      elsif current_domain.get_option.to_s != "kernel" && next_domain.get_option.to_s == "kernel"
          # ユーザドメインからカーネルドメインへの結合
          # @plugin_body = HRP2SVCPlugin.new(cell_name, plugin_arg, next_cell, next_cell_port_name, signature, celltype, caller_cell)
          # puts "***** svc"
          return [ :HRPSVCThroughPlugin, "" ] + add_through_plugin_class( join, current_region, next_region, through_type )
      else
          # 別のユーザドメインへの結合
          # @plugin_body = HRP2RPCPlugin.new(cell_name, plugin_arg, next_cell, next_cell_port_name, signature, celltype, caller_cell)
          # puts "***** rpc"
          # return [ :HRPRPCPlugin, "channelCelltype=tMessagebufferChannel,noClientSemaphore=true" ]
          # puts "HRMPPlugin:RPC:PPAllocatorSize=256"
          return [ :HRPRPCPlugin, "noClientSemaphore=false,PPAllocatorSize=256" ]
      end
    else
      return add_through_plugin_class( join, current_region, next_region, through_type )
    end
  end

  def add_through_plugin_class( join, current_region, next_region, through_type )
    # p "FMPPlugin: add_through_plugin"
    # join.get_owner:Cell  左辺のセル
    # join.get_definition:Port 呼び口
    # join.get_subscript:Integer or nil 呼び口配列の添数 (Join::@subscript の説明参照)
    # join.get_cell:Cell 右辺のセル
    # join.get_port_name:Symbol 受け口
    # get_rhs_subscript:Integer or nil 受け口配列の添数 (Join::@rhs_subscript の説明参照)
    # return []
    dbgPrint "HRMPJoinManger: add_through_plugin_class: #{current_region.get_name}=>#{next_region.get_name}, #{join.get_owner.get_name}.#{join.get_definition.get_name}=>#{join.get_cell.get_name}.#{join.get_port_name}, #{through_type}\n"

    # PU: Processing Unit, NPU: Non-Processing Unit
    # NPU の locality だけ見れば十分である
    # 厳密には、PU==only, NPU!=only の場合、リージョン外からの呼出しがあり、
    # リージョン外への呼出しフローがあれば、all とする方がよい

    # PU の情報は使用する必要がない
    # NPU が only なら PU は必ず only
    current_class_attr = current_region.get_class_root.get_class_type.get_plugin.get_NPU_attr
    caller_prc_NPU_cell      = current_class_attr[:processorID]
    caller_locality_NPU_cell = current_class_attr[:locality]

    next_class_attr = next_region.get_class_root.get_class_type.get_plugin.get_NPU_attr
    callee_prc_NPU_cell      = next_class_attr[:processorID]
    callee_locality_NPU_cell = next_class_attr[:locality]

    dbgPrint "HRMPPlugin: Join: " + (join.get_cell.get_real_celltype(join.get_rhs_port1).get_global_name).to_s +
             " current:" + current_class_attr[:class_name].to_s + " next:" + next_class_attr[:class_name].to_s + "\n"
    
    rhs_ct = join.get_cell.get_real_celltype(join.get_rhs_port1).get_global_name
                                                                                 
    if HRPKernelObjectManager.include_celltype?(join.
                                                  get_cell.
                                                  get_real_celltype(join.get_rhs_port1).
                                                  get_global_name)                #oyama: get_name => get_global_name 
      # 呼び先がカーネルオブジェクトセル
      # 何もしないthrough
      dbgPrint "***** #{join.get_cell.get_celltype.get_name} is kernel object\n"
      return []
    else
      dbgPrint "HRMPPlugin: **** Direct Call\n"
      return []
    end
  end
end
