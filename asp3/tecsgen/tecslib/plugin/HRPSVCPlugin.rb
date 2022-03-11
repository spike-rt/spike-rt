# coding: utf-8
#
#  TECS Generator
#      Generator for TOPPERS Embedded Component System
#  
#   Copyright (C) 2008-2018 by TOPPERS Project
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
#  $Id: HRPSVCPlugin.rb 3055 2019-03-16 01:04:55Z okuma-top $
#++

# mikan through plugin: namespace が考慮されていない
# これを利用する場合、以下のように toppers_jsp.cdl sChannel.cdl を指定する必要がある
# tecsgen toppers_jsp.cdl sChannel.cdl your_description.cdl

# 以下を仮定（制限事項）
#  呼び元、呼び先のエンディアン、char, short, int_t, long_t, intptr_t のサイズが同じ
#  有符号、無符号でサイズが同じ

class SVCManage
    #
    #  @@id: TECSのプラグインで生成した拡張サービスコールに割り当てるid
    #        実際の拡張サービスコールIDは，TFN_TECSGEN_ORIGINで下駄を
    #        履かせた値となる
    #        TFN_TECSGEN_ORIGINは，hrp3/include/extsvc_fncode.h で定義
    #        される
    #  @@func_ids: 拡張サービスコールの関数名と拡張サービスコールIDを
    #  　　　対応づけるハッシュ
    #
    @@id = 0
    @@func_ids = {}
    def initialize()
        # 
      #  本クラスはインスタンスを持たない仮想的なクラスである
        #
        raise "class #{self.class.name} shall not have instances"
    end
    def self.get_func_id func_name
        return @@func_ids[ func_name ]
    end
    def self.set_func_id func_name
        @@func_ids[ func_name ] = self.assign_id
        # puts @@func_ids[ func_name ]
    end
    def self.get_id
        return @@id
    end
    def self.set_id id
        @@id = id
    end
    def self.assign_id
        assignedId = @@id
        @@id += 1
        return assignedId
    end
end

#
#  拡張サービスコールを用いたドメイン間通信の
#  throughプラグイン
#  HRPドメインプラグインによって挿入される
#  前提条件: 呼出し先がカーネルオブジェクトかどうかの判定はHRPドメインプラグイン
#  　　　　　で実施されるため，ここでは判定しないこととした
#
class HRPSVCPlugin < ThroughPlugin

  NUM_SVC_ARG_MAX = 5              # HRP3 の拡張サービスコールで扱うことのできる引数の最大個数

  @@generated_celltype ={}    # セルタイプの重複排除用
  @@generated_cell = {}       # セルの重複排除用

  def initialize( cell_name, plugin_arg, next_cell, next_cell_port_name, next_cell_port_subscript, signature, celltype, caller_cell )
    super

    # 受け口配列の場合、配列添数ごとに別のセルタイプとする
    # セルタイプをシングルトン化したいため。
    # さもないと、セルを識別する引数を渡す必要があり、NUM_SVC_ARG_MAX(5) つしか渡せない引数の一つを消費することになるため。
    if @next_cell_port_subscript then
      subscript = "__" + @next_cell_port_subscript.to_s
    else
      subscript = ""
    end
    @ct_name_body = "#{@ct_name}SVCBody_#{@next_cell.get_name}_#{@next_cell_port_name}#{subscript}".to_sym
    @ct_name = "#{@ct_name}SVCCaller_#{@next_cell.get_name}_#{@next_cell_port_name}#{subscript}".to_sym
    @cell_name_body = "#{@cell_name}SVCBody".to_sym
    @cell_name = "#{@cell_name}SVCCaller".to_sym
    # puts "%%%% "
    # p @next_cell.get_name
    # p @caller_cell.get_name
    @b_printed_include_stdint = false
    check_signature signature
  end

  #=== NamespacePath を得る
  # 生成するセルの namespace path を生成する
  def get_cell_namespace_path
