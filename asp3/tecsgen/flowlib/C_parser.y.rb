# coding: utf-8
#
#  TECS Generator
#      Generator for TOPPERS Embedded Component System
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
#   $Id: C_parser.y.rb 3075 2019-06-08 01:51:59Z okuma-top $
#++

class TCFLOW_Cparser
rule
# トップレベルの構文要素は C_parser
all: C_parser

# Expr
##########################  式  ##########################
# K&Rの文法(プログラミング言語C 第2版 付録)に準拠しているが、C_Parser.y.rb から変形して作っている

primary_expression
        : IDENTIFIER			{ result = val[0].val }
        | INTEGER_CONSTANT		{ result = nil } 
        | FLOATING_CONSTANT		{ result = nil }
        | OCTAL_CONSTANT		{ result = nil }
        | HEX_CONSTANT			{ result = nil }
        | CHARACTER_LITERAL		{ result = nil }
        | string_literal_list		{ result = nil }
#        | '(' expression ')'		{ result = val[1] }     # 一度は postfix_expression として扱われている
#                                                               #  これを扱わないと、ポインタを使った関数呼び出しが出ない
        | '(' expression ')'		{ result = nil }     # 一度は postfix_expression として扱われている

string_literal_list
        : STRING_LITERAL
        | string_literal_list STRING_LITERAL


# 関数呼び出しと後置インクリメント、デクリメント演算子がない
postfix_expression
        : primary_expression				{ result = val[0] }
#        | primary_expression '(' argument_list ')'
        | postfix_expression '(' argument_list ')'      # postfix_expression 化した．func_ptr_array[]() が可能となる
		{
####ここのvalにはセル内部変数は無い
			if val[0]
			  if @current_function
			    @current_function.ref_func val[0].to_sym
			  end
			  result = "#{val[0]}()".to_sym
			end
		}
        | postfix_expression '[' expression ']'		{ result = "#{val[0]}[]" }
        | postfix_expression '.' IDENTIFIER		{ result = "#{val[0]}.#{val[2]}" }
        | postfix_expression '->' IDENTIFIER		{ result = "#{val[0]}->#{val[2]}" }
        | postfix_expression '++' 			{ result = val[0] }
        | postfix_expression '--' 			{ result = val[0] }


argument_list
        :
        | assignment_expression
        | argument_list ',' assignment_expression


# 前置インクリメント、デクリメント演算子がない
unary_expression
        : postfix_expression
		{
#p val[0] #######ここでglobal変数を持つ
			if val[0]
			  if /\(\)$/ =~ val[0].to_s then
			    # () で終わっているものは関数として報告済み
			  else
			    if @current_function
			      @current_function.ref_var val[0].to_sym ###########
			      result =  val[0].to_sym
#p result ####global_var
			    end
			  end
			end
		}
        | '++' unary_expression			{ result = nil }
        | '--' unary_expression			{ result = nil }
        | unary_operator cast_expression	{ result = val[1] }
        | SIZEOF unary_expression		{ result = nil }
        | SIZEOF '(' type_name ')'		{ result = nil }

unary_operator
        : '&'
        | '*'
        | '+'
        | '-'
        | '~'
        | '!'

cast_expression
        : unary_expression
        | '(' type_name ')' cast_expression	{ result = val[3] }

multiplicative_expression
        : cast_expression
        | multiplicative_expression '*' cast_expression
        | multiplicative_expression '/' cast_expression
        | multiplicative_expression '%' cast_expression


additive_expression
        : multiplicative_expression
        | additive_expression '+' multiplicative_expression
        | additive_expression '-' multiplicative_expression

shift_expression
        : additive_expression
        | shift_expression '<<' additive_expression
        | shift_expression '>>' additive_expression

relational_expression
        : shift_expression
        | relational_expression '<' shift_expression
        | relational_expression '>' shift_expression
        | relational_expression '<=' shift_expression
        | relational_expression '>=' shift_expression

