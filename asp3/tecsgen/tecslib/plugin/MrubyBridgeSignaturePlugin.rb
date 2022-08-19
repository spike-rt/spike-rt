# -*- coding: utf-8 -*-
#
#  mruby => TECS bridge
#  
#   Copyright (C) 2008-2019 by TOPPERS Project
#
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
#   $Id: MrubyBridgeSignaturePlugin.rb 3225 2021-09-26 05:08:38Z okuma-top $
#

# Todo:
# 生成するもの
#  gen_cdl_file
#    ブリッジ初期化セルタイプ (シグニチャごと)
#       @@init_celltypes に記録
#    ブリッジセルタイプ (シグニチャごと)
#       @@celltypes に記録
#    構造体セルタイプ
#       @@struct_list に記録
#  new_cell
#    TECS 初期化セル(プロトタイプ宣言)
#       @@VM_init_cells に記録
#       @@VM_struct_list に記録
#       @@VM_ptr_list に記録
#  gen_post_code
#    構造体初期化セル
#    ポインタ初期化セル
#    TECS 初期化セル
#
#   ep_func の作成
#     signature, ポインタ、構造体 … 初期化受け口＝mruby VM への登録
#     ポインタ
#   preamble の作成
#     ポインタ、構造体 … アクセス用コードの生成
#     signature … ブリッジ関数のプロトタイプ宣言
#   postamble の作成
#     signature … ブリッジ関数の定義