#    nsp = @region.get_namespace.get_namespace_path
    nsp = @start_region.get_namespace_path
    return nsp.append( @cell_name )
  end

  def gen_plugin_decl_code( file )

    # このセルタイプ（同じシグニチャ）は既に生成されているか？
    if @@generated_celltype[ @ct_name_body ] == nil then
        @@generated_celltype[ @ct_name_body ] = [ self ]
        file2 = CFile.open( "#{$gen}/#{@ct_name_body}.cdl", "w" )
        file2.print <<EOT
import_C( "t_stdlib.h" );

/* HRPSVC0001 */
[active,singleton]
celltype #{@ct_name_body} {
    call #{@signature.get_name} #{@call_port_name};
    FACTORY {
		write("$ct$_tecsgen.h", "#include \\"kernel_cfg.h\\"");
    };
};
EOT
        file2.close
    else
      @@generated_celltype[ @ct_name_body ] << self
    end
    file.print "import( \"#{$gen}/#{@ct_name_body}.cdl\" );\n"

    # このセルタイプ（同じシグニチャ）は既に生成されているか？
    if @@generated_celltype[ @ct_name ] == nil then
        @@generated_celltype[ @ct_name ] = [ self ]
        file2 = CFile.open( "#{$gen}/#{@ct_name}.cdl", "w" )
        file2.print <<EOT
/* HRPSVC0002 */
celltype #{@ct_name} {
    entry #{@signature.get_name} #{@entry_port_name};
    FACTORY {
		write("$ct$_tecsgen.h", "#include \\"extsvc_fncode.h\\"");
    };
};
EOT
        file2.close
    else
      @@generated_celltype[ @ct_name ] << self
    end
    file.print "import( \"#{$gen}/#{@ct_name}.cdl\" );\n"

=begin
    # TODO: send. receive 対応
    send_receive = []
    if @signature != nil then
      @signature.each_param{ |fd,param|
        dir =param.get_direction
        case dir
        when :SEND, :RECEIVE
          send_receive << [ dir, fd, param ]
        end
      }
    end
=end

=begin
composite #{@ct_name} {
    entry #{@signature.get_name} #{@entry_port_name};
    call #{@signature.get_name} #{@call_port_name};

    cell #{@ct_name}Client #{@cell_name}Client{
    };

    [active]
    cell #{@ct_name}Server #{@cell_name}Server{
        #{@call_port_name} => composite.#{@call_port_name};
    };

    composite.#{@entry_port_name} => #{@cell_name}.#{@entry_port_name};
};
=end

  end

  #===  through cell コードを生成
  #
  #
  def gen_through_cell_code( file )

    # gen_plugin_decl_code( file )  this is called from super.

    # セルを探す
    # path =["::",@next_cell.get_name]  # mikan namespace
    # cell = Namespace.find( path )
    # cell = Namespace.find( @next_cell.get_namespace_path )

    ##### クライアント側のセルの生成 #####
    # file.print "[domain(HRP, \"kernel\")]"
    nest = @start_region.gen_region_str_pre file
    nest_str = "  " * nest

    # クライアント側チャンネルの生成
    # 拡張サービスコール呼出し
    file.print <<EOT
/* HRPSVC0003 */
#{nest_str}  //  Client Side Channel
#{nest_str}  cell #{@ct_name} #{@cell_name}{
#{nest_str}  };

EOT

    @start_region.gen_region_str_post file
    file.print "\n\n"

    ##### サーバー側のセルの生成 #####
    # print "######  #{@cell_name_body} ... #{@ct_name_body} #{@@generated_cell[ @ct_name_body ].class}  #####\n"

    if @@generated_cell[ @ct_name_body ] then
      file.print "/* cell #{@ct_name_body} #{@@generated_cell[ @ct_name_body ]}: already generated. */\n"
    else
      @@generated_cell[ @ct_name_body ] = @cell_name_body
      nest = @end_region.gen_region_str_pre file
      nest_str = "  " * nest
      if @next_cell_port_subscript then
        subscript = '[' + @next_cell_port_subscript.to_s + ']'
      else
        subscript = ""
      end

      # サーバー側チャンネルの生成 
      # 拡張サービスコール本体
      file.print <<EOT

