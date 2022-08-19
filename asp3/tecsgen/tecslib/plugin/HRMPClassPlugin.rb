# coding: utf-8
#
#  TECS Generator
#      Generator for TOPPERS Embedded Component System
#  
#   Copyright (C) 2019-2020 by TOPPERS Project
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
#  $Id: HRMPClassPlugin.rb 3241 2021-11-21 12:06:12Z okuma-top $
#++

require_tecsgen_lib "HRPKernelObjectManager.rb"

#
# 各メソッドの役割りは、ClassPlugin.rb を参照のこと
# HRMPカーネル用クラスプラグイン
class HRMPClassPlugin < ClassPlugin

#@class::String: クラス名
# @region::Region
# @name::Symobl      :HRMP
# @class1::String   クラス名

  include HRMPJoinManager

  # クラス定義(.cdf) の JSON スキーマ
  CLASS_DEF_schema = {
    :HRMP_class_def => {
      :class_def    => [ :class ]               # array
    },
    :class  => {
      :type         => "class",                 # fixed string (type name)
      :class_name   => :string,                 # "CLS_PRC1", "CLS_ALL_PRC1", ":root" (TECS でのクラス名)
      :processorID  => :integer,                # 1, 2, 3...
      :locality     => :string,                 # "only", "all", "root"
    },
    :__class  => {                              # optional
      :class_name_in_kernel => :string         # "CLS_PRC1", "CLS_ALL_PRC1", ":root" 
                                                # (HRMP3, FMP3 のクラス名; TECS と変えれるように設けている)
    }
  }

  # nTECSGENConfig::tClassConfig セルタイプのセル
  # { class_name_in_TECS => {:processorID=>processor_No, :class_name=>class_name_in_HRMP3, :locality=>locality_str}}
  @@class_info = nil
  
  def initialize( region, name, option )
    super
    if @@class_info == nil then
      validate_and_set_class_info
    end
    dbgPrint "HRMPClassPlugin: initialize: region=#{region.get_name}, className=#{name}, option=#{option}\n"
    @region = region
    @name   = name      # HRMP
    @class1 = option.to_sym
    @class_specified = @class1
    attr1 = @@class_info[ @class1 ]
    if attr1 == nil then
      cdl_error( "HRMP9999 $1 is not acceptable class in TECS", @class1.to_s )
      @class1 = "root"    # root に置き換えて置く
      return
    end
  end

  def joinable?(current_region, next_region, through_type )
    dbgPrint "HRMPClassPlugin: joinable? from #{current_region.get_name} to #{next_region.get_name} (#{through_type})\n"
    return true
  end

  #=== validate
  # validate JSON format data in __tool_info__( "HRMP_class_def" )
  def validate_and_set_class_info
    validator = TOOL_INFO::VALIDATOR.new( :HRMP_class_def, CLASS_DEF_schema )
    if validator.validate then
      @@class_info = {}
      info = TOOL_INFO.get_tool_info( :HRMP_class_def )
      if info == nil then
        cdl_error( "HRMP9999 not found __tool_info__( HRMP_class_def )" )
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

  # クラス名が有効かチェックする
  def check_class class_name
    if @@class_info[ class_name.to_sym ] then
      return true
    else
      return false
    end
  end

  #= HRMPClassPlugin#カーネル(HRMP3)での処理単位のクラス名を得る
  #class_tecs::Symbol
  def get_PU_kernel_class  # PU: Processing Unit
    if @@class_info[ @class1 ] then
      @@class_info[ @class1 ][ :class_name ]
    else
      @class1.to_s
    end
  end

  # attr for Processing Unit 
  def get_PU_attr
    @@class_info[ @class1 ]
  end
  # attr for Non-Processing Unit 
  #  当初は PU と NPU で分けていたが、現在は分けていない
  def get_NPU_attr
    @@class_info[ @class1 ]
  end
  def get_class_name
    get_PU_kernel_class
  end
  def get_processorID 
    @@class_info[@class1][:processorID]
  end
  def self.get_processorID  class_name
    @@class_info[class_name.to_sym][:processorID]
  end
  def self.gen_post_code file
  end

  def gen_factory node_root
  end
end

