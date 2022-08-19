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
#   $Id: pluginModule.rb 2633 2017-04-02 06:02:05Z okuma-top $
#++

#== プラグインをロードする側のモジュール
# @@loaded_plugin_list:: {Symbol=>Integer}
module PluginModule

  @@loaded_plugin_list = {}

  # 後ろのコードの出力順の指定 (数字が少ないほどプライオリティは高い)
  MULTI_PLUGIN_POST_CODE_PRIORITY     = 00000
  DOMAIN_PLUGIN_POST_CODE_PRIORITY    = 10000
  CLASS_PLUGIN_POST_CODE_PRIORITY     = 10000
  THROUGH_PLUGIN_POST_CODE_PRIORITY   = 20000
  CELLTYPE_PLUGIN_POST_CODE_PRIORITY  = 30000
  COMPOSITE_PLUGIN_POST_CODE_PRIORITY = 40000
  CELL_PLUGIN_POST_CODE_PRIORITY      = 50000
  SIGNATURE_PLUGIN_POST_CODE_PRIORITY = 60000

  #=== プラグインをロードする
  # return:: PluginClass
  # V1.4.1 まで return:: true : 成功、 false : 失敗
  #
  # #{plugin_name}.rb をロードし、plugin_name クラスのオブジェクトを生成する．
  # plugin_name が MultiPlugin の場合、get_plugin により、superClass のプラグインオブジェクトをロードする．
  #
  # すでにロードされているものは、重複してロードしない
  # load 時の例外はこのメソッドの中でキャッチされて false が返される
  def load_plugin( plugin_name, superClass )

    dbgPrint "PluginModule: load_plugin: #{plugin_name}\n"
    begin
      unless @@loaded_plugin_list[ plugin_name.to_sym ] then
        @@loaded_plugin_list[ plugin_name.to_sym ] = 0
        if ( $verbose ) then
          print( "load '#{plugin_name}.rb'\n" )
        end
        # "#{plugin_name}.rb" をロード（システム用ではないので、fatal エラーにしない）
        if require_tecsgen_lib( "#{plugin_name}.rb", false ) == false then
          cdl_error( "P2001 $1.rb : fail to load plugin" , plugin_name )
          return nil
        end
      end

      plClass = Object.const_get plugin_name
      if ( plClass <= superClass ) then       # plClass inherits superClass
        return plClass
      elsif (plClass <= MultiPlugin) then     # plClass inherits MultiPlugin
        dbgPrint "pluginClass=#{plClass}\n"
        plugin_object = plClass.get_plugin superClass
        dbgPrint "pluginClass=#{plugin_object}\n"
        if plugin_object == nil then
          cdl_error( "P9999 '$1': MultiPlugin not support '$2'", plugin_name, superClass.name )
        end
        @@loaded_plugin_list[ plugin_name.to_sym ] = :MultiPlugin
        return plugin_object
      else
        cdl_error( "P2002 $1: not kind of $2" ,  plugin_name, superClass.name )
        return nil
      end
    rescue Exception => evar
      if $debug then
        p evar.class
        pp evar.backtrace
      end
      cdl_error( "P2003 $1: load failed" , plugin_name )
      return nil
    end
    # ここへは来ない
    return nil
  end

  #=== プラグインの gen_cdl_file を呼びして cdl ファイルを生成させ、解釈を行う
  def generate_and_parse plugin_object
    if plugin_object == nil     # プラグインのロードに失敗している（既にエラー）
      return
    end
    plugin_name = plugin_object.class.name.to_sym
    if @@loaded_plugin_list[ plugin_name ] == :MultiPlugin then
      p "#{plugin_name}: MultiPlugin"
      return
    elsif @@loaded_plugin_list[ plugin_name ] == nil then
      #raise "#{plugin_name} might have different name "
      ## プラグインのファイル名と、プラグインのクラス名が相違する場合
      #MultiPlugin の get_plugin で返されたケースでは nil になっている
      @@loaded_plugin_list[ plugin_name ] = 0
    end
    count = @@loaded_plugin_list[ plugin_name ]
    @@loaded_plugin_list[ plugin_name ] += 1
    tmp_file_name = "#{$gen}/tmp_#{plugin_name}_#{count}.cdl"

    begin
      tmp_file = CFile.open( tmp_file_name, "w" )
    rescue Exception => evar
      cdl_error( "P2004 $1: open error \'$2\'" , plugin_name, tmp_file_name )
      print_exception( evar )
    end
    dbgPrint "generate_and_parse: #{plugin_object.class}: gen_cdl_file\n"
    begin
      plugin_object.gen_cdl_file( tmp_file )
    rescue Exception => evar
      cdl_error( "P2005 $1: plugin error in gen_through_cell_code " , plugin_name )
      print_exception( evar )
    end
    begin
      tmp_file.close
    rescue Exception => evar
      cdl_error( "P2006 $1: close error \'$2\'" , plugin_name, tmp_file_name )
      print_exception( evar )
    end

    generator = Generator.new
    generator.set_plugin( plugin_object )
    generator.parse( [ tmp_file_name ] )
    generator.finalize
  end

  #=== プラグインが CDL の POST コードを生成
  # tmp_plugin_post_code.cdl への出力
  def self.gen_plugin_post_code
    dbgPrint "------------  gen_plugin_post_code  -------------\n"
    dbgPrint "PluginModule #{@@loaded_plugin_list}\n"
    sorted_plugin_list = self.sort_and_load @@loaded_plugin_list.keys
    new_plugin_list = []
    @@loaded_plugin_list.each{ |plugin_name, count|
      b_found = false
      sorted_plugin_list.each{ |plugin|
        if plugin.name.to_sym == plugin_name then
          b_found = true
          break
        end
      }
      if b_found == false then
        new_plugin_list << plugin_name
      end
    }
    self.sort_and_load new_plugin_list
  end

  def self.sort_and_load plugin_list
    #------ post_code priority 順のリストを作成  ------#
    plugin_priority_list = {}
    plugin_list.each{ |plugin_name|
      next if ! const_defined? plugin_name    # undefined PluginModule
      plClass = Object.const_get plugin_name
      if plClass.respond_to? :get_post_code_priority then
        prio = plClass.get_post_code_priority
      else
        prio = self.get_post_code_priority plClass
      end
      if plugin_priority_list[ prio ] == nil then
        plugin_priority_list[ prio ] = [ plClass ]
      else
        plugin_priority_list[ prio ] << plClass
      end
      dbgPrint "pluginModule: prio=#{prio} plugin=#{plClass.name}\n"
    }
    sorted_plugin_list = []
    plugin_priority_list.keys.sort.each{ |prio|
      if prio != 0 then   # exclude MultiPlugin
        plClassList = plugin_priority_list[ prio ]
        sorted_plugin_list += plClassList
      end
    }
    #----- post code priority 順に post code を生成  -------#
    sorted_plugin_list.each{ |plClass|
      tmp_file_name = "#{$gen}/tmp_#{plClass.name}_post_code.cdl"
      file = nil
      begin
        file = CFile.open( tmp_file_name, "w" )
      rescue
        Generator.error( "G9999 fail to create #{tmp_file_name}" )
      end

      if file then
        dbgPrint "PluginModule: #{plClass.name}\n"
        eval_str = "plClass.name}.gen_post_code( file )"
        if $verbose then
          print "gen_plugin_post_code: #{eval_str}\n"
        end
        begin
          plClass.gen_post_code( file )
        rescue Exception => evar
          Generator.error( "P2007 $1: fail to generate post code" , plClass.name )
          print_exception( evar )
        end
                begin
          file.close
        rescue
          Generator.error( "G9999 fail to close #{tmp_file_name}" )
        end
        dbgPrint( "## Import Post Code\n")
        Import.new( "#{tmp_file_name}" )
      end
    }
    return sorted_plugin_list
  end
  
  #=== プラグインの後ろの CDL コードの生成順を指定
  def self.get_post_code_priority plClass
    if plClass <= MultiPlugin then
      return MULTI_PLUGIN_POST_CODE_PRIORITY
    elsif plClass <= DomainPlugin then
      return DOMAIN_PLUGIN_POST_CODE_PRIORITY
    elsif plClass <= ClassPlugin then
      return CLASS_PLUGIN_POST_CODE_PRIORITY
    elsif plClass <= ThroughPlugin then
      return THROUGH_PLUGIN_POST_CODE_PRIORITY
    elsif plClass <= CompositePlugin then
      return COMPOSITE_PLUGIN_POST_CODE_PRIORITY
    elsif plClass <=  CelltypePlugin then
      return CELLTYPE_PLUGIN_POST_CODE_PRIORITY
    elsif plClass <= CellPlugin then
      return CELL_PLUGIN_POST_CODE_PRIORITY
    elsif plClass <= SignaturePlugin then
      return SIGNATURE_PLUGIN_POST_CODE_PRIORITY
    else
      raise "Unknown Plugin type '#{self.class.name}'"
    end
  end

end
