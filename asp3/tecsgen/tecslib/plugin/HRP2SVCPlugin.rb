# -*- coding: utf-8 -*-
#
#  TECS Generator
#      Generator for TOPPERS Embedded Component System
#  
#   Copyright (C) 2008-2014 by TOPPERS Project
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
#   $Id: HRP2SVCPlugin.rb 2952 2018-05-07 10:19:07Z okuma-top $
#++

# mikan through plugin: namespace が考慮されていない
# これを利用する場合、以下のように toppers_jsp.cdl sChannel.cdl を指定する必要がある
# tecsgen toppers_jsp.cdl sChannel.cdl your_description.cdl

# 以下を仮定（制限事項）
#  呼び元、呼び先のエンディアン、char, short, int_t, long_t, intptr_t のサイズが同じ
#  有符号、無符号でサイズが同じ

class SVCManage
    # TODO: デフォルトの拡張サービスコール分を予約しておく
    # デフォルトの拡張サービスコール（syslogなど）もコンポーネント化するまで
    @@id = 20
    @@func_ids = {}
    def self.get_func_id func_name
        return @@func_ids[ func_name ]
    end
    def self.set_func_id func_name
        @@func_ids[ func_name ] = self.assign_id
        puts @@func_ids[ func_name ]
    end
    def self.include_func_id? func_name
        return @@func_ids.has_key?( func_name )
    end
    def self.get_id
        return @@id
    end
    def self.set_id id
        @@id = id
    end
    def self.assign_id
        @@id += 1
        return @@id
    end
end

#
# 拡張サービスコールを用いたドメイン間通信の
# throughプラグイン
# HRP2ドメインプラグインによって挿入される
#
class HRP2SVCPlugin < ThroughPlugin

  #=== RPCPlugin の initialize
  #  説明は ThroughPlugin (plugin.rb) を参照
  @@generated_celltype_header = {}

  def initialize( cell_name, plugin_arg, next_cell, next_cell_port_name, next_cell_port_subscript, signature, celltype, caller_cell )
    super
    @ct_name_body = "#{@ct_name}SVCBody_#{@next_cell.get_name}_#{@next_cell_port_name}".to_sym
    @ct_name = "#{@ct_name}SVCCaller_#{@next_cell.get_name}_#{@next_cell_port_name}".to_sym
    @cell_name_body = "#{@cell_name}SVCBody".to_sym
    @cell_name = "#{@cell_name}SVCCaller".to_sym
    puts "%%%% "
    p @next_cell.get_name
    p @caller_cell.get_name
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
    if !HRP2KernelObjectPlugin.include_celltype?(@next_cell.get_celltype)
    if @@generated_celltype[ @ct_name_body ] == nil then
        @@generated_celltype[ @ct_name_body ] = [ self ]
        file2 = CFile.open( "#{$gen}/#{@ct_name_body}.cdl", "w" )
        file2.print <<EOT
[active]
celltype #{@ct_name_body} {
    call #{@signature.get_name} #{@call_port_name};
};
EOT
        file2.close
    else
      @@generated_celltype[ @ct_name_body ] << self
    end
    file.print "import( \"#{$gen}/#{@ct_name_body}.cdl\" );\n"
    end

    # このセルタイプ（同じシグニチャ）は既に生成されているか？
    if @@generated_celltype[ @ct_name ] == nil then
        @@generated_celltype[ @ct_name ] = [ self ]
        file2 = CFile.open( "#{$gen}/#{@ct_name}.cdl", "w" )
        if !HRP2KernelObjectPlugin.include_celltype?(@next_cell.get_celltype)
        file2.print <<EOT
celltype #{@ct_name} {
    entry #{@signature.get_name} #{@entry_port_name};
};
EOT
        else
            # TODO inlineにした方が効率がよいが，tecsgenの生成したヘッダファイルの
            # 読込順のためにエラーとなる
        file2.print <<EOT
