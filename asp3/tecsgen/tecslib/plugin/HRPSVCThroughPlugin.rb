# coding: utf-8
#
#  TECS Generator
#      Generator for TOPPERS Embedded Component System
#  
#   Copyright (C) 2008-2020 by TOPPERS Project
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
#  $Id: HRPSVCPlugin.rb 3140 2020-03-29 09:21:42Z okuma-top $
#++

# mikan through plugin: namespace が考慮されていない
# これを利用する場合、以下のように toppers_jsp.cdl sChannel.cdl を指定する必要がある
# tecsgen toppers_jsp.cdl sChannel.cdl your_description.cdl

# 以下を仮定（制限事項）
#  呼び元、呼び先のエンディアン、char, short, int_t, long_t, intptr_t のサイズが同じ
#  有符号、無符号でサイズが同じ

#
#  拡張サービスコールを用いたドメイン間通信の
#  throughプラグイン
#  HRPドメインプラグインによって挿入される
#  前提条件: 呼出し先がカーネルオブジェクトかどうかの判定はHRPドメインプラグイン
#  　　　　　で実施されるため，ここでは判定しないこととした
#

class HRPSVCThroughPlugin < ThroughPlugin

  NUM_SVC_ARG_MAX = 5              # HRP3 の拡張サービスコールで扱うことのできる引数の最大個数
  @@generated_cell = {}       # セルの重複排除用

  def initialize( cell_name, plugin_arg, next_cell, next_cell_port_name, next_cell_port_subscript, signature, celltype, caller_cell )
    super

    caller_dt = caller_cell.get_region.get_domain_root.get_domain_type
    if caller_dt == nil then
      # 呼び元がドメイン指定のないリージョンに属する場合、エラーとしない (ローダブルモジュール対応)
      # cdl_error( "HSV0010 caller cell '$1' is not in domain region.", caller_cell.get_name )
    elsif caller_dt.get_name != :HRP && caller_dt.get_name != :HRMP then
      cdl_error( "HSV0011 domain of caller cell '$1' is '$1'. This plugin supports HRP or HRMP", caller_cell.get_name, caller_dt.get_name )
    end
    callee_dt = next_cell.get_region.get_domain_root.get_domain_type
    if callee_dt == nil then
      cdl_error( "HSV0012 callee cell '$1' is not in domain region.", next_cell.get_name )
    elsif ( callee_dt.get_name != :HRP && callee_dt.get_name != :HRMP ) then
      cdl_error( "HSV0013 domain of callee cell '$1' is '$2'. This plugin supports HRP or HRMP", next_cell.get_name, callee_dt.get_name )
    end
    
    # 受け口配列の場合、配列添数ごとに別のセルタイプとする
    # セルタイプをシングルトン化したいため。
    # さもないと、セルを識別する引数を渡す必要があり、NUM_SVC_ARG_MAX(5) つしか渡せない引数の一つを消費することになるため。
    if @next_cell_port_subscript then
      subscript = "__" + @next_cell_port_subscript.to_s
    else
      subscript = ""
    end
    sig_name = signature.get_global_name
    @ct_name_body = "tHRPSVCBody_#{sig_name}".to_sym
    @ct_name = "tHRPSVCCaller_#{sig_name}".to_sym
    @cell_name_body = "#{@cell_name}SVCBody".to_sym
    @cell_name = "#{@cell_name}SVCCaller".to_sym
    @entry_port_name = :"eEntry"

    @b_printed_include_stdint = false
  end

  #=== NamespacePath を得る
  # 生成するセルの namespace path を生成する
  def get_cell_namespace_path
#    nsp = @region.get_namespace.get_namespace_path
    nsp = @start_region.get_namespace_path
    return nsp.append( @cell_name )
  end

  def gen_plugin_decl_code( file )
    subsc = @next_cell_port_subscript ? ('[' + @next_cell_port_subscript.to_s + ']') : ''
    file.print <<EOT
/*
 * generated automatically by HRPSVCThroughPlugin.
 *   caller port=#{@caller_cell.get_name}.#{@join.get_name}
 *   callee port=#{@next_cell.get_name}.#{@next_cell_port_name}#{subsc}
 */