/* HRPSVC0004 */
#{nest_str}  //  Server Side Channel
#{nest_str}  cell #{@ct_name_body} #{@cell_name_body}{
#{nest_str}    #{@call_port_name} = #{@next_cell.get_namespace_path.get_path_str}.#{@next_cell_port_name}#{subscript};
#{nest_str}  };
EOT

      @end_region.gen_region_str_post file

      file2 = AppFile.open( "#{$gen}/tecsgen.cfg" )
      file2.print "\n/* Generated by HRPSVCPlugin */\n\n"
      file2.print <<EOT
/* HRPSVC0005 */
#include "#{@ct_name_body}_factory.h"
EOT
      file2.close

      # callee_cell のget_restricted_regions を呼び出しておく
      # restrict が参照された印をつけるため
      @callee_cell.get_restricted_regions( :dummy_entry_name, :dummy_func_name )
    end
  end

  #===  受け口関数の本体(C言語)を生成する
  #     通常であれば、ジェネレータは受け口関数のテンプレートを生成する
  #     プラグインの場合、変更する必要のないセルタイプコードを生成する
  #file::           FILE        出力先ファイル
  #b_singleton::    bool        true if singleton
  #ct_name::        Symbol
  #global_ct_name:: string
  #sig_name::       string
  #ep_name::        string
  #func_name::      string
  #func_global_name:: string
  #func_type::      class derived from Type
  def gen_ep_func_body( file, b_singleton, ct_name, global_ct_name, sig_name, ep_name, func_name, func_global_name, func_type, params )
        # puts "generate ep_func for #{ct_name}"

        #
        #  拡張サービスコール呼出し側の関数生成
        #
        #  完成形のイメージ
        #  
        # ER_UINT
        # eThroughEntry_write(CELLIDX idx, const char* buffer, uint_t length)
        # {
        #   ER_UINT  retval;
        #   tHRPSVCPlugin_<Sig>SVCCaller_<Cell>_<Entry>_CB    *p_cellcb;
        #   if( VALID_IDX( idx ) ){
        #       p_cellcb = tHRPSVCPlugin_<Sig>SVCBody_<Cell>_<Entry>_GET_CELLCB(idx);
        #   }else{
        #       /* エラー処理コードをここに記述 */
        #   }
        # 
        #   retval = (ER_UINT)cal_svc( TFN_TECSGEN_ORIGIN + svcid, 
        #                              (intptr_t)par1, (intptr_t)par2, 0, 0, 0 );
        #
        #   return retval;
        # }

        if ! func_type.get_type.kind_of?( VoidType ) then
            file.print( "  #{func_type.get_type_str}  retval;\n" )
        end

        # p "celltype_name, sig_name, func_name, func_global_name"
        # p "#{ct_name}, #{sig_name}, #{func_name}, #{func_global_name}"

        delim = ""
        if ! func_type.get_type.kind_of?( VoidType ) then
            file.print( "  retval = (#{func_type.get_type_str})" )
        else
            file.print( "  " )
        end

        #file.print( "#{@call_port_name}_#{func_name}(" )
        SVCManage.set_func_id( "#{@ct_name_body}_#{func_name}" )
        svcid = SVCManage.get_func_id( "#{@ct_name_body}_#{func_name}" )
        #file.print( "cal_svc( #{@ct_name_body}_#{func_name}" )
        file.print( "cal_svc( TFN_TECSGEN_ORIGIN + #{svcid.to_s}" )

        #    if ( ! b_singleton ) then
        #      file.print( " tecs_this" )
        #      delim = ","
        #    end

        i = 0
        passed_param = {}
        params.each{ |param|
            delim = ","
            file.printf( "#{delim} (intptr_t)#{param.get_name}" )
            passed_param[i] = param.get_name
            i += 1
        }

        while(i < NUM_SVC_ARG_MAX) do
            delim = ","
            file.printf( "#{delim} 0" )
            passed_param[i] = "par#{i+1}"
            i += 1
        end

        file.print( " );\n\n" )

        if ! func_type.get_type.kind_of?( VoidType ) then
            file.print( "  return retval;\n" )
        end

        #
        #  拡張サービスコール本体側の関数生成
        #
        #  完成形のイメージ
        #  
        # ER_UINT
        # eThroughEntry_write(CELLIDX idx, const char* buffer, uint_t length)
        # {
        #   ER_UINT  retval;
        #   tHRPSVCPlugin_<Sig>SVCBody_<Cell>_<Entry>_CB    *p_cellcb;
        #   if( VALID_IDX( idx ) ){
        #       p_cellcb = tHRPSVCPlugin_<Sig>SVCBody_<Cell>_<Entry>_GET_CELLCB(idx);
        #   }else{
        #       /* エラー処理コードをここに記述 */
        #   }
        # 
        #   retval = (ER_UINT)cal_svc( TFN_TECSGEN_ORIGIN + svcid, 
        #                              (intptr_t)par1, (intptr_t)par2, 0, 0, 0 );
        #
        #   return retval;
        # }
            file2 = AppFile.open( "#{$gen}/#{@ct_name_body}.c" )
            if @b_printed_include_stdint == false then
              file2.print <<EOT
