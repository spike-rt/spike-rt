# -*- coding: utf-8 -*-
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
#   $Id: MrubyBridgeCellPlugin.rb 3225 2021-09-26 05:08:38Z okuma-top $
#++

#== celltype プラグインの共通の親クラス
class MrubyBridgeCellPlugin < CellPlugin

  # プラグイン引数名 => Proc
  MrubyBridgePluginArgProc = { 
      "ignoreUnsigned" => Proc.new { |obj,rhs| obj.set_ignoreUnsigned rhs },
      "exclude_port" => Proc.new { |obj,rhs| obj.set_exclude_port rhs },
      "exclude_port_func" => Proc.new { |obj,rhs| obj.set_exclude_port_func rhs },
      "auto_exclude" => Proc.new { |obj,rhs| obj.set_auto_exclude rhs },
  }
  @@cell_list = {}      # gen_cdl_file'ed list to avoid duplicate generation
  @@signature_list = {}

  require_tecsgen_lib( "MrubyBridgeSignaturePlugin.rb" )

  #@exclude_port::[ String(port_name) ] : 
  #@exclude_port_func::{ [ String(port_name) ] => [ String(func_name) ] } : 
  #@port_list::{ String(port_name) => String(include/exclude=func_name) }  : all included port and opt_string

  #=== CellPlugin# initialize
  #cell::     Cell        セル（インスタンス）
  # このメソッドは、セルの構文解析が終わったところで呼び出される
  # この段階では意味解析が終わっていない
  def initialize( cell, option )
    dbgPrint "  #{self.class.name}: initialzie=#{cell.get_name} option=#{option}\n"
    super
    @cell = cell

    # option
    @b_ignoreUnsigned = false
    @exclude_port = []
    @exclude_port_func = {}
    @b_auto_exclude = true     # auto_exclude = true by default 

    @plugin_arg_str = CDLString.remove_dquote option
    # @plugin_arg_str = option.gsub( /\A"(.*)/, '\1' )    # 前後の "" を取り除く
    # @plugin_arg_str.sub!( /(.*)"\z/, '\1' )
    @plugin_arg_list = {}
    dbgPrint "#{self.class.name}: initialzie: #{cell.get_name}\n"
    @plugin_arg_check_proc_tab = MrubyBridgePluginArgProc
    parse_plugin_arg

    @port_list = {}         # set @port_list before return
    ct = @cell.get_celltype
    return if ct == nil     # error case

    port_list = ct.get_port_list
    if @exclude_port.length > 0 then
      # この実装では、存在しない port を指定されてもチェックできない
      # print "MRBBridgeCellPlugin: exclude list\n"
      port_list.each{ |port|
        if ! @exclude_port.include?( port.get_name.to_s ) then
          # print "MRBBridgeCellPlugin: #{port.get_name} included\n"
          @port_list[ port ] = ""
        else
          # print "MRBBridgeCellPlugin: #{port.get_name} excluded\n"
        end
      }
    else
      port_list.each{ |port|
        @port_list[ port ] = ""
      }
    end

    if @exclude_port_func.length > 0 then
      @port_list.each{ |port, opt_str|
        delim = ""
        if @exclude_port_func.include?( port.get_name.to_s ) then
          @exclude_port_func[ port.get_name.to_s ].each{ |func_name|
            opt_str += delim + "exclude=" + func_name
            delim = ","
          }
        end
        @port_list[ port ] = opt_str
      }
    end
  end

  def gen_cdl_file file
    dbgPrint "#{self.class.name}: gen_cdl_file: #{@cell.get_name}\n"

    file.print <<EOT
/* MrubyBridgeCellPlugin: generate for cell=#{@cell.get_name} */
EOT

    if @@cell_list[ @cell ] then
      file.print <<EOT

/*
 * generate for #{@cell.get_name} duplicate and ignored.
 * This might comes from generate for celltype.
 */