/* generate celltypes for extended service call for signature #{@signature.get_name}  */
generate( HRPSVCSignaturePlugin, #{@signature.get_namespace_path}, "" );

EOT
  end

  #===  through cell コードを生成
  #
  #
  def gen_through_cell_code( file )

    ##### クライアント側のセルの生成 #####
    # file.print "[domain(HRP, \"kernel\")]"
    # if @@generated_cell[ @next_cell ] then
    #   tfn = "TFN_#{@@generated_cell[ @next_cell ].to_s.upcase}"
    # else
    #   tfn = "TFN_#{@cell_name_body.to_s.upcase}"
    # end
    tfn = "TFN_SVCBody_#{@next_cell.get_name}_#{@next_cell_port_name}".upcase
    nest = @start_region.gen_region_str_pre file
    nest_str = "  " * nest

    # クライアント側チャンネルの生成
    # 拡張サービスコール呼出し
    file.print <<EOT
#{nest_str}/* HRPSVCTHROUGH0003 */
#{nest_str}//  Client Side Channel
#{nest_str}cell #{@ct_name} #{@cell_name}{
#{nest_str}  tfn_base = C_EXP( \"#{tfn}\" );
#{nest_str}};

EOT

    @start_region.gen_region_str_post file
    file.print "\n\n"

    ##### サーバー側のセルの生成 #####
    # print "######  #{@cell_name_body} ... #{@ct_name_body} #{@@generated_cell[ @ct_name_body ].class}  #####\n"
    file.print <<EOT
/* HRPSVCTHROUGH0004 : generate kernel side cell */
generate( HRPSVCBodyCellPlugin, #{@next_cell.get_namespace_path}, "exportEntryPort=#{@next_cell_port_name}:#{@start_region.get_name}" );
EOT

=begin
    if @@generated_cell[ @next_cell ] then
      file.print "/* cell #{@ct_name_body} #{@@generated_cell[ @next_cell ]}: already generated. */\n"
    else
      @@generated_cell[ @next_cell ] = @cell_name_body
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

#{nest_str}/* HRPSVCTHROUGH0004 */
#{nest_str}//  Server Side Channel
#{nest_str}cell #{@ct_name_body} #{@cell_name_body}{
EOT
      @signature.get_function_head_array.each{ |func_head|
        func_name = func_head.get_name
        restrict_acptn = gen_caller_check_code(func_name)
        file.print <<EOT
#{nest_str}   restrict_acptn_#{func_name} = #{restrict_acptn};
EOT
      }
      file.print <<EOT
#{nest_str}  #{@call_port_name} = #{@next_cell.get_namespace_path.get_path_str}.#{@next_cell_port_name}#{subscript};
#{nest_str}};
EOT
      @end_region.gen_region_str_post file

      # サーバー側セルの情報
      rgn_nsp = @end_region.get_namespace_path
      cell_nsp = rgn_nsp.append @cell_name_body
      HRPSVCManage.add_cell @end_region.get_node_root, cell_nsp, @signature

      # callee_cell のget_restricted_regions を呼び出しておく
      # restrict が参照された印をつけるため
      @callee_cell.get_restricted_regions( :dummy_entry_name, :dummy_func_name )
    end
=end
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
    print "gen_caller_check_code(func_name): #{@callee_cell.get_name}\n"
      # 
      #  エラーチェック処理
      #
      user_cannot_callable = false
      caller_unrestricted = false
      callable_domains = []

      # 
      #  呼出し元ドメインのチェック処理
      #  callable_domains: 拡張サービスコールを呼出し可能なドメインのリスト
      #    - 無所属のセルから結合されている場合，すべてのセルに対して，
      #    　callable?をチェックし，呼出し可能なすべてのドメインを返す
      #    - 無所属以外のセルから結合されている場合，そのセルに対して
      #      callable?をチェックし，呼出し可能であれば，そのドメインを返す
      #
      if @caller_cell.get_region.get_domain_root.get_domain_type == nil ||  # 呼び元は、ドメインに属さない
         @caller_cell.get_region.get_domain_root.get_domain_type.get_option == "OutOfDomain"
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

      elsif @caller_cell.callable?( @callee_cell, get_callee_ep_name, func_name )
          # 
          #  特定のドメインから接続されている場合は，呼出し元セルの
          #  restrictをチェック
          #
          callable_domains << @caller_cell.get_region.get_domain_root
      else
          # 
          #  無所属から結合されておらず，特定の呼出し元ドメインにアクセス権
          #  がない場合，callable_domainsは空となる
          #
          # pp "#{@caller_cell.get_name} cannot call #{svcplugin.get_callee_cell.get_name}_#{svcplugin.get_callee_ep_name}_#{func_name}"
      end

      if caller_unrestricted
          # pp "caller_unrestricted: #{@ct_name_body}"
          return "C_EXP(\"TACP_SHARED\")"
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
      # HRP3, HRMP3 両対応
      if DomainType.get_domain_regions(@end_region.get_node_root)[:HRP] then
        regions = DomainType.get_domain_regions(@end_region.get_node_root)[:HRP]
      elsif DomainType.get_domain_regions(@end_region.get_node_root)[:HRMP] then
        regions = DomainType.get_domain_regions(@end_region.get_node_root)[:HRMP]
      else
        cdl_error( "HRMPSVCPlugin invoked under non-HRP, nor HRMP domain" )
      end
      all_domain_regions = regions.select { |reg|
          ((reg.get_domain_type.get_option != "OutOfDomain") && \
           (reg.get_domain_type.get_option != "kernel"))

      }

      # 
      #  呼出し元ドメインのチェック処理本体の生成
      #
      if callable_domains.length == 0
        dbgPrint "callable_domain.length = 0\n"
          # 
          #  ユーザドメインから呼出し不可能な場合は
          #  個別のエラーチェックはせず，問答無用でE_OACVを返す
          #
          check_code = "C_EXP(\"TACP_KERNEL\")"
      else
        dbgPrint "callable_domain.length > 1 && not all_domains \n"
          # 
          #  TACP(cdmid) & (TACP(<domain名>) | ...) != 0U
          #  の形式でチェックする
          #
          check_code = (callable_domains.map { |domain| "TACP(#{domain.get_name})"}).join("|")
          check_code = "C_EXP( \"#{check_code}\" )"
      end

      return check_code
  end
end
