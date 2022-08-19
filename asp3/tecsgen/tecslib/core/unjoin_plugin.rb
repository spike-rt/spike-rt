# -*- coding: utf-8 -*-
#
#  TECS Generator
#      Generator for TOPPERS Embedded Component System
#  
#   Copyright (C) 2008-2019 by TOPPERS Project
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
#   $Id: unjoin_plugin.rb 3075 2019-06-08 01:51:59Z okuma-top $
#++

# Marshal.dump で不都合な事項への対策
# Proc は _dump_data が定義されていないため、定義する．
# plugin 関係は、tecsflow に Plugin クラスが存在しないため、消しておく．

# Proc は、Marshal.dump することができないため _dump_data を定義
class Proc
  def _dump_data
    nil
  end
end

class Object
  @@visited = {}
  @@n_visited = 0
  @@n_found = 0
  @@n_plugin_proxy = 0

  # do nothing now. this function is used to find remained Proxy object.
    def find_plugin level, object_list

    # find_plugin_1 level, object_list
    # print "find_plugin: n_visted=", @@n_visited, " (len=", @@visited.length, ")  found=", @@n_found, "  proxy=", @@n_plugin_proxy, "\n"
  end

  def find_plugin_1 level, object_list
    if @@visited[ self ] then
      return
    end
    @@visited[ self ] = true
    @@n_visited += 1

    if kind_of? Expression then
      # print   # Expression では print が別に定義されている
      get_elements.each{|ele|
        ele.find_plugin_1 level+1, object_list
      }
      return
    end

    if level > 100 then    # 100 is enough now, 50 is too small
      print "reached max level: "
      if respond_to? :get_name then
        print "#{get_name}(#{self.class.name})\n"
      else
        print "(#{self.class.name})\n"
      end
      return
    end

    object_list.push self                 ##### ここから return 不可

    if (kind_of? Plugin) || (kind_of? HRPSVCPlugin) then
      @@n_found += 1
      print "Plugin found: #{self.class.name}\n"
      object_list.each{ |obj|
        if obj.respond_to? :get_name then
          print "#{obj.get_name}(#{obj.class.name}), "
        else
          print "(#{obj.class.name}), "
        end
      }
      print "\n"
    elsif kind_of? Join::ThroughPluginProxy then
      @@n_plugin_proxy += 1
    end

    level += 1
    if kind_of? Array then
      each{ |val|
        val.find_plugin_1 level, object_list
      }
    elsif kind_of? Hash then
      each{ |key, val|
        key.find_plugin_1 level, object_list
        val.find_plugin_1 level, object_list
      }
    else
      instance_variables.each{|iv|
        iv_val = instance_variable_get iv
        iv_val.find_plugin_1 level, object_list
      }
    end
    object_list.pop                       ##### ここまで return 不可
  end
end

class Namespace
  def unjoin_plugin
    @signature_list.each{ |sig| sig.unjoin_plugin }
    @celltype_list.each{ |ct| ct.unjoin_plugin }
    @compositecelltype_list.each{ |ct| ct.unjoin_plugin }
    @cell_list.each{ |cell| cell.unjoin_plugin }
    @namespace_list.each{ |ns| ns.unjoin_plugin}
  end
end

class Region < Namespace
  def unjoin_plugin
    if @domain_type then
      @domain_type.unjoin_plugin
    end
    @cell_port_throug_plugin_list.each{ |key, po|
      proxy = Join::ThroughPluginProxy.new po.get_cell_namespace_path, po.get_through_entry_port_name, po.get_through_entry_port_subscript
      @cell_port_throug_plugin_list[ key ] = proxy
    }
    super
  end
end

class DomainType < Node
  def unjoin_plugin
    # p "unjoin_plugin ############"
    @plugin = nil
  end
end

class Signature < NSBDNode
  def unjoin_plugin
    @generate = nil
  end
end

class Celltype < NSBDNode
  def unjoin_plugin
    @generate = nil
    @generate_list = []
    @plugin = nil
  end
end

class CompositeCelltype < NSBDNode
  def unjoin_plugin
    @generate = nil
    @generate_list = []
  end
end

class Cell < NSBDNode
  def unjoin_plugin
    @generate = nil
    @plugin   = nil
    @join_list.get_items.each{ |j| j.unjoin_plugin }
  end
end

class Join < BDNode
  class ThroughPluginProxy
    def initialize namespace_path, entry_port_name, entry_port_subscript
      @namespace_path = namespace_path
      @entry_port_name = entry_port_name
      @entry_port_subscript = entry_port_subscript
    end 
    def get_cell_namespace_path
      @namespace_path
    end
    def get_through_entry_port_name
      @entry_port_name
    end
    def get_through_entry_port_subscript
      @entry_port_subscript
    end
  end 

  def unjoin_plugin
    # @thourhg_generated_list is refered in get_rhs_cell, get_rhs_port
    @through_generated_list.map!{|po| # po: plugin object
      ThroughPluginProxy.new po.get_cell_namespace_path, po.get_through_entry_port_name, po.get_through_entry_port_subscript
    }
    @region_through_generated_list.map!{|po| # po: plugin object
      ThroughPluginProxy.new po.get_cell_namespace_path, po.get_through_entry_port_name, po.get_through_entry_port_subscript
    }
    if @array_member2 then
      @array_member2.each{ |j| 
        if j then
          j.unjoin_plugin2
        end
      }
    end
  end
  def unjoin_plugin2
    @through_generated_list.map!{|po| # po: plugin object
      ThroughPluginProxy.new po.get_cell_namespace_path, po.get_through_entry_port_name, po.get_through_entry_port_subscript
    }
    @region_through_generated_list.map!{|po| # po: plugin object
      ThroughPluginProxy.new po.get_cell_namespace_path, po.get_through_entry_port_name, po.get_through_entry_port_subscript
    }
  end
end