class MrubyBridgeSignaturePlugin < SignaturePlugin

  # require_tecsgen_lib 'tecslib/plugin/lib/MrubyBridgeSignaturePlugin.rb'
  require_tecsgen_lib 'lib/MrubyBridgeSignaturePluginModule.rb'
  include MrubyBridgeSignaturePluginModule

  @@b_no_banner = false         #

  # included  or excluded functions

  ### ロードされた時点で実行される ###

  # -I に $(TECSPATH)/mruby を追加
  # TECSGEN::Makefile.add_obj "$(MRUBY_MAIN_OBJ)"
  TECSGEN::Makefile.add_ldflag "-lmruby -L$(MRUBYPATH)/lib -lm"
  TECSGEN::Makefile.add_search_path "$(MRUBYPATH)/include"
  TECSGEN::Makefile.add_var "MRUBYPATH",      "..",  "CHANGE this to suitable path"
  # TECSGEN::Makefile.add_var "MRUBY_MAIN_OBJ", "$(_TECS_OBJ_DIR)tecs_mruby.o", "CHANGE this if your have your main"

  #=== プラグインインスタンスの初期化
  # 戻り値、引数の型が使用可能なものかチェックする
  #
  def initialize( signature, option )
    super

    if ! @@b_no_banner
      STDERR << "MrubyBridgePlugin: version 2.0.0 (Suitable for mruby above ver 1.2.0).\n"
      @@b_no_banner = true
    end

    @b_ignoreUnsigned = false
    @includes = []
    @excludes = []
    @struct_list = { }
    @ptr_list = { }
    @auto_exclude_list = {}
    @b_auto_exclude = true     # auto_exclude = true by default 

    @plugin_arg_check_proc_tab = MrubyBridgePluginArgProc
    parse_plugin_arg

    @celltype_name = :"t#{@signature.get_global_name}"
    @init_celltype_name = :"#{@celltype_name}_Initializer"
        # this variable is sometimes not used. rhs coded directry.
    @class_name = :"T#{@signature.get_global_name}"

    @func_head_array = []
    fh_array = []
    if @includes.length > 0 && @excludes.length > 0 then
      cdl_error( "MRB1011 both include && exclude are specified" )
    end

    if signature.get_function_head_array == nil then
      return   # 以前に文法エラー発生
    end

    signature.get_function_head_array.each{ |func_head|
      if @includes.length > 0 then
        if @includes.index func_head.get_name then
          dbgPrint "MrubyBridgePlugin: #{func_head.get_name} INCLUDED\n"
          fh_array << func_head
        else
          dbgPrint "MrubyBridgePlugin: #{func_head.get_name} NOT included\n"
        end
      elsif @excludes.length > 0 then
        if @excludes.index( func_head.get_name ) == nil then
          dbgPrint "MrubyBridgePlugin: #{func_head.get_name} NOT excluded\n"
          fh_array << func_head
        else
          dbgPrint "MrubyBridgePlugin: #{func_head.get_name} EXCLUDED\n"
        end
      else
        fh_array << func_head
      end
    }

    check_name_and_return_type fh_array
    check_parameter_type fh_array

    fh_array.each{ |fh|
      if @auto_exclude_list[ fh.get_name ] == nil then
        @func_head_array << fh
      else
        dbgPrint "MrubyBridgePlugin: auto_exclude #{fh.get_name}\n"
      end
    }

    if @func_head_array.length == 0 then
      cdl_error( "MRB1012 '$1' no function remained by exclude", @signature.get_name )
    end
  end

  #=== check function name & return type
  def check_name_and_return_type func_head_array
    b_init = false; b_init_cell = false
    func_head_array.each{ |func_head|
      if( func_head.get_name == :initialize )then
        cdl_warning( "MRW2001 initialize: internally defined. change to initialize_cell in ruby" )
        b_init = true
      elsif( func_head.get_name == :initialize_cell )then
        b_init_cell = true
      end
      rtype = func_head.get_return_type.get_original_type
      case rtype
      when BoolType, IntType, FloatType, VoidType
      else
        if @b_auto_exclude then
          cdl_info( "MRI0001 cannot return type $1, $2 automatcally excluded", rtype.get_type_str, func_head.get_name )
          @auto_exclude_list[ func_head.get_name ] = func_head
        else
          cdl_error( "MRB1001 cannot return type $1", rtype.get_type_str )
        end
      end
    }
    if( b_init && b_init_cell )then
      cdl_warning( "MRB1002 initialize: internally defined. change to initialize_cell in ruby" )
    end
  end

  #=== check paramter type
  def check_parameter_type func_head_array
    # check type of parameters
    func_head_array.each{ |fh|
      fh.get_paramlist.get_items.each{ |param_decl|
        case param_decl.get_direction
        when :SEND, :RECEIVE
          if @b_auto_exclude then
            cdl_info( "MRI0002 $1: $2 parameter cannot be used in mruby Bridge, $3 automatcally excluded",
                      param_decl.get_name, param_decl.get_direction.to_s.downcase, fh.get_name )
            @auto_exclude_list[ fh.get_name ] = fh
          else
            cdl_error( "MRB1003 $1: $2 parameter cannot be used in mruby Bridge",  param_decl.get_name, param_decl.get_direction.to_s.downcase )
          end
        end
        type = param_decl.get_type
        type_org = type.get_original_type
        type_str = type.get_type_str + type.get_type_str_post

        b_ng = false
        case type_org
        when IntType
          case  type_org.get_bit_size
          when 8, 16, 32, 64
          when -1, -2, -3, -4, -11
          else
            b_ng = true
          end
        when BoolType
        when FloatType
        when PtrType
          ttype_org = type_org.get_type       # ポインタの指している先の型
          ttype = ttype_org.get_original_type # 上記の typedef されている場合、元の型
          register_ptr_type ttype_org, fh

          if( type_org.get_string.to_s == "-1" ) then
            case param_decl.get_direction
            when :OUT, :INOUT
              if @b_auto_exclude then
                cdl_info( "MRB9999 string specifier without length cannot be used for out & inout parameter, $1 automatcally excluded", 
                        fh.get_name )
                @auto_exclude_list[ fh.get_name ] = fh
              else
                cdl_error( "MRB9999 string specifier without length cannot be used for out & inout parameter")
              end
            end
          end

          case ttype
          when IntType
            bit_size = ttype.get_bit_size
            # if  bit_size < 0 && bit_size != -1 then
            #   b_ng = true
            # end
          when FloatType
          when BoolType
          when StructType
            if( type_org.get_size || type_org.get_string || type_org.get_count ) then
              if @b_auto_exclude then
                cdl_info( "MRI9999 $1: size_is, count_is, string cannot be specified for struct pointer, $2 automatcally excluded",
                          param_decl.get_name, fh.get_name )
                @auto_exclude_list[ fh.get_name ] = fh
              else
                cdl_error( "MRB1004 $1: size_is, count_is, string cannot be specified for struct pointer", param_decl.get_name )
              end
            end
            check_struct_member ttype_org, fh
          else
            b_ng = true
          end
        when StructType
          check_struct_member type_org, fh
        else  # ArrayType, FuncType, EnumType, VoidType
          b_ng = true
        end
        if b_ng then
          if @b_auto_exclude then
            cdl_info( "MRI9999 $1: type $2 cannot be used in mruby Bridge, $3 automatcally excluded",
                      param_decl.get_name, type_str, fh.get_name )
            @auto_exclude_list[ fh.get_name ] = fh
          else
            cdl_error( "MRB1005 $1: type $2 cannot be used in mruby Bridge", param_decl.get_name, type_str )
          end
        end
      }
    }
  end

  #=== 構造体のメンバーの型のチェック
  def check_struct_member struct_type, fh
    #p "tag name:#{struct_type.get_name}"
    # sttype = Namespace.find_tag( struct_type.get_name )
    sttype = struct_type.get_original_type
    if sttype.get_name == nil then
      if @b_auto_exclude then
        cdl_info( "MRI9999 tagless-struct cannot be handled, $1 automatcally excluded", fh.get_name )
        @auto_exclude_list[ fh.get_name ] = fh
        return  # 登録しないように打ち切る
      else
        cdl_error( "MRB10007 tagless-struct cannot be handled")
      end
    end
    sttype.get_members_decl.get_items.each { |d|
      t = d.get_type.get_original_type
      case t
      when IntType, FloatType, BoolType
      else
        if @b_auto_exclude then
          cdl_info( "MRI9999 $1: type '$2' not allowed for struct member, '$3' automatcally excluded",
                    d.get_name, d.get_type.get_type_str + d.get_type.get_type_str_post, fh.get_name )
          @auto_exclude_list[ fh.get_name ] = fh
          return  # 登録しないように打ち切る
        else
          cdl_error( "MRB1006 $1: type '$2' not allowed for struct member", d.get_name, d.get_type.get_type_str + d.get_type.get_type_str_post )
        end
      end
    }
    st_name = :"t{}"
    if @struct_list[ sttype.get_name ] == nil then
      # print_msg "  MrubyBridgePlugin: [struct]   #{struct_type.get_type_str} => class TECS::Struct#{sttype.get_name}\n"
      print "  MrubyBridgePlugin: [struct]   #{struct_type.get_type_str} => [class] TECS::Struct#{sttype.get_name}\n"
      @struct_list[ sttype.get_name ] = sttype
    end
  end

  def register_ptr_type ttype, fh
    t_org = ttype.get_original_type
    tment = get_type_map_ent t_org
    if tment == nil then
      return
      # cdl_error( "MRB1008 unknown pointer type '$1'", ttype.get_type_str )
    end
    ptr_celltype_name = :"t#{tment[1]}Pointer"
    if @@ptr_list[ ptr_celltype_name ] == nil then
      # print_msg "  MrubyBridgePlugin: [pointer]  #{ttype.get_type_str}* => class TECS::#{tment[1]}Pointer\n"
      print "  MrubyBridgePlugin: [pointer]  #{ttype.get_type_str}* => [class] TECS::#{tment[1]}Pointer\n"
      @@ptr_list[ ptr_celltype_name ] = tment
    end
    if @ptr_list[ ptr_celltype_name ] == nil then
      @ptr_list[ ptr_celltype_name ] = tment
    end
  end

  def get_type_map_ent ttype
    # structure type is registerd in check_struct_member
    if ttype.kind_of? StructType
      return
    end
    tstr = ttype.get_type_str.sub( /const /, "" )    # const は無視
    tstr = tstr.sub( /volatile /, "" )               # volatile も無視
    if @b_ignoreUnsigned then
      tstr = tstr.sub( /unsigned /, "" )             # volatile も無視
      tstr = tstr.sub( /uint/, "int" )               # volatile も無視
      tstr = tstr.sub( /[cs]char/, "char" )          # volatile も無視
    end
    return @@TYPE_MAP[ tstr.to_sym ]
  end

  #===  CDL ファイルの生成
  #      typedef, signature, celltype, cell コードを生成
  #file::        FILE       生成するファイル
  def gen_cdl_file(file)

    # ブリッジセルタイプの生成
    if @@celltypes[ @celltype_name ] == nil then
      @@celltypes[ @celltype_name ] = [ self ]
      @@init_celltypes[ @init_celltype_name ] = true
      print_msg <<EOT
  MrubyBridgePlugin: [signature] #{@signature.get_namespace_path} => [celltype] nMruby::#{@celltype_name} => [class] TECS::#{@class_name}
