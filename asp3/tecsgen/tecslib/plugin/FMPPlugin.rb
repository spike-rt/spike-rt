# coding: utf-8
#
#  TECS Generator
#      Generator for TOPPERS Embedded Component System
#  
#   Copyright (C) 2019-2021 by TOPPERS Project
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
#  $Id: FMPPlugin.rb 3207 2021-03-14 02:03:22Z okuma-top $
#++

require_tecsgen_lib "HRPKernelObjectManager.rb"
require_tecsgen_lib "lib/GenAttMod.rb"

#
# 各メソッドの役割りは、ClassPlugin.rb を参照のこと
# HRPカーネル用ドメインプラグイン
class FMPPlugin < ClassPlugin
#@class1::String: クラス名
  include GenAttMod

  @@class_info = nil
  CLASS_DEF_schema = {
    :FMP_class_def => {
      :class_def    => [ :class ]               # array
    },
    :class  => {
      :type         => "class",                 # fixed string (type name)
      :class_name   => :string,                 # "CLS_PRC1", "CLS_ALL_PRC1", "GLOBAL" (TECS でのクラス名)
      :processorID  => :integer,                # 1, 2, 3...
      :locality     => :string,                 # "only", "all", "global"
    },
    :__class  => {                              # optional
      :class_name_in_kernel => :string         # "CLS_PRC1", "CLS_ALL_PRC1", "GLOBAL" 
                                                # (HRMP3, FMP3 のクラス名; TECS と変えれるように設けている)
    }
  }
  
  def initialize( region, name, option )
    super
    dbgPrint "FMPPlugin: initialize: region=#{region.get_name}, className=#{name}, option=#{option}\n"
    if @@class_info == nil then
      validate_and_set_class_info
    end
    @region = region
    @name   = name
    @region = region
    @name   = name      # FMP
    # @class1 = option1.to_sym
    # @class1 = option2.to_sym
    @class1 = option.to_sym
    attr1 = @@class_info[ @class1 ]
    if attr1 == nil then
      cdl_error( "FMP9999 $1 is not acceptable class in TECS", @class1.to_s )
      return
    end
 end

  def add_through_plugin( join, current_region, next_region, through_type )
    # p "FMPPlugin: add_through_plugin"
    # join.get_owner:Cell  左辺のセル
    # join.get_definition:Port 呼び口
    # join.get_subscript:Integer or nil 呼び口配列の添数 (Join::@subscript の説明参照)
    # join.get_cell:Cell 右辺のセル
    # join.get_port_name:Symbol 受け口
    # get_rhs_subscript:Integer or nil 受け口配列の添数 (Join::@rhs_subscript の説明参照)
    # return []
    dbgPrint "FMPPlugin: add_through_plugin: #{current_region.get_name}=>#{next_region.get_name}, #{join.get_owner.get_name}.#{join.get_definition.get_name}=>#{join.get_cell.get_name}.#{join.get_port_name}, #{through_type}\n"

    # PU: Processing Unit, NPU: Non-Processing Unit
    # NPU の locality だけ見れば十分である
    # 厳密には、PU==only, NPU!=only の場合、リージョン外からの呼出しがあり、
    # リージョン外への呼出しフローがあれば、all とする方がよい

    # PU の情報は使用する必要がない
    # NPU が only なら PU は必ず only
    current_class_attr = current_region.get_class_root.get_class_type.get_plugin.get_NPU_attr
    caller_prc_NPU_cell      = current_class_attr[:processorID]
    caller_locality_NPU_cell = current_class_attr[:locality]

    next_class_attr = next_region.get_class_root.get_class_type.get_plugin.get_NPU_attr
    callee_prc_NPU_cell      = next_class_attr[:processorID]
    callee_locality_NPU_cell = next_class_attr[:locality]

    dbgPrint "FMPPlugin: Join: " + (join.get_cell.get_real_celltype(join.get_rhs_port1).get_global_name).to_s +
             " current:" + current_class_attr[:class_name].to_s + " next:" + next_class_attr[:class_name].to_s + "\n"
    
    rhs_ct = join.get_cell.get_real_celltype(join.get_rhs_port1).get_global_name
                                                                                 
    if HRPKernelObjectManager.include_celltype?(join.
                                                  get_cell.
                                                  get_real_celltype(join.get_rhs_port1).
                                                  get_global_name)                #oyama: get_name => get_global_name 
      # 結合先がカーネルオブジェクトセル
      # @plugin_body = HRP2SVCPlugin.new(cell_name, plugin_arg, next_cell, next_cell_port_name, signature, celltype, caller_cell)
      # 何もしないthrough
      dbgPrint "***** #{join.get_cell.get_celltype.get_name} is kernel object\n"
      return []
    elsif callee_locality_NPU_cell == "only" && (caller_locality_NPU_cell != "only" || caller_prc_NPU_cell != callee_prc_NPU_cell) then
      # 呼び先は、特定プロセッサに限定されている場合
      # 呼び元が、特定プロセッサ限定ではない、または異なるプロセッサである場合 RPC
      # dbgPrint "**** RPC\n"
      # return [ :FMPRPCPlugin, "noClientSemaphore=false,PPAllocatorSize=256" ]
      # return [ :SelectiveJoinPlugin, "processorID=#{callee_prc_NPU_cell}" ]
      return []
    else
      dbgPrint "**** Direct Call\n"
      return []
    end

    # puts "=====Join Check End====="
  end

  def joinable?(current_region, next_region, through_type )
    dbgPrint "FMPPlugin: joinable? from #{current_region.get_name} to #{next_region.get_name} (#{through_type})\n"
    return true
  end
 
  #=== validate
  # validate JSON format data in __tool_info__( "FMP_class_def" )
  def validate_and_set_class_info
    validator = TOOL_INFO::VALIDATOR.new( :FMP_class_def, CLASS_DEF_schema )
    if validator.validate then
      @@class_info = {}
      info = TOOL_INFO.get_tool_info( :FMP_class_def )
      if info == nil then
        cdl_error( "FMP9999 not found __tool_info__( FMP_class_def )" )
        return
      end
      class_info = info[ :class_def ]
      class_info.each{ |cls|
        attr = {}
        @@class_info[ cls[ :class_name ].to_sym ] = attr     # TECS でのクラス名
        attr[ :processorID ] = cls[ :processorID ]
        attr[ :locality ]    = cls[ :locality ]
        attr[ :class_name ]  = cls[ :class_name ]     # kernel でのクラス名
        if cls[ :class_name_in_kernel ] then
          attr[ :class_name ] = cls[ :class_name_in_kernel ]
        end
      }
      @@class_info[ :root ] = {:processorID=>0, :class_name=>"root", :locality=>"root"}
    end
  end

  #= FMPPlugin#カーネル(FMP3)での処理単位のクラス名を得る
  #class_tecs::Symbol
  def get_PU_kernel_class
    if @@class_info[ @class1 ] then
      @@class_info[ @class1 ][ :class_name ]
    else
      @class1.to_s
    end
  end
  def get_NPU_kernel_class
    if @@class_info[ @class1 ] then
      @@class_info[ @class1 ][ :class_name ]
    else
      @class1.to_s
    end
  end

  def get_PU_attr
    @@class_info[ @class1 ]
  end
  def get_NPU_attr
    @@class_info[ @class1 ]
  end
  
  def self.gen_post_code file
  end

  # 
  #  ATT_MODの生成
  #  gen_factory実行時には，すべてのセルタイププラグインを生成済みのはずなので，
  #  カーネルAPIコードのメモリ保護を省略できる．
  #
  def gen_factory node_root
    gen_include
    gen_att_mod node_root
  end

  #
  # プロセッサID が一致し、ローカリティが only であるクラスを探す
  #  見つからない場合、プロセッサ ID が一致するクラスを仮で返す
  #
  def self.get_prc_only prc_id
    tmp = ""
    @@class_info.each{ |cls, attr|
      if attr[ :processorID ] == prc_id 
        if attr[ :locality ] == "only" then
          return cls
        end
        tmp = cls       # エラー時に返す仮の値
      end
    }
    return tmp
  end
end
