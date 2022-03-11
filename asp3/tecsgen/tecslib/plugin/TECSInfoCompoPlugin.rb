# -*- coding: utf-8 -*-
#
#  TECS Generator
#      Generator for TOPPERS Embedded Component System
# 
#   Copyright (C) 2017-2018 by TOPPERS Project
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
#   $Id: TECSInfoPlugin.rb 2952 2018-05-07 10:19:07Z okuma-top $
#++

#== CelltypePlugin for tTECSInfo
class TECSInfoCompoPlugin < CompositePlugin

  @@cell_list = []
  #@celltype::Celltype
  #@plugin_arg_str::String
  #@plugin_arg_list::{argNameString=>argOptionString}
  #@cell_list::[Cell]

  #celltype::     Celltype        セルタイプ（インスタンス）
  def initialize( celltype, option )
    super
    if $unopt_entry == false then
      cdl_info( "TIF0001 forcely set --unoptimize-entry by TECSInfoPlugin (by importing TECSInfo.cdl)" )
      $unopt_entry = true
    end
  end

  #=== 新しいセル
  #cell::        Cell            セル
  #
  # celltype プラグインを指定されたセルタイプのセルが生成された
  # セルタイププラグインに対する新しいセルの報告
  def new_cell( cell )
    @@cell_list << cell

    # AppFile は、重ね書きようなので、やめる
    # p "import: cell nTECSInfo::tTECSInfoSub #{cell.get_namespace_path.to_s} under #{cell.get_region.get_name}"
    # cell.show_tree 0
    # TECSInfoSub セルのプロトタイプ宣言
    fn = "#{$gen}/tmp_#{cell.get_region.get_global_name}_TECSInfoSub.cdl"
    File.open( fn, "w" ){ |f|
      f.print "/* prototype declaration of TECSInfoSub */\n"
      nest = cell.get_region.gen_region_str_pre f
      indent = "    " * nest
      f.print <<EOT
#{indent}[in_through()]
#{indent}region rTECSInfo {
#{indent}    cell nTECSInfo::tTECSInfoSub TECSInfoSub;
#{indent}}; /* rTECSInfo} */
EOT
      cell.get_region.gen_region_str_post f
    }
    Import.new fn

    # セルに cTECSInfo の結合があるか？
    if cell.get_join_list.get_item( :cTECSInfo ) == nil then
      # cTECSInfo = rTECSInfo::TECSInfosub.eTECSInfo; の追加
      nsp = NamespacePath.new( :rTECSInfo, false )
      nsp.append! :TECSInfoSub
      rhs = Expression.create_cell_join_expression nsp, nil, :eTECSInfo
      join = Join.new :cTECSInfo, nil, rhs
      cell.new_join join
    end
  end
end
