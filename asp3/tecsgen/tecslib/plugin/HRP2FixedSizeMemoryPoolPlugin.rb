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
#   $Id: HRP2FixedSizeMemoryPoolPlugin.rb 2952 2018-05-07 10:19:07Z okuma-top $
#++

#require "HRP2KernelObjectPlugin"
require_tecsgen_lib "HRP2KernelObjectPlugin.rb"


class HRP2FixedSizeMemoryPoolPlugin < HRP2KernelObjectPlugin
    @@ep = [:eSendFixedSizeMemoryPool, :eReceiveFixedSizeMemoryPool, :eManageFixedSizeMemoryPool, :eReferFixedSizeMemoryPool ]
    #=== HRP2FixedSizeMemoryPoolPlugin#print_cfg_cre
    # CRE_MPFの出力
    # file:: FILE:     出力先ファイル
    # val :: string:   カーネルオブジェクトの属性の解析結果
    # tab :: string:   インデント用のtab
    def print_cfg_cre(file, cell, val, tab)
        file.print tab
        file.puts "CRE_MPF(#{val[:id]}, {#{val[:attribute]}, #{val[:blockCount]}, #{val[:blockSize]}, #{val[:mpf]}, #{val[:mpfmb]}});"
    end
    #=== HRP2FixedSizeMemoryPoolPlugin#print_cfg_sac
    # SAC_MPFの出力
    # file:: FILE:     出力先ファイル
    # val :: string:   カーネルオブジェクトの属性の解析結果
    # acv :: string:   アクセスベクタ    
    def print_cfg_sac(file, val, acv)
        file.puts "SAC_MPF(#{val[:id]}, {#{acv[0]}, #{acv[1]}, #{acv[2]}, #{acv[3]}});"
    end
    #tFixedSizeMemoryPoolの受け口リスト 
    def get_entry_ports_name_list()
        @@ep
    end
end


  
