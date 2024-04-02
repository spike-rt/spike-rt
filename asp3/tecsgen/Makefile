#
#  TECS Generator
#      Generator for TOPPERS Embedded Component System
#  
#   Copyright (C) 2008-2013 by TOPPERS Project
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
#   $Id: Makefile 2640 2017-06-03 11:27:12Z okuma-top $
#++

LIB_PATH= tecslib/core
PLUGIN_PATH= tecslib/plugin
GEN_SRC =   tecsgen.rb\
  $(LIB_PATH)/bnf.tab.rb\
  $(LIB_PATH)/bnf.y.rb\
  $(LIB_PATH)/componentobj.rb\
  $(LIB_PATH)/ctypes.rb\
  $(LIB_PATH)/C_parser.tab.rb\
  $(LIB_PATH)/C_parser.y.rb\
  $(LIB_PATH)/expression.rb\
  $(LIB_PATH)/generate.rb\
  $(LIB_PATH)/messages.rb\
  $(LIB_PATH)/optimize.rb\
  $(LIB_PATH)/pluginModule.rb\
  $(LIB_PATH)/syntaxobj.rb\
  $(LIB_PATH)/types.rb\
  $(LIB_PATH)/value.rb\
  tecslib/version.rb

PLUGIN_SRC = \
  $(PLUGIN_PATH)/CellPlugin.rb\
  $(PLUGIN_PATH)/CelltypePlugin.rb\
  $(PLUGIN_PATH)/lib/GenTransparentMarshaler.rb\
  $(PLUGIN_PATH)/lib/GenOpaqueMarshaler.rb\
  $(PLUGIN_PATH)/lib/GenParamCopy.rb\
  $(PLUGIN_PATH)/OpaqueRPCPlugin.rb\
  $(PLUGIN_PATH)/RPCPlugin.rb\
  $(PLUGIN_PATH)/SharedRPCPlugin.rb\
  $(PLUGIN_PATH)/SignaturePlugin.rb\
  $(PLUGIN_PATH)/ThroughPlugin.rb\
  $(PLUGIN_PATH)/TracePlugin.rb


BNF = $(LIB_PATH)/bnf.y.rb
GEN_TAB =  $(LIB_PATH)/bnf.tab.rb
GEN_TAB_DEB =  $(LIB_PATH)/bnf-deb.tab.rb
C_PARSER = $(LIB_PATH)/C_parser.y.rb
C_TAB = $(LIB_PATH)/C_parser.tab.rb
EXERB_EXE = tecsgen.exe

all: generator

debug : yydebug

generator : $(GEN_TAB) $(GEN_SRC) $(PLUGIN_SRC) $(C_TAB)

clean : clean2
	rm -f $(GEN_TAB) $(GEN_TAB_DEB) $(C_TAB)

clean2 :
	rm -f $(LIB_PATH)/bnf.output $(LIB_PATH)/C_parser.output
	rm -rf rdoc

real_clean : clean
	rm -f $(EXERB_EXE)

$(GEN_TAB) : $(BNF)
	racc -v $(BNF)

$(C_TAB) : $(C_PARSER)
	racc -v $(C_PARSER)

yydebug : $(GEN_TAB_DEB)

$(GEN_TAB_DEB) : $(BNF) $(GEN_SRC) $(PLUGIN_SRC)
	racc -v -g -o $(GEN_TAB_DEB) $(BNF)	# yydebug を有効にする

print_error :	# エラーメッセージを拾い出す
	grep -e "Generator\.error" -e cdl_error $(LIB_PATH)/*.rb \
		| grep -v bnf.tab.rb \
		| sed -e "s/\s*Generator\.error(//" -e "s/\s*cdl_error[0-9]*(//" -e "s/).*$$//"

print_error2 :	# エラーメッセージを拾い出す
	grep '[^A-Z][A-VXYZ][0-9][0-9][0-9][0-9]' $(LIB_PATH)/*.rb \
		| grep -v bnf.tab.rb \
		| grep -v '^Makefile' \
		| sed -e "s/.*\([A-Z][0-9][0-9][0-9][0-9].*\)/\1/" \
		| sed -e 's/\(.*\)".*/\1/'

print_warning : # ウォーニングメッセージを拾い出す
	grep -e 'Generator\.warning' -e cdl_warning $(LIB_PATH)/*.rb \
		| grep -v bnf.tab.rb \
		| sed -e "s/\s*Generator\.warning(/  /" -e "s/).*$$//"

print_expr_token :
	# Expression に含まれるトークンを拾い出す
	# OP_... より後ろは式とは関係ないトークン
	grep ':[A-Z]' bnf.y.rb | sed -e 's/.*\(:[A-Z]\w*\).*/\1/'

print_mikan : # 設計未完部分を拾い出す
	grep mikan $(LIB_PATH)/*.rb 

print_def :
	grep -e '^ *def' -e '^ *class' $(LIB_PATH)/*.rb | grep -v bnf.tab.rb

print_class :
	grep '^ *class' $(LIB_PATH)/*.rb | grep -v bnf.tab.rb

rdoc :
	rdoc --charset euc --op rdoc $(GEN_SRC) $(PLUGIN_SRC)
