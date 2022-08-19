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

require_tecsgen_lib "HRPKernelObjectManager.rb"

#
#  拡張サービスコールを用いたドメイン間通信の
#  シグニチャプラグイン
#  HRPドメインプラグインによって挿入されるセルの、セルタイプを生成する
#  前提条件: 呼出し先がカーネルオブジェクトかどうかの判定はHRPドメインプラグイン
#  　　　　　で実施されるため，ここでは判定しないこととした
#
class HRPSVCSignaturePlugin < SignaturePlugin

  NUM_SVC_ARG_MAX = 5              # HRP3 の拡張サービスコールで扱うことのできる引数の最大個数

  @@generated_celltype ={}    # セルタイプの重複排除用
  @@signature_checked = {}    # check_signature 済を記憶する

  #signature::     Signature        シグニチャ（インスタンス）
  def initialize( signature, option )
    super

    @signature = signature
    sig_name = signature.get_global_name
    @ct_name_body = "tHRPSVCBody_#{sig_name}".to_sym
    @ct_name = "tHRPSVCCaller_#{sig_name}".to_sym
    @call_port_name = :cCall
    @entry_port_name = :eEntry
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

  def gen_cdl_file( file )

    # このセルタイプ（同じシグニチャ）は既に生成されているか？
    if @@generated_celltype[ @ct_name_body ] == nil then
        @@generated_celltype[ @ct_name_body ] = [ self ]
        file.print <<EOT
import_C( "t_stdlib.h" );

/* HRPSVC0001 : callee side celltype */
[active]
celltype #{@ct_name_body} {
    call #{@signature.get_name} #{@call_port_name};
    attr {
EOT
        @signature.get_function_head_array.each{ |func_head|
          func_name = func_head.get_name
          file.print <<EOT
      ACPTN   restrict_acptn_#{func_name};
EOT
        }
        file.print <<EOT
      };
    FACTORY {
    write("$ct$_tecsgen.h", "#include \\"kernel_cfg.h\\"");
    };
};
EOT
    else
      @@generated_celltype[ @ct_name_body ] << self
    end

    # このセルタイプ（同じシグニチャ）は既に生成されているか？
    if @@generated_celltype[ @ct_name ] == nil then
        @@generated_celltype[ @ct_name ] = [ self ]
        file.print <<EOT
/* HRPSVC0002 : caller side celltype */
celltype #{@ct_name} {
    attr {
      FN  tfn_base;    //  TFN base number
    };
    entry #{@signature.get_name} #{@entry_port_name};
    FACTORY {
      write("$ct$_factory.h", "#include \\"#{@ct_name}_id.h\\"");
    };
};
EOT
    else
      @@generated_celltype[ @ct_name ] << self
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

        file.print( "  CELLCB *p_cellcb = GET_CELLCB(idx);\n")
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
        file.print( "cal_svc( ATTR_tfn_base + FUNCID_#{@signature.get_global_name.upcase}_#{func_name.to_s.upcase} - 1")
        # file.print( "cal_svc( TFN_TECSGEN_ORIGIN + #{svcid.to_s}" )

        delim = ","
        i = 0
        passed_param = {}
        params.each{ |param|
            file.printf( "#{delim} (intptr_t)#{param.get_name}" )
            passed_param[i] = param.get_name
            i += 1
        }

        while(i < NUM_SVC_ARG_MAX) do
            file.printf( "#{delim} 0" )
            passed_param[i] = "par#{i+1}"
            i += 1
        end

        file.print( " );\n\n" )

        if ! func_type.get_type.kind_of?( VoidType ) then
            file.print( "  return retval;\n" )
        end
  end

  #===  受け口関数の postamble (C言語)を生成する
  #     必要なら postamble 部に出力する
  #      gen_cdl_file の中で生成されたセルタイプに対して呼び出される
  #file::           FILE        出力先ファイル
  #b_singleton::    bool        true if singleton
  #ct_name::        Symbol
  #global_ct_name:: string
  def gen_postamble( file, b_singleton, ct_name, global_ct_name )
    # Callee (Body) 側のみ生成. Caller 側は生成しない
    if ct_name != @ct_name_body then
      return
    end
  
    # デフォルトでは何も出力しない
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
    file2 = file
    if @b_printed_include_stdint == false then
        file2.print <<EOT
  #ifndef SIZE_MAX
  #define  SIZE_MAX   (~0UL)
  #endif

EOT
        @b_printed_include_stdint = true
    end

    @signature.get_function_head_array.each{ |func_head|
      func_type = func_head.get_type
      func_name = func_head.get_name
      if func_type.get_type.kind_of?( VoidType ) then
          retval_assign = ""
      else
          retval_assign = "retval = (ER_UINT)"
      end
      
      params = func_head.get_paramlist.get_items
      i = 0
      passed_param = {}
      params.each{ |param|
          passed_param[i] = param.get_name
          i += 1
      }

      while(i < NUM_SVC_ARG_MAX) do
          passed_param[i] = "par#{i+1}"
          i += 1
      end

      file2.print <<EOT
  /* HRPSVC0006 */
  ER_UINT
  #{@ct_name_body}_#{func_name}(CELLCB *p_cellcb, intptr_t #{passed_param[0]}, intptr_t #{passed_param[1]}, intptr_t #{passed_param[2]}, intptr_t #{passed_param[3]}, intptr_t #{passed_param[4]}, ID cdmid)
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
      # generated_check_code = gen_caller_check_code(func_name)
      # check_code = generated_check_code["check_code"]
      # user_cannot_callable = generated_check_code["user_cannot_callable"]
      check_code = ""
      user_cannot_callable = false

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
          check_code = "    return E_OACV;"
      end

      # if check_code != ""
          # 
          #  呼出し元がカーネルドメインの場合，アクセス権のチェック
          #  処理をスキップさせる
          #
          file2.print <<EOT
    if (cdmid != TDOM_KERNEL) {
#{check_code}
      /* HRPSVC0009 */
      if( (TACP(cdmid) & ATTR_restrict_acptn_#{func_name}) == 0 ){
        return E_OACV;
      }
    }
EOT
      # end

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
      file2.print "  }\n\n"
    }
  end


  #---------------------------------------------------------#
  #=== シグニチャのチェック
  def check_signature signature
    if @@signature_checked[ signature ] then
      return
    end
    @@signature_checked[ signature ] = true
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

