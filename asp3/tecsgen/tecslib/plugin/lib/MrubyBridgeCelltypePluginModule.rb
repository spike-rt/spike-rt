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
#   $Id: MrubyBridgeCelltypePluginModule.rb 3077 2019-06-09 07:23:31Z okuma-top $
#++

#== celltype プラグインの共通の親クラス
module MrubyBridgeCelltypePluginModule

  # プラグイン引数名 => Proc
  MrubyBridgePluginArgProc = { 
      "ignoreUnsigned" => Proc.new { |obj,rhs| obj.set_ignoreUnsigned rhs },
      "include_inner_cell" => Proc.new { |obj,rhs| obj.set_include_inner_cell rhs },
      "exclude_cell" => Proc.new { |obj,rhs| obj.set_exclude_cell rhs },
      "exclude_port" => Proc.new { |obj,rhs| obj.set_exclude_port rhs },
      "exclude_port_func" => Proc.new { |obj,rhs| obj.set_exclude_port_func rhs },
      "auto_exclude" => Proc.new { |obj,rhs| obj.set_auto_exclude rhs },
  }

  require_tecsgen_lib( "MrubyBridgeCellPlugin.rb" )
  @@plugin_list = []
  @@count = 1

  #celltype::     Celltype        セルタイプ（インスタンス）
  def initialize( celltype, option )
    dbgPrint "#{self.class.name}: initialzie: #{celltype.get_name}\n"

    super
    @celltype = celltype
    @cell_list = []
    @include_inner_cell = false
    @exclude_cells = []
    @exclude_port = []
    @exclude_port_func = {}
    @b_ignoreUnsigned = false
    @b_auto_exclude = true     # auto_exclude = true by default 
    @@plugin_list << self

    @plugin_arg_check_proc_tab = MrubyBridgePluginArgProc
    @plugin_arg_str = CDLString.remove_dquote option
    parse_plugin_arg
  end

  #=== 新しいセル
  #cell::        Cell            セル
  #
  # celltype プラグインを指定されたセルタイプのセルが生成された
  # セルタイププラグインに対する新しいセルの報告
  # generate 文により呼び出された場合、それまでに定義された cell については、initialize のタイミングで呼び出される
  def new_cell( cell )
    dbgPrint "MrubyBridgeCelltypePluginModule: new_cell: #{cell.get_name}\n"

    return if @cell_list.include? cell   # この行は、本来不要のはず
    if TECSGEN.post_coded?               # post_code 以降のセルは対象から外す
      cdl_info( "I9999 MrubyBridgeCelltypePlugin: $1 is excluded because cell generated after post_coded", cell.get_name )
      return
    end

    # include_inner_cell option
    if cell.is_cloned? && @include_inner_cell == false then
    #   p "#{cell.get_name} excluded"
      cdl_info( "I9999 MrubyBridgeCelltypePlugin: inner cell $1 is excluded", cell.get_name )
      return
    # else
    #   p "#{cell.get_name} included"
    end

    # exclude_cell option
    if @exclude_cells.include?( cell.get_name ) then
      return
    end

    opt_str = "ignoreUnsigned=#{@b_ignoreUnsigned}, auto_exclude=#{@b_auto_exclude}"
    @exclude_port.each{ |port|
      opt_str += ",exclude_port=#{port}"
    }
    @exclude_port_func.each{ |port, funcs|
      funcs.each{ |func|
        opt_str += ",exclude_port_func=#{port}.#{func}"
      }
    }

    # p "MrubyBridgeCelltypePlugin: opt_str=#{opt_str}"

    fn2 = "#{$gen}/tmp_MrubyBridgeCelltypePluginModule_#{@celltype.get_name}_#{@@count}.cdl"
    f2 = File.open( fn2, "w" )
    f2.print <<EOT
