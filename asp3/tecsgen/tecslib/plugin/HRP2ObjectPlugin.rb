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
#   $Id: HRP2ObjectPlugin.rb 2952 2018-05-07 10:19:07Z okuma-top $
#++

#require "HRP2KernelObjectPlugin"
require_tecsgen_lib "HRP2KernelObjectPlugin.rb"


class HRP2ObjectPlugin < HRP2KernelObjectPlugin
    # @@ep = [:eSignalSemaphore, :eWaitSemaphore, :eManageSemaphore, :eReferSemaphore ]
    @@api = {
        "SEMAPHORE"=>["SEM", :id, :attribute, :count, :max],
        "EVENTFLAG"=>["FLG", :id, :attribute, :flagPattern],
        "DATAQUEUE"=>["DTQ", :id, :attribute, :count, :dtqmb],
        "PRIORITY_DATAQUEUE"=>["PDQ", :id, :attribute, :count, :maxDataPriority, :pdqmb],
        "FIXED_SIZE_MEMORYPOOL"=>["MPF", :id, :attribute, :blockCount, :blockSize, :mpf, :mpfmb],
        "KERNEL"=>["SYS"],
        "MESSAGE_BUFFER"=>["MBF", :id, :attribute, :maxSize, :bufferSize, :mbfmb],
    }

    #=== HRP2SemaphorePlugin#print_cfg_cre
    # CRE_SEMの出力
    # file:: FILE:     出力先ファイル
    # val :: string:   カーネルオブジェクトの属性の解析結果
    # tab :: string:   インデント用のtab
    def print_cfg_cre(file, cell, val, tab)
        if @@api.has_key?(@plugin_arg_str) == false
            raise "#{@plugin_arg_str} is unknown"
        elsif @plugin_arg_str != "KERNEL"
            arg_list = []
            @@api[@plugin_arg_str].slice(2..-1).each { |attr|
                arg_list << "#{val[attr]}"
            }
            file.print tab
            file.puts "CRE_#{@@api[@plugin_arg_str].at(0)}(#{val[:id]}, { #{arg_list.join(", ")} });"
        end
    end
    #=== HRP2SemaphorePlugin#print_cfg_sac
    # SAC_SEMの出力
    # file:: FILE:     出力先ファイル
    # val :: string:   カーネルオブジェクトの属性の解析結果
    # acv :: string:   アクセスベクタ    
    def print_cfg_sac(file, val, acv)
        if @@api.has_key?(@plugin_arg_str) == false
            raise "#{@plugin_arg_str} is unknown"
        elsif @plugin_arg_str != "KERNEL"
            file.puts "SAC_#{@@api[@plugin_arg_str].at(0)}(#{val[:id]}, { #{acv[0]}, #{acv[1]}, #{acv[2]}, #{acv[3] }});"
        else
            file.puts "SAC_#{@@api[@plugin_arg_str].at(0)}({ #{acv[0]}, #{acv[1]}, #{acv[2]}, #{acv[3]} });"
        end
    end

=begin
    #tSemaphoreの受け口リスト 
    def get_entry_ports_name_list()
        @@ep
    end
=end
end


  
