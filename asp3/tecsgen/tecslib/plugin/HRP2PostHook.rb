# -*- coding: utf-8 -*-
#
#  TECS Generator
#      Generator for TOPPERS Embedded Component System
#  
#   Copyright (C) 2017 by TOPPERS Project
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
#   $Id: HRP2PostHook.rb 2952 2018-05-07 10:19:07Z okuma-top $
#++

#require "HRP2KernelObjectPlugin"
require_tecsgen_lib "HRP2KernelObjectPlugin.rb"

class HRP2PostHook
    #@celltype:: Celltype
    def initialize( celltype )
        puts "new post hook #{celltype.get_name}"
        @celltype = celltype
    end

    def generate
        # カーネルオブジェクトのセルタイプはメモリ保護しない
#=begin
        # TODO: テストのために無効化
        if HRP2KernelObjectPlugin.include_celltype?( @celltype )
            puts "nothing is generated for kernel object component"
            return
        end
#=end

        rlist = @celltype.get_region_list
        if @celltype.is_singleton?
            # singletonの場合
            puts "<singleton>"

            if rlist.length == 0
                file = AppFile.open( "#{$gen}/tecsgen.cfg" )
            else
                file = AppFile.open( "#{$gen}/tecsgen_#{rlist[0].get_param.to_s}.cfg" )
            end
            # セル管理ブロックとスケルトンのメモリ保護
            file.print "ATT_MOD(\"#{@celltype.get_global_name}_tecsgen.o\");\n"
            if !@celltype.is_all_entry_inline?
                # セルタイプコードのメモリ保護
                file.print "ATT_MOD(\"#{@celltype.get_global_name}.o\");\n"
            end

            file.close

        else
            # singletonでない場合
            puts "<non-singleton>"

            # スケルトンとセルタイプコードの保護
            if (rlist.length == 1) && 
                (@celltype.get_n_cell_gen == @celltype.get_n_cell_gen_r( rlist[0] ))
                # 所属する保護ドメインが1つの場合
                puts "<private celltype>"
                file = AppFile.open( "#{$gen}/tecsgen_#{rlist[0].get_param.to_s}.cfg" )
            else
                # 無所属 or 所属する保護ドメインが複数の場合
                puts "<shared celltype>"
                file = AppFile.open( "#{$gen}/tecsgen.cfg" )
            end

            # スケルトンのメモリ保護
            file.print "ATT_MOD(\"#{@celltype.get_global_name}_tecsgen.o\");\n"
            if !@celltype.is_all_entry_inline?
                # セルタイプコードのメモリ保護
                file.print "ATT_MOD(\"#{@celltype.get_global_name}.o\");\n"
            end
            file.close

            # セル管理ブロックの保護
            rlist.each { |reg|
                file = AppFile.open( "#{$gen}/tecsgen_#{reg.get_param.to_s}.cfg" )
                file.print "ATT_MOD(\"#{@celltype.get_global_name}_#{reg.get_name.to_s}_tecsgen.o\");\n"
                file.close
            }

        end
    end
end

