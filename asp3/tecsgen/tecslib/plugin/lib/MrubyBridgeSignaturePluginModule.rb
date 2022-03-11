# coding: utf-8
module MrubyBridgeSignaturePluginModule

  # プラグイン引数名 => Proc
  MrubyBridgePluginArgProc = { 
      "ignoreUnsigned" => Proc.new { |obj,rhs| obj.set_ignoreUnsigned rhs },
      "include" => Proc.new { |obj,rhs| obj.set_include rhs },
      "exclude" => Proc.new { |obj,rhs| obj.set_exclude rhs },
      "auto_exclude" => Proc.new { |obj,rhs| obj.set_auto_exclude rhs },
  }

  @@celltypes = { }             # {celltype_name => [ BridgePlugin のインスタンスの配列 }
  @@init_celltypes = { }        # {celltype_name => [ BridgePlugin のインスタンスの配列 }
  @@struct_list = { }           # {struct_name=>StructType}
  @@ptr_list = { }              # {ptr_celltype_name=> @@TYPE_MAP の対応するもの}
  @@VM_list = { }               # VM_name => true
  @@VM_celltypes = {  }             # VM_name => { @celltype_name => セルの配列 }
  @@VM_struct_list = { }           # {name=>StructType}
  @@VM_ptr_list = { }              # { VM_name => {name=> @@TYPE_MAP の対応するもの} }
  @@b_post_coded = false        # gen_post_code 済み

  @@TYPE_MAP = {           # type_str   class             GET_SET
    :char_t            => [:char_t,    "Char",     :Char,  :INT   ],
    :uchar_t           => [:uchar_t,   "UChar",    :Char,  :INT   ],
    :schar_t           => [:schar_t,   "SChar",    :Char,  :INT   ],

    :bool_t            => [:bool_t,    "Bool",     :Bool,  :BOOL  ],
    :int8_t            => [:int8_t,    "Int8",     :Int,   :INT   ],
    :int16_t           => [:int16_t,   "Int16",    :Int,   :INT   ],
    :int32_t           => [:int32_t,   "Int32",    :Int,   :INT   ],
    :int64_t           => [:int64_t,   "Int64",    :Int,   :INT   ],
    :uint8_t           => [:uint8_t,   "UInt8",    :Int,   :INT   ],
    :uint16_t          => [:uint16_t,  "UInt16",   :Int,   :INT   ],
    :uint32_t          => [:uint32_t,  "UInt32",   :Int,   :INT   ],
    :uint64_t          => [:uint64_t,  "UInt64",   :Int,   :INT   ],

    :int               => [:int,       "Int",      :Int,  :INT   ], 
    :char              => [:char,      "Char",     :Char, :INT   ],    # char は char_t として扱う
    :short             => [:short,     "Short",    :Int,  :INT   ],
    :long              => [:long,      "Long",     :Int,  :INT   ],

    :"unsigned char"   => [:uchar_t,   "UChar",         :Char, :INT   ],
    :"unsigned int"    => [:"unsigned int",   "UInt",   :Int,  :INT   ],
    :"unsigned short"  => [:"unsigned short", "UShort", :Int,  :INT   ],
    :"unsigned long"   => [:"unsigned long",  "ULong",  :Int,  :INT   ],
    :"signed char"     => [:schar_t,   "SChar",    :Char,  :INT   ],
    :"signed int"      => [:int,       "Int",      :Int,   :INT   ],
    :"signed short"    => [:short,     "Short",    :Int,   :INT   ],
    :"signed long"     => [:long,      "Long",     :Int,   :INT   ],

    :float32_t         => [:float32_t, "Float32",  :Float, :FLOAT ],
    :double64_t        => [:double64_t,"Double64", :Float, :FLOAT ],

    :float             => [:float,     "Float32",  :Float, :FLOAT ],
    :double            => [:double,    "Double64", :Float, :FLOAT ]
  }

  def gen_preamble_ptr( file, b_singleton, ct_name, global_ct_name )
    tment = @@ptr_list[ ct_name ]
    file.print <<EOT

  GET_SET_#{tment[3]}( #{tment[1]}, #{tment[0]} )
  POINTER_CLASS( #{tment[1]}, #{tment[0]} )
EOT
  end
  
  def gen_preamble_struct( file, b_singleton, ct_name, global_ct_name )
    tag = ct_name
    structType = @@struct_list[ tag ]
    file.print <<EOT
  /* struct #{tag} */
  STRUCT_CLASS( #{tag} )
EOT
    structType.get_members_decl.get_items.each{ |d|
      type = d.get_type.get_original_type
      case type
      when IntType, CIntType
        bit_size = type.get_bit_size
        case bit_size
        when -11, -1
          tType = "Char"
          ttype = "char"
        when -2
          tType = "Short"
          ttype = "short"
        when -3
          tType = "Int"
          ttype = "int"
        when -4
          tType = "Long"
          ttype = "long"
        when -5
          tType = "IntPtr"
          ttype = "intptr"
        when 8, 16, 32, 64
          tType = "Int#{bit_size}"
          ttype = "int#{bit_size}"
        else
          raise "MrubyBridgeSignaturePlugin: MrubyBridgeSignaturePlugin: cannot handle bit_size #{bit_size}"
        end
        file.print "MEMBER_GET_SET_INT( #{tag}, #{d.get_name}, #{tType}, #{ttype} )\n"
      when FloatType, CFloatType
        file.print "MEMBER_GET_SET_FLOAT( #{tag}, #{d.get_name} )\n"
      else
        raise "MrubyBridgeSignaturePlugin: MrubyBridgeSignaturePlugin: cannot handle type"
      end
    }
  end

  def gen_ep_func_body_ptr( file, b_singleton, ct_name, global_ct_name, sig_name, ep_name, func_name, func_global_name, func_type, params )

    t = @@ptr_list[ct_name]
    type = t[1]
    file.print <<EOT
  struct RClass *a;                                /* MBP710 */

  a = mrb_define_class_under(mrb, TECS, "#{type}Pointer", mrb->object_class);
  MRB_SET_INSTANCE_TT(a, MRB_TT_DATA);

  mrb_define_method(mrb, a, "initialize",      #{type}Pointer_initialize,   MRB_ARGS_REQ(1));
  mrb_define_method(mrb, a, "[]",              #{type}Pointer_aget,         MRB_ARGS_REQ(1));
  mrb_define_method(mrb, a, "value",           #{type}Pointer_get_val,      MRB_ARGS_NONE());
  mrb_define_method(mrb, a, "[]=",             #{type}Pointer_aset,         MRB_ARGS_REQ(2));
  mrb_define_method(mrb, a, "value=",          #{type}Pointer_set_val,      MRB_ARGS_REQ(1));
  mrb_define_method(mrb, a, "size",            #{type}Pointer_size,         MRB_ARGS_NONE());
  mrb_define_method(mrb, a, "length",          #{type}Pointer_size,         MRB_ARGS_NONE());
EOT

    if t[2] == :Char then
      file.print <<EOT
  mrb_define_method(mrb, a, "to_s",            CharPointer_to_s, MRB_ARGS_NONE());
  mrb_define_method(mrb, a, "from_s",          CharPointer_from_s, MRB_ARGS_REQ(1));
EOT
    end
  end

  def gen_ep_func_body_struct( file, b_singleton, ct_name, global_ct_name, sig_name, ep_name, func_name, func_global_name, func_type, params )
    tag = ct_name
    structType = @@struct_list[ tag ]
    file.print  <<EOT
    struct RClass *a;                                /* MBP720 */

    a = mrb_define_class_under(mrb, TECS, "Struct#{tag}", mrb->object_class);
    MRB_SET_INSTANCE_TT(a, MRB_TT_DATA);

    mrb_define_method(mrb, a, "initialize", Struct_#{tag}_initialize, MRB_ARGS_NONE());
EOT

    structType.get_members_decl.get_items.each{ |d|
      file.print "  STRUCT_INIT_MEMBER( #{tag}, #{d.get_name} )\n"
    }
  end

  def ptrMrb2C file, type, param
    ttype = type.get_type.get_original_type
    tment = get_type_map_ent ttype
    tstr = tment[1]
=begin
    case ttype
    when IntType
      bit_size = ttype.get_bit_size
      case bit_size
      when -1, -11
        tstr = "Char"
      when 8, 16, 32, 64
        tstr = "Int#{bit_size}"
      when -2
        tstr = "Short"
      when -3
        tstr = "Int"
      when -4
        tstr = "Long"
      when -5
        tstr = "IntPtr"
      else
        raise "MrubyBridgeSignaturePlugin: not handle type"
      end
    when FloatType
      if ttype.get_bit_size == 32 then
        tstr = "Float32"
      else
        tstr = "Double64"
      end
    when BoolType
      tstr = "Bool"
    when StructType
      raise "MrubyBridgeSignaturePlugin: not handle type 2 #{ttype}"
    else
      raise "MrubyBridgeSignaturePlugin: not handle type 2 #{ttype}"
    end
=end
    if( param.get_size ) then
      sz_str = param.get_size.to_s
    elsif param.get_string then      # mikan とりあえず size_is と string の同時指定 (二重ポインタ) はなし
      sz_str = param.get_string.to_s
    else
      sz_str = "1"
    end
    # unsigned 型の場合には cast が必要
    if ttype.get_original_type.get_type_str != param.get_type.get_type.get_type_str then
      cast_str = "(#{param.get_type.get_type_str})"
    else
      cast_str = ""
    end

    modify = ""
    case param.get_direction
    when :OUT, :INOUT
      case tstr
      when "Char", "SChar", "UChar"
        modify = "Mod"
      end
    end
    if param.is_nullable? then
      nullable = "Nullable"
    else
      nullable = ""
    end

    # file.print "	CHECK_POINTER( #{tstr}, mrb_#{param.get_name}, #{sz_str} );\n"
    # file.print "	#{param.get_name} = #{cast_str}((struct #{tstr}PointerBody*)(DATA_PTR(mrb_#{param.get_name})))->buf;\n"
    file.print "	#{param.get_name} = CheckAndGet#{tstr}Pointer#{modify}#{nullable}( mrb, mrb_#{param.get_name}, #{sz_str} );\n"
  end

  def get_celltype_name
    @celltype_name
  end

  #=== プラグイン引数 ignoreUnsigned
  def set_ignoreUnsigned rhs
    if rhs == "true" || rhs == nil then
      @b_ignoreUnsigned = true
    end
  end

  #=== プラグイン引数 include
  def set_include rhs
    funcs = rhs.split ','
    funcs.each{ |rhs_func|
      found = false
      rhs_func.gsub!( /\s/, "" )
      @signature.get_function_head_array.each{ |a|
        if rhs_func.to_sym == a.get_name then
          found = true
        end
      }
      if found == false then
        cdl_error( "MRB1009 include function '$1' not found in signagture '$2'", rhs, @signature.get_name )
      else
        @includes << rhs_func.to_sym
      end
    }
  end

  #=== プラグイン引数 exclude
  def set_exclude rhs
    funcs = rhs.split ','
    funcs.each{ |rhs_func|
      rhs_func.gsub!( /\s/, "" )
      func_head = @signature.get_function_head rhs_func.to_sym
      if func_head == false then
        cdl_error( "MRB1010 exclude function '$1' not found in signagture '$2", rhs, @signature.get_name )
      else
        @excludes << rhs_func.to_sym
      end
    }
  end

  #=== プラグイン引数 auto_exclude
  def set_auto_exclude rhs
    if rhs == "false" then
      @b_auto_exclude = false
    elsif rhs == "true" then
      @b_auto_exclude = true     # auto_exclude = true by default 
    else
      cdl_warning( "MRB9999 auto_exclude: unknown rhs value ignored. specify true or false" )
    end
  end
end
