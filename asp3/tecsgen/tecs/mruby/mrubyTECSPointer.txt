TECS::Int8Pointer       TECS::UInt8Pointer
TECS::Int16Pointer      TECS::UInt16Pointer
TECS::Int32Pointer      TECS::UInt32Pointer
TECS::Int64Pointer      TECS::UInt64Pointer
TECS::CharPointer       TECS::SCharPointer      TECS::UCharPointer
TECS::IntPointer	    TECS::UIntPointer
TECS::ShortPointer	    TECS::UShortPointer
TECS::LongPointer       TECS::ULongPointer
TECS::BoolPointer
TECS::Float32Pointer
TECS::Double64Pointer

TECS のポインタ型の整数データ、浮動小数点データ、文字データを扱うため
の記憶域を扱います。これらのクラスは、シグニチャで使用されているものを 
MrubyBridgePlugin プラグインが自動判別して生成します。

ポインタ値そのものを渡すには intptr_t 型として渡すのが妥当です。

C 言語の型とは、以下のように対応します。

   int8_t                 TECS::Int8Pointer
   int16_t                TECS::Int16Pointer
   int32_t                TECS::Int32Pointer
   int64_t                TECS::Int64Pointer
   char_t                 TECS::CharPointer
   schar_t                TECS::SCharPointer
   uchar_t                TECS::UCharPointer
   uint8_t                TECS::UInt8Pointer
   uint16_t               TECS::UInt16Pointer
   uint32_t               TECS::UInt32Pointer
   uint64_t               TECS::UInt64Pointer
   bool_t                 TECS::BoolPointer
   float32_t              TECS::Float32Pointer
   double64_t             TECS::Double64Pointer

 非推奨の型
   char                   TECS::CharPointer
   int                    TECS::IntPointer
   short                  TECS::ShortPointer
   long                   TECS::LongPointer
   signed char            TECS::SCharPointer
   unsigned char          TECS::UCharPointer
   unsigned int           TECS::UIntPointer
   unsigned short         TECS::UShortPointer
   unsigned long          TECS::ULongPointer
   float                  TECS::Float32Pointer
   double                 TECS::Double64Pointer

char, signed char, unsgined char は char_t, schar_t, uchar_t と見なされます。

【備考】
ターゲット環境において char, singed char, unsigned char のサイズが 1 バイトでは無い場合には、相違が生じる。

== スーパークラス ==

Object

== クラスメソッド ==

new( size )

  size の大きさを持つ記憶域を指すポインタを生成して返します。
  size の単位は、要素の個数です。

== メソッド ==

self[ nth ]

  nth 番目の要素を返します。nth は 0..(length-1) の整数です。
  '=' の左辺では、nth 番目の要素を右辺の値に置き換えます。

  nth に整数または浮動小数以外を渡した場合、例外が発生します。

self[ nth ] = val

  整数型のポインタの場合、値の範囲がチェックされます。
  val の値が範囲を超える場合、例外が発生します。

value

  ポインタの指す先の値を取り出します。
  self[0] と同じです。

value = val

  ポインタの指す先に値を設定します。
  self[0] = val と同じです。

length
size

  ポインタが指す要素の個数を返します。

to_s

  CharPointer クラスでのみ使用できます。
  mruby の String クラスのインスタンスに変換して返します。

from_s

  CharPointer クラスでのみ使用できます。
  mruby の文字列から CharPointer の指す記憶域に文字列をコピーします。
  
  new の際に確保された記憶域が用いられます。記憶域の大きさが文字列よりも短い場合には、記憶域の大きさに収まるようにコピーされます。
  記憶域よりも文字列が長い場合、記憶域の末尾に終端文字 '\0' は置かれません。

