# coding: utf-8
#
#  TECS Generator
#      Generator for TOPPERS Embedded Component System
#  
#   Copyright (C) 2020-2021 by TOPPERS Project
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
#  $Id: HRMPDomainPlugin.rb 3185 2020-11-11 13:35:08Z okuma-top $
#++

# HRP3, HRMP3 の ATT_MOD を生成する
# 生成先のファイルは、ドメイン、クラスに分けて generate.rb で生成される
module GenAttMod
  #=== tecsgen.cfg の INCLUDE を生成
  # 
  #  すべてのドメインに対する cfg を先に生成しておく
  #  もし、ドメインに属するカーネルオブジェクトも、モジュールもない場合でも、cfg が出力される
  def gen_include
    # regions = DomainType.get_domain_regions(node_root)[ :HRMP ]
    regions = Celltype.get_domain_class_roots_total.keys
    file = AppFile.open( "#{$gen}/tecsgen.cfg" )
    file.print "/* GenAttMod 001 */\n"
    regions.each{ |region|
      if ! region.is_root? then
        nsp = "#{region.get_global_name}"
        file2 = AppFile.open( "#{$gen}/tecsgen_#{nsp}.cfg" )
        file2.close
        if region.get_domain_root.get_domain_type then
          case region.get_domain_root.get_domain_type.get_kind
          when :kernel
            pre = "KERNEL_DOMAIN{ "
            post = "}"
            indent = "  "
          when :user
            pre = "DOMAIN(#{region.get_domain_root.get_name}){ "
            post = "}"
            indent = "  "
          when :OutOfDomain
            pre = ""
            post = ""
            indent = ""
          end
        else
          pre = ""
          post = ""
          indent = ""
        end
        if region.get_class_root.get_class_type then
          opt_non_pu = region.get_class_root.get_class_type.get_option
          if opt_non_pu != :"root" then
            if indent == "" then
              cr = ""
            else
              cr = "\n"
            end
            pre2 = "#{cr}#{indent}CLASS(#{opt_non_pu}){"
            post2 = "#{indent}}#{cr}"
            indent = "#{indent}  "
          else
            pre2 = ""
            post2 = ""
            # indent = "#{indent}"
          end
        else
          pre2 = ""
          post2 = ""
          # indent = "#{indent}"
        end

        file.puts "#{pre}#{pre2}\n#{indent}INCLUDE(\"#{$gen}/tecsgen_#{nsp}.cfg\");\n#{post2}#{post}\n"
      end
    }
    file.print "/* GenAttMod 001 end */\n\n"
    file.close
  end

  #===  ATT_MODの生成
  def gen_att_mod node_root
    dbgPrint "gen_att_mod root=#{node_root.get_namespace_path.to_s}\n"
    check_celltype_list = []

    Cell.get_cell_list2.each { |cell|
      # すべてのセルを走査してセルタイプをチェック
      ct = cell.get_celltype
      if ct.class == Celltype && check_celltype_list.include?( ct ) == false  then
        if ( ct.get_domain_roots[ :HRMP ] == nil && ct.get_domain_roots[ :HRP ] == nil ) then
          # FMP3 は、対象外となる
          next
        end
        # チェック済みセルタイプに登録
        check_celltype_list << ct
        dbgPrint "att_mod:  celltype: #{ct.get_name}\n"

        # 未チェックのセルタイプだった場合
        # puts "check for ATT_MOD : #{ct.classget_global_name}"
        # puts "check for ATT_MOD : #{ct.get_global_name}"

        # カーネルAPIのコード，データはメモリ保護しない # HRP3 oyama delete by Takada's request
        # next if HRPKernelObjectManager.include_celltype?( ct.get_name )
        # 必要のないセルタイプのコード，データはメモリ保護しない
        next if ! ct.need_generate?

        # HRMPのドメインリージョンを取得
        regions_ct = ct.get_domain_class_roots2 # Hash
        regions_hrmp = regions_ct.keys          # Array
        if $debug then
          print "ATT_MOD domain in #{ct.get_name}: "
          regions_hrmp.each { |reg|
            print reg.get_name
          }
          print "\n"
        end
        node_root = regions_hrmp[0].get_node_root

        # セル管理ブロックとスケルトンのメモリ保護
        # gen_celltype_names_domain 相当の処理
        if regions_hrmp.include?( node_root ) == false && regions_hrmp.length > 1
          # ドメインが複数で，OutOfDomainにセルが存在しないセルタイプの場合
          # 共有のセル管理ブロックとスケルトンコードを登録する
          file = AppFile.open( "#{$gen}/tecsgen.cfg" )
          file.printf "%-60s/* GenAttMod 002 */\n", "ATT_MOD(\"#{ct.get_global_name}_tecsgen.o\");"
          file.close
        end

        regions_hrmp.each { |reg|
          if reg.is_root? then
              nsp = ""
          else
              nsp = "_#{reg.get_global_name}"
          end
          # カーネルオブジェクトの場合 OutOfDomain に置く(暫定)
          # 望ましい仕様は、他のユーザードメインから操作されるものがある場合 OutOfDomain に置く
          # if HRPKernelObjectManager.include_celltype?( ct.get_name ) then
          #   nsp_dom = ""
          # else
              nsp_dom = nsp
          # end
          file = AppFile.open( "#{$gen}/tecsgen#{nsp_dom}.cfg" )
          file.printf "%-60s/* GenAttMod 003 */\n", "ATT_MOD(\"#{ct.get_global_name}#{nsp}_tecsgen.o\");"
          file.close
        }

        # セルタイプコードがない場合はスキップ
        next if ct.is_all_entry_inline? && ! ct.is_active?

        # セルタイプコードのメモリ保護
        # gen_celltype_names_domain2 相当の処理
        if regions_hrmp.include?( node_root ) == true || regions_hrmp.length > 1
          # OutOfDomainにセルが存在するセルタイプの場合
          # または，複数のドメインにセルが存在するセルタイプの場合
          # セルタイプコードを共有するように登録する
          file = AppFile.open( "#{$gen}/tecsgen.cfg" )
        else
          # OutOfDomainでない単一のドメインにセルが存在するセルタイプの場合
          # セルタイプコードを専有するように登録する
          file = AppFile.open( "#{$gen}/tecsgen_#{regions_hrmp[0].get_global_name}.cfg" )
        end

        file.printf "%-60s/* GenAttMod 004 */\n", "ATT_MOD(\"#{ct.get_global_name}.o\");"
        file.close
      else
        # 何もしない
      end
    }
  end