EOT

      file.print <<EOT
/*
 * MrubyBridgeSignaturePlugin:
 *     signature=#{@signature.get_namespace_path}
 *
 *   => celltype=nMruby::#{@celltype_name}
 *      (bridge cell's celltype; generated in this file)
 *      cell nMruby::#{@celltype_name} BridgeCellName { cTECS = CellName.eEntry; };
 *        where eEntry's signature must be #{@signature.get_namespace_path}.
 *      => class=TECS::#{@class_name}
 *         (mruby's class; accessible from your script)
 *          bridge = TECS::#{@class_name}.new("BridgeCellName")
 */
import( <mruby.cdl> );

/****  Ruby => TECS Bridge Celltype (MBP500) ****/
namespace nMruby{
    // bridge celltype
    [idx_is_id,active]   // not actually active, to avoid warning W1002, W1007
    celltype #{@celltype_name} {
        call #{@signature.get_namespace_path.to_s} cTECS;
        attr {
            [omit]
            char_t *VMname = "VM";
            [omit]
            char_t *bridgeName = C_EXP( "$cell$" );
        };
    };
    // bridge initializer celltype
    celltype #{@init_celltype_name} {
        entry sInitializeTECSBridge eInitialize;
    };
};
EOT

      # 構造体セルタイプの生成
      @struct_list.each{ |name, sttype|
        if @@struct_list[ name ] == nil then
          file.print <<EOT
namespace nMruby{
    [singleton]
    celltype #{name} {
        entry sInitializeTECSBridge eInitialize;
    };
};
EOT
          @@struct_list[ name ] = sttype
        end
      }

    else
      cdl_info( "MRBW001 MrubyBridgePlugin: signature '$1' duplicate. ignored current one", @signature.get_namespace_path )
      @@celltypes[ @celltype_name ] << self
    end
  end

  #=== gen_cdl_file で定義したセルタイプに 新しいセルが定義された
  # cell のセルタイプの名前は @celltype_name
  def new_cell cell
    if cell.get_celltype.get_name != @celltype_name then
      return
    end

    join = cell.get_join_list.get_item :VMname
    if join then
      vm_name = CDLString.remove_dquote(join.get_rhs.to_s).to_sym
    else
      vm_name = :"VM"
    end

    if @@VM_list[ vm_name ] == nil then
      @@VM_list[ vm_name ] = true

      initializer_celltype_cdl = "#{$gen}/#{cell.get_name}Initializer.cdl"
      file = CFile.open( initializer_celltype_cdl, "w" )

      # TECS 初期化セル(プロトタイプ宣言)
      print_msg "  MrubyBridgePlugin: join your VM's cInitialize to #{vm_name}_TECSInitializer.eInitialize\n"

      file.print <<EOT

  // prototype of TECSInitializer (MBP510)
  cell nMruby::tTECSInitializer #{vm_name}_TECSInitializer;
