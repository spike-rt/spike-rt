# -*- coding: utf-8 -*-
#
#  TCFlow
#     Clanguage flow analyzer
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
#   $Id: classes.rb 3075 2019-06-08 01:51:59Z okuma-top $
#++

module Locale_printer
  def print_locale locale
    print " (#{locale[0]} #{locale[1]})"
  end
  def self.print_locale locale
    print " (#{locale[0]} #{locale[1]})"
  end
end

module TCFlow
class Function
  include Locale_printer

  #@name:: Symbol
  #@param_var::{ Symbol=>true }
  #@local_var::{ Symbol=>true }
  #@global_var::{ Symbol=>[n_access, n_write] }
  #@call_func::{ Symbol=>Function }
  #@b_ref:: Bool : true if referenced
  #@b_printed:: Bool : true if printed already
  #@locale::[file_name, lineno, col]

  @@Function_list = {}    # String(name) => Function | Integer(0)
  @@param_list = nil

  def initialize locale
    @param_var = {}
    @local_var = {}
    @global_var = {}
    @call_func = {}
    @b_ref = false
    @b_printed = false
    @locale = locale

    @pre_name = [0, "", 0, ""]
    @pre_printed = false
    @pfunc = ""
#    @pp = ""

    # param_list は先に解釈されている
    if @@param_list
      @@param_list.each { |param|
        new_param param
      }
    end
  end

  def set_name name    # 関数をパースする最後で名前が確定
#    print "^^^ function: #{name}\n"
    @name = name.to_sym
    if @@Function_list[@name] == 0
      @b_ref = true
    end
    @@Function_list[@name] = self
    @@param_list = nil
#p @name ###150216
#    print "test:", @name, "\n"
  end

  # 関数が new される前に param_list が解釈されるため
  def self.param_list param_list
    @@param_list = param_list
  end

  def new_param param
    # print "new_param: #{param}\n"
    @param_var[ param ] = param
  end

  def new_local local
    # print "new_local: #{local}\n"
    @local_var[ local ] = local
  end

  def ref_var var, b_write = false
#p var
#p @local_var
#p @global_var
    if @local_var[ var ]
      # print "ref local: #{var}\n"
    elsif @param_var[ var ]
      # print "ref param: #{var}\n"
    else
      # print "ref global: #{var}\n"

      #変数のread/write回数をカウント ################
      count_array = @global_var[ var ]
      if count_array == nil
      	count_array = [ 0, 0 ]
	      @global_var[ var ] = count_array
      end
      if b_write
	      count_array[1] += 1
      else
	      count_array[0] += 1
      end
    end
  end

  def ref_func func_name
    func_name = func_name.to_sym