celltype #{@ct_name} {
    //[inline] entry #{@signature.get_name} #{@entry_port_name};
    entry #{@signature.get_name} #{@entry_port_name};
    call #{@signature.get_name} #{@call_port_name};
};
EOT
        end
        
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
    #require "HRP2Cache"

    gen_plugin_decl_code( file )

    if !HRP2KernelObjectPlugin.include_celltype?(@next_cell.get_celltype)
    # セルを探す
    # path =["::",@next_cell.get_name]  # mikan namespace
    # cell = Namespace.find( path )
    # cell = Namespace.find( @next_cell.get_namespace_path )

    ##### クライアント側のセルの生成 #####
    # file.print "[domain(HRP2, \"trusted\")]"
    nest = @start_region.gen_region_str_pre file
    nest_str = "  " * nest

    # クライアント側チャンネルの生成
    # 拡張サービスコール呼出し
    file.print <<EOT
#{nest_str}  //  Client Side Channel
#{nest_str}  cell #{@ct_name} #{@cell_name}{
#{nest_str}  };

EOT

    @start_region.gen_region_str_post file
    file.print "\n\n"

    ##### サーバー側のセルの生成 #####
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

#{nest_str}  //  Server Side Channel
#{nest_str}  cell #{@ct_name_body} #{@cell_name_body}{
#{nest_str}    #{@call_port_name} = #{@next_cell.get_namespace_path.get_path_str}.#{@next_cell_port_name}#{subscript};
#{nest_str}  };
EOT

    @end_region.gen_region_str_post file

    file2 = AppFile.open( "#{$gen}/tecsgen.cfg" )
    file2.print "\n/* Generated by HRP2SVCPlugin */\n\n"
    file2.print <<EOT
#include "#{@ct_name_body}_factory.h"
EOT
    file2.close
    else

    ##### クライアント側のセルの生成 #####
    nest = @start_region.gen_region_str_pre file
    nest_str = "  " * nest
    if @next_cell_port_subscript then
      subscript = '[' + @next_cell_port_subscript.to_s + ']'
    else
      subscript = ""
    end

    # クライアント側チャンネルの生成
    # 拡張サービスコール呼出し
    file.print <<EOT
#{nest_str}  //  Client Side Channel
#{nest_str}  cell #{@ct_name} #{@cell_name}{
#{nest_str}    #{@call_port_name} = #{@next_cell.get_namespace_path.get_path_str}.#{@next_cell_port_name}#{subscript};
#{nest_str}  };

EOT

    @start_region.gen_region_str_post file
    file.print "\n\n"

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
      puts "generate ep_func for #{ct_name}"

    if !HRP2KernelObjectPlugin.include_celltype?(@next_cell.get_celltype)
        # 拡張サービスコール呼出し
        if ! func_type.get_type.kind_of?( VoidType ) then
            file.print( "  #{func_type.get_type_str}  retval;\n" )
        end

        if ! b_singleton then

            file.print <<EOT
  #{ct_name}_CB    *p_cellcb;
  if( VALID_IDX( idx ) ){
    p_cellcb = #{global_ct_name}_GET_CELLCB(idx);
  }else{
     /* エラー処理コードをここに記述 */
  }

EOT
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
        # svcid = SVCManage.assign_id
        new_func = false
        if SVCManage.include_func_id?( "#{@ct_name_body}_#{func_name}" ) == false
            new_func = true
            SVCManage.set_func_id( "#{@ct_name_body}_#{func_name}" )
        end
        svcid = SVCManage.get_func_id( "#{@ct_name_body}_#{func_name}" )
        #file.print( "cal_svc( #{@ct_name_body}_#{func_name}" )
        file.print( "cal_svc( #{svcid.to_s}" )

        #    if ( ! b_singleton ) then
        #      file.print( " tecs_this" )
        #      delim = ","
        #    end

        i = 0
        params.each{ |param|
            delim = ","
            file.printf( "#{delim} #{param.get_name}" )
            i += 1
        }

        while(i < 5) do
            delim = ","
            file.printf( "#{delim} 0" )
            i += 1
        end

        file.print( " );\n" )

        if ! func_type.get_type.kind_of?( VoidType ) then
            file.print( "  return retval;\n" )
        end

        # 拡張サービスコール本体
        if new_func
            file2 = AppFile.open( "#{$gen}/#{@ct_name_body}.c" )