EOT
      file.close

      Generator.parse( initializer_celltype_cdl, self )
    end

    if @@VM_celltypes[ vm_name ] then
      vma = @@VM_celltypes[ vm_name ]

      if vma[ @celltype_name ] then
        vma[ @celltype_name ] << cell
      else
        vma[ @celltype_name ] = [cell]
        @@VM_celltypes[ vm_name ] = vma
      end
    else
      vma = { }
      vma[ @celltype_name ] = [cell]
      @@VM_celltypes[ vm_name ] = vma
    end

    @struct_list.each{ |stname, sttype|
      if @@VM_struct_list[ vm_name ] then
        @@VM_struct_list[ vm_name ][ sttype.get_name ] = sttype
      else
        @@VM_struct_list[ vm_name ] = { sttype.get_name => sttype }
      end
    }
    @ptr_list.each{ |ptr_celltype_name, tment|
      if @@VM_ptr_list[ vm_name ] then
        @@VM_ptr_list[ vm_name ][ ptr_celltype_name ] = tment
      else
        @@VM_ptr_list[ vm_name ] = { ptr_celltype_name => tment }
      end
    }

  end

  #=== プラグインが CDL の POST コードを生成
  # tmp_plugin_post_code.cdl への出力
  def self.gen_post_code file
    dbgPrint "#{self.name}: gen_post_code\n"
    gen_post_code_body file
  end

  def self.gen_post_code_body file
    if @@b_post_coded == false then
      @@b_post_coded = true
    else
      return
    end
    dbgPrint "#{self.name}: gen_post_code_body\n"

#     file.print <<EOT
# namespace nMruby {
# EOT
# 
#     @@ptr_list.each{ |name,tment|
#       file.print <<EOT
# 
#     // MBP600
#     [singleton]
#     celltype #{name} {
#         entry  sInitializeTECSBridge eInitialize;
#     };
# EOT
#     }
# 
#     file.print <<EOT
# };
# EOT