equality_expression
        : relational_expression
        | equality_expression '==' relational_expression
        | equality_expression '!=' relational_expression


and_expression
        : equality_expression
        | and_expression '&' equality_expression

exclusive_or_expression
        : and_expression
        | exclusive_or_expression '^' and_expression


inclusive_or_expression
        : exclusive_or_expression
        | inclusive_or_expression '|' exclusive_or_expression


logical_and_expression
        : inclusive_or_expression
        | logical_and_expression '&&' inclusive_or_expression

logical_or_expression
        : logical_and_expression
        | logical_or_expression '||' logical_and_expression

conditional_expression
        : logical_or_expression
        | logical_or_expression '?' expression ':' conditional_expression


# コンマ演算子が使えない
expression
        : assignment_expression
        | expression ',' assignment_expression

constant_expression
        : conditional_expression



# Types
##########################  宣言  ##########################

##!
#declaration_list      # declaration_list は compoundstatement からしか参照されないと仮定
#        : declaration
#		{
#			if @current_function && val[0]
#			  val[0].each { |v|
#				@current_function.new_local v
#			  }
#			end
#		}
#        | declaration_list declaration
#		{
#			if @current_function && val[1]
#			  val[1].each { |v|
#				@current_function.new_local v
#			  }
#			end
#		}
#        | typedef			# MS 拡張文法  関数内での typedef
#        | declaration_list typedef

declaration
        : declaration_specifiers init_declarator_list ';'	{ result = val[1] }
        | declaration_specifiers  ';'				{ result = nil } 
 
declaration_specifiers
        : storage_class
        | type_specifier
        | type_qualifier
        | storage_class declaration_specifiers
        | type_specifier declaration_specifiers
        | type_qualifier declaration_specifiers

init_declarator_list
        : init_declarator	{ result = [ val[0] ] }
        | init_declarator_list ',' init_declarator
		{
			result << val[2]
		}

init_declarator
        : declarator			{ result = val[0] }
        | declarator '=' initializer	{ result = val[0] }

type_specifier
        : VOID				{ set_no_type_name true }
        | FLOAT				{ set_no_type_name true }
        | DOUBLE			{ set_no_type_name true }
        | struct_or_union_specifier
        | enum_specifier		{ set_no_type_name true }
        | TYPE_NAME			{ set_no_type_name true }
        | CHAR				{ set_no_type_name true }
        | SHORT				{ set_no_type_name true }
        | INT				{ set_no_type_name true }
        | LONG				{ set_no_type_name true }
        | SIGNED			{ set_no_type_name true }
        | UNSIGNED			{ set_no_type_name true }
        | __INT8			{ set_no_type_name true }
        | __INT16			{ set_no_type_name true }
        | __INT32			{ set_no_type_name true }
        | __INT64			{ set_no_type_name true }
        | __BOOL			{ set_no_type_name true }

struct_or_union_specifier
        : struct_or_union
          struct_tag '{' struct_declaration_list '}'
          | struct_or_union '{' struct_declaration_list '}'
          | struct_or_union '{'  '}'   # add 19.01.06 empty struct
          | struct_or_union struct_tag '{'  '}'   # add 19.01.06 empty struct
          | struct_or_union struct_tag

struct_or_union
        : STRUCT 	{ set_no_type_name true }
        | UNION 	{ set_no_type_name true }

struct_declaration_list
        : struct_declaration
        | struct_declaration_list struct_declaration

struct_tag:
	IDENTIFIER

# ポインタ修飾子を追加
struct_declaration
        : type_specifier_qualifier_list struct_declarator_list ';'
	| type_specifier_qualifier_list ';' ##!


# K&Rのspecifier_qualifier_listと同じ
# 名前がまぎらわしかったのでtype_を付けた
type_specifier_qualifier_list
        : type_specifier
        | type_specifier type_specifier_qualifier_list
        | type_qualifier
        | type_qualifier type_specifier_qualifier_list