end

module GenDefSvc
  #=== 拡張サービスコールの登録
  def gen_def_svc node_root
    file2 = AppFile.open( "#{$gen}/tecsgen.cfg" )
    file2.print <<EOT

/* HRP010 HRPPlugin */
EOT
    HRPSVCManage.get_list(node_root).each{ |cell_nsp, signature|
      dbgPrint "RPPPlugin: cell_nsp=#{cell_nsp}\n"
      cell = Namespace.find cell_nsp
      ct = cell.get_celltype
      file2.print <<EOT
#include "#{signature.get_global_name}_tecsgen.h"
#include "tHRPSVCCaller_#{signature.get_global_name}_factory.h"
#include "tHRPSVCBody_#{signature.get_global_name}_factory.h"
EOT
      signature.get_function_head_array.each{|func_head|
        file2.print <<EOT
  KERNEL_DOMAIN{
    DEF_SVC( TFN_#{cell.get_name.to_s.upcase} + FUNCID_#{signature.get_global_name.to_s.upcase}_#{func_head.get_name.to_s.upcase}-1, { TA_NULL, #{cell.get_global_name}_#{func_head.get_name}, SSZ_#{cell.get_name}_#{func_head.get_name} } );
  }
EOT
      }
    }
    file2.print "/****/\n\n"
    file2.close

    HRPSVCManage.get_list(node_root).each{ |cell_nsp, signature|
      cell = Namespace.find cell_nsp
      ct = cell.get_celltype
      file2 = AppFile.open( "#{$gen}/#{ct.get_global_name}.c")
      file2.print "/* GenAttMod 005 */\n\n"
      signature.get_function_head_array.each{|func_head|
        file2.print <<EOT

ER_UINT
#{cell.get_global_name}_#{func_head.get_name}(intptr_t par1, intptr_t par2, intptr_t par3, intptr_t par4, intptr_t par5, ID cdmid)
{
CELLIDX idx = #{ct.get_name_array(cell)[7]};
return #{ct.get_name}_#{func_head.get_name}(idx, par1, par2, par3, par4, par5, cdmid);
}
EOT
      }
    }
    file2.close

    val = "TFN_TECSGEN_ORIGIN"
    offset = 0
    HRPSVCManage.get_list(node_root).each{ |cell_nsp, signature|
      cell = Namespace.find cell_nsp
      ct = cell.get_celltype
      # file2 = AppFile.open( "#{$gen}/#{ct.get_global_name}_factory.h" )
      file2 = AppFile.open( "#{$gen}/tHRPSVCCaller_#{signature.get_global_name}_id.h" )
      file2.print <<EOT
/* HRP012 HRPPlugin gen_factory */
#ifndef  TFN_#{cell.get_name.to_s.upcase}
#include "extsvc_fncode.h"
#define  TFN_#{cell.get_name.to_s.upcase}    (#{val}+#{offset.to_s})
#endif
EOT
      file2.close

      file2 = AppFile.open( "#{$gen}/tHRPSVCBody_#{signature.get_global_name}_factory.h" )
      signature.get_function_head_array.each{|func_head|
        file2.print <<EOT
/* HRP013 */
#include "tHRPSVCCaller_#{signature.get_global_name}_id.h"

#ifndef SSZ_#{cell.get_name}_#{func_head.get_name}
#define SSZ_#{cell.get_name}_#{func_head.get_name} DefaultExtsvcStackSize
#endif /* SSZ_#{cell.get_name}_#{func_head.get_name} */
ER_UINT #{cell.get_global_name}_#{func_head.get_name}(intptr_t par1, intptr_t par2, intptr_t par3, intptr_t par4, intptr_t par5, ID cdmid);

EOT
      }
      file2.close
      offset += signature.get_function_head_array.length
    }
  end
end
