# -*- coding: utf-8 -*-
#
#  TECS Generator
#      Generator for TOPPERS Embedded Component System
#  
#   Copyright (C) 2008-2017 by TOPPERS Project
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
#   $Id: HRP2KernelObjectPlugin.rb 2952 2018-05-07 10:19:07Z okuma-top $
#++

# Regionクラスにメソッドを追加
=begin
class Region
    def get_params
        return @region_type_param
    end

    def get_param
        return @region_type_param[0]
    end

    def is_root
        return self == @@root_namespace
    end
end
=end

# Cellクラスにインスタンス変数とメソッドを追加
=begin
class Cell
    def set_referenced_region( region )
        if @referenced_regions.nil?
            @referenced_regions = Array.new
        end
        if !@referenced_regions.include?(region)
            @referenced_regions << region
        end
    end

    def get_referenced_regions
        return @referenced_regions
    end

    # set_referenced_cellにしたほうがよい
    # def set_referenced_region( region, port_name )
    def set_referenced_cell( cell, port_name )
        if @referenced_cells.nil?
            @referenced_cells = Hash.new
        end
        if !@referenced_cells.include?(port_name)
            @referenced_cells[port_name] = Array.new
        end
        if !@referenced_cells[port_name].include?(cell)
            @referenced_cells[port_name] << cell
        end

        region = cell.get_region
        puts "***** set_referenced_regions *****"
        puts "region=#{region.get_name}, port=#{port_name}(#{port_name.class})"
        if @referenced_regions.nil?
            @referenced_regions = Hash.new
        end
        if !@referenced_regions.include?(port_name)
            @referenced_regions[port_name] = Array.new
        end
        if !@referenced_regions[port_name].include?(region)
            @referenced_regions[port_name] << region
        end
        puts "#{@referenced_regions[port_name]}"
    end

    def get_referenced_cells( port_name )
        if !@referenced_cells.nil? && @referenced_cells.include?(port_name)
            return @referenced_cells[port_name]
        else
            return nil
        end
    end

    def get_referenced_regions( port_name )
        if !@referenced_regions.nil? && @referenced_regions.include?(port_name)
            return @referenced_regions[port_name]
        else
            return nil
        end
    end

    def self.get_cell_list
        return @@cell_list
    end

    def self.get_cell_list2
        return @@cell_list2
    end
end
=end