struct_declarator_list
        : struct_declarator
        | struct_declarator_list ',' struct_declarator

struct_declarator
        : declarator
        | declarator ':' constant_expression  # bit フィールド

enum_specifier
        : enum_type            '{' enumerator_list '}'
        | enum_type IDENTIFIER '{' enumerator_list '}'
        | enum_type IDENTIFIER

enum_type
        : ENUM

enumerator_list
        : enumerator
        | enumerator_list ',' enumerator
        | enumerator_list ','

enumerator
        : IDENTIFIER
        | IDENTIFIER '=' constant_expression

type_qualifier
        : CONST
        | VOLATILE

declarator
        : pointer direct_declarator	{ result = val[1] }
        | direct_declarator		{ result = val[0]  }

direct_declarator
        : IDENTIFIER			{ result = val[0].val }
        | '(' declarator ')'		{ result = val[1] }
        | direct_declarator '[' constant_expression ']'	{ result = val[0] }
        | direct_declarator '[' ']'			{ result = val[0] }
        | direct_declarator '(' parameter_type_list ')'	{ result = val[0] }
        | direct_declarator '(' identifier_list ')'	{ result = val[0] }  # これは古い関数定義用
        | direct_declarator '(' ')'			{ result = val[0] }

pointer
        : pointer0
        | call_convention pointer0            # 関数ポインタに __cdecl 等が付けられているケース
pointer0
        : '*'
        | '*' type_qualifier
        | '*' pointer
        | '*' type_qualifier pointer
        | call_convention

call_convention
        : __CDECL
        | __STDCALL
        | __FASTCALL

parameter_type_list
        : parameter_list		{ TCFlow::Function.param_list val[0] }
        | parameter_list '...'		{ TCFlow::Function.param_list val[0] }
        | parameter_list ',' '...'	{ TCFlow::Function.param_list val[0] }

parameter_list
        : parameter_declaration
		{
			if val[0]
			  result = [val[0]]
			else
			  result = nil
			end
		}
        | parameter_list ',' parameter_declaration
		{
			if val[0] && val[2]
			  result << val[2]
			elsif val[2]
			  result = [ val[2] ]   # これは文法エラー時しかありえない
			end
		}


# パラメータ修飾子を追加
parameter_declaration
        : declaration_specifiers declarator
		{ result = val[1] }
        | declaration_specifiers abstract_declarator	# 仮引数なし
		{ result = nil	}
        | declaration_specifiers			# 仮引数なし
		{ result = nil	}


identifier_list       # 060211  不用になった
        : IDENTIFIER
        | identifier_list ',' IDENTIFIER

type_name
        : type_specifier_qualifier_list
        | type_specifier_qualifier_list abstract_declarator

abstract_declarator
        : pointer
        | direct_abstract_declarator
        | pointer direct_abstract_declarator

direct_abstract_declarator
        : '(' abstract_declarator ')'
        | '[' ']'
        | '[' constant_expression ']'
        | direct_abstract_declarator '[' ']'
        | direct_abstract_declarator '[' constant_expression ']'
        | '(' ')'
        | direct_abstract_declarator '(' ')'
        | direct_abstract_declarator '(' parameter_type_list ')'

# assignment_expressionをconstant_expressionに変更
initializer			# mikan
        : constant_expression
        | '{' initializer_list '}'
        | '{' initializer_list ',' '}'

initializer_list
        : initializer
        | initializer_list ',' initializer


##########################  ここからはCDL独自  ##########################

#トップレベルの構文規則
C_parser
        : C_parser statement
        |



statement
        : typedef
        | func_def
        | declaration
        | declarator ';'                  # 戻り値型指定のない関数を想定
        | ';'
        | error   # エラー回復ポイント


