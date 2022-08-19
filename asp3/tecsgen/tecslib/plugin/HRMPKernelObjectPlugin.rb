# coding: utf-8
#
#  TECS Generator
#      Generator for TOPPERS Embedded Component System
#  
#   Copyright (C) 2020-2021 by TOPPERS Project
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
#  $Id: HRMPKernelObjectPlugin.rb 3241 2021-11-21 12:06:12Z okuma-top $
#++

class HRMPKernelObjectPlugin < CelltypePlugin
  @@api = {
    # "Kernel_Object_type" => [ クラス内かどうか、ドメイン内かどうか、オブジェクトタイプ略名、
    #                           静的API生成文字列]
    "TASK" =>
               [ :InClass, :InDomain, "TSK",
                 "CRE_TSK( {{id}}, { {{attribute}}, $cbp$, tTask_start, {{priority}}, {{stackSize}} } )" ],
    "SEMAPHORE" =>
               [ :InClass, :Any, "SEM",
                 "CRE_SEM( {{id}}, { {{attribute}}, {{initialCount}}, {{maxCount}} })" ],
    "EVENTFLAG" =>
               [ :InClass, :Any, "FLG",
                 "CRE_FLG( {{id}}, { {{attribute}}, {{flagPattern}} } )" ],
    "DATAQUEUE" =>
               [ :InClass, :Any, "DTQ",
                 "CRE_DTQ( {{id}}, { {{attribute}}, {{dataCount}}, {{dataqueueManagementBuffer}} })" ],
    "PRIORITY_DATAQUEUE" =>
               [ :InClass, :Any, "PDQ",
                 "CRE_PDQ( {{id}}, { {{attribute}}, {{dataCount}}, {{maxDataPriority}}, {{priorityDataqueueManagementBuffer}} })" ],
    "FIXED_SIZE_MEMORYPOOL" =>
               [ :InClass, :Any, "MPF",
                 "CRE_MPF( {{id}}, { {{attribute}}, {{blockCount}}, {{blockSize}}, {{memoryPool}}, {{memoryPoolManagementBuffer}} })" ],
    "MESSAGE_BUFFER" =>
               [ :InClass, :Any, "MBF",
                 "CRE_MBF( {{id}}, { {{attribute}}, {{maxMessageSize}}, {{bufferSize}}, {{messageBufferManagementBuffer}} })" ],
    "SPINLOCK" =>
               [ :InClass, :Any, "SPN",
                 "CRE_SPN( {{id}}, { {{attribute}} })" ],
    "MUTEX" =>
               [ :InClass, :Any, "MTX",
                 "CRE_MTX( {{id}}, { {{attribute}}, {{ceilingPriority}} })" ],
    "KERNEL" =>
               [ :InClass, :Any, "SYS",
                 "CRE_SYS"],

    ########## Handlers #########
    "INT_REQUEST"   =>
              [:InClass, :kernel, "INT",
              "CFG_INT( {{interruptNumber}}, { {{attribute}}, {{interruptPriority}} })"],
    "INT_SERVICE_ROUTINE" =>
              [:InClass, :kernel, "ISR",
              "CRE_ISR( {{id}}, { {{attribute}}, $cbp$, {{interruptNumber}}, tISR_start, {{isrPriority}} })"],
    "INT_HANDLER"   =>
              [:InClass, :kernel, "INH",
              "DEF_INH( {{interruptHandlerNumber}}, { {{attribute}}, $id$_start})"],
    "CPU_EXCEPTION_HANDLER" =>
              [:InClass, :kernel, "EXC",
              "DEF_EXC( {{cpuExceptionHandlerNumber}}, { {{attribute}}, $id$_start})"],
    "INIT_ROUTINE"  =>
              [:any, :kernel, "INI",
              "ATT_INI({ {{attribute}}, $cbp$, tInitializeRoutine_start })"],
    "TERM_ROUTINE"  =>
              [:any, :kernel, "TER",
              "ATT_TER({ {{attribute}}, $cbp$, tTerminateRoutine_start })"],
  }

  #celltype::     Celltype        セルタイプ（インスタンス）
  def initialize( celltype, option )
    super
    @celltype = celltype
    # @plugin_arg_str = option.gsub( /\A"(.*)/, '\1' )    # 前後の "" を取り除く
    # @plugin_arg_str.sub!( /(.*)"\z/, '\1' )
    @plugin_arg_str = CDLString.remove_dquote option
    @plugin_arg_list = {}
    @cell_list = []
  end

  #=== 新しいセル
  #cell::        Cell            セル
  #
  # celltype プラグインを指定されたセルタイプのセルが生成された
  # セルタイププラグインに対する新しいセルの報告
  def new_cell( cell )
    @cell_list << cell
    if @@api.has_key?(@plugin_arg_str) == false
      raise "#{@plugin_arg_str} is unknown"
    end

    # class check
    root = cell.get_owner.get_class_root
    if root then
      class_type = root.get_class_type
      if class_type == nil then
        cdl_error2( cell.get_locale, "HRMP9999 $1: not in class region", cell.get_name )
      elsif @@api[@plugin_arg_str][0] == :InClass then
        if class_type.get_option.to_s == "root" then
          cdl_error2( cell.get_locale, "HRMP9999 $1: cannot be placed in out-of-class region", cell.get_name )
        end
      end
    else
      raise "root is nil"
    end

    # domain check
    root = cell.get_owner.get_domain_root
    if root then
      domain_type = root.get_domain_type
      if domain_type == nil then
        cdl_error2( cell.get_locale, "HRMP9999 $1: not in domain region", cell.get_name )
      elsif @@api[@plugin_arg_str][1] == :InDomain then
        if domain_type.get_option == "OutOfDomain" then
          cdl_error2( cell.get_locale, "HRMP9999 $1: not place out of domain region", cell.get_name )
        end
      elsif @@api[@plugin_arg_str][1] == :Kernel then
        if domain_type.get_option != "kernel" then
          cdl_error2( cell.get_locale, "HRMP9999 $1: not place out of kernel domain region", cell.get_name )
        end
      end
    else
      raise "root is nil"
    end
  end

  #=== HRMPObjectPlugin#print_cfg_cre
  # CRE_XXXの出力
  # file:: FILE:     出力先ファイル
  # indent :: string:   インデント用のtab
  def print_cfg_cre(file, cell, indent)
    # p "print_cfg_cre: #{cell.get_name} #{@plugin_arg_str}"
    if @@api.has_key?(@plugin_arg_str) == false
      raise "#{@plugin_arg_str} is unknown"
    else
      cfg_str = @@api[@plugin_arg_str][3]
      file.print indent
    end

    celltype   = cell.get_celltype
    name_array = celltype.get_name_array cell

    # {{attribute}} の置換
    cfg_str2 = cfg_str.gsub(/\{\{([a-zA-Z0-9_]*?)\}\}/) { |match|
      # p "match: #{match}"
    	name = $1.to_sym
      get_attr_var_val( cell, name_array, name )
		}

    # $xxx$ の置換
    cfg_str3 = celltype.subst_name( cfg_str2, name_array )

    file.print cfg_str3, ";\n"
  end

  def print_cfg_sac file, cell, indent
    celltype   = cell.get_celltype
    if celltype.find( :accessPattern1 ) != nil then
      sac_str = HRMPDomainPlugin.get_sac_str cell
      # print "HRMPKernelObjectPlugin: forcely set sac to TACP_SHARED !!!!\n"
      # sac_str = "{TACP_SHARED, TACP_SHARED, TACP_SHARED, TACP_SHARED}"

      if @@api.has_key?(@plugin_arg_str) == false
        raise "#{@plugin_arg_str} is unknown"
      elsif @plugin_arg_str != "KERNEL"
        name_array = celltype.get_name_array cell
        file.puts "#{indent}SAC_#{@@api[@plugin_arg_str].at(2)}(#{get_attr_var_val cell, name_array, :id}, #{sac_str});"
      else
        file.puts "#{indent}SAC_#{@@api[@plugin_arg_str].at(2)}(#{sac_str});"
      end
 
    end
  end

  def get_attr_var_val cell, name_array, attr
    celltype = cell.get_celltype
    str =  celltype.get_cell_attr_var_init_str( cell, name_array, attr )
    return celltype.subst_name( str, name_array )
  end

  #=== tCelltype_factory.h に挿入するコードを生成する
  # file 以外の他のファイルにファクトリコードを生成してもよい
  # セルタイププラグインが指定されたセルタイプのみ呼び出される
  def gen_factory file
    f = AppFile.open( "#{$gen}/tecsgen.cfg" )
    f.print "/* generated by HRMPKernelObjectPlugin */\n"
    option_prev = nil
    @cell_list.each{ |cell|
      region = cell.get_owner
      croot  = region.get_class_root
      class_type = croot.get_class_type
      coption = class_type.get_plugin.get_PU_kernel_class

      droot   = region.get_domain_root
      domain_type = droot.get_domain_type
      doption = domain_type.get_option

      indent = ""
      if doption == "kernel" then
        f.print "KERNEL_DOMAIN{\n"
        indent = "  "
      elsif doption == "user" then
        f.print "DOMAIN(#{droot.get_name}){\n"
        indent = "  "
      elsif doption == "OutOfDomain" then
      else
        raise "unknown"
      end
      dbgPrint "HRMPKernelObjectPlugin: cell=#{cell.get_name} celltype=#{cell.get_celltype.get_name} coption=#{coption}\n"
      indent2 = indent
      if coption != "root" then
        f.print "#{indent}CLASS(#{coption}){\n"
        indent2 = indent + "  "
      end
      print_cfg_cre f, cell, indent2
      print_cfg_sac f, cell, indent2

      if coption != "root" then
        f.print "#{indent}}\n"
      end
      if doption != "OutOfDomain" then
        f.print "}\n"
      end
    }
    f.close
  end

end