#             if @@generated_celltype_header[ @ct_name_body ].nil?
#                 @@generated_celltype_header[ @ct_name_body ] = true
#                 p @@generated_celltype_header[ @ct_name_body ]
#                 file2.print <<EOT
# /*
#  * このファイルは tecsgen により自動生成されました
#  * このファイルを編集して使用することは、意図されていません
#  */
# /* #[<PREAMBLE>]#
#  * #[<...>]# から #[</...>]# で囲まれたコメントは編集しないでください
#  * tecsmerge によるマージに使用されます
#  *
#  * #[</PREAMBLE>]# */
# 
# /* プロトタイプ宣言や変数の定義をここに書きます #_PAC_# */
# #include "#{@ct_name_body}_tecsgen.h"
# 
# #ifndef E_OK
# #define	E_OK	0		/* success */
# #define	E_ID	(-18)	/* illegal ID */
# #endif
# 
# EOT
#             end

            if func_type.get_type.kind_of?( VoidType ) then
                retval_assign = ""
                retval_return = ""
            else
                retval_assign = "retval = (ER_UINT)"
                retval_return = "retval"
            end

            file2.print <<EOT
ER_UINT
#{@ct_name_body}_#{func_name}(intptr_t par1, intptr_t par2, intptr_t par3, intptr_t par4, intptr_t par5, ID cdmid)
{
    #{@ct_name_body}_CB    *p_cellcb;
#if 0
    if( VALID_IDX( idx ) ){
      p_cellcb = #{@ct_name_body}_GET_CELLCB(idx);
    }else{
       /* エラー処理コードをここに記述 */
    }
#else
    p_cellcb = NULL;
#endif

    ER_UINT retval = 0;

EOT

            num = 1
            params.each{ |param|
                if param.get_declarator.get_ptr_level > 0 then
                    if param.get_direction == :IN then
                        file2.print <<EOT
    if(prb_mem((void *)par#{num.to_s}, sizeof(#{param.get_type.get_type_str}), TSK_SELF, TPM_READ) != E_OK){
        return E_MACV;
    }
EOT

                    elsif param.get_direction == :OUT then
                        file2.print <<EOT
    if(prb_mem((void *)par#{num.to_s}, sizeof(#{param.get_type.get_type_str}), TSK_SELF, TPM_WRITE) != E_OK){
        return E_MACV;
    }
EOT

                    end
                end
                num += 1
            }

            file2.print"    #{retval_assign}#{@call_port_name}_#{func_name}("

            delim = ""
            num = 1
            params.each{ |param|
                file2.print "#{delim}"
                delim = ", "
                file2.print "(#{param.get_type.get_type_str})"
                file2.print "par" + num.to_s
                file2.print param.get_type.get_type_str_post
                num += 1
            }

            file2.print ");\n"

            if !func_type.get_type.kind_of?( VoidType ) then
                file2.print "\n    return #{retval_return};\n"
            end
            file2.print "}\n\n"

            file2.close

            file2 = AppFile.open( "#{$gen}/tecsgen.cfg" )
            file2.print "\n/* Generated by HRP2SVCPlugin */\n\n"
            # TODO: スタックサイズは適当
            file2.print <<EOT
KERNEL_DOMAIN{
    DEF_SVC( #{svcid.to_s}, { TA_NULL, #{@ct_name_body}_#{func_name}, 256 } );
}
EOT
            file2.close

            file2 = AppFile.open( "#{$gen}/#{@ct_name_body}_factory.h" )
            file2.print "\n/* Generated by HRP2SVCPlugin */\n\n"
            file2.print <<EOT
extern ER_UINT #{@ct_name_body}_#{func_name}(intptr_t par1, intptr_t par2, intptr_t par3, intptr_t par4, intptr_t par5, ID cdmid);
EOT
            file2.close
        end
    else
        # カーネルドメインのセルは特別なことは何もせず，普通に呼び出す
        super
    end
  end
end