typedef
        : TYPEDEF type_specifier_qualifier_list declarator_list ';'
		{
			val[2].each{ |t|
				# print( "typdef: #{t.to_s}\n" )
				@type_name_list[ t.to_sym ] = t
			}
		}
        | TYPEDEF type_specifier_qualifier_list ';'	{ error( "maybe typedef duplicate, ignored" ) }
                # 二重定義の場合 無視


func_def
        : declaration_specifiers declarator
		{
			@current_function = TCFlow::Function.new @@current_locale
		}
          kr_param_list compoundstatement
		{
			@current_function.set_name val[1]
#p val[1]
			@current_function = nil
		}
        | declarator                       # 戻り値の型指定省略
		{
			@current_function = TCFlow::Function.new @@current_locale
		}
          kr_param_list compoundstatement
		{
			@current_function.set_name val[0]
#p val[0]
			@current_function = nil
		}

kr_param                                   # 旧文法の引数
        : type_specifier_qualifier_list declarator_list ';'
kr_param_list:
        : kr_param_list kr_param
        |

declarator_list
        : declarator				{  result = [val[0]]}
        | declarator_list ',' declarator	{  result << val[2] }

infunc_statement_list
        : infunc_statement
        | infunc_statement_list infunc_statement

infunc_statement
        : ifstatement
        | whilestatement
        | dowhilestatement
        | forstatement
        | switchstatement
        | labelstatement
        | compoundstatement
        | gotostatement
        | expressionstatement
      	| declaration ##!
        | asm_statement
        | ';'   # 空文

ifstatement
        : IF '(' expression ')' infunc_statement
        | IF '(' expression ')' infunc_statement ELSE infunc_statement

whilestatement
        : WHILE '(' expression ')' infunc_statement

dowhilestatement
        : DO infunc_statement WHILE '(' expression ')' ';'

forstatement
        : FOR '(' expression_or_empty ';' expression_or_empty ';' expression_or_empty ')' infunc_statement
        | FOR '(' declaration_specifiers init_declarator_list ';' expression_or_empty ';' expression_or_empty ')' infunc_statement

expression_or_empty
        : expression
        |

switchstatement
        : SWITCH '(' expression ')'  infunc_statement

labelstatement
        : IDENTIFIER ':' infunc_statement
        | CASE constant_expression ':' infunc_statement
        | DEFAULT ':' infunc_statement

#追加部ここから ##!
#compoundstatement ::= '{' (declaration_list | infunc_statement_list)* '}'
compoundstatement
#        : '{' declaration_list infunc_statement_list '}'
#        | '{' declaration_list '}'
        : '{' infunc_statement_list '}'
        | '{'  '}'

#追加部ここまで

gotostatement
        : GOTO IDENTIFIER ';'
        | CONTINUE ';'
        | BREAK ';'
        | RETURN ';'
        | RETURN expression ';'

expressionstatement
        : expression ';'

assignment_expression
        : conditional_expression
        | unary_expression assignment_operator assignment_expression
		{
#p val[0] ###書き込みされる変数群
			if val[0]
			  if /\(\)$/ =~ val[0].to_s then
			    # () で終わっているものは関数, 通常来ないはず
			  else
			    if @current_function
####p @current_function #### global_var選別後
			      @current_function.ref_var( val[0].to_sym, true ) ##### R/Wカウント
			      result =  val[0].to_sym
			    end
			  end
			end
		}

assignment_operator
        : '='
        | '+='
        | '-='
        | '*='
        | '/='
        | '%='
        | '<<='
        | '>>='
        | '&='
        | '|='
        | '^='

storage_class
        : __INLINE__
        | __FORCEINLINE
        | INLINE
        | EXTERN
        | STATIC
        | AUTO
        | REGISTER
        | __DECLSPEC '(' IDENTIFIER ')'