#ifndef SIZE_MAX
#define  SIZE_MAX   (~0UL)
#endif

EOT
              @b_printed_include_stdint = true
            end

            if func_type.get_type.kind_of?( VoidType ) then
                retval_assign = ""
            else
                retval_assign = "retval = (ER_UINT)"
            end

            file2.print <<EOT
/* HRPSVC0006 */
ER_UINT
#{@ct_name_body}_#{func_name}(intptr_t #{passed_param[0]}, intptr_t #{passed_param[1]}, intptr_t #{passed_param[2]}, intptr_t #{passed_param[3]}, intptr_t #{passed_param[4]}, ID cdmid)
{
    ER_UINT retval = E_OK;

EOT

            # 
            #  エラーチェック処理の生成
            #

            # 
            #  呼出し元ドメインのチェック
            #   * private method: gen_caller_check_code参照
            #
            generated_check_code = gen_caller_check_code(func_name)
            check_code = generated_check_code["check_code"]
            user_cannot_callable = generated_check_code["user_cannot_callable"]

            # 
            #  パラメータにポインタが存在する場合，呼出し元タスクに対する
            #  アクセス権のチェック処理を出力する
            #  ※ cdmidがカーネルドメイン(拡張サービスコール呼出し中のユーザ
            #    ドメインを含む)であればprb_memの処理をスキップし，初段の
            #    拡張サービスコールのみprb_memを呼出し元タスクに発行する
            #
            num = 0
            params.each{ |param|
                if param.get_declarator.get_ptr_level > 0 then
                    align_check_str = "!ALIGN_TYPE(#{passed_param[num]}, #{param.get_type.get_referto.get_type_str}) || "
                  
                    if param.get_type.get_referto.kind_of?(IntType) then
                        case param.get_type.get_referto.get_bit_size
                        when -11, -1, 8   # char, char_t, int8_t (無符号含む)
                        #
                        #  charデータの場合，ALIGN_TYPEは必ずTRUE
                        #  となるので，エラーチェックを省略
                        #  char型の@bit_sizeは-11
                        #  tecsgen/tecslib/core/types.rbを参照
                        #
                          align_check_str = ""
                        end
                    end
                    if param.get_direction == :IN then
                        #
                        #  入力([in])のポインタパラメータは，呼出し元タスクに
                        #  TPM_READ(読出し可能)のアクセス権が必要
                        #
                        # 二重ポインタが不可のため、size_is と string が同時に設定されることはない
                        prb_func = "prb_mem"
                        if param.get_size then
                          size_str = param.get_size.to_s
                        elsif param.get_string == -1 then
                          size_str = "SIZE_MAX"
                          prb_func = "prb_str"
                        elsif param.get_string then
                          size_str = param.get_string.to_s
                          prb_func = "prb_str"
                        else
                          size_str = "1"
                        end
                        check_code.concat <<EOT
        /* HRPSVC0007 */
        if (#{align_check_str}#{prb_func}((void *)#{passed_param[num]}, sizeof(#{param.get_type.get_referto.get_type_str}) * (#{size_str}), TSK_SELF, TPM_READ) != E_OK) {
            return E_MACV;
        }
EOT

                    elsif param.get_direction == :OUT || param.get_direction == :INOUT then
                        #
                        #  出力([out])のポインタパラメータは，呼出し元タスクに
                        #  TPM_WRITE(書込み可能)のアクセス権が必要
                        #
                        prb_func = "prb_mem"
                        if param.get_size then
                          size_str = param.get_size.to_s
                        elsif param.get_string then          # 引数なしの string はない
                          size_str = param.get_string.to_s
                          # prb_func = "prb_str"             # out, inout の場合、必ず領域を確保する. prb_mem を用いる
                        else
                          size_str = "1"
                        end
                        check_code.concat <<EOT
        /* HRPSVC0008 */
        if (#{align_check_str}#{prb_func}((void *)#{passed_param[num]}, sizeof(#{param.get_type.get_referto.get_type_str}) * (#{size_str}), TSK_SELF, TPM_WRITE) != E_OK) {
            return E_MACV;
        }
EOT

                    end
                end
                num += 1
            }

            # 
            #  呼出し元がカーネルドメインのみ許可されている場合，
            #  すべてのユーザドメインからの呼出しに対し，E_OACVを返す
            #
            if user_cannot_callable
                check_code = "\t\treturn E_OACV;"
            end

            if check_code != ""
                # 
                #  呼出し元がカーネルドメインの場合，アクセス権のチェック
                #  処理をスキップさせる
                #
                file2.print <<eot
    if (cdmid != TDOM_KERNEL) {
#{check_code}
    }
eot
            end

            # 
            #  拡張サービスコール本体(本来の受け口関数)を呼び出す
            #
            file2.print"    #{retval_assign}#{@call_port_name}_#{func_name}("

            delim = ""
            num = 0
            params.each{ |param|
                file2.print "#{delim}"
                delim = ", "
                file2.print "(#{param.get_type.get_type_str})"
                file2.print passed_param[num]
                file2.print param.get_type.get_type_str_post
                num += 1
            }

            file2.print ");\n"

            file2.print "\n    return retval;\n"
            file2.print "}\n\n"

            file2.close

            # 
            #  拡張サービスコールの登録
            #
            file2 = AppFile.open( "#{$gen}/tecsgen.cfg" )
            file2.print "\n/* Generated by HRPSVCPlugin */\n\n"
            file2.print <<EOT
/* HRPSVC0009 */
KERNEL_DOMAIN{
    DEF_SVC( TFN_TECSGEN_ORIGIN + #{svcid.to_s}, { TA_NULL, #{@ct_name_body}_#{func_name}, SSZ_#{func_global_name} } );
}
EOT
            file2.close

            # 
            #  拡張サービスコール登録に必要な情報をヘッダに出力
            #   - 拡張サービスコール呼出し時のチェックで使用するスタックサイズを出力
            #   - 拡張サービスコールとして登録する関数名のextern宣言を出力
            #
            file2 = AppFile.open( "#{$gen}/#{@ct_name_body}_factory.h" )
            file2.print "\n/* Generated by HRPSVCPlugin */\n\n"
            file2.print <<EOT
/* HRPSVC0010 */
#ifndef SSZ_#{func_global_name}
#define SSZ_#{func_global_name}	DefaultExtsvcStackSize
#endif /* SSZ_#{func_global_name} */

/* HRPSVC0011 */
extern ER_UINT #{@ct_name_body}_#{func_name}(intptr_t par1, intptr_t par2, intptr_t par3, intptr_t par4, intptr_t par5, ID cdmid);
EOT
            file2.close
  end

  def get_callee_cell
      return @callee_cell
  end

  def get_caller_cell
      return @caller_cell
  end

  def get_callee_ep_name
      return @join.get_port_name
  end

  private
  # 
  #  拡張サービスコール本体における，呼出し元チェックのコードを
  #  出力する
  #  gen_ep_func_body からのみ呼び出される
  #  引数: 対象の関数名
  #  返り値: 下記のハッシュ
  #    {"check_code"=>出力するエラーチェックコード, 
  #     "user_cannot_callable"=>ユーザドメインが呼出し不可能かどうかのフラグ}
  #
  def gen_caller_check_code(func_name)  
    dbgPrint "gen_caller_check_code(func_name): #{@callee_cell.get_name}\n"
      # 
      #  エラーチェック処理
      #
      check_code = ""
      user_cannot_callable = false
      all_domain_callable = false
      caller_unrestricted = false

      # 
      #  呼出し元ドメインのチェック処理
      #  callable_domains: 拡張サービスコールを呼出し可能なドメインのリスト
      #    - 無所属のセルから結合されている場合，すべてのセルに対して，
      #    　callable?をチェックし，呼出し可能なすべてのドメインを返す
      #    - 無所属以外のセルから結合されている場合，そのセルに対して
      #      callable?をチェックし，呼出し可能であれば，そのドメインを返す
      #
      callable_domains = []
      @@generated_celltype[ @ct_name_body ].each { |svcplugin|
          if svcplugin.get_caller_cell.get_region.get_domain_root.get_domain_type.get_option == "OutOfDomain"
              # 無所属かつ active な場合も、restrict に従う
              # if svcplugin.get_caller_cell.get_celltype.is_active?
              #    #
              #    #  無所属かつactiveなセルは、TECSから存在が認識されていないのを
              #    #  含む任意のドメインから呼び出される可能性も存在する
              #    #
              #    caller_unrestricted = true
              # else
              #     # 
              #     #  無所属から接続されている場合は，すべてのセルの
              #     #  restrictをチェック
              #     #
              #     Cell.get_cell_list2.each { |cell|
              #         if cell.callable?(svcplugin.get_callee_cell, svcplugin.get_callee_ep_name, func_name)
              #             callable_domains << cell.get_region.get_domain_root
              #         end
              #     }
              #     print "callable_domains: "
              #     callable_domains.each{ |dm| print dm.get_name, " " }
              #     print "\n"
              # end

              # restrict 指定がある場合には、それに従う。さもなければ、チェックしない
              callable_domains = @callee_cell.get_restricted_regions( get_callee_ep_name, func_name )
              if callable_domains == nil then
                caller_unrestricted = true
              end
              #  print "restrict_list: "
              #  delim = ""
              #  callable_domains.each{ |domain|
              #    print delim, domain
              #    delim = ", "
              #  }
              #  print "\n"

          elsif svcplugin.get_caller_cell.callable?( svcplugin.get_callee_cell, svcplugin.get_callee_ep_name, func_name )
              # 
              #  特定のドメインから接続されている場合は，呼出し元セルの
              #  restrictをチェック
              #
              callable_domains << svcplugin.get_caller_cell.get_region.get_domain_root
          else
              # 
              #  無所属から結合されておらず，特定の呼出し元ドメインにアクセス権
              #  がない場合，callable_domainsは空となる
              #
              # pp "#{svcplugin.get_caller_cell.get_name} cannot call #{svcplugin.get_callee_cell.get_name}_#{svcplugin.get_callee_ep_name}_#{func_name}"
          end
      }

      if caller_unrestricted
          # pp "caller_unrestricted: #{@ct_name_body}"
          return {"check_code"=>"", "user_cannot_callable"=>false}
      end

      # 
      #  重複を削除
      #
      callable_domains.uniq!
      # 
      #  無所属に対するドメインチェックは実施しない
      #  カーネルドメインに対するドメインチェックは実施しない
      #
      callable_domains = callable_domains.select { |domain|
          ((domain.get_domain_type.get_option != "OutOfDomain") && \
           (domain.get_domain_type.get_option != "kernel"))
      }
      # pp "callable_domains"
      # pp callable_domains.map{|domain| domain.get_name }
      # 
      #  すべてのユーザドメインから呼出し可能な場合，ドメインチェックは
      #  実施しない
      #
      all_domain_regions = DomainType.get_domain_regions[:HRP].select { |reg|
          ((reg.get_domain_type.get_option != "OutOfDomain") && \
           (reg.get_domain_type.get_option != "kernel"))

      }
      # pp "all domains"
      # pp all_domain_regions.map {|reg| reg.get_name}
      if all_domain_regions.all? {|reg| callable_domains.include?(reg)}
          all_domain_callable = true
      end

      # 
      #  呼出し元ドメインのチェック処理本体の生成
      #
      if callable_domains.length == 0
        dbgPrint "callable_domain.length = 0\n"
          # 
          #  ユーザドメインから呼出し不可能な場合は
          #  個別のエラーチェックはせず，問答無用でE_OACVを返す
          #
          user_cannot_callable = true
      elsif callable_domains.length == 1
        dbgPrint "callable_domain.length = 1\n"
          # 
          #  呼出し可能なユーザドメインが単一の場合は
          #  cdmid != <domain名> の形式でチェックする
          #
          check_code += "\t/* HRPSVC0012.1 */\n"
          check_code += "\tif (cdmid != #{callable_domains[0].get_name}) {\n"
      elsif callable_domains.length > 1 && !all_domain_callable
        dbgPrint "callable_domain.length > 1 && not all_domains \n"
          # 
          #  呼出し可能なユーザドメインが複数の場合は
          #  TACP(cdmid) & (TACP(<domain名>) | ...) != 0U
          #  の形式でチェックする
          #
          check_code += "\t/* HRPSVC0012.2 */\n"
          check_code += "\tif((TACP(cdmid) & ("
          check_code += (callable_domains.map { |domain| "TACP(#{domain.get_name})"}).join("|")
          check_code += ")) != 0U) {\n"
      else
        dbgPrint "callable_all_domains\n"
      end
      if check_code != ""
          # 
          #  呼出し可能なユーザドメインのチェックがある場合は
          #  エラーコードを返すためのコードを出力する
          #
          check_code += <<EOS
            /* HRPSVC0013 */
            return E_OACV;
        }
EOS
      end

      return {"check_code"=>check_code, "user_cannot_callable"=>user_cannot_callable}
  end

  #---------------------------------------------------------#
  #=== シグニチャのチェック
  def check_signature signature
    signature.get_function_head_array.each{ |fh|
      type = fh.get_return_type
      check_return_type signature, fh, type
      if fh.get_paramlist.get_items.length > NUM_SVC_ARG_MAX then
        cdl_error( "HSV0005 $1.$2: # of parameter more than #{NUM_SVC_ARG_MAX}", signature.get_name, fh.get_name )
      end
      fh.get_paramlist.get_items.each{ |param|
        check_param signature, fh, param
      }
    }
  end
  #=== 戻り値の型のチェック
  # ER, ER_UINT は推奨される型
  # 整数、ブール、void は可能、他は不可
  def check_return_type signature, fh, type
    ot = type.get_original_type
    if( type.get_type_str == "ER" || type.get_type_str == "ER_UINT" ) then
      # OK!
    elsif ot.kind_of?( IntType ) || ot.kind_of?( VoidType ) || ot.kind_of?( BoolType ) then
      cdl_warning( "HSW0001 $1.$2: $3 return type cannot get access violation error", signature.get_name, fh.get_name, type.get_type_str.downcase )
      check_intptr "#{signature.get_name}.#{fh.get_name} return type", type
    else
      cdl_error( "HSV0001 $1.$2 return type $3 cannot be used", signature.get_name, fh.get_name, type.get_type_str.to_s+type.get_type_str_post.to_s )
    end
  end
  #=== 引数の型のチェック
  def check_param signature, fh, param
    type = param.get_type
    ot = type.get_original_type
    dir = param.get_direction
    case dir
    when :IN
      if ot.kind_of?( IntType ) || ot.kind_of?( BoolType ) then
        # OK!
        check_intptr "#{signature.get_name}.#{fh.get_name}.#{param.get_name}", type
      elsif ot.kind_of? PtrType then
        check_ptr signature, fh, param, dir
      else
        cdl_error( "HSV0002 $1.$2.$3 $4 param type cannot be used", signature.get_name, fh.get_name, param.get_name, type.get_type_str.to_s+type.get_type_str_post.to_s )
      end
    when :OUT, :INOUT
      if ot.kind_of? PtrType then
        check_ptr signature, fh, param, dir
      else
        # error
      end
    when :SEND, :RECEIVE
      cdl_error( "HSV0008 $1.$2.$3 param direction '$4' cannot be used", signature.get_name, fh.get_name, param.get_name, param.get_direction.to_s.downcase )
    end
  end
  def check_ptr signature, fh, param, dir
    type = param.get_type.get_referto
    ot = type.get_original_type
    if ot.kind_of?( IntType ) || ot.kind_of?( BoolType ) || ot.kind_of?( FloatType ) then
      # OK!
      check_intptr "#{signature.get_name}.#{fh.get_name}.#{param.get_name}", type
    elsif ot.kind_of? PtrType then
      cdl_error( "HSV0003 $1.$2.$3 multi-pointer type cannot be used", signature.get_name, fh.get_name, param.get_name, type.get_type_str.to_s+type.get_type_str_post.to_s )
    elsif  ot.kind_of? StructType then
      check_struct signature, fh, param
    else
      cdl_error( "HSV0004 $1.$2.$3 $4 type cannot be used", signature.get_name, fh.get_name, param.get_name, type.get_type_str.to_s+type.get_type_str_post.to_s )
    end
    if ( dir == :OUT || dir == :INOUT) && param.get_string == -1 then
      cdl_error( "HSV0009 $1.$2.$3 string argment is necessary for out/inout parameter", signature.get_name, fh.get_name, param.get_name )
    end
  end
  def check_struct signature, fh, param
    type = param.get_type.get_referto
    ot = type.get_original_type
    ot.get_members_decl.get_items.each{ |decl|
      type = decl.get_type
      ot = type.get_original_type
      if ot.kind_of?( IntType ) || ot.kind_of?( BoolType ) || ot.kind_of?( FloatType ) then
        # OK!
        check_intptr "#{signature.get_name}.#{fh.get_name}.#{param.get_name}.#{decl.get_name} member", type
      else
        dbgPrint "struct member #{decl.get_name} #{type} #{decl.get_type} #{decl.get_type.get_original_type}\n"
        if( decl.get_type.get_original_type.kind_of? ArrayType ) then
          dbgPrint "member array type #{decl.get_type.get_original_type.get_type} #{decl.get_type.get_original_type.get_type.get_original_type}\n"
          check_struct_member_array signature, fh, param, decl
        else
          cdl_error( "HSV0006 $1.$2.$3 $4 type cannot be used as struct member", signature.get_name, fh.get_name, param.get_name, type.get_type_str.to_s+type.get_type_str_post.to_s )
        end
      end
    }
  end
  def check_struct_member_array signature, fh, param, member_decl
    # p "check_struct_member_array: #{member_decl.get_type.get_type_str}"
    type = member_decl.get_type.get_type
    ot = type.get_original_type
    if ot.kind_of?( IntType ) || ot.kind_of?( BoolType ) || ot.kind_of?( FloatType ) then
      # OK!
      check_intptr "#{signature.get_name}.#{fh.get_name}.#{param.get_name}.#{member_decl.get_name} member", type
    else
      cdl_error( "HSV0007 $1.$2.$3 $4 type cannot be used as struct member", signature.get_name, fh.get_name, param.get_name, type.get_type_str.to_s+type.get_type_str_post )
    end
  end
  def check_intptr msg, type
    dbgPrint "check_intptr IN\n"
    t = type
    while( t.kind_of? DefinedType )
      dbgPrint "check_intptr #{msg} #{t.get_type_str} #{t.get_original_type.get_type_str}\n"
      tstr = t.get_type_str
      tstr.sub!( /const /, "" )
      tstr.sub!( /volatile /, "" )
      if tstr == "intptr_t" || tstr == "uintptr_t" then
        cdl_info( "HSI0001 $1 type '$2' not checked by plugin", msg, type.get_type_str )
      end
      t = t.get_type
    end
  end
end

