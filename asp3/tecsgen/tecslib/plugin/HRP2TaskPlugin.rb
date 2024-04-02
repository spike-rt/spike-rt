# -*- coding: utf-8 -*-
#
#  TECS Generator
#      Generator for TOPPERS Embedded Component System
#  
#   Copyright (C) 2008-2014 by TOPPERS Project
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
#   $Id: HRP2TaskPlugin.rb 2952 2018-05-07 10:19:07Z okuma-top $
#++

#require "HRP2KernelObjectPlugin"
require_tecsgen_lib "HRP2KernelObjectPlugin.rb"
#== celltype プラグインの共通の親クラス
class HRP2TaskPlugin < HRP2KernelObjectPlugin
    # @@ep = [:eActivateTask, :eControlTask, :eManageTask, :eReferTask ]
    # ATT_MODを生成済みかどうか
    @@generate_memory_module = false

    ##
    #
    # file : output file (ex.tecsgen.cfg)
    # cell : 
    # val  : 
    # tab  : 
    def print_cfg_cre(file, cell, val, tab)
        #val[:id] = val[:id].gsub( /(^|[^\$])\$id\$/, "\\1#{@celltype.get_name.to_s}_#{cell.get_global_name.to_s}" )
        #val[:id] = @celltype.subst_name( val[:id], @celltype.get_name_array( cell ) )
        # $cbp$の代わり
        index = cell.get_id - @celltype.get_id_base
        #cell_CB_name = "#{@celltype.get_global_name}_pCB_tab[#{index}]"
        cell_CB_name = "#{index}"
        # CRE_XXX/DEF_XXXの生成
        dbgPrint "assign task plugin\n"
        domainOption = cell.get_region.get_domain_root.get_domain_type.get_option
        # if cell.get_region.get_region_type == :DOMAIN
# cell.show_tree 1
        if domainOption != "OutOfDomain"
            # 保護ドメインに属する場合
            if domainOption == "trusted"
                # カーネルドメイン
                if val[:userStackSize] != "OMIT"
                    raise "system task cannot have user stack."
                end
p "CRE_TSK 0 user=#{val[:userStackSize]} system=#{val[:systemStackSize]}"
                file.print <<EOT
#{tab}CRE_TSK(#{val[:id]}, { #{val[:taskAttribute]}, #{cell_CB_name}, tTask_start_task, #{val[:priority]}, #{val[:systemStackSize]}, NULL });
EOT
            else
                # ユーザドメイン
                if val[:userStackSize] == "OMIT"
                    raise "user task must have user stack."
                end
p "CRE_TSK 1"
                if val[:systemStackSize] == "OMIT"
                    file.print <<EOT
#{tab}CRE_TSK(#{val[:id]}, { #{val[:taskAttribute]}, #{cell_CB_name}, tTask_start_task, #{val[:priority]}, #{val[:userStackSize]}, NULL });
EOT
                else
p "CRE_TSK 2"
                    file.print <<EOT
#{tab}CRE_TSK(#{val[:id]}, { #{val[:taskAttribute]}, #{cell_CB_name}, tTask_start_task, #{val[:priority]}, #{val[:userStackSize]}, NULL, #{val[:systemStackSize]}, NULL });
EOT
                end
            end
        else
            # 無所属の場合
            raise "task #{val[:id]} must belong to a domain."
        end
   
        # 例外用のを生成
        file.print <<EOT
#{tab}DEF_TEX(#{val[:id]}, { #{val[:exceptionAttribute]}, tTask_start_exception });
EOT
    end

    def print_cfg_sac(file, val, acv)
       file.puts "SAC_TSK(#{val[:id]}, { #{acv[0]}, #{acv[1]}, #{acv[2]}, #{acv[3]} });"
    end

=begin
    #tTaskの受け口リスト 
    def get_entry_ports_name_list()
        @@ep
    end
=end
    # 
    #  ATT_MODの生成
    #  gen_factory実行時には，すべてのセルタイププラグインを生成済みのはずなので，
    #  カーネルAPIコードのメモリ保護を省略できる．
    #
    def gen_factory file
        super
        if @@generate_memory_module == false
            check_celltype_list = []

            Cell.get_cell_list2.each { |cell|
                # すべてのセルを走査してセルタイプをチェック
                ct = cell.get_celltype
                if ct.class == Celltype && check_celltype_list.include?( ct ) == false
                    # チェック済みセルタイプに登録
                    check_celltype_list << ct

                    # 未チェックのセルタイプだった場合
                    # puts "check for ATT_MOD : #{ct.classget_global_name}"
                    puts "check for ATT_MOD : #{ct.get_global_name}"
                        
                    # カーネルAPIのコード，データはメモリ保護しない
                    next if HRP2KernelObjectPlugin.include_celltype?( ct )

                    # 必要のないセルタイプのコード，データはメモリ保護しない
                    next if ! ct.need_generate?

                    # HRP2のドメインリージョンを取得
                    regions = ct.get_domain_roots
                    regions_hrp2 = regions[ :HRP2 ]
                    dbgPrint "HRP2 domain in #{ct.get_name}: "
                    regions_hrp2.each { |reg|
                        dbgPrint reg.get_name
                    }
                    puts ""

                    # セル管理ブロックとスケルトンのメモリ保護
                    # gen_celltype_names_domain 相当の処理
                    if regions_hrp2.include?( Region.get_root ) == false && regions_hrp2.length > 1
                        # ドメインが複数で，OutOfDomainにセルが存在しないセルタイプの場合
                        # 共有のセル管理ブロックとスケルトンコードを登録する
                        file = AppFile.open( "#{$gen}/tecsgen.cfg" )
                        file.print "ATT_MOD(\"#{ct.get_global_name}_tecsgen.o\");\n"
                        file.close
                    end

                    regions_hrp2.each { |reg|
                        if reg.is_root? 
                            nsp = ""
                        else
                            nsp = "_#{reg.get_namespace_path.get_global_name}"
                        end
                        file = AppFile.open( "#{$gen}/tecsgen#{nsp}.cfg" )
                        file.print "ATT_MOD(\"#{ct.get_global_name}#{nsp}_tecsgen.o\");\n"
                        file.close
                    }

                    # セルタイプコードがない場合はスキップ
                    next if ct.is_all_entry_inline? && ! ct.is_active?

                    # セルタイプコードのメモリ保護
                    # gen_celltype_names_domain2 相当の処理
                    if regions_hrp2.include?( Region.get_root ) == true || regions_hrp2.length > 1
                        # OutOfDomainにセルが存在するセルタイプの場合
                        # または，複数のドメインにセルが存在するセルタイプの場合
                        # セルタイプコードを共有するように登録する
                        file = AppFile.open( "#{$gen}/tecsgen.cfg" )
                    else
                        # OutOfDomainでない単一のドメインにセルが存在するセルタイプの場合
                        # セルタイプコードを専有するように登録する
                        file = AppFile.open( "#{$gen}/tecsgen_#{regions_hrp2[0].get_namespace_path.get_global_name}.cfg" )
                    end

                    file.print "ATT_MOD(\"#{ct.get_global_name}.o\");\n"
                    file.close
                else
                    # 何もしない
                end
            }

            @@generate_memory_module = true
        else
            # 何もしない
        end
    end
    
end