asm_statement
        : _ASM {
        while true
          # ';' が表れるまで、トークンを読み飛ばす。
          # gcc の構文拡張に対応すべきだが、単純な実装として、';' まで読み飛ばす。
          # トークン単位で読み飛ばしているので、文字列やコメント内の ';' は対象にならない。
          token = next_token
          if token[1].val == ";"
            break
          end
		      # p "skip: #{token[1].val}" 
        end
      }

end

---- inner

  RESERVED = {
    # keword
    'typedef' => :TYPEDEF,
    'struct' => :STRUCT,
    'union' => :UNION,

    # specifier
    # types
    'void'    => :VOID,
    'char'    => :CHAR,
    'short'   => :SHORT,

    'volatile'=> :VOLATILE,
    'const'   => :CONST,
    'extern'   => :EXTERN,

    'long'    => :LONG,
    'float'   => :FLOAT,
    'double'  => :DOUBLE,
    'signed'  => :SIGNED,
    'unsigned'=> :UNSIGNED,

    'int'     => :INT,
    'enum'    => :ENUM,
    '__int8'  => :__INT8,
    '__int16' => :__INT16,
    '__int32' => :__INT32,
    '__int64' => :__INT64,
    '__bool'  => :__BOOL,

    'return'  => :RETURN,
    'if'      => :IF,
    'else'    => :ELSE,
    'while'   => :WHILE,
    'do'      => :DO,
    'for'     => :FOR,
    'case'    => :CASE,
    'default' => :DEFAULT,
    'goto'    => :GOTO,
    'continue' => :CONTINUE,
    'break'   => :BREAK,
    'static'  => :STATIC,
    'register' => :REGISTER,
    'auto'    => :AUTO,
    'switch'  => :SWITCH,
    'sizeof'  => :SIZEOF,

    '__inline__'  => :__INLINE__,
    '__forceinline' => :__FORCEINLINE,
    'inline'      => :INLINE,
    '__inline'      => :INLINE,

    '__declspec'  => :__DECLSPEC,
    '__cdecl'     => :__CDECL,
    '__stdcall'   => :__STDCALL,
    '__fastcall'  => :__FASTCALL,

    '__extension__'    => :EXTENSION,
    '__asm__' => :_ASM,

  }

  @@current_locale = []

  def finalize

  
  end

# GCC extension
@@header_lines =<<EOT
#ifndef TECS_NO_GCC_EXTENSION_SUPPORT

/*
 * these extension can be eliminated also by spefcifying option
 * --no-gcc-extension-support for tecsgen.
 */
#ifdef __GNUC__

#ifndef __attribute__
#define __attribute__(x)
#endif

#ifndef __extension__
#define __extension__
#endif

#ifndef __builtin_va_list
typedef char  *__builtin_va_list;
// #define __builtin_va_list va_list
#endif

#ifndef restrict
#define restrict
#define __restrict
#endif

#define __asm__(x)

#define __builtin_offsetof( x, y )  (1)

#endif /* ifdef __GNUC__ */
#endif /* TECS_NO_GCC_EXTENSION_SUPPORT */
EOT


  #===
  #files::String  ファイル名またはpipe コマンド

  ##parse
  def parse(files, cpp_cmd)

    #インスタンス変数
    @current_function = nil
    @type_name_list = {}
    @b_no_type_name = false
    @q = [ :EOF ]
    @processing_file=nil

