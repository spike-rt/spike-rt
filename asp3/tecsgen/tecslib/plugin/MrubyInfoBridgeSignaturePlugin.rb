# -*- coding: utf-8 -*-
#
#  mruby => TECS bridge
#  
#   Copyright (C) 2008-2017 by TOPPERS Project
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
#   $Id: MrubyInfoBridgeSignaturePlugin.rb 3073 2019-05-10 23:18:01Z okuma-top $
#

class MrubyInfoBridgeSignaturePlugin < SignaturePlugin

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
      STDERR << "MrubyInfoBridgeSignaturePlugin: version 2.0.0 (Suitable for mruby above ver 1.2.0). \n"
      @@b_no_banner = true
    end

    @b_ignoreUnsigned = false
    @includes = []             # function name list
    @excludes = []             # function name list
    @struct_list = { }
    @ptr_list = { }
    @auto_exclude_list = {}    # function name list
    @b_auto_exclude = true     # auto_exclude = true by default 
    @b_refused_signature = false  # exclude TECSInfo & mruby: sInitializeBridge, sInitializeTECSBridge

    @plugin_arg_check_proc_tab = MrubyBridgePluginArgProc
    parse_plugin_arg

    @celltype_name = :"tInfo#{@signature.get_global_name}"
    @init_celltype_name = :"#{@celltype_name}_Initializer"
        # this variable is sometimes not used. rhs coded directry.
    @class_name = :"Info#{@signature.get_global_name}"

    @func_head_array = []
    fh_array = []
    if @includes.length > 0 && @excludes.length > 0 then
      cdl_error( "MRB1011 both include && exclude are specified" )
    end

    sig_path = signature.get_namespace_path.to_s
    case sig_path
    when "::nTECSInfo::sAccessor"
      # accept
    when "::nMruby::sInitializeBridge", "::nMruby::sInitializeTECSBridge"
      # refuse
      @b_refused_signature = true
      return
    when /\A::nTECSInfo::/
      # refuse
      @b_refused_signature = true
      return
    else
      # accept
    end
    
    if signature.get_function_head_array == nil then
      return   # 以前に文法エラー発生
    end

    signature.get_function_head_array.each{ |func_head|
      if @includes.length > 0 then
        if @includes.index func_head.get_name then
          dbgPrint "MrubyInfoBridgePlugin: #{func_head.get_name} INCLUDED\n"
          fh_array << func_head
        else
          dbgPrint "MrubyInfoBridgePlugin: #{func_head.get_name} NOT included\n"
        end
      elsif @excludes.length > 0 then
        if @excludes.index( func_head.get_name ) == nil then
          dbgPrint "MrubyInfoBridgePlugin: #{func_head.get_name} NOT excluded\n"
          fh_array << func_head
        else
          dbgPrint "MrubyInfoBridgePlugin: #{func_head.get_name} EXCLUDED\n"
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
        dbgPrint "MrubyInfoBridgePlugin: auto_exclude #{fh.get_name}\n"
      end
    }

    if @func_head_array.length == 0 then
      # cdl_error( "MRB1012 '$1' no function remained by exclude", @signature.get_name )
      cdl_warning( "MRB1012 '$1' no function remained by exclude", @signature.get_name )
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
      # print_msg "  MrubyInfoBridgePlugin: [struct]   #{struct_type.get_type_str} => class TECS::Struct#{sttype.get_name}\n"
      print "  MrubyInfoBridgePlugin: [struct]   #{struct_type.get_type_str} => [class] TECS::Struct#{sttype.get_name}\n"
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
      # print_msg "  MrubyInfoBridgePlugin: [pointer]  #{ttype.get_type_str}* => class TECS::#{tment[1]}Pointer\n"
      print "  MrubyInfoBridgePlugin: [pointer]  #{ttype.get_type_str}* => [class] TECS::#{tment[1]}Pointer\n"
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
      tstr = tstr.sub( /unsigned /, "" )             # unsigned も無視
      tstr = tstr.sub( /uint/, "int" )               # int の前置 u も無視
      tstr = tstr.sub( /[cs]char/, "char" )          # char の前置 c, s も無視
    end
    return @@TYPE_MAP[ tstr.to_sym ]
  end

  #===  CDL ファイルの生成
  #      typedef, signature, celltype, cell コードを生成
  #file::        FILE       生成するファイル
  def gen_cdl_file(file)

    if @b_refused_signature then
      return
    end

    # ブリッジセルタイプの生成
    if @@celltypes[ @celltype_name ] == nil then
      @@celltypes[ @celltype_name ] = [ self ]
      @@init_celltypes[ @init_celltype_name ] = true
      # if @signature.get_namespace_path.to_s =~ /nMruby::/ then
      #   return
      # end
      print_msg <<EOT
  MrubyInfoBridgePlugin: [signature] #{@signature.get_namespace_path} => [class] TECS::#{@class_name}