#p func_name ###150216 この時点では表記順
    if @@Function_list[func_name].instance_of? Function
      function = @@Function_list[func_name]
      function.referenced self
      @call_func[ func_name ] = function
      # print "call func: #{func_name} : defined\n" 
    else
      @@Function_list[func_name] = 0
      @call_func[ func_name ] = 0
      # print "call func: #{func_name} : not defined\n"
    end
  end

  def referenced ref_func
    @b_ref = true
    @ref_func = ref_func
  end

  #=== ツリー表示
  def self.print_all_functions

    @@Function_list.each{ |name, func|
      if func.instance_of? Function
    	  func.print_all_functions(0, "")
      end
    }

    if ! self.all_printed?
      # print "===== recursive functions\n"
      funcs.each {|name|
      	func = @@Function_list[name]
	if func.instance_of? Function
	  func.print_all_functions(-1, "")
	end
      }
    end
  end

  #=== ツリー表示する
  #level:: Integer:  0: top_level のものを表示する, -1: 未表示のものを表示する
  # level = 0, -1 の場合、レベルに一致した関数だけが表示される
  # それ以外の場合、再帰的にツリー表示が行われる
  def print_all_functions (level)

    # TOP レベルは参照されていないものだけとする
    return if level == 0 && @b_ref == true

    # @b_ref だが未印刷のもの（循環参照されている）
    if level == -1
      return if @b_printed
      level = 0
    end

    indent = "    " * level

    if @b_printed
      # 印刷済みは再帰的に印刷しない
      print indent
      print @name
      print " : printed\n"
      return
    end
    @b_printed = true

    #変数一覧印字 ##########
    if @global_var.length > 0 
      #print indent
      #print "  Global Vars: \n"
      @global_var.each { |name, count_array|
	#print indent
	#print "    "
	#print name
	#printf( "   [R:%dW:%d]\n", count_array[0]-count_array[1], count_array[1] )
      }
    end

    #関数一覧印字 #############
    if @call_func.length > 0 
      print indent
      print "  Calling Function: \n"
      cfuncs = @call_func.keys.sort{ |a,b| a.to_s <=> b.to_s } #名前順ソート
      # cfuncs = @call_func.keys #関数呼び出し順  hash の順序は保障されていない【Matz版は保障されるらしい）
      cfuncs.each{ |name|
	      func = @call_func[ name ]
	      if func.instance_of? Function
          print indent, "    ", name
          print_locale @locale
          print "\n"
	        func.print_all_functions(level+1) #順番変更するとrecursive functionになる?
      	else
	        print indent
	        print "    " ##
	        print name
	        print "() : not defined\n"
      	end
      }
    end
  end

  #Funciton#get_call_funcs
  # ハッシュを返す (呼出し関数名=>Function)
  def get_call_funcs
    @call_func
  end

  def self.all_printed? 
    # 参照されているが未印刷のものを探す
    b_all_printed = true
    @@Function_list.each{ |name,func|
      if func.instance_of? Function
      	if func.is_printed? == false
	        b_all_printed = false
      	  break
      	# else
	      # print "#{name} : printed\n"
      	end
      end
    }
    return b_all_printed
  end

  #=== @call_func を update する
  # @call_func の右辺は、構文解釈中に設定されるが、不完全である
  # 完全になるように更新する
  # サマリー表示の場合には必要だが、ツリー表示の場合、処理の途中で更新するので不要
  # summarize の中でやると分かりにくくなるので、独立させた
  def self.update
  #  print "updating: "
    @@Function_list.each{ |name,func|
      # print name, ", "
      if func.instance_of? Function
      	func.update
      end
    }
  #  print "\n"
  end

  def update
    @call_func.each { |name, func|
      if func.instance_of? Function   # 関数名重複の場合、前のものを忘れない方が、よりよい（恐らく）
      	next
      end
      func = @@Function_list[name]
      if func.instance_of? Function
      	@call_func[name]=func
	      func.referenced self
      end
    }
  end

  #=== サマリー表示
  def self.print_summarize
    self.update
    # print "==== summary mode (all functions called from top level function)\n"
    self.print_summarize0 false
    if ! self.all_printed?
      # print "===== recursive functions\n"
      self.print_summarize0 true
    end
  end

  def self.print_summarize0 b_unprinted
    funcs = @@Function_list.keys.sort{ |a,b| a.to_s <=> b.to_s }
    funcs.each {|name|
      func = @@Function_list[name]
      if func.instance_of? Function
	if ! b_unprinted
	  next if ! func.is_top?
	else
	  next if func.is_printed?
	end

	@@ref_function_list = {}
	@@ref_global_var_list = {}
	func.summarize
	#print name
	locale = func.get_locale
	#printf( "()   <%s:%d>\n", locale[0], locale[1] )
	if @@ref_function_list.length > 0 
	  #print "  Calling Function Summary:\n"
	  ref_funcs = @@ref_function_list.keys.sort{ |a,b| a.to_s <=> b.to_s }
	  ref_funcs.each { |name|
	    function = @@ref_function_list[name]
	    #print "    "
	    #print name
	    if function.instance_of? Function
	      locale = function.get_locale
	      #printf( "()   <%s:%d>\n", locale[0], locale[1] )
	    else
	      #print "()    not defined\n"
	    end
	  }
	end
	if @@ref_global_var_list.length > 0
	  #print "  Global Vars Summary:\n"
	  vars = @@ref_global_var_list.keys.sort{ |a,b| a.to_s <=> b.to_s }
	  vars.each{ |name|
	    count_array = @@ref_global_var_list[name]
	    #print "    "
	    #print name
	    #printf( "   [R:%dW:%d]\n", count_array[0]-count_array[1], count_array[1] )
	  }
	end
      end
    }
  end

  def summarize
    @global_var.each { |name, count_array|
      count_array2 = @@ref_global_var_list[name]
      if count_array2
	      count_array2[0] += count_array[0]
	      count_array2[1] += count_array[1]
      else
      	@@ref_global_var_list[name] = count_array.clone
      end
    }
    @call_func.each { |name, function|
      next if @@ref_function_list[name]     # カウント済み
      @@ref_function_list[name] = function
      if function.instance_of? Function
	      function.summarize
      end
    }

    @b_printed = true
  end

##############
  def is_local_var? var
 #    p "#{var}, #{@local_var[ var ]}, #{@param_var[ var ]}"
##ここで@local_var[var]が真にならない
    if @local_var[ var ] || @param_var[ var ]
      return true #local or param
    else
      return false #global
    end
  end

  def is_top?
    @b_ref == false
  end

  def is_printed?
    @b_printed
  end

  def get_locale
    @locale
  end

  def self.dump_funclist dump_file_name
    p "Dump Funclist"
    mar = Marshal.dump @@Function_list
    File.write( dump_file_name, mar )
  end

  def self.load_funclist dump_file_name
    mar_in = File.read( dump_file_name )
    @@Function_list = Marshal.load( mar_in )
    return @@Function_list
  end

  def self.list_all_functions
    #print "==== function list\n"
    funcs = @@Function_list.keys.sort{ |a,b| a.to_s <=> b.to_s }
    funcs.each { |name|
      if @@Function_list[name].instance_of? Function
        func = @@Function_list[name]
        func.print_func
      else
        #printf( "%-20s: %-s\n", name, "not defined" )
      end
    }
  end

  def set_printed
    @b_printed = true
  end
  
  def print_func
    #if @ref_func
    #  ref_func = @ref_func.get_name
    #else
      ref_func = ""
    #end
    #printf( "%-20s: %-30s line: %d   %s\n", @name, @locale[0], @locale[1], ref_func )
  end

  def get_name
    @name
  end
end # class
end # module
