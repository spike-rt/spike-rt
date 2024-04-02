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
#  $Id: HRPTaskPlugin.rb 2952 2018-05-07 10:19:07Z okuma-top $
#++

require_tecsgen_lib "HRPKernelObjectPlugin.rb"
require_tecsgen_lib "HRPKernelObjectManager.rb"
#== celltype プラグインの共通の親クラス
class HRPTaskPlugin < HRPKernelObjectPlugin
    ##
    # oyama: add to avoid Ruby exception in code generation phase.
    def new_cell cell
      region = cell.get_region
      dbgPrint "HRPTaskCelltypePlugin new_cell=#{cell.get_name} region=#{region.get_name}\n"
      domainOption = cell.get_region.get_domain_root.get_domain_type.get_option
      if domainOption == "OutOfDomain"
        cdl_error( "HRP9999 HRP task '$1' must belong to a domain", cell.get_name )
      end
    end

    # # ATT_MODを生成済みかどうか                   # 2017.8.27
    # @@generate_memory_module = false

    #
    #  CRE_TSKの出力
    #  file:: FILE:     出力先ファイル
    #  cell :: Cell タスクセルインスタンス
    #  val :: Hash {attr => string}:   カーネルオブジェクトの属性の解析結果
    #  tab :: string: タブ
    #
    def print_cfg_cre(file, cell, val, tab)
        val[:id] = val[:id].gsub( /(^|[^\$])\$id\$/, "\\1#{@celltype.get_name.to_s}_#{cell.get_name.to_s}" )
        # $cbp$の代わり
        cell_IDX = @celltype.get_name_array(cell)[7]
        # CRE_TSKの生成
        dbgPrint "assign task plugin\n"
        domainOption = cell.get_region.get_domain_root.get_domain_type.get_option
        # if cell.get_region.get_region_type == :DOMAIN
        if domainOption != "OutOfDomain"
            if val[:systemStackSize] == "0"
                # 
                #  システムスタックの定義を省略
                #
                file.print <<EOT
CRE_TSK(#{val[:id]}, { #{val[:attribute]}, #{cell_IDX}, tTask_start, #{val[:priority]}, #{val[:stackSize]}, NULL });
EOT
            else
                # 
                #  システムスタックの定義を追加
                #
                file.print <<EOT
CRE_TSK(#{val[:id]}, { #{val[:attribute]}, #{cell_IDX}, tTask_start, #{val[:priority]}, #{val[:stackSize]}, NULL, #{val[:systemStackSize]}, NULL });
EOT
            end
        else
            #
            #  無所属のタスクは存在してはならないため，エラーとする
            #
            raise "task #{val[:id]} must belong to a domain."
        end
    end

    #
    #  SAC_TSKの出力
    #  file:: FILE:     出力先ファイル
    #  val :: Hash {attr => string}:   カーネルオブジェクトの属性の解析結果
    #  acv :: Hash {accessPatternx => string}:   アクセス許可ベクタ
    #
    def print_cfg_sac(file, val, acv)
       # pp "SAC_TSK"
       # pp val
       file.puts "SAC_TSK(#{val[:id]}, { #{acv[:accessPattern1]}, #{acv[:accessPattern2]}, #{acv[:accessPattern3]}, #{acv[:accessPattern4]} });"
    end
end