EOT

      file.print <<EOT
 /*
  * MrubyInfoBridgeSignaturePlugin:
  *     signature=#{@signature.get_namespace_path}
  *
  *   => celltype=nMrubyInfo::#{@celltype_name}
  *      (bridge cell 's celltype; generated in this file)
  *      cell nMrubyInfo::#{@celltype_name} BridgeCellName { cTECS = CellName.eEntry; };
  *        where eEntry's signature must be #{@signature.get_namespace_path}.
  *      => class=TECS::#{@class_name}
  *         (mruby’s class; accessible from your script)
  *          bridge = TECS::#{@class_name}.new("BridgeCellName")
  */
import( <mruby.cdl> );

/****  Ruby => TECS Bridge Celltype (MBP500) ****/
namespace nMrubyInfo{
    // bridge celltype
    [singleton, idx_is_id,active]   // not actually active, to avoid warning W1002, W1007
    celltype #{@celltype_name} {
        [dynamic,optional]
            call #{@signature.get_namespace_path.to_s} cTECS;
        call nTECSInfo::sTECSInfo cTECSInfo;
        [dynamic,optional]
            call nTECSInfo::sEntryInfo cEntryInfo;
        [dynamic,optional]
            call nTECSInfo::sSignatureInfo cSignatureInfo;
        [dynamic,optional]
            call nTECSInfo::sRawEntryDescriptorInfo cRawEntryDescriptorInfo;
        attr {
            [omit]
            char_t *VMname = "VM";
            [omit]
            char_t *bridgeName = C_EXP( "$cell$" );
        };
    };
    // bridge initializer celltype
    celltype #{@init_celltype_name} {
        entry nMruby::sInitializeTECSBridge eInitialize;
    };
};

