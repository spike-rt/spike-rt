#!/usr/bin/env ruby
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
#   $Id: bnf.y.rb 2850 2018-04-01 12:38:45Z okuma-top $
#++

require 'optparse'
require_relative "flowlib/classes.rb"

$exe_name="tcflow"
$version="V1.0.0.1"
$cpp_cmd=nil
$b_version=false
$b_summarize = false
$b_list = false
$gen_dir = "gen"
$read_dump = false
$b_print_all_token = false
###$debug = true
begin

  ARGV.options {|parser|
    parser.banner = "Usage: tcflow [options] files"
    parser.on('-c', '--cpp=cpp_cmd', 'C pre-processor command, if not specified cpp not invoked,  ex) "-cpp_cmd=cl /E -I include_path"' ){
      |arg|
      $cpp_cmd = arg
    }
    parser.on('-l', '--list', 'list all functions') {
      $b_list = true
    }
    parser.on('-s', '--summarize', 'summarize for top functions') {
      $b_summarize = true
    }
    parser.on( "-g", "--gen=dir", "generate dir"){ |arg|
      $gen_dir = arg
    }
    parser.on( "-r", "--read-rbdmp", "read ruby dump"){
      $read_dump = true
    }
    parser.on( "-t", "--print-all-token", "print all token"){
      $b_print_all_token = true
    }
    parser.version = "#{$version}"
    parser.release = nil
    parser.parse!
  }
  if ARGV.empty? && $read_dump == false
    ARGV.options{|parser|
      puts "#{$exe_name} : #{$version}"
      puts parser.help
      exit 1
    }
  end

#  $file = open("#{$gen_dir}/FlowC.txt", "w") ##

  if $read_dump then
    TCFlow::Function.load_funclist "#{$gen_dir}/tcflow.rbdmp"
  else
    require_relative "flowlib/C_parser.tab.rb"
    STDERR.puts "#{$exe_name} : #{$version}"
    ARGV.each{ |file|

      if /\@(.*)/ =~ file
        file_list = []
        begin
          File.open( $1, "r:ASCII-8BIT" ).each{ |f|
            if /^\s*#/ =~ f
              next
            end
            f.strip!
            if f != ""
              file_list << f
            end
         }
        rescue
          STDERR.print "faile to open #{file}"
          exit 1
        end
      else
        file_list = [ file ]
      end

      file_list.each{ |f|
        # C 言語のパーサインスタンスを生成
        c_parser = TCFLOW_Cparser.new

        # パース
        c_parser.parse( [ f ], $cpp_cmd )

        # 終期化
        c_parser.finalize
      }
    }
    TCFlow::Function.dump_funclist "#{$gen_dir}/tcflow.rbdmp"
# if $b_summarize
#   TCFlow::Function.print_summarize
# else
#    TCFlow::Function.print_all_functions
# end

# if $b_list
#    Function.list_all_functions
# end
  end

rescue Exception => evar
  # "stack level too deep" が exerb 版では、表示されなかった。非 exerb 版を利用すべし。
  STDERR.print "fatal error: " + evar.to_s + "\n"
  # "stack level too deep" の場合、大量に表示されるので、コメントアウトしてある
  # p $@
# ensure
  # $file.close
end