#    integer_qualifier = "([Uu][Ll]?[Ll]?|[Ll][Ll]?[Uu]?)?" #これだとうまくいく(厳密でない)
#    integer_qualifier = "([Uu][Ll][Ll]|[Uu][Ll]|[Uu]|[Ll][Ll][Uu]|[Ll][Uu]|[Ll][Ll]|[Ll])?" #これもうまくいく(厳密でない)
    integer_qualifier = "([Uu](LL|ll|L|l)|(LL|ll|L|l)[Uu]?|[Uu])?"

    #例外補足1
    begin
      comment = false

      ## 各ファイルに対する処理
      file2_name = ""
      files.each {|file|

        if cpp_cmd then
          file2_name = "#{$gen_dir}/tmp_#{file.sub(/.*\//,"")}"
          File.open( file2_name, "w:ASCII-8BIT" ){
            |f|
            lines = IO.readlines( file )
            f.puts( @@header_lines )
            f.puts lines
          }
        end
        @processing_file=file
        STDERR.print "reading #{file}\n" #ファイルのパスを表示
        lineno = 0

        #例外補足2
        begin #実行する処理
          io = nil
          if cpp_cmd
            io = IO.popen( cpp_cmd + " " + file2_name, "r:ASCII-8BIT" ) #サブプロセスの標準入出力との間にパイプラインを確立
            file3_name = "#{file2_name}_2.c"
            file3 = File.open( file3_name, "w:ASCII-8BIT" )
          else
            io = File.open( file, "r:ASCII-8BIT" )
          end

          ## ioに対する各処理
          io.each {|line|
            lineno += 1
            if cpp_cmd then
                file3.puts line
            end
            col = 1
            line.rstrip! #末尾の空白文字を除去

            until line.empty? ## untilの処理内容

              if comment
                case line
                  # コメント終了
                when /\A\*\//
                  comment = false
                when /\A./
                  ;
                end
              # elsif asm_state != :NONE
              #   if asm_state == :WAIT_NEXT	# __asm が来た直後
              #     case line
              #       # コメント開始
              #     when /\A\s*\{/
              #       asm_state = :WAIT_CB	# 閉じ括弧待ち
              #     when /\As*/
              #       asm_state = :WAIT_SC	# セミコロン待ち
              #     end
              #   elsif asm_state == :WAIT_CB	# 閉じ括弧待ち
              #     case line
              #     when /\A.*\}/			# 本当は最短一致にすべきだろう		
              #           # asm は特種で、通常ブロックが後続するケースはないので
              #       asm_state = :NONE
              #     when /\A.*/
              #       # asm_state = :WAIT_CB	# 閉じ括弧待ちの継続
              #     end
              #   elsif asm_state == :WAIT_SC	# セミコロン待ち
              #     case line
              #     when /\A.*;/
              #       asm_state = :NONE		# 本当は最短一致にすべきだろう
              #     when /\A.*/
              #       # asm_state = :WAIT_SC	# 閉じ括弧待ちの継続
              #     end
              #   end
              else
                case line
                  # 空白、プリプロセスディレクティブ
                  #		when /\A#line\s+(\d+)\s+\"(.*)\"\.*$/ ####ここで行番号がうまく拾えない \d
                when /\A#\s+(\d+)\s+\"(.*)\".*$/ #元のセルタイプコード中での行番号表示
                #  p "#{$1} #{$2}"
                # lineno = __LINE__
                  lineno = $1.to_i - 1         # lineno
                  file = $2
                # # プリプロセッサディレクティブ
                when /\A#.*$/
                # 読み飛ばすだけ
                when /\A\s+/
                  ;
                #   # __asm
                # when /\A__asm/, /\A_asm/
                #   asm_state = :WAIT_NEXT
                  # 識別子
                when /\A[a-zA-Z_]\w*/
                  word = $&
                  @q << [RESERVED[word] || :IDENTIFIER, Token.new(word.intern, file, lineno, col)]

                  # 浮動小数定数
                when /\A[0-9]+\.([0-9]*)?([Ee][+-]?[0-9]+)?([Ff]|[Ll])?/
                  @q << [:FLOATING_CONSTANT, Token.new($&, file, lineno, col)]
                when /\A\.[0-9]+([Ee][+-]?[0-9]+)?([Ff]|[Ll])?/    # . で始まる
                  @q << [:FLOATING_CONSTANT, Token.new($&, file, lineno, col)]
                when /\A[0-9]+[Ee][+-]?[0-9]+([Ff]|[Ll])?/	    # . 以下がない
                  @q << [:FLOATING_CONSTANT, Token.new($&, file, lineno, col)]

                # 16 進数定数 ##! 大文字・小文字区別なし
                when /\A0x[0-9A-Fa-f]+#{integer_qualifier}/
                    @q << [:HEX_CONSTANT, Token.new($&, file, lineno, col)]
                    #when /\A0[Xx][0-9A-Fa-f]+(ULL?|ull?)/, /\A0[Xx][0-9A-Fa-f]+([UL]|[ul])/, /\A0[Xx][0-9A-Fa-f]+/
                  # 8 進数定数 ##! 大文字・小文字区別なし
                when /\A0[0-7]+#{integer_qualifier}/
            #				when /\A0[0-7]+(UL|ul)/, /\A0[0-7]+([UL]|[ul])/, /\A0[0-7]+/
                  @q << [:OCTAL_CONSTANT, Token.new($&, file, lineno, col)]
                  # 整数定数 ##! 大文字・小文字区別なし
                when /\A\d+#{integer_qualifier}/
            #			when /\A\d+(UL|ul)/, /\A\d+([UL]|[ul])/, /\A\d+/
                  @q << [:INTEGER_CONSTANT, Token.new($&, file, lineno, col)]
                  # 文字
            #		  		when /\A'(?:[^'\\]|\\.)'/
                when /\A'(?:[^'\\]|\\.){1,4}'/      # 1〜4文字まで可能とする
                  @q << [:CHARACTER_LITERAL, Token.new($&, file, lineno, col)]
                  # 文字列
            #		  		  "#include  #include #include \"../systask/logtask.cfg\"     #  最後の " 忘れ)で無限ループ
            #				  		when /\A"(?:[^"\\]+|\\.)*"/
                when /\A"(?:[^"\\]|\\.)*"/   # これはうまく行くようだ
                  @q << [:STRING_LITERAL, Token.new($&, file, lineno, col)]
                  # 行コメント
                when /\A\/\/.*$/
                  # # プリプロセッサディレクティブ(ファイル名)
                when /\A\/\*/
                  comment = true
                when /\A>>=/, /\A<<=/, /\A>>/,  /\A<</, /\A\|\|/, /\A\&\&/, /\A\-\>/, /\A\+\+/, /\A\-\-/
                  @q << [$&, Token.new($&, file, lineno, col)]
                when /\A\+=/, /\A\-=/, /\A\*=/, /\A\/=/, /\A%=/, /\A&=/, /\A\|=/, /\A\^=/, /\A\.\.\./
                  @q << [$&, Token.new($&, file, lineno, col)]
                when /\A\::/, /\A==/, /\A!=/, /\A>=/, /\A<=/
                  @q << [$&, Token.new($&, file, lineno, col)]
                when /\A./
                  @q << [$&, Token.new($&, file, lineno, col)]
                else
                  raise
                end
              end

              line = $'
              col += $&.length
            end ## untilの処理内容

          } ## ioに対する各処理
          @q << :EOF
        rescue => evar 	#例外が発生した時の処理、例外の内容を受け取る→例外補足
          error( evar )
      # p $@
        ensure #例外の発生有無にかかわらず最後に必ず実行する処理
          if io
            io.close
          end
        end
      }## 各ファイルに対する処理

      # 終了の印
      @q << nil

      @yydebug = true
      do_parse

    end
  end ##parse

  ##next_token
  def next_token

    token = @q.shift
    while token == :EOF       # ファイルが空のとき :EOF が連続する
      @type_name_list = {}
      token = @q.shift
      if token.instance_of? Array
	#STDERR.print "parsing #{@processing_file}\n" #ファイルのパスを表示
      end
    end

    if token then
      @@current_locale = token[1].locale

      case token[1].val
      when ";", ":", ",", "(", ")", "{", "}"
	set_no_type_name false
      when ".", "->"
	set_no_type_name true
      end


      # TYPE_NAME トークンへ置き換え
