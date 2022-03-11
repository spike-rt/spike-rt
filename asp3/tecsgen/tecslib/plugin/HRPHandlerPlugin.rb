# coding: utf-8
#
#  TECS Generator
#      Generator for TOPPERS Embedded Component System
#  
#   Copyright (C) 2008-2018 by TOPPERS Project
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
#++

require_tecsgen_lib "HRPKernelObjectPlugin.rb"


#== celltype プラグインの共通の親クラス
class HRPHandlerPlugin < HRPKernelObjectPlugin

    ##
    # oyama: add to avoid Ruby exception in code generation phase.
    def new_cell cell
        domainOption = cell.get_region.get_domain_root.get_domain_type.get_option
        if (domainOption == "OutOfDomain") || (domainOption != "kernel")
          cdl_error( "HRP9999 HRP handler '$1' must belong to kernel domain", cell.get_name )
        end
    end

    ##
    #
    # file : output file (ex.tecsgen.cfg)
    # cell : 
    # val  : 
    # tab  : 
    def print_cfg_cre(file, cell, val, tab)
        if !val[:id].nil?
            val[:id] = val[:id].gsub( /(^|[^\$])\$id\$/, "\\1#{@celltype.get_name.to_s}_#{cell.get_name.to_s}" )
        end
        # $cbp$の代わり
        index = cell.get_id - @celltype.get_id_base
        #cell_CB_name = "#{@celltype.get_global_name}_CB_tab[#{index}]"
        name_array = cell.get_celltype.get_name_array cell
        cell_CBP = name_array[8]
        # CRE_XXXの生成
        domainOption = cell.get_region.get_domain_root.get_domain_type.get_option
        if (domainOption == "OutOfDomain") || (domainOption != "kernel")
            # 無所属 or ユーザドメインに属する場合
                raise "#{@plugin_arg_str.to_s.downcase} handler #{val[:id]} must belong to kernel domain."
        elsif @plugin_arg_str == "CONFIG_INT"
            file.print <<EOT
#{tab}CFG_INT( #{val[:interruptNumber]}, { #{val[:attribute]}, #{val[:interruptPriority]} });
EOT
        elsif @plugin_arg_str == "ISR"
            file.print <<EOT
#{tab}CRE_ISR(#{val[:id]}, { #{val[:attribute]}, #{cell_CBP}, #{val[:interruptNumber]}, tISR_start, #{val[:isrPriority]} });
EOT
        elsif @plugin_arg_str == "DEF_INH"
            name_array = @celltype.get_name_array cell
            start = @celltype.subst_name( "$id$_start", name_array )
            file.print <<EOT
#{tab}DEF_INH(#{val[:interruptHandlerNumber]}, { #{val[:attribute]}, #{start} });
EOT
        elsif @plugin_arg_str == "DEF_EXC"
            name_array = @celltype.get_name_array cell
            start = @celltype.subst_name( "$id$_start", name_array )
            file.print <<EOT
#{tab}DEF_EXC(#{val[:cpuExceptionHandlerNumber]}, { #{val[:attribute]}, #{start} });
EOT
        elsif @plugin_arg_str == "INIT_ROUTINE"
            file.print <<EOT
#{tab}ATT_INI({ #{val[:attribute]}, #{cell_CBP}, tInitializeRoutine_start });
EOT
        elsif @plugin_arg_str == "TERM_ROUTINE"
            file.print <<EOT
#{tab}ATT_TER({ #{val[:attribute]}, #{cell_CBP}, tTerminateRoutine_start });
EOT
        else
            raise "#{@plugin_arg_str} is unknown option"
        end
    end

    def print_cfg_sac(file, val, acv)
        if @plugin_arg_str == "CONFIG_INT"
            # nothing to do
        elsif @plugin_arg_str == "ISR"
            # kernel.cdl の tISR で accessPattern1 ～ accessPattern4 が定義されているときの名残。
            # accessPattern1 が定義されていない場合、こここへは来ない．復活したときに備えて残しておく．
            # file.puts "SAC_#{@plugin_arg_str}(#{val[:id]}, { #{acv[:accessPattern1]}, #{acv[:accessPattern2]}, #{acv[:accessPattern3]}, #{acv[:accessPattern4]} });"
            puts "*** Unsupported *** SAC_#{@plugin_arg_str}(#{val[:id]}, { #{acv[:accessPattern1]}, #{acv[:accessPattern2]}, #{acv[:accessPattern3]}, #{acv[:accessPattern4]} });\n"
        elsif @plugin_arg_str == "DEF_INH"
            # nothing to do
        elsif @plugin_arg_str == "DEF_EXC"
            # nothing to do
        elsif @plugin_arg_str == "INIT_ROUTINE"
            # nothing to do
        elsif @plugin_arg_str == "TERM_ROUTINE"
            # nothing to do
        else
            raise "#{@plugin_arg_str} is unknown option"
        end
    end

end

