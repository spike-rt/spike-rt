# -*- coding: utf-8 -*-
#
#  TECS Generator
#      Generator for TOPPERS Embedded Component System
#
#  Copyright (C) 2015-2018 by TOPPERS Project
#
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
#  $Id: RepeatJoinPlugin.rb 2952 2018-05-07 10:19:07Z okuma-top $
#++

class RepeatJoinPlugin < CellPlugin

  RepeatJoinPluginArgProc = {
    "count" => Proc.new { |obj,rhs| obj.set_count rhs }
  }

  def initialize( cell, option )
    super
    print "RepeatJoinPlugin: #{@cell.get_name}\n"
    # cell.show_tree 0
    @plugin_arg_check_proc_tab = RepeatJoinPluginArgProc
    parse_plugin_arg

    @cell.get_join_list.get_items.each{ |j|
      # print "Join: #{j.get_name} = #{j.get_rhs.to_s}\n"

      # Join の右辺を解析
      ret = j.get_rhs.analyze_cell_join_expression
      if ret == nil then
        next
      end
      rhs_nsp = ret[0]; rhs_subscript = ret[1]; rhs_port_name = ret[2]

      # 呼び口配列で、添数が 0 の場合にのみカウントアップさせる
      if j.get_subscript == 0 then
        rhs_name = rhs_nsp.get_name.to_s
        if rhs_name =~ /(.*[^0-9])([0-9]+)\z/ then
          b_rhs_name_count = true
          rhs_name = $1
          n_digits = $2.length
          rhs_count_base = $2.to_i
        else
          b_rhs_name_count = false
          rhs_count_base = 0
        end

        if rhs_subscript && rhs_subscript == 0 then
          b_rhs_subscript_count = true
        else
          b_rhs_subscript_count = false
        end

        count = 1
        while count < @count
          count_str = (count+rhs_count_base).to_s

          if b_rhs_subscript_count then
            rhs_subscript = count
          end

          if b_rhs_name_count then
            if n_digits - count_str.length > 0 then
              leading_zero = "0" * ( n_digits - count_str.length )
            else
              leading_zero = ""
            end

            rhs_name_real = rhs_name + leading_zero + count_str
          else
            rhs_name_real = rhs_name
          end
          rhs_nsp2 = rhs_nsp.change_name_clone rhs_name_real.to_sym

          rhs = Expression.create_cell_join_expression( rhs_nsp2, rhs_subscript, rhs_port_name )
          j2 = Join.new( j.get_name, count, rhs )
          cell.new_join j2

          count += 1
        end
      end
    }
  end

  #=== count オプションの解析
  def set_count rhs
    if rhs =~ /\A\d+\z/
      @count = rhs.to_i
    else
      nsp = NamespacePath.new( rhs.to_sym, true )
      expr = Expression.create_single_identifier( nsp, nil )
      res = expr.eval_const( nil )
      if res == nil then
        cdl_error( "count value ($1): not single identifier or integer number", rhs.to_s )
        @count = 0
      else
        @count = res
      end
    end
  end

end