#      print( "#{token[0]} : #{token[1]}\n" ) #トークン表示
      if @b_no_type_name == false
	if token[0] == :IDENTIFIER #形名、関数名、変数名
#p token[1].val ###ここに変数名は無い
#p @current_function ###ここの@global_varに変数格納済み(R/Wカウント済み)
	  if @current_function && @current_function.is_local_var?( token[1].val )
	    # local 変数または引数
	  else
	    if @type_name_list[ token[1].val ] then
#print ("#{token[1]}\n")
	      token[0] = :TYPE_NAME
	    end
	  end
	end
      end

      #$debug = true ####
      if $b_print_all_token then     # 070107 token 無効時ここを通さないようした (through 対応 -d の時に例外発生) 
        locale = @@current_locale
        if token then
          print( "#{locale[0]}: line #{locale[1]} : #{token[0]} : '#{token[1].val}'\n" )
        else
          print( "#{locale[0]}: line #{locale[1]} : EOF\n" )
        end
      end
    end

    token
  end ##next_token

  ##on_error
  def on_error(t, v, vstack)
    if v == "$" then
      error( "Unexpected EOF" )
    else
      error( "syntax error near '#{v.val}'" )
    end

  end ##on_error

  def self.current_locale
    @@current_locale
  end

  @@n_error = 0
  @@n_warning = 0

  ##error
  # このメソッドは構文解析、意味解析からのみ呼出し可（コード生成でエラー発生は不適切）
  def error( msg )
    @@n_error += 1
    locale = @@current_locale

    if locale then
      puts "error: #{locale[0]}: line #{locale[1]} #{msg}"
      if locale[0] != nil && locale[0] != @processing_file
	puts "  #{locale[0]} is included from #{@processing_file}"
      end
    else
      puts "error: #{msg}"
    end
  end ##error

  # このメソッドは構文解析、意味解析からのみ呼出し可（コード生成でウォーニング発生は不適切）
  def warning( msg )
    @@n_warning += 1
    locale = @@current_locale
    puts "warning: #{locale[0]}: line #{locale[1]} #{msg}"
  end

  def self.get_n_error
    @@n_error
  end

  def self.get_n_warning
    @@n_warning
  end

  def set_no_type_name b_no_type_name
    # print "b_no_type_name=#{b_no_type_name}\n"
    @b_no_type_name = b_no_type_name
  end