# gen_post_code で生成した celltype は gen_ep_func が呼び出されない #847
#    @@struct_list.each{ |name,sttype|
#      file.print <<EOT
#
#    [singleton]
#    celltype #{name} {
#        entry  sInitializeTECSBridge eInitialize;
#    };
#EOT
#    }

    file.print "\n  // MrubyBridgeSignaturePlugin: MBP601\n"
    @@VM_celltypes.each{ |vm_name, instance_list|
      instance_list.each { |celltype_name, array|
        cell = array[0]
	      if cell.get_celltype then
	        ct_name = cell.get_celltype.get_name
          if ct_name =~ /\AtInfo/ then
            info = "Info"
          else
            info = ""
          end
          # print "celltype=#{ct_name}\n"
          file.print <<EOT
  cell nMruby#{info}::#{ct_name}_Initializer #{vm_name}_#{ct_name}_Initializer{ };
EOT
        end
      }
    }

    file.print "  // MBP602\n"
    @@ptr_list.each{ |name,tment|
      file.print <<EOT
  cell nMruby::#{name} C#{name} { };
EOT
    }

    file.print "  // MBP603\n"
    @@struct_list.each{ |name,sttype|
      file.print <<EOT
  cell nMruby::#{name} C#{name} { };
EOT
    }

    if @@VM_celltypes == nil
      raise "MrubyBridgeSignaturePlugin: are0"
    end
    @@VM_celltypes.each{ |vm_name, instance_list|
      file.print "  /* === VM name is '#{vm_name}' === (MBP610) */\n"
      init_cell_name = "#{vm_name}_TECSInitializer"

      file.print "  cell nMruby::tTECSInitializer #{init_cell_name} {\n"

      instance_list.each { |celltype_name, array|
          ct_name = celltype_name
          file.print "    cInitialize[] = #{vm_name}_#{ct_name}_Initializer.eInitialize;\n"
      }
      if @@VM_ptr_list[vm_name] then
        @@VM_ptr_list[vm_name].each{ |name, tment|
          file.print "    cInitialize[] = C#{name}.eInitialize;\n"
        }
      end
      if @@VM_struct_list[vm_name] then
        @@VM_struct_list[vm_name].each{ |name, sttype|
          file.print "    cInitialize[] = C#{name}.eInitialize;\n"
        }
      end
      file.print "  };"
    }
    
  end

  ####### 以下コード生成段階 ######

  #===  受け口関数の本体コードを生成（頭部と末尾は別途出力）
  #ct_name:: Symbol    (プラグインで生成された) セルタイプ名 ．Symbol として送られてくる
  def gen_ep_func_body( file, b_singleton, ct_name, global_ct_name, sig_name, ep_name, func_name, func_global_name, func_type, params )
    if @@celltypes[ ct_name ] then
      gen_ep_func_body_bridge( file, b_singleton, ct_name, global_ct_name, sig_name, ep_name, func_name, func_global_name, func_type, params )
    elsif @@init_celltypes[ ct_name ] then
      gen_ep_func_body_bridge_init( file, b_singleton, ct_name, global_ct_name, sig_name, ep_name, func_name, func_global_name, func_type, params )
    elsif @@ptr_list[ ct_name ] then
      gen_ep_func_body_ptr( file, b_singleton, ct_name, global_ct_name, sig_name, ep_name, func_name, func_global_name, func_type, params )
    elsif @@struct_list[ ct_name ] then
      gen_ep_func_body_struct( file, b_singleton, ct_name, global_ct_name, sig_name, ep_name, func_name, func_global_name, func_type, params )
    else
      raise "MrubyBridgeSignaturePlugin: Unknown #{ct_name}"
    end
  end

  def gen_ep_func_body_bridge( file, b_singleton, ct_name, global_ct_name, sig_name, ep_name, func_name, func_global_name, func_type, params )
    raise "MrubyBridgeSignaturePlugin: unexpected "
  end

  def gen_ep_func_body_bridge_init( file, b_singleton, ct_name, global_ct_name, sig_name, ep_name, func_name, func_global_name, func_type, params )
    file.print <<EOT
    // CELLCB *p_cellcb = GET_CELLCB( idx );  /* no error check */     /* MBP700 */
    struct RClass	*rc;

    rc = mrb_define_class_under( mrb, TECS, \"#{@class_name}\", mrb->object_class );
    mrb_define_method( mrb, rc, "initialize", MrubyBridge_#{@celltype_name}_initialize, MRB_ARGS_REQ(1) );
    MRB_SET_INSTANCE_TT(rc, MRB_TT_DATA);
EOT

    @func_head_array.each{ |f|
      if ! f.is_function? then
        next
      end
      if f.get_name != :initialize then
        func_name = f.get_name
      else
        func_name = :initialize_cell
      end

      ret_type = f.get_return_type
      n_param = 0
      f.get_paramlist.get_items.each{ |param|
        case param.get_direction
        when :IN, :INOUT, :OUT
          n_param += 1
        when :SEND, :RECEIVE
          raise "MrubyBridgeSignaturePlugin: send, receive"
        end
      }
      if n_param > 0 then
        p_str = "MRB_ARGS_REQ( #{n_param} )"
      else
        p_str = "MRB_ARGS_NONE()"
      end
      file.print <<EOT
	mrb_define_method( mrb, rc, "#{func_name}", MrubyBridge_#{@celltype_name}_#{func_name}, #{p_str} );
EOT
    }
  end

  #===  受け口関数の preamble (C言語)を生成する
  #     必要なら preamble 部に出力する
  #file::           FILE        出力先ファイル
  #b_singleton::    bool        true if singleton
  #ct_name::        Symbol
  #global_ct_name:: string
  def gen_preamble( file, b_singleton, ct_name, global_ct_name )
    if @@celltypes[ ct_name ] then
      gen_preamble_mruby( file, b_singleton, ct_name, global_ct_name )
      gen_preamble_instance( file, b_singleton, ct_name, global_ct_name )
      gen_preamble_instance_initialize( file, b_singleton, ct_name, global_ct_name )
      gen_preamble_bridge_func( file, b_singleton, ct_name, global_ct_name )
    elsif @@init_celltypes[ ct_name ] then
      gen_preamble_mruby( file, b_singleton, ct_name, global_ct_name )
      gen_preamble_instance_proto( file, b_singleton, ct_name, global_ct_name )
    elsif @@ptr_list[ ct_name ] then
      gen_preamble_ptr( file, b_singleton, ct_name, global_ct_name )
    elsif @@struct_list[ ct_name ] then
      gen_preamble_struct( file, b_singleton, ct_name, global_ct_name )
    else
      raise "MrubyBridgeSignaturePlugin: MrubyBridgeSignaturePlugin: Unknown #{ct_name}"
    end
  end

  def gen_preamble_mruby( file, b_singleton, ct_name, global_ct_name )
    file.print <<EOT
/* MBP: MrubyBridgePlugin: MBP000 */
#ifndef TECSGEN
#include "mruby.h"
#include "mruby/class.h"
#include "mruby/data.h"
#include "mruby/string.h"
#endif /* TECSGEN */
#include "TECSPointer.h"
#include "TECSStruct.h"

#if defined(_WIN32) || defined(__WIN32__) || defined(__CYGWIN__)
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

#ifndef NULL
#define NULL 0
#endif
EOT
  end

  def gen_preamble_instance( file, b_singleton, ct_name, global_ct_name )
    # 「#1005  MrubyBridgePlugin-test のビルドに失敗」にて追加
    # 「#1004 idx_is_id が true の場合(domain の場合は未指定でも true) の CBP のプロトタイプ宣言」にて不要になる見込み
    nsp = NamespacePath.new( :nMruby, true )
    nsp.append! ct_name
    ct = Namespace.find nsp
    if ct.idx_is_id_act? then
      if ct.has_CB? then
        inib_cb = "CB"
      elsif ct.has_INIB? then
        inib_cb = "INIB"
      else
        inib_cb = nil
      end
      if inib_cb then
        ct.get_cell_list.each{ |cell|
          if cell.is_generate? then
            name_array = ct.get_name_array( cell )
            file.print "extern #{ct.get_global_name}_CB  #{cell.get_global_name}_#{inib_cb};\n"
          end
        }
      end
    end

    file.print <<EOT

/* RData MBP001 */
static void 
#{@celltype_name}_free( mrb_state *mrb, void *p )
{
	if( p )
		(void)mrb_free( mrb, p );
}

/* RData MBP002 */
struct mrb_data_type data_type_#{@celltype_name} =
{
	"#{@celltype_name}",
	#{@celltype_name}_free
};

/* RData MBP003 */
struct tecs_#{@celltype_name} {
    CELLCB  *cbp;
};

/* name_to_cbp MBP010 */
const struct name_to_cbp_#{@celltype_name} {
    char   *name;    /* Cell Name */
    CELLCB *cbp;
} Name_to_cbp_#{@celltype_name}[] = {
EOT

    nsp = NamespacePath.new( :nMruby, true )
    nsp.append! ct_name
    ct = Namespace.find nsp

    ct.get_cell_list.each{ |cell|
      if cell.is_generate? then
        join_list = cell.get_join_list
        join = join_list.get_item( :bridgeName )
        if join == nil then
          name = "\"#{cell.get_name}\""
        else
          rhs = join.get_rhs
          attr_list = cell.get_celltype.get_attribute_list
          name = rhs.eval_const join_list, attr_list
        end

        name_array = ct.get_name_array( cell )
        file.print "\t{ #{name}, #{name_array[8]} },\n"
      end
    }

    file.print <<EOT
\t{ 0, 0 },
};

