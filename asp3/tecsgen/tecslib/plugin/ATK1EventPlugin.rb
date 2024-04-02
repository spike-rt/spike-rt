# -*- coding: utf-8 -*-
#
#  TECS Generator Cell-type plugin
#      for TOPPERS Embedded Component System
#  
#   Copyright (C) 2008-2014 by TOPPERS Project TECS-WG
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
#   $Id
#++

#== ATK1 Event celltype プラグインクラス
class ATK1EventPlugin < CelltypePlugin
#@@instancies:: []     :セル実体群
@@instancies = []

  #===  初期化
  #      全てのセルの実体を意味解析後に参照する必要があるために
  #      初期化時に singleton の instancies に格納する
  #signature::     Celltype        シグニチャ（インスタンス）
  def initialize( celltype, option )
    super
    @@instancies << self
  end

  #===  cellの生成
  def new_cell cell
  end

  #===  意味解析実行後のコード生成
  def self.gen_post_code file

    @@instancies.each { |inst|
      inst.gen_oil_code file
    }

  end

  #===  OILファイル出力
  def gen_oil_code file

    file2 = CFile.open( "#{$gen}/EVENT_tecsgen.oil", "w" )

    whole_mask = 0

    # EVENT
    @celltype.get_cell_list.each { |cell|

#      if cell.is_generate?
        join = cell.get_join_list.get_item( :mask )

        # AUTO type search and change to zero
        if join then
          #mask_bit = join.get_rhs.to_s.eval_cnst( nil )
          mask_bit = join.get_rhs.to_s.to_i
          mask_str = join.get_rhs.to_s
          if mask_str == "AUTO" then
            new_rhs = Expression.create_integer_constant( 32, @locale )
            join.change_rhs new_rhs
          else
            mask_pattern = 1 << mask_bit
            whole_mask |= mask_pattern
          end
        else
          new_rhs = Expression.create_integer_constant( 32, @locale )
          join = Join.new( :mask, nil, new_rhs )
          cell.new_join(join)
        end

#        file2.print "\t /* working #{join.get_rhs.to_s} */\n"
#      end

    }

    mask_count = 0;
    mask_place = 0;

    @celltype.get_cell_list.each { |cell|

#      if cell.is_generate?
        # bit place to mask pattern
        file2.print "\tEVENT #{cell.get_name} {\n"
        join = cell.get_join_list.get_item( :mask )
        if join then
          mask_bit = join.get_rhs.to_s.to_i
          if mask_bit == 32 then
            while 1
    p("mask_count, current_mask")
    p mask_count
              current_mask = (1 << mask_count)
    p current_mask
              if 0 == whole_mask & current_mask
                new_rhs = Expression.create_integer_constant( current_mask, @locale )
                join.change_rhs new_rhs
                whole_mask = whole_mask | current_mask
                mask_place = mask_count
                break
              end
              if current_mask >= 0x8000
                mask_place = 0
                break
              end
              mask_count = mask_count + 1
            end
          else
            mask_place = join.get_rhs.to_s.to_i
            current_mask = 1 << mask_place
            new_rhs = Expression.create_integer_constant( current_mask, @locale )
            join.change_rhs new_rhs
          end
          file2.print "\t\tMASK = #{mask_place.to_s};\n"
        end

        file2.print "\t};\n"
        file2.print "\n"

#      end

    }

    file2.close

  end

end