---- footer



class Token
# @parameter
#	SIZE_IS, STRING, COUNT_IS : expression
#	SEND, RECEIVE : string ( IDENTIFIER )

  attr_accessor :val, :file, :lineno, :col

  def initialize(val, file, lineno, col)
    @val = val
    @file = file
    @lineno = lineno
    @col = col

  end

  def to_s
    @val.to_s
  end

  def to_sym
    @val.to_sym
  end

  def get_name
    @val
  end

  def set_parameter( parameter )
    @parameter = parameter
  end

  def scan(*arg, &block)
    val.to_s.scan(*arg, &block)
  end

  def to_i
    if @val.is_a? Integer
      @val.to_i
    else
      raise TypeError
    end
  end

  def locale
    [@file, @lineno, @col]
  end

  def eql?(other)
    if other.is_a? Symbol
      @val == other
    elsif other.is_a? Token
      @val == other.val
    elsif other.is_a? String
      @val.to_s == other
    else
      raise ArgumentError
    end
  end

  alias == eql?

  def <=>(other)
    if other.class == Token
      if @val.is_a? Integer
        @val <=> other.val
      elsif @val.is_a? Symbol
        @val.to_s <=> other.val.to_s
      else
        raise ArgumentError
      end
    else
      raise ArgumentError
    end
  end

  def show_tree( indent )
    indent.times { print "  " }
    print "#{@val}\n"
  end

end # class C_Parser