/* MrubyBridgeCelltypePluginModule: celltype=#{@celltype.get_name} */
generate( MrubyBridgeCellPlugin, #{cell.get_namespace_path}, "#{opt_str}" );
EOT
    f2.close
    dbgPrint "MrubyBridgeCelltypePluginModule new_cell: Import #{fn2}\n"
    Import.new "#{fn2}"
    @@count += 1
  end

### 意味解析段階で呼び出されるメソッド ###
  #===  CDL ファイルの生成
  #      typedef, signature, celltype, cell のコードを生成
  #      重複して生成してはならない
  #      すでに生成されている場合は出力しないこと。
  #      もしくは同名の import により、重複を避けること。
  #file::        FILE       生成するファイル
  def gen_cdl_file file
#   この段階で呼びだすと generate 文が呼び出される前のセルのみの出力となる
    
#    dbgPrint "MrubyBridgeCelltypePlugin: gen_cdl_file: #{@celltype.get_name}\n"
#    file.print <<EOT
#/* MrubyBridgeCelltypePlugin: celltype=#{@celltype.get_name}
# *
# *    cell's generate before celltype's generate
# */
#
#EOT
#    @celltype.get_cell_list.each { |cell|
#      @cell_list << cell
#      # mikan option, region
#      dbgPrint "MrubyBridgeCelltypePlugin: cell=#{cell.get_name}\n"
#      file.print <<EOT
#generate( MrubyBridgeCellPlugin, #{cell.get_namespace_path}, "" );
#EOT
#    }

  end

  #=== tCelltype_factory.h に挿入するコードを生成する
  # file 以外の他のファイルにファクトリコードを生成してもよい
  # セルタイププラグインが指定されたセルタイプのみ呼び出される
  def gen_factory file
  end

  def get_celltype
    @celltype
  end

  @@b_gen_post_code_called = false
  #=== 後ろの CDL コードを生成
  #プラグインの後ろの CDL コードを生成
  #file:: File: 
  def self.gen_post_code( file )
    dbgPrint "#{self.name}: gen_post_code_body\n"

    if @@b_gen_post_code_called == false then
      @@b_gen_post_code_called = true
      # MrubyBridgeCellPlugin.gen_post_code_body file
    end

#    この段階で生成すると、同じポストコードで出力される mruby の初期化コードに反映されない
    
#    # MrubyBridgeCelltypePlugin の生成する generate 文は、
#    fn2 = "#{$gen}/tmp_MrubyBridgeCelltypePlugin_post.cdl"
#    f2 = File.open( fn2, "w" )
#    # 複数のプラグインの post_code が一つのファイルに含まれるため、以下のような見出しをつけること
#    dbgPrint "MrubyBridgeCelltypePlugin: gen_post_code\n"
#    f2.print "/* '#{self.name}' post code */\n"
#    @@plugin_list.each{ |plugin|
#      plugin.get_celltype.get_cell_list.each{ |cell|
#        # mikan option, region
#        f2.print <<EOT
#generate( MrubyBridgeCellPlugin, #{cell.get_namespace_path}, "" );
#EOT
#      }
#    }
#    f2.close
#    p Import
#    Import.new "#{fn2}"
  end

  #=== プラグイン引数 

  #=== プラグイン引数 ignoreUnsigned
  def set_ignoreUnsigned rhs
    if rhs == "true" || rhs == nil then
      @b_ignoreUnsigned = true
    end
  end

  def set_include_inner_cell rhs
    if rhs == "true" || rhs == nil then
      @include_inner_cell = true
    end
  end

  def set_exclude_cell rhs
    cells = rhs.split ','
    cells.each{ |rhs_cell|
      rhs_cell.gsub!( /\s/, "" )
      @exclude_cells << rhs_cell.to_sym
    }
  end
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
    ct = @celltype
    return if ct == nil    # error case
    port_funcs.each{ |rhs_port_func|
      port_func = rhs_port_func.split( '.' )
      if port_func.length != 2 then
        cdl_error( "MRB9999 exclude_port_func: '$1' not in 'port.func' form", rhs_port_func )
      end
      obj = ct.find( port_func[0].to_sym )
      if( ( ! obj.instance_of? Port ) || obj.get_port_type != :ENTRY ) then
        cdl_error( "MRB9999 exclude_port_func: port '$1' not found in celltype '$2'", rhs_port_func, ct.get_name )
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
          cdl_error( "MRB9999 include_port_func: func '$1' not found in port '$2' celltype $3",
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

