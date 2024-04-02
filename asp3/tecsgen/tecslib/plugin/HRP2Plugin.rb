# -*- coding: utf-8 -*-
#
#  TECS Generator
#      Generator for TOPPERS Embedded Component System
#  
#   Copyright (C) 2018 by TOPPERS Project
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
#   $Id: HRP2Plugin.rb 2952 2018-05-07 10:19:07Z okuma-top $
#++

#
# 各メソッドの役割りは、DomainPlugin.rb を参照のこと
# HRP2カーネル用ドメインプラグイン
class HRP2Plugin < DomainPlugin

  def initialize( region, name, option )
    super
    print "HRP2Plugin: initialize: region=#{region.get_name}, domainName=#{name}, option=#{option}\n"
    @region = region
    @name   = name

    case option
    when "trusted", "nontrusted", "OutOfDomain"
      # OK
      @option = option
    else
      cdl_error( "HRPPlugin: '$1' is unacceptable domain kind, specify 'trusted' or 'nontrusted'", option )
      @option = "trusted"   # とりあえず trusted を設定しておく
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
    print "HRP2Plugin: add_through_plugin: #{current_region.get_name}=>#{next_region.get_name}, #{join.get_owner.get_name}.#{join.get_definition.get_name}=>#{join.get_cell.get_name}.#{join.get_port_name}, #{through_type}\n"

    puts "=====Join Check Start====="
    puts "caller: #{join.get_owner.get_name.to_s}, callee: #{join.get_cell.get_name.to_s}"
    puts "=====Join Check param====="
    current_domain = current_region.get_domain_root.get_domain_type
    next_domain = next_region.get_domain_root.get_domain_type
    puts current_domain.get_option.to_s
    if !next_domain.nil?
        puts next_domain.get_option.to_s
    else
        puts "next domain is nil!"
    end

    if HRP2KernelObjectPlugin.include_celltype?(join.get_cell.get_celltype)
        # 結合先がカーネルオブジェクトセル
        # @plugin_body = HRP2SVCPlugin.new(cell_name, plugin_arg, next_cell, next_cell_port_name, signature, celltype, caller_cell)
        # 何もしないthrough
        puts "***** #{join.get_cell.get_celltype.get_name} is kernel object"
        return []
        # return [ :HRP2SVCPlugin, "channelCelltype=tMessagebufferChannel,noClientSemaphore=true" ]
    # elsif @end_region.is_root?
    # elsif next_region.get_option == OutOfDomain
    # elsif next_domain.nil?
    elsif next_domain.get_option.to_s == "OutOfDomain"
    #     # 結合先が無所属
    #     # 何もしないthrough
    #     # @plugin_body = HRP2SVCPlugin.new(cell_name, plugin_arg, next_cell, next_cell_port_name, signature, celltype, caller_cell)
        puts "***** nil"
        return []
        # return [ :HRP2SVCPlugin, "channelCelltype=tMessagebufferChannel,noClientSemaphore=true" ]
    #elsif @start_region.get_param != :KERNEL_DOMAIN && @end_region.get_param == :KERNEL_DOMAIN
    elsif current_domain.get_option.to_s != "trusted" && next_domain.get_option.to_s == "trusted"
        # ユーザドメインからカーネルドメインへの結合
        # @plugin_body = HRP2SVCPlugin.new(cell_name, plugin_arg, next_cell, next_cell_port_name, signature, celltype, caller_cell)
        puts "***** svc"
        return [ :HRP2SVCPlugin, "" ]
    elsif current_domain != next_domain
        # ユーザドメインから別のユーザドメインへの結合
        # @plugin_body = HRP2RPCPlugin.new(cell_name, plugin_arg, next_cell, next_cell_port_name, signature, celltype, caller_cell)
        puts "***** rpc"
        return [ :HRP2RPCPlugin, "channelCelltype=tMessagebufferChannel,noClientSemaphore=true" ]
    else
        # その他
        # 何もしないthrough
        # @plugin_body = HRP2SVCPlugin.new(cell_name, plugin_arg, next_cell, next_cell_port_name, signature, celltype, caller_cell)
        print "warning: at HRP2 Join Check"
        return nil
    end

    puts "=====Join Check End====="
  end

  #== ドメイン種別を返す
  #return::Symbol :kernel, :user, :OutOfDomain
  def get_kind
    case @option
    when "trusted"
      return :kernel
    when "nontrusted"
      return :user
    when "OutOfDomain"
      return :OutOfDomain
    end
  end

  def joinable?(current_region, next_region, through_type )
    print "HRP2Plugin: joinable? from #{current_region.get_name} to #{next_region.get_name} (#{through_type})\n"
    return true
  end

  def self.gen_post_code file
  end
end