// Bridge Cell
cell nMrubyInfo::#{@celltype_name} MrubyInfoBridge_#{@signature.get_global_name} {
  // cTECS = Sample.eEnt;
  cTECSInfo = TECSInfo.eTECSInfo;
  // bridgeName = "Simple";
};
EOT

      # 構造体セルタイプの生成
      @struct_list.each{ |name, sttype|
        if @@struct_list[ name ] == nil then
          file.print <<EOT
namespace nMruby{
    [singleton]
    celltype #{name} {
        entry nMruby::sInitializeTECSBridge eInitialize;
    };
};
EOT
          @@struct_list[ name ] = sttype
        end
      }

    else
      cdl_info( "MRBW001 MrubyInfoBridgePlugin: signature '$1' duplicate. ignored current one", @signature.get_namespace_path )
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
      print_msg "  MrubyInfoBridgePlugin: join your VM's cInitialize to #{vm_name}_TECSInitializer.eInitialize\n"

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

    file.print "\n  // MrubyInfoBridgeSignaturePlugin: MBP601\n"
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
      raise "MrubyInfoBridgeSignaturePlugin: are0"
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
      raise "MrubyInfoBridgeSignaturePlugin: Unknown #{ct_name}"
    end
  end

  def gen_ep_func_body_bridge( file, b_singleton, ct_name, global_ct_name, sig_name, ep_name, func_name, func_global_name, func_type, params )
    raise "MrubyInfoBridgeSignaturePlugin: unexpected "
  end

  def gen_ep_func_body_bridge_init( file, b_singleton, ct_name, global_ct_name, sig_name, ep_name, func_name, func_global_name, func_type, params )
    file.print <<EOT
    // CELLCB *p_cellcb = GET_CELLCB( idx );  /* no error check */     /* MBP700 */
    struct RClass	*rc;

    rc = mrb_define_class_under( mrb, TECS, \"#{@class_name}\", mrb->object_class );
    mrb_define_method( mrb, rc, "initialize", MrubyInfoBridge_#{@celltype_name}_initialize, MRB_ARGS_REQ(1) );
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
          raise "MrubyInfoBridgeSignaturePlugin: send, receive"
        end
      }
      if n_param > 0 then
        p_str = "MRB_ARGS_REQ( #{n_param} )"
      else
        p_str = "MRB_ARGS_NONE()"
      end
      file.print <<EOT
    mrb_define_method( mrb, rc, "#{func_name}", MrubyInfoBridge_#{@celltype_name}_#{func_name}, #{p_str} );
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
      raise "MrubyInfoBridgeSignaturePlugin: MrubyInfoBridgeSignaturePlugin: Unknown #{ct_name}"
    end
  end

  def gen_preamble_mruby( file, b_singleton, ct_name, global_ct_name )
    file.print <<EOT
/* MBP: MrubyInfoBridgePlugin: MBP000 */
#include "mruby.h"
#include "mruby/class.h"
#include "mruby/data.h"
#include "mruby/string.h"
#include "TECSPointer.h"
#include "TECSStruct.h"
#include "t_syslog.h"
#include "stdlib.h"

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
    nsp = NamespacePath.new( :nMrubyInfo, true )
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
    Descriptor( #{@signature.get_global_name} ) desc;
};

#ifndef MRUBYINFOBRIDGE_NAME_LEN
#define MRUBYINFOBRIDGE_NAME_LEN  256
#endif
EOT
  end

  def gen_preamble_instance_proto( file, b_singleton, ct_name, global_ct_name )
    file.print <<EOT
//  Prototype MBP400
mrb_value  MrubyInfoBridge_#{@celltype_name}_initialize( mrb_state *mrb, mrb_value self);
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
mrb_value  MrubyInfoBridge_#{@celltype_name}_#{func_name}( mrb_state *mrb, mrb_value self );
EOT
    }
  end

  def gen_preamble_instance_initialize( file, b_singleton, ct_name, global_ct_name )
    file.print <<EOT

/* MBP100 */
mrb_value
MrubyInfoBridge_#{@celltype_name}_initialize( mrb_state *mrb, mrb_value self)
{
    mrb_value	name;
    struct tecs_#{@celltype_name} *tecs_desc;
    Descriptor( nTECSInfo_sRawEntryDescriptorInfo ) rawEntryDescDesc;
    Descriptor( nTECSInfo_sEntryInfo ) entryDesc;
    Descriptor( nTECSInfo_sSignatureInfo ) signatureDesc;
    Descriptor( #{@signature.get_global_name} ) cellEntryDesc;
    char_t   tecs_name[ MRUBYINFOBRIDGE_NAME_LEN ];
    void     *rawDesc;
    char     *p;
    int      subsc;

    /* set DATA_TYPE earlier to avoid SEGV */
    DATA_TYPE( self ) = &data_type_#{@celltype_name};

    mrb_get_args(mrb, "o", &name );
    if( mrb_type( name ) != MRB_TT_STRING ){
        mrb_raise(mrb, E_NAME_ERROR, "cell name not string");
    }
    if( cTECSInfo_findRawEntryDescriptor( RSTRING_PTR( name ), &rawEntryDescDesc, &entryDesc ) != E_OK ){
         mrb_raise( mrb, E_ARGUMENT_ERROR, "MrubyInfoBridgeSignaturePlugin: path not found" );
    }
    else{
#ifdef MRUBYINFOBRIDGEPLUGIN_VERBOSE
         syslog( LOG_NOTICE, "MrubyInfoBridgePlugin: %s: found", RSTRING_PTR( name ) );
#endif /* MRUBYINFOBRIDGEPLUGIN_VERBOSE */
    }

    // retrieve Signature Name from EntryInfo to check
    cEntryInfo_set_descriptor( entryDesc );
    cEntryInfo_getSignatureInfo( &signatureDesc );
    cSignatureInfo_set_descriptor( signatureDesc );
    cSignatureInfo_getName( tecs_name, MRUBYINFOBRIDGE_NAME_LEN );
    if( strncmp( tecs_name, "#{@signature.get_name}", MRUBYINFOBRIDGE_NAME_LEN ) != 0 ){
         mrb_raise( mrb, E_ARGUMENT_ERROR, "MrubyInfoBridgeSignaturePlugin: signature name mismatch" );
    }
    else{
#ifdef MRUBYINFOBRIDGEPLUGIN_VERBOSE
         syslog( LOG_NOTICE, "MrubyInfoBridgePlugin: signature name matched: %s", "#{@signature.get_name}" );
#endif /* MRUBYINFOBRIDGEPLUGIN_VERBOSE */
    }

    /* retrieve entry array suscript if exist */
    for( p = RSTRING_PTR( name ), subsc = 0; *p != 0; p++ ){
        if( *p == '[' ){
            p++;
            subsc = atoi( p );
            break;
        }
    }

    // retrieve Cell's Entry Descriptor
    cRawEntryDescriptorInfo_set_descriptor( rawEntryDescDesc );
    if( cRawEntryDescriptorInfo_getRawDescriptor( subsc, &rawDesc ) != E_OK ){
         mrb_raise( mrb, E_ARGUMENT_ERROR, "MrubyInfoBridgeSignaturePlugin: cannot get RawDescriptor (maybe subscript out of range)" );
    }
    else{
#ifdef MRUBYINFOBRIDGEPLUGIN_VERBOSE
         syslog( LOG_NOTICE, "MrubyInfoBridgePlugin: got RawDescriptor: %s %d", "#{@signature.get_name}", subsc );
#endif /* MRUBYINFOBRIDGEPLUGIN_VERBOSE */
    }
    cellEntryDesc.vdes = (struct tag_#{@signature.get_global_name}_VDES *)rawDesc;
  
    tecs_desc = (struct tecs_#{@celltype_name} *)mrb_malloc(mrb, sizeof(struct tecs_#{@celltype_name}) );
    tecs_desc->desc = cellEntryDesc;
    DATA_PTR( self ) = (void *)tecs_desc;

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
MrubyInfoBridge_#{ct_name}_#{func_name}( mrb_state *mrb, mrb_value self )
{
  /* cellcbp (MBP105) */
  // CELLCB	*p_cellcb = ((struct tecs_#{@celltype_name} *)DATA_PTR(self))->cbp;
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
            raise "MrubyInfoBridgeSignaturePlugin: Unknown type"
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
              raise "MrubyInfoBridgeSignaturePlugin: Unknown type"
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
                raise "MrubyInfoBridgeSignaturePlugin: cannot handle type"
              end
            when StructType
              file.print "	CHECK_STRUCT( #{type.get_name}, mrb_#{param.get_name} );\n"
              file.print "	#{param.get_name} = (struct #{type.get_name}*)DATA_PTR(mrb_#{param.get_name});\n"
            else
              raise( "MrubyInfoBridgeSignaturePlugin: canot treat class" )
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
                raise "MrubyInfoBridgeSignaturePlugin: cannot handle type"
              end
            when StructType
            else
              raise( "MrubyInfoBridgeSignaturePlugin: canot treat class" )
            end
          end
        }

      end

      file.print <<EOT
	/* calling target (MBP120) */
  cTECS_set_descriptor( ((struct tecs_#{@celltype_name} *)DATA_PTR(self))->desc );
EOT
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
        raise "MrubyInfoBridgeSignaturePlugin: unknown type"
      end

      file.print "}\n"
    }
  end
end
