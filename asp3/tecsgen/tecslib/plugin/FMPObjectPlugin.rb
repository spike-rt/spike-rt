# coding: utf-8
#
#  TECS Generator
#      Generator for TOPPERS Embedded Component System
#  
#   Copyright (C) 2019-2021 by TOPPERS Project
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
#  $Id: FMPObjectPlugin.rb 3203 2021-02-06 12:27:30Z okuma-top $
#++

class FMPObjectPlugin < CelltypePlugin

  cre_params_proc        = Proc.new{ |plugin, cell, params| plugin.cre_params cell, params }
  cre_params_task_proc   = Proc.new{ |plugin, cell, params| plugin.cre_params_task cell, params }
  cre_params_mutex_proc  = Proc.new{ |plugin, cell, params| plugin.cre_params_mutex cell, params }
  cre_params_kernel_proc = Proc.new{ |plugin, cell, params| plugin.cre_params_kernel cell, params }

  @@api = {
    "TASK"                  => [ cre_params_task_proc,   "TSK", :id, :attribute, :priority, :stackSize ],
    "SEMAPHORE"             => [ cre_params_proc,        "SEM", :id, :attribute, :initialCount, :maxCount],
    "EVENTFLAG"             => [ cre_params_proc,        "FLG", :id, :attribute, :flagPattern],
    "DATAQUEUE"             => [ cre_params_proc,        "DTQ", :id, :attribute, :dataCount, :dataqueueManagementBuffer],
    "PRIORITY_DATAQUEUE"    => [ cre_params_proc,        "PDQ", :id, :attribute, :dataCount, :maxDataPriority, :priorityDataqueueManagementBuffer],
    "FIXED_SIZE_MEMORYPOOL" => [ cre_params_proc,        "MPF", :id, :attribute, :blockCount, :blockSize, :memoryPool, :memoryPoolManagementBuffer],
    "MESSAGE_BUFFER"        => [ cre_params_proc,        "MBF", :id, :attribute, :maxMessageSize, :bufferSize, :mbfmb],
    "SPINLOCK"              => [ cre_params_proc,        "SPN", :id, :attribute],
    "MUTEX"                 => [ cre_params_mutex_proc,  "MTX", :id, :attribute, :ceilingPriority],
    "KERNEL"                => [ cre_params_kernel_proc, "SYS"],
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

  #=== FMPObjectPlugin#print_cfg_cre
  # CRE_XXXの出力
  # file:: FILE:     出力先ファイル
  # indent :: string:   インデント用のtab
  def print_cfg_cre(file, cell, indent)
    dbgPrint "print_cfg_cre: #{cell.get_name} #{@plugin_arg_str}"
    file.print indent, "/* FMPObjectPlugin: #{cell.get_name} */\n"
    if @@api.has_key?(@plugin_arg_str) == false
      raise "#{@plugin_arg_str} is unknown"
    else
      params = @@api[@plugin_arg_str]
      proc = params[0]
      file.print indent
      file.puts "CRE_#{params[1]}(#{proc.call self, cell, params});"
    end
  end

  #------  KERNEL  ------#
  def cre_params_kernel cell, params
    # tKernel セルタイプは、カーネルオブジェクトを生成する必要がない
  end

  #------  MUTEX  ------#
  def cre_params_mutex cell, params
    celltype   = cell.get_celltype
    name_array = celltype.get_name_array cell

    arg_list = []
    if ((celltype.get_cell_attr_var_init_str( cell, name_array, :attribute) =~ /\bTA_CEILING\b/)==nil)
      slice_end = -2   # :ceilingPriority を外す
    else
      slice_end = -1
    end
    params.slice(3..slice_end).each { |attr|
      arg_list << celltype.get_cell_attr_var_init_str( cell, name_array, attr )
    }
    id = celltype.get_cell_attr_var_init_str cell, name_array, :id
    return "#{id}, { #{arg_list.join(", ")} }"
  end

  #----- TASK ------#
  def cre_params_task cell, params
    celltype   = cell.get_celltype
    name_array = celltype.get_name_array cell
    attr = params[3]
    arg_list = [ celltype.get_cell_attr_var_init_str( cell, name_array, attr ) ]
    arg_list += [ "(intptr_t)#{name_array[7]}", "tTask_start" ]
    params.slice(4..-1).each { |attr|
      arg_list << celltype.get_cell_attr_var_init_str( cell, name_array, attr )
    }
    id = celltype.get_cell_attr_var_init_str cell, name_array, :id
    return "#{id}, { #{arg_list.join(", ")} }"
  end
  
  #----- SEMAPHORE, EVENTFLAG, DATAQUEUE, PRIORITY_DATAQUEUE ------#
  #----- FIXED_SIZE_MEMORYPOOL, MESSAGE_BUFFER -------#
  def cre_params cell, params
    celltype   = cell.get_celltype
    name_array = celltype.get_name_array cell

    arg_list = []
    params.slice(3..-1).each { |attr|
      arg_list << celltype.get_cell_attr_var_init_str( cell, name_array, attr )
    }
    id = celltype.get_cell_attr_var_init_str cell, name_array, :id
    return "#{id}, { #{arg_list.join(", ")} }"
  end
  
  #=== 新しいセル
  #cell::        Cell            セル
  #
  # celltype プラグインを指定されたセルタイプのセルが生成された
  # セルタイププラグインに対する新しいセルの報告
  def new_cell( cell )
    @cell_list << cell
    root = cell.get_owner.get_class_root
    if root then
      class_type = root.get_class_type
      if class_type == nil then
        cdl_error2( cell.get_locale, "FMP9999 $1: not in class region or no class specified region exists.", cell.get_name )
      end
    else
      raise "root is nil"
    end
  end

  #=== tCelltype_factory.h に挿入するコードを生成する
  # file 以外の他のファイルにファクトリコードを生成してもよい
  # セルタイププラグインが指定されたセルタイプのみ呼び出される
  def gen_factory file
    f = AppFile.open( "#{$gen}/tecsgen.cfg" )
    f.print "/* Generated by FMPObjectPlugin */\n"
    cls_kernel_prev = nil
    @cell_list.each{ |cell|
      root = cell.get_owner.get_class_root

      class_type = root.get_class_type
      if cell.get_celltype.is_active? then
        cls_tecs = class_type.get_option[0].to_sym
        cls_kernel = class_type.get_plugin.get_PU_kernel_class
      else
        cls_tecs = class_type.get_option[1].to_sym
        cls_kernel = class_type.get_plugin.get_NPU_kernel_class
      end
      dbgPrint "FMPObjectPlugin#gen_factory: option=#{cls_tecs} class_in_kernel=#{cls_kernel} cell=#{cell.get_name}\n"
      if cls_kernel != cls_kernel_prev then
        if cls_kernel_prev != nil then
          if cls_kernel_prev != "global" then
            f.print "}\n"
          end
        end
        if cls_kernel != "global" then
          f.print "CLASS(#{cls_kernel}){\n"
          indent = "  "
        else
          indent = ""
        end
        cls_kernel_prev = cls_kernel
      end
      print_cfg_cre f, cell, indent
    }
    if cls_kernel_prev != nil then  # 実際のところ nil になることはないハズ
      if cls_kernel_prev != "global" then
        f.print "}\n"
      end
    end
    f.close
  end

  #=== 後ろの CDL コードを生成
  #プラグインの後ろの CDL コードを生成
  #file:: File: 
  def self.gen_post_code( file )
    # 複数のプラグインの post_code が一つのファイルに含まれるため、以下のような見出しをつけること
    # file.print "/* '#{self.class.name}' post code */\n"
  end

end


  