EOT
      cdl_info( "MrubyBridgeCellPlugin: generate duplicate for cell '$1'", @cell.get_name )
      return
    end
    @@cell_list[ @cell ] = @cell
                
    @port_list.each{ |port, opt_str|
      next if port.get_signature == nil

      case port.get_signature.get_context
      when "task", "any"
      else
        next
      end

      if port.get_port_type == :ENTRY then
        print "  MrubyBridgeCellPlugin: [cell.port] #{@cell.get_name}.#{port.get_name} => [mruby instance] TECS::T#{port.get_signature.get_global_name}.new( '#{@cell.get_name}#{port.get_name}Bridge' ) \n"
        if @@signature_list[ port.get_signature ] == nil then
          opt_str = "ignoreUnsigned=#{@b_ignoreUnsigned}, auto_exclude=#{@b_auto_exclude}, " + opt_str
          # p "opt_str=#{opt_str}"

          file.print <<EOT

/* cell.port=#{@cell.get_name}.#{port.get_name} */
generate( MrubyBridgePlugin, #{port.get_signature.get_namespace_path}, "#{opt_str}" );
EOT
          @@signature_list[ port.get_signature ] = true
        end

        # mikan option
        nest = @cell.get_region.gen_region_str_pre file
        nest_str = "  " * nest

        file.print <<EOT
#{nest_str}/* BridgeCell */
#{nest_str}cell nMruby::t#{port.get_signature.get_global_name} #{@cell.get_name}#{port.get_name}Bridge {
#{nest_str}    cTECS = #{@cell.get_namespace_path}.#{port.get_name};
#{nest_str}};
EOT
        @cell.get_region.gen_region_str_post file
      end
    }
  end

  #=== 後ろの CDL コードを生成
  #プラグインの後ろの CDL コードを生成
  #file:: File: 
  def self.gen_post_code( file )
    dbgPrint "#{self.name}: gen_post_code\n"
    gen_post_code_body file
  end

  def self.gen_post_code_body file
    # 複数のプラグインの post_code が一つのファイルに含まれるため、以下のような見出しをつけること
    # file.print "/* '#{self.class.name}' post code */\n"
    dbgPrint "#{self.name}: gen_post_code_body\n"
    # MrubyBridgeSignaturePlugin.gen_post_code_body file
  end

  #=== プラグイン引数 ignoreUnsigned
  def set_ignoreUnsigned rhs
    if rhs == "true" || rhs == nil then
      @b_ignoreUnsigned = true
    end
  end

  #=== プラグイン引数 exclude_port
  def set_exclude_port rhs
    ports = rhs.split ','
    ct = @cell.get_celltype
    return if ct == nil    # error case
    ports.each{ |rhs_port|
      obj = ct.find( rhs_port.to_sym )
      if( ( ! obj.instance_of? Port ) || obj.get_port_type != :ENTRY ) then
        cdl_error( "MRB9999 exclude_port '$1' not found or not entry in celltype '$2'", rhs_port, ct.get_name )
      else
        # print "MRBBridgeCellPlugin: exclude #{rhs_port}\n"
        @exclude_port << rhs_port
      end
    }
  end

  #=== プラグイン引数 exclude_port_func
  def set_exclude_port_func rhs
    port_funcs = rhs.split ','
    ct = @cell.get_celltype
    return if ct == nil    # error case
    port_funcs.each{ |rhs_port_func|
      port_func = rhs_port_func.split '.'
      if port_func.length != 2 then
        cdl_error( "MRB9999 exclude_port_func: '$1' not in 'port.func' form", rhs_port_func )
      end
      obj = ct.find( port_func[0].to_sym )
      if( ( ! obj.instance_of? Port ) || obj.get_port_type != :ENTRY ) then
        cdl_error( "MRB9999 exclude_port_func: port '$1' not found in celltype '$2'", rhs_port, ct.get_name )
      else
        signature = obj.get_signature
        next if signature == nil     # error case
        if signature.get_function_head port_func[1].to_sym
          # print "MRBBridgeCellPlugin: #{port_func[0]}.#{port_func[1]} exclude\n"
          if @exclude_port_func[ port_func[0] ] then
            @exclude_port_func[ port_func[0] ] <<  port_func[1]
          else
            @exclude_port_func[ port_func[0] ] = [ port_func[1] ]
          end
        else
          cdl_error( "MRB9999 exclude_port_func: func '$1' not found in port '$2' celltype $3",
                     port_func[1], port_func[0], ct.get_name )
        end
      end
    }
  end

  #=== プラグイン引数 auto_exclude
  def set_auto_exclude rhs
    # print "MrubyBridgeCellPlugin: auto_exclude=#{rhs}\n"
    if rhs == "false" then
      @b_auto_exclude = false
    elsif rhs == "true" then
      @b_auto_exclude = true     # auto_exclude = true by default 
    else
      cdl_warning( "MRB9999 auto_exclude: unknown rhs value ignored. specify true or false" )
    end
  end

end