#== celltype プラグインの共通の親クラス
class HRP2KernelObjectPlugin < CelltypePlugin
    # @@obj_hash = {}

    #@celltype:: Celltype
    #@option:: String     :オプション文字列
    def initialize( celltype, option )
        super
        # それぞれのカーネルオブジェクトを解析対象セルタイプに追加
        HRP2KernelObjectPlugin.set_celltype(celltype)
    end
  
    #=== HRP2KernelObjectPlugin#print_cfg_cre
    # 各種カーネルオブジェクトのCRE_*の出力
    # file:: FILE:     出力先ファイル
    # val :: string:   カーネルオブジェクトの属性の解析結果
    # tab :: string:   インデント用のtab
    def print_cfg_cre(file, cell, val, tab)
        raise "called virtual method print_cfg_cre in #{@celltype.get_name} plugin"
    end
  
    #=== HRP2KernelObjectPlugin#print_cfg_sac
    # 各種カーネルオブジェクトのSAC_*の出力
    # file:: FILE:     出力先ファイル
    # val :: string:   カーネルオブジェクトの属性の解析結果
    # acv :: string:   アクセスベクタ
    def print_cfg_sac(file, val, acv)
        raise "called virtual method print_cfg_sac in #{@celltype.get_name} plugin"
    end
  
    #=== HRP2KernelObjectPlugin#get_entry_ports_name_list
    # カーネルオブジェクトの各種アクセスの種別の受け口名を取得
    def get_entry_ports_name_list()
        raise "called virtual method get_entry_ports_name_list in #{@celltype.get_name} plugin"
    end

    def string_cfg_cre(cell, val)
        raise "called virtual method string_cfg_cre in #{@celltype.get_name} plugin"
    end

    def gen_factory file
        puts "===== begin #{@celltype.get_name.to_s} plugin ====="

        # 対象となるすべてのセルについて、受け口に結合されている
        # セルの所属ドメインを解析
        if !HRP2KernelObjectPlugin.isChecked()
            HRP2KernelObjectPlugin.check_referenced_cells()
        else
            puts "***** already checked"
        end

        # 追記するために AppFile を使う（文字コード変換されない）
        file2 = AppFile.open( "#{$gen}/tecsgen.cfg" )
        file2.print "\n/* Generated by #{self.class.name} */\n\n"

        @celltype.get_cell_list.each { |cell|
            if cell.is_generate?
                puts "===== begin check my domain #{cell.get_name} ====="
                # attrの値をハッシュに入れる
                val = {}
                @celltype.get_attribute_list.each{ |a|
                    # p a.get_name
                    if a.get_type.kind_of?( ArrayType )
                        val[a.get_name] = []
                        if j = cell.get_join_list.get_item(a.get_name)
                            # セル生成時に初期化する場合
                            j.get_rhs.each { |elem|
                                val[a.get_name] << elem.to_s
                            }
                        elsif i = a.get_initializer
                            # セルタイプの初期化値を使う場合
                            i.each { |elem|
                                val[a.get_name] << elem.to_s
                            }
                        else
                            raise "attribute is not initialized"
                        end
                    else
                        if j = cell.get_join_list.get_item(a.get_name)
                            # セル生成時に初期化する場合
                            val[a.get_name] = j.get_rhs.to_s
                        elsif i = a.get_initializer
                            # セルタイプの初期化値を使う場合
                            val[a.get_name] = i.to_s
                        else
                            raise "attribute is not initialized"
                        end
                    end
                }
                # generate.rbを参考に
                # $id$を置換
                if val[:id].nil? != true
                    puts val[:id]
                    #val[:id] = val[:id].gsub( /(^|[^\$])\$id\$/, "\\1#{@celltype.get_name.to_s}_#{cell.get_name.to_s}" )
                    val[:id] = @celltype.subst_name( val[:id], @celltype.get_name_array( cell ) )
                end
                # $cbp$の代わり
                index = cell.get_id - @celltype.get_id_base
                cell_CB_name = "#{@celltype.get_global_name}_CB_tab[#{index}]"
                cell_domain_root = cell.get_region.get_domain_root
                cell_domain_type = cell.get_region.get_domain_root.get_domain_type

                # CRE_XXX/DEF_XXXの生成
                if cell_domain_type.get_option.to_s != "OutOfDomain"
                    # 保護ドメインに属する場合
                    if !HRP2KernelObjectPlugin.include_region(cell_domain_root.get_name.to_s)
                        # その保護ドメインの.cfgが生成されていない場合
                        HRP2KernelObjectPlugin.set_region_list(cell_domain_root.get_name.to_s)
                        puts "~~~~~ #{cell_domain_root.get_name.to_s} is registered!"

                        # if cell.get_region.get_param == :KERNEL_DOMAIN
                        if cell_domain_type.get_option.to_s == "trusted"
                            # file2.print "#{cell.get_region.get_name.to_s}{\n"
                            file2.print "KERNEL_DOMAIN{\n"
                        else
                            file2.print "DOMAIN(#{cell_domain_root.get_name.to_s}){\n"
                        end
                        file2.puts "\tINCLUDE(\"#{$gen}/tecsgen_#{cell_domain_root.get_name.to_s}.cfg\");" 
                        file2.puts "}\n"
                    else
                        dbgPrint "~~~~~ #{cell.get_region.get_name.to_s} is included in"
                        # p @@region_list
                    end
                    file3 = AppFile.open( "#{$gen}/tecsgen_#{cell_domain_root.get_name.to_s}.cfg" )
                    print_cfg_cre(file3, cell, val,"")
                    file3.close
                else
                    # 無所属の場合
                    puts "~~~~~ #{cell_domain_root.get_name.to_s} is OutOfDomain"
                    print_cfg_cre(file2, cell, val, "")
                end

                puts "===== end check my domain #{cell.get_name} ====="
                # SAC_XXXの生成
                if !val[:accessPattern].nil?
                    puts "===== begin check regions #{cell.get_name} ====="
                    # p val[:accessPattern]
                    # p val[:accessPattern].class

                    #ep = [ :eTaskActivate, :eTaskControl, :eTaskManage, :eTaskRefer ]
                    #各カーネルオブジェクトの受け口名を取得
                    # ep = get_entry_ports_name_list()
                    i = 0
                    acv = []
                    # アクセス許可ベクタの生成
                    val[:accessPattern].each { |acptnx|
                        # アクセス許可パターンの生成
                        if acptnx != "OMIT"
                            acv << acptnx
                            # p acv[i]
                        elsif cell_domain_type.get_option.to_s == "trusted"
                            acv << "TACP_KERNEL"
                            # p acv[i]
                        elsif cell_domain_type.get_option.to_s != "OutOfDomain"
                            acv << "TACP(#{cell_domain_root.get_name.to_s})"
                        else
                            acv << "TACP_SHARED"
                        end

                        i += 1
                    }

                    dbgPrint "acv = "
                    p acv

                    #各種SACの生成
                    domainOption = cell_domain_type.get_option
                    # if cell.get_region.get_region_type == :DOMAIN
                    if domainOption != "OutOfDomain"
                        # 保護ドメインに属する場合
                        file3 = AppFile.open( "#{$gen}/tecsgen_#{cell.get_region.get_name.to_s}.cfg" )
                        print_cfg_sac(file3, val, acv)
                        file3.close
                    else
                        # 無所属の場合
                        print_cfg_sac(file2, val, acv)
                    end

                    puts "===== end check regions #{cell.get_name} ====="
                end
            end
        }

        puts "===== end #{@celltype.get_name.to_s} plugin ====="
        file2.close
    end

    # カーネルオブジェクトセルタイプの管理
    # HRP2KernelObjectPluginクラスに対してメソッド呼出しを行うことを想定
    @@checked = false
    @@celltype_list = []
    @@region_list = []

    def self.isChecked()
        return @@checked
    end

    def self.check_referenced_cells()
        dbgPrint "===== begin check registered celltype =====\n"
        self.get_celltype_list.each { |ct|
            dbgPrint( ct.get_name.to_s + "\n" )
        }
        dbgPrint "===== end check registered celltype =====\n"

=begin
        Cell.get_cell_list2.each { |cell|
            cell.get_celltype.get_port_list.each{ |p|
                next if p.get_port_type != :CALL
                j = cell.get_join_list.get_item(p.get_name)
                printf "===== check call port : "
                # p p.get_name.to_s
                next if j.nil? # 未結合の場合
                if @@celltype_list.include?(j.get_celltype)
                    # j.get_cell.set_referenced_region(cell.get_region)
                    # j.get_cell.set_referenced_region(cell.get_region, j.get_port_name)
                    j.get_cell.set_referenced_cell(cell, j.get_port_name)
                    printf "===== check joined rhs cell : "
                    # p j.get_cell.get_name.to_s
                    printf "===== check joined rhs port_name : "
                    # p j.get_port_name
                end
            }
        }
=end
        @@checked = true
    end

    def self.set_celltype( celltype )
        @@celltype_list << celltype
    end

    def self.get_celltype_list
        return @@celltype_list
    end

    def self.set_region_list( region )
        @@region_list << region
    end

    def self.include_region( region )
        return @@region_list.include?(region)
    end

    def self.include_celltype?( celltype )
        return @@celltype_list.include?(celltype)
    end

end

