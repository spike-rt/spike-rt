# -*- coding: utf-8 -*-
#
#  TECS Generator
#      Generator for TOPPERS Embedded Component System
#  
#   Copyright (C) 2008-2011 by TOPPERS Project
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
#   $Id: CellPlugin.rb 2952 2018-05-07 10:19:07Z okuma-top $
#++

#== celltype プラグインの共通の親クラス
class HRPSVCBodyCellPlugin < CellPlugin
#@caller_regions::Hash: { :entry_port => ["region1","region2"...] }

  @@cell_entry_export = {}    # { :'cell_namespace_path.entry' => [ plugin, ... ], ... }  (添数は exportID)
  @@plugin_list= []    # [ instanceOf(HRPSVCBodyCellPlugin), ... ]

  HRPSVCBodyCellPluginArgProc = {
    "exportEntryPort"     => Proc.new { |obj,rhs| obj.set_exportEntryPort rhs },
    "substRegion"            => Proc.new { |obj,rhs| obj.set_substRegion rhs },
    # "exportID"            => Proc.new { |obj,rhs| obj.set_exportID rhs },
  };

  #=== CellPlugin# initialize
  #cell::     Cell        セル（インスタンス）
  # このメソッドは、セルの構文解析が終わったところで呼び出される
  # この段階では意味解析が終わっていない
  def initialize( cell, option )
    dbgPrint "HRPSVCBodyCellPlugin: cell=#{cell.get_name}\n"
    super
    @cell = cell
    @caller_regions = {}
    @exportID = nil
  
    @plugin_arg_str = CDLString.remove_dquote option
    # @plugin_arg_str = option.gsub( /\A"(.*)/, '\1' )    # 前後の "" を取り除く
    # @plugin_arg_str.sub!( /(.*)"\z/, '\1' )
    @plugin_arg_check_proc_tab = HRPSVCBodyCellPluginArgProc
    parse_plugin_arg
    @@plugin_list << self
    if @cell.get_region.get_domain_root == nil then
      # ドメイン指定されていない
      cdl_error( "HRPSB9999 $1 is in region not domain specified", @cell.get_name )
    elsif @cell.get_region.get_domain_root.get_domain_type.get_option != "kernel" then
      # カーネルドメイン指定されていない
      cdl_error( "HRPSB9999 $1 is in region not specified kernel domain", @cell.get_name )
    end
  end

  #=== 後ろの CDL コードを生成
  #プラグインの後ろの CDL コードを生成
  #file:: File: 
  def self.gen_post_code( file )
    # 複数のプラグインの post_code が一つのファイルに含まれるため、以下のような見出しをつけること
    # file.print "/* '#{self.class.name}' post code */\n"
    check_and_set_id
    @@cell_entry_export.each{ |ce, plugins|
      path = ce.to_s.split( "." )
      # cell_path = path[0]  # plugins[0].get_cell できるので不要
      ep_name = path[1].to_sym
      cell = plugins[0].get_cell
      celltype = cell.get_celltype
      next if celltype == nil  # これまでに文法エラーが発生している
      ep = celltype.find ep_name  
      next if ep == nil        # entry port (エラーチェック済み)
      signature = ep.get_signature
      next if signature == nil # これまでに文法エラーが発生している

      cell_nsp = cell.get_namespace_path
      # cell_nsp = cell_nsp.append ep_name   # 一時的に外してある！！！

      acpt = ""
      regions = []
      plugins.each{ |plugin|
        regions += plugin.get_caller_regions[ ep_name ]
      }
      regions.uniq!
      if regions.include?( "SHARED" ) then
        acpt = "C_EXP( \"TACP_SHARED\" )"
      else
        region_list = []
        regions.each{ |rp|
          nsp = NamespacePath.analyze rp
          region_list << (Namespace.find nsp)
        }
        acpt = regions_to_tacp region_list
      end

      #--- Generate Kernel Domain Code ---#
      cell_name = :"SVCBody_#{cell.get_name}_#{ep_name}"
      nest = cell.get_region.gen_region_str_pre file
      indent = "  " * nest
      file.print <<EOT
#{indent}cell tHRPSVCBody_#{signature.get_global_name} #{cell_name} {
#{indent}  cCall = #{cell.get_name}.#{ep_name};
EOT
      signature.get_function_head_array.each{ |func_head|
        func_name = func_head.get_name
        callable_domains = cell.get_restricted_regions( ep_name, func_name )
        if callable_domains && callable_domains.length > 0 then
          aacpt = regions_to_tacp callable_domains
        elsif callable_domains && callable_domains.length == 0 then
          aacpt = "C_EXP( \"TACP_KERNEL\" )"
        else
          aacpt = acpt
        end
        dbgPrint "HRPSVCBodyCellPlugin:regions_to_tacp:#{cell.get_name} #{func_name} #{acpt} #{aacpt}\n"

        file.print <<EOT
#{indent}  restrict_acptn_#{func_name} = #{acpt};
EOT
      }
      file.print <<EOT
#{indent}};
EOT
      cell.get_region.gen_region_str_post file

      rgn_nsp = cell.get_region.get_namespace_path
      cell_nsp = rgn_nsp.append cell_name
      HRPSVCManage.add_cell cell.get_region.get_node_root, cell_nsp, signature
    }
  end

  def self.regions_to_tacp regions
    acpt = ""
    delim = ""
    b_shared = false
    regions.each{ |region|
      domain_type = region.get_domain_type
      if domain_type then
        domain_option = domain_type.get_option
      else
        @@plugin_list[0].cdl_warning( "HSW0002 HRPSVCBodyCellPlugin: region $1 not under domain", region.get_namespace_path )
        domain_option = "kernel"
      end
      case domain_option
      when "user"
        acpt += delim + "TACP(" + region.get_name.to_s + ")"
        delim = " | "
      when "OutOfDomain"
        b_shared = true
      when "kernel"
      else
        raise "unkown domain option "
      end
    }
    if b_shared then
      aacpt = "C_EXP( \"TACP_SHARED\" )"
    elsif acpt == "" then
      aacpt = "C_EXP( \"TACP_KERNEL\" )"
    else
      aacpt = "C_EXP( \"#{acpt}\" )"
    end
    return aacpt
  end

  def self.check_and_set_id
    @@plugin_list.each{ |plugin|
      if plugin.get_exportID then
        
      end
    }
  end

  def get_exportID
    @exportID
  end

  def get_cell
    @cell
  end

  def get_caller_regions
    @caller_regions
  end

  #-----  オプション -----#
  # HRPSVCBodyCellPlugin#exportEntryPort オプションの設定
  #  exportEntryPort='eEntry:rRegion1,rRegion2,...','eEntry2:rRegion1'
  def set_exportEntryPort rhs
    rhs.gsub!( /:::/, ":##" )  # :: が : の直後にいた場合
    rhs.gsub!( /::/, "##" )    # :: は ## に置き換える
    entry = rhs.split(":")     # ： で区切る
    entry_port = entry[0].to_sym
    ep = @cell.get_celltype.find  entry_port  # get_celltype は (celltype not found な時) nil を返す可能性があるが処置しない
    if ! ep.kind_of?( Port ) || ep.get_port_type != :ENTRY then
      cdl_error( "HRPSB9999 '$1' is not entry port or not found in $2", entry_port, @cell.get_name )
    elsif ep.get_array_size then
      cdl_error( "HRPSB9999 '$1' is entry array which is not supported in current version", entry_port)
    end
    caller_regions = entry[1].to_s
    caller_regions.gsub!( /##/, "::" )  # ## を :: に戻す
    dbgPrint "set_exportEntryPort: cell=#{@cell.get_name} entry=#{entry_port} regions=#{caller_regions}\n"
    if @caller_regions[ entry_port ] == nil then
      @caller_regions[ entry_port ] = []
    end
    caller_regions.split(",").each{ |str|
      dbgPrint "set_exportEntryPort: #{@cell.get_name}=#{str}\n"
      @caller_regions[ entry_port ] << str  # region path str
      path = :"#{@cell.get_namespace_path}.#{entry_port}"
      if @@cell_entry_export[ path ] then
        @@cell_entry_export[ path ] << self
      else
        @@cell_entry_export[ path ] = [ self ]
      end
      dbgPrint "set_exportEntryPort: #{path}\n"
    }
  end

  # HRPSVCBodyCellPlugin#exportEntryPort オプションの設定
  #  substRegion='original_region:subst_region'
  #  original_region は domain 外
  #  subst_region は domain 内
  def set_substRegion rhs
    entry = rhs.split(":")
    original_region_name = entry[0]
    subst_region_name = entry[1]
    dbgPrint "HRPSVCBodyCell: substRegion origin=#{original_region_name} subst=#{subst_region_name}\n"

    # 変更前 (ドメイン外のリージョンであることを確認)
    nsp = NamespacePath.analyze original_region_name
    original_region = Namespace.find nsp
    dbgPrint "HRPSVCBodyCell: original_region_nsp=#{nsp.to_s} original_region=#{original_region.class.name}\n"
    if original_region == nil || ! original_region.kind_of?( Region ) then
      cdl_error("HRPSB9999 HRPSVCBodyCellPlugin: substRegion option: '$1': not exist or not region", original_region_name)
    elsif original_region.get_domain_type then
      cdl_error("HRPSB9999 HRPSVCBodyCellPlugin: substRegion option: '$1': under domain", original_region_name)
    end

    # 変更後 (ドメイン内のリージョンであることを確認)
    nsp = NamespacePath.analyze subst_region_name
    dbgPrint "HRPSVCBodyCell: subst_region_nsp=#{nsp.to_s}\n"
    subst_region = Namespace.find nsp
    if subst_region == nil || ! subst_region.kind_of?( Region ) then
      cdl_error("HRPSB9999 HRPSVCBodyCellPlugin: substRegion option: '$1': not exist or not region", subst_region_name)
    elsif subst_region.get_domain_type == nil then
      cdl_error("HRPSB9999 HRPSVCBodyCellPlugin: substRegion option: '$1': not under domain", subst_region_name)
    end
    if subst_region.get_node_root != @cell.get_region.get_node_root then
      cdl_error("HRPSB9999 HRPSVCBodyCellPlugin: substRegion option: '$1': not in the same node as '$2'", subst_region_name, @cell.get_name)
    end
  end

=begin
  #  exportEntryPort='eEntry:rRegion1,rRegion2,...','eEntry2:rRegion1'
  def set_exportEntryPort rhs
  # HRPSVCBodyCellPlugin#exportID オプションの設定
  def set_exportID rhs
    if rhs.to_i == 0 then
      cdl_error( "HRPSB9999 export ID for $1 must be none-zero integer number", @cell.get_name )
    end
    @exportID = rhs.to_i
  end
=end
end