EOT

  end

  def gen_preamble_instance_proto( file, b_singleton, ct_name, global_ct_name )
    file.print <<EOT
//  Prototype MBP400
mrb_value  MrubyBridge_#{@celltype_name}_initialize( mrb_state *mrb, mrb_value self);
EOT

    @func_head_array.each{ |f|
      if ! f.is_function? then
        next
      end
      if f.get_name != :initialize then
        func_name = f.get_name
      else
        func_name = :initialize_cell
      end

      ret_type  = f.get_return_type
      ret_type0 = f.get_return_type.get_original_type
      b_void    = ret_type0.is_void?
      plist     = f.get_paramlist.get_items

      file.print <<EOT
mrb_value  MrubyBridge_#{@celltype_name}_#{func_name}( mrb_state *mrb, mrb_value self );
EOT
    }
  end

  def gen_preamble_instance_initialize( file, b_singleton, ct_name, global_ct_name )
    file.print <<EOT

/* MBP100 */
mrb_value
MrubyBridge_#{@celltype_name}_initialize( mrb_state *mrb, mrb_value self)
{
	mrb_value	name;
	struct tecs_#{@celltype_name} *tecs_cb;
	const struct name_to_cbp_#{@celltype_name} *ntc;

	/* set DATA_TYPE earlier to avoid SEGV */
	DATA_TYPE( self ) = &data_type_#{@celltype_name};

	mrb_get_args(mrb, "o", &name );
	if( mrb_type( name ) != MRB_TT_STRING ){
		mrb_raise(mrb, E_NAME_ERROR, "cell name not string");
	}
	for( ntc = &Name_to_cbp_#{@celltype_name}[0]; ntc->name != NULL; ntc++ ){
		if( strcmp( ntc->name, RSTRING_PTR( name ) ) == 0 )
			break;
	}
	if( ntc->name == 0 ){
		mrb_raise(mrb, E_ARGUMENT_ERROR, "cell not found");
	}
	tecs_cb = (struct tecs_#{@celltype_name} *)mrb_malloc(mrb, sizeof(struct tecs_#{@celltype_name}) );
	tecs_cb->cbp = ntc->cbp;
	DATA_PTR( self ) = (void *)tecs_cb;

	return self;
}
EOT
  end

  def gen_preamble_bridge_func( file, b_singleton, ct_name, global_ct_name )

    @func_head_array.each{ |f|
      if ! f.is_function? then
        next
      end
      if f.get_name != :initialize then
        func_name = f.get_name
      else
        func_name = :initialize_cell
      end

      ret_type  = f.get_return_type
      ret_type0 = f.get_return_type.get_original_type
      b_void    = ret_type0.is_void?
      plist     = f.get_paramlist.get_items

      file.print <<EOT

/* bridge function (MBP101) */
mrb_value
MrubyBridge_#{ct_name}_#{func_name}( mrb_state *mrb, mrb_value self )
{
	/* cellcbp (MBP105) */
	CELLCB	*p_cellcb = ((struct tecs_#{@celltype_name} *)DATA_PTR(self))->cbp;
EOT
      
      file.print "	/* variables for return & parameter (MBP110) */\n"
      if ! b_void then
        file.print "	", ret_type.get_type_str, "\tret_val", ret_type.get_type_str_post, ";\n"
      end
      arg_str = ""
      n_param  = 0
      n_scalar = 0
      n_ptr    = 0
      n_struct = 0
      plist.each{ |param|
        case param.get_direction
        when :IN, :INOUT, :OUT
          type = param.get_type.get_original_type
          case type
          when IntType
            file.print "	mrb_int	mrb_", param.get_name, ";\n"
            file.print "	#{param.get_type.get_type_str}	#{param.get_name}#{param.get_type.get_type_str_post};\n"
            arg_str += "i"
            n_param += 1
            n_scalar += 1
          when FloatType
            file.print "	mrb_float	mrb_", param.get_name, ";\n"
            file.print "	#{param.get_type.get_type_str}	#{param.get_name}#{param.get_type.get_type_str_post};\n"
            arg_str += "f"
            n_param += 1
            n_scalar += 1
          when BoolType
            file.print "	mrb_value	mrb_", param.get_name, ";\n"
            file.print "	#{param.get_type.get_type_str}	#{param.get_name}#{param.get_type.get_type_str_post};\n"
            arg_str += "o"
            n_param += 1
            n_scalar += 1
          when PtrType
            file.print "	mrb_value	mrb_", param.get_name, ";\n"
            file.print "	#{param.get_type.get_type_str}	#{param.get_name}#{param.get_type.get_type_str_post};\n"
            arg_str += "o"
            n_param += 1
            n_ptr += 1
          when StructType
            file.print "	mrb_value	mrb_", param.get_name, ";\n"
            file.print "	#{param.get_type.get_type_str}	*#{param.get_name}#{param.get_type.get_type_str_post};\n"
            arg_str += "o"
            n_param += 1
            n_struct += 1
          else
            raise "MrubyBridgeSignaturePlugin: Unknown type"
          end
        end
      }

      if n_param > 0 then
        file.print "	/* retrieve arguments (MBP111) */\n"
        file.print "	mrb_get_args(mrb, \"#{arg_str}\""
        plist.each{ |param|
          case param.get_direction
          when :IN, :INOUT, :OUT
            type = param.get_type.get_original_type
            case type
            when IntType
              file.print ", &mrb_", param.get_name
            when FloatType
              file.print ", &mrb_", param.get_name
            when BoolType
              file.print ", &mrb_", param.get_name
            when PtrType
              file.print ", &mrb_", param.get_name
            when StructType
              file.print ", &mrb_", param.get_name
            else
              raise "MrubyBridgeSignaturePlugin: Unknown type"
            end
          end
        }
        file.print " );\n"

        if n_scalar > 0 || n_struct > 0 then
          file.print "	/* convert mrb to C (MBP112) */\n"
        end
        plist.each{ |param|
          case param.get_direction
          when :IN, :INOUT, :OUT
            type = param.get_type.get_original_type
            case type
            when IntType
              ttype = type.get_original_type
              tment = get_type_map_ent ttype
              file.print "	VALCHECK_#{tment[1]}( mrb, mrb_#{param.get_name} );\n"
              file.print "	#{param.get_name} = (#{param.get_type.get_type_str})mrb_#{param.get_name};\n"
            when FloatType
              file.print "	#{param.get_name} = (#{param.get_type.get_type_str})mrb_#{param.get_name};\n"
            when BoolType
              file.print "	#{param.get_name} = mrb_test( mrb_#{param.get_name} );\n"
            when PtrType
              ttype = type.get_type.get_original_type
              case ttype
              when StructType
                file.print "	CHECK_STRUCT( #{ttype.get_name}, mrb_#{param.get_name} );\n"
                file.print "	#{param.get_name} = (struct #{ttype.get_name}*)DATA_PTR(mrb_#{param.get_name});\n"
              when IntType
              when FloatType
              when BoolType
              else
                raise "MrubyBridgeSignaturePlugin: cannot handle type"
              end
            when StructType
              file.print "	CHECK_STRUCT( #{type.get_name}, mrb_#{param.get_name} );\n"
              file.print "	#{param.get_name} = (struct #{type.get_name}*)DATA_PTR(mrb_#{param.get_name});\n"
            else
              raise( "MrubyBridgeSignaturePlugin: canot treat class" )
            end
          end
        }

        if n_ptr > 0 then
          file.print "	/* convert mrb to C for pointer types (MBP113) */\n"
        end
        plist.each{ |param|
          case param.get_direction
          when :IN, :INOUT, :OUT
            type = param.get_type.get_original_type
            case type
            when IntType
            when FloatType
            when BoolType
            when PtrType
              case type.get_type.get_original_type
              when StructType
              when IntType
                ptrMrb2C file, type, param
              when FloatType
                ptrMrb2C file, type, param
              when BoolType
                ptrMrb2C file, type, param
              else
                raise "MrubyBridgeSignaturePlugin: cannot handle type"
              end
            when StructType
            else
              raise( "MrubyBridgeSignaturePlugin: canot treat class" )
            end
          end
        }

      end

      file.print "	/* calling target (MBP120) */\n"
      if ! b_void then
        file.print "	ret_val = "
      else
        file.print "	"
      end
      delim = ""
      file.print "cTECS_", f.get_name, "( "
      plist.each{ |param|
        if param.get_type.get_original_type.kind_of? StructType then
          aster = "*"
        else
          aster = ""
        end
        file.print delim, aster, param.get_name
        delim = ", "
      }
      file.print " );\n"
      
      file.print "	/* return (MBP130) */\n"
      case ret_type0
      when BoolType
        file.print "	return ret_val ? mrb_true_value() : mrb_false_value();\n"
      when IntType
        file.print "	return mrb_fixnum_value( ret_val );\n"
      when FloatType
        file.print "	return mrb_float_value( mrb, ret_val );\n"
      when VoidType
        file.print "	return  mrb_nil_value();\n"
      else
        raise "MrubyBridgeSignaturePlugin: unknown type"
      end

      file.print "}\n"
    }
  end
end

