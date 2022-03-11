MrubyBridgePlugin に関するメモ
										2012年11月 7日　初版作成
										2013年 5月26日　見直し
                                                        大山　博司

【概要】

MrubyBridgePlugin は、TECS CDL の signature 記述に基づき、mruby と TECS 
コンポーネントをブリッジする、mruby の C 言語実装クラスであり TECS のセ
ルタイプとなるコンポーネントを生成する、TECS のシグニチャプラグインであ
る。

【構造】

         +--------------------------+              +-------------------+
         |     (ブリッジセル)         |              |    (目的セル)      |
         |                          |              |                   |
mruby    | nMruby::tsSignatureBridge|  sSignature  |  tTargetCelltype  |
 - - - > |       BridgeCell         |--------------|>   TargetCell     |
         |                          |cTECS    eTest|                   |
         | mruby クラス：            |              |                   |
         |  TECS::TSignatureBridge  |              |                   |
         +--------------------------+              +-------------------+

MrubyBridgePlugin は、mruby から TECS へブリッジをするセルタイプを生成
する。mruby=>TECS をブリッジするセルは、そのセルタイプのセルとして、プ
ラグインの利用者が CDL 内に設置し、mruby から操作したいセル、すなわち
目的セルに結合する。

このセルタイプは、mruby のクラスの定義を行い、ブリッジセルを mruby の
インスタンスとして取り扱えるようにし、mruby のインスタンスメソッドの呼
び出しに応じて、目的セルの受け口関数を呼び出す働きを持つ。

【簡単な使用例】

簡単な使用例は、SimpleSample.cdl および SimpleSample.rb にある。
MrubyBrigePlugin はシグニチャプラグインであり、シグニチャプラグインの
使用方法を理解していれば、これらのソースファイルのコメントを参照するこ
とで、MrubyBrigePlugin の大よその使い方は理解できるはずである。
以下の方法で、ビルドおよび実行できる。

   % make                             # mruby 本体のビルド (.a を作成)
   % cd MrubyBridge                   # MrubyBridge へ移動
   % make                             # ビルド
   % ./mruby-Sample SimpleSample.rb   # 実行

【Makefile の構成】

tecsgen により生成される gen/Makefile.templ に対し、以下の変更を加える必要があ
る。

・gen/Makefile.temp は、一つ上のディレクトリへ Makefile という名前で置
  けば、cygwin などの POSIX 環境で試験実行する場合には、無調整で使用で
  きるように生成される(ただし gnu make を使用する)
・tSysLog を使う場合、リロケータブルオブジェクトを追加 (下行参照)
   OTHER_OBJS = $(_TECS_OBJ_DIR)vasyslog.o

【データ型】

MrubyBridgePlugin は、以下の TECS の型を扱うことができる (制限される)。
この制限は、MrubyBridgePlugin を適用しようとするシグニチャに対するものであり、
それ以外へ制限がおよぶものではない。

 スカラー型			bool_t, char_t, intN_t, double64_t, float32_t
									(N=8,16,32,64)
					int, short, long, char (TECS 非推奨の型)

 ポインタ型			bool_t*, char_t*, intN_t*, double64_t*, float32_t*
					int*, short*, long*, char*
					構造体型へのポインタ（構造体型と同じ制限あり）

                    これらは、ポインタの指している先の値を扱うためのも
                    のである。ポインタ値(アドレス)を渡したい場合には、
                    これらを使用できない。代わりに intptr_t, uintptr_t 
                    を使用する。

 					size_is, string 指定子あり/なしのいずれも扱える。
					ただし構造体型へのポインタに size_is 指定されたも
					のは扱えない。(仕様書に明言されていないが構造体ポ
					インタの string は、元々サポートされない)

					string の長さ指定がない場合、in 引数に対してのみ用
					いることができる。

 構造体型			メンバは、スカラー型でなくてはならない

 typedef型          上記のスカラー型、構造体型、ポインタ型の typedef 
                    された型を扱うことができる。元の型として扱われる。
					このためポインタ型や構造体型の場合、元の型名により
					これらを扱うための mruby クラスを指定する。

【対応する mruby の型(クラス)】

  bool_t			true または false。
					mruby で false と見なされるもの(false または nil) 
					を C の false として扱う。mruby で false と見なさ
					れないものを C の true として扱う。整数値 0 は、
					mruby では false とは見なされない。
  char_t			FIXNUM  (値は 0..255)		
  schar_t			FIXNUM  (値は -128..127)		
  uchar_t			FIXNUM  (値は 0..255)		
  intN_t			FIXNUM
  uintN_t			FIXNUM
  double64_t		FLOAT (mruby の FLOAT が double か float かは実装
                          による)
  float32_t			FLOAT
  int				FIXNUM
  short				FIXNUM
  long				FIXNUM
  char				FIXNUM  (char_t と同じ)

・TECS 専用のクラスを用いるもの

  bool_t*			TECS::BoolPointer
  char_t*			TECS::CharPointer
  uchar_t*			TECS::UCharPointer
  schar_t*			TECS::SCharPointer
  intN_t*			TECS::IntNPointer   N:8,16,32,64
  uintN_t*			TECS::UIntNPointer  N:8,16,32,64
  float32_t*		TECS::Float32Pointer
  double64_t*		TECS::Double64Pointer
  int*				TECS::IntPointer
  short*			TECS::ShortPointer
  long*				TECS::LongPointer
  char*				TECS::CharPointer  (char_t* と同じ)

  struct TAG		TECS:TECSStructTAG
  struct TAG*		TECS:TECSStructTAG
                    TAG の部分は、構造体のタグ名に置き換える。関数への
                    引数として、構造体を渡すか、構造体へのポインタを渡
                    すかは、プラグインが自動で判定する。TAG 名を付けて
                    いない構造体を扱うことはできない。typedef した名前
                    で構造体を扱うことはできない。

・その他

  void * は扱えない
  intptr_t は、通常 stdint.h で typedef されているが、元の型として扱う

プラグイン引数として ignoreUnsigned=true を指定すると、無符号整数型へ
のポインタ型を有符号整数型のポインタ型として扱う（値の範囲チェックを含
む）。ポインタを扱うクラスのメモリ使用量を抑えること、有符号、無符号の
使い分けをルーズにすることができる。

ビルド時のオプション (C 言語のマクロ定義) TECS_NO_VAL_CHECK を指定する
ことで、値の範囲チェックを行わないようにすることができる。

【関数名】

・関数名に関数する制限

  関数名 initialize は ruby で既定のメソッド名と重複するため、initialize_cell
 に置換される。

  initialize と initialize_cell の両方が signature 内に存在する場合、エラーと
  なる。エラーを解消するには、少なくとも一方の名前を変更する必要がある。

【戻り値】

戻り値で扱えるのは、以下の型に限られる。

   void, bool_t, char_t, intN_t, float32_t, double64_t
   int, short, long, char
   元の型が、これらである typedef された型

【引数】

・順序の通り対応する mruby の型（クラス）の値を引数として渡す
・指定可能な基本指定子  in, inout, out  (send/receive は不可)
・例外の発生
  ・整数の値が、引数の型の範囲を超えている場合、例外を発生する。
    無符号型の場合、負の値を取ることはできない。ただし、mrb_int と同じ
    またはそれ以上のサイズの場合には、負の値が指定されても例外とはしな
    い。さもないと最上位ビットが 1 の値を入力できないため。
    char_t (char) は無符号と見なされる。
    なお uintptr_t に 0x80000000 にを渡す際に RangeError 例外が発生す
    るが、これは mruby によるものであり、TECS による制限ではない
  ・ポインタ型や構造体型に対応する mruby の型(クラス) と一致しない
  ・size_is, string のサイズが小さい

【参考】

 TECSPointer.txt

  以下の mruby クラス、すなわち TECS のポインタ値を扱うためのクラスに
  ついての説明。
　これらのクラスは必要に応じて生成される。引数として使用されていないク
  ラスは生成されない。

  TECS::Int8Pointer,   TECS::Int16Pointer,   TECS::Int32Pointer
  TECS::Int64Pointer,  TECS::UInt64Pointer
  TECS::UInt8Pointer,  TECS::UInt16Pointer,  TECS::UInt32Pointer
  TECS::CharPointer,   TECS::SCharPointer,   TECS::UCharPointer
  TECS::IntPointer,    TECS::ShortPointer,   TECS::LongPointer
  TECS::UIntPointer,   TECS::UShortPointer,  TECS::ULongPointer
  TECS::BoolPointer,   TECS::Float32Pointer, TECS::Double64Pointer

 TECSStruct.txt

  以下の mruby クラス、すなわち TECS の構造体（ポインタ）値を扱うため
  のクラスについての説明。

  TECS::StructTAG

【TECS CDLによる記述】

 ブリッジプラグインにより、ブリッジセルタイプが自動生成される。
 ブリッジセルは、CDL に記述して設置する必要がある

・ブリッジプラグインの呼出し

mruby=>TECS ブリッジプラグインは、シグニチャプラグインである。CDL 上の以下のよ
うな記述により、mruby=>TECS ブリッジプラグインを呼び出す。

  ex)
    generate( MrubyBridgePlugin, sSignature, "" );

 シグニチャ sSignature は、この記述よりも前に定義されていなくてはならない。
 この generate の記述により、次のようなブリッジセルタイプが生成される。

generate の第3引数は、プラグイン引数（オプション）である。

  ignoreUnsigned   true を指定すると無符号整数へのポインタ型に有符号のポ
                   インタクラスを使用する（メモリを節約できる）
                   デフォルトは false である。
  include          ブリッジに含める関数を指定する。exclude と同時指定不可。
  exclude          ブリッジに含める関数を指定する。include と同時指定不可。

  ex)
    generate( MrubyBridgePlugin, sSignature, "exclude=fun99" );

多くの関数を指定する場合、以下のように、分割した文字列の連接として師定
することができます。

  ex)
    generate( MrubyBridgePlugin, sSignature,
        "ignoreUnsigned=true, "
        "include='func1,func2', "
        "include='func3,func4', "
     );

・ブリッジセルタイプ

 ブリッジセルタイプは、ネームスペース nMruby の下に作られる。ブリッジ
セルタイプの名前は、シグニチャ名に 't' を前置きしたものである。以下は、
ブリッジプラグインにより自動生成されるセルタイプのコードの例である。シ
グニチャ sSignature に対するブリッジセルタイプ名は tsSignature であり、
呼び口cTECS を持つ。

  ex) 自動生成される CDL コード（内部で生成され、解釈される）

   namespace nMruby {
     celltype SSignature{
       call sSignature cTECS;
     };
   };

・ブリッジセルの設置

 ブリッジセルは、ブリッジセルタイプのセルとして生成する。ブリッジセル
 の設置は CDL の記述者が行うものである。ブリッジセルの呼び口 cTECS を 
 mRuby から呼び出したいセルへ結合する。

  ex) ブリッジセルを設置する CDL 記述

   cell nMruby::tsSignature BridgeCell {
     cTECS = Cell.eEnt;
   };

  ex) ブリッジセルを設置する CDL 記述(VM 名指定あり)

   cell nMruby::tsSignature BridgeCell {
     cTECS = Cell.eEnt;
	 VMname = "VM2";
   };

  VMname は mruby VM のセルの名前の指定である。VMname を指定しない場合、
  "VM" が仮定される。VMname は、実際には、VM_TECSInitializer などのよ
  うに、ブリッジを登録するセルの、セル名に用いられる。tecsgen 実行時に
  以下のようなメッセージが出力される。受け口 
  VM_TECSInitializer.eInitialize へ VM の初期化用呼び口から結合する。

   MrubyBridgePlugin: join your VMs cInitialize to VM_TECSInitializer.eInitialize

【mruby コード】

 ブリッジプラグインが適用されたシグニチャに対応する mruby クラスが生成
 される。これをブリッジクラスと呼ぶ。
 ブリッジクラスを通してセルの受け口関数を呼び出すことができる。

・ブリッジクラス

 ブリッジクラスは、モジュール TECS の下に作成される。ブリッジクラスの
 名前は、シグニチャ名に 'T' を前置きしたものとなる。

 ex)  TECS::TsSignature

・ブリッジのインスタンス

  ブリッジクラスにて、ブリッジセル名を引数として new することで、ブリッ
  ジクラスのインスタンスが生成される。ブリッジクラスのインスタンスのこ
  とを、短かくブリッジのインスンタンスとも呼ぶ。ブリッジのインスタンス
  は、ブリッジセルへのポインタ情報を保持するだけである。同一のブリッジ
  のインスタンスは何度でも生成することができる。

 ex)  brdige = TECS::TsSignature.new( "CellName" )

【残課題】

[済] 整数型の unsigned の扱い
   キャストをしているので、コンパイルは警告なく通る。
   値の範囲チェックと整合していない。
   以下を新たに設けるか。
   UInt8Pointer, UInt16Pointer, UInt32Pointer, UInt64Pointer
   UIntPointer, UShortPointer, ULongPointer, UCharPointer, SCharPointer, 

[済] 値の範囲のチェック
   TECSPointer, TECSStruct では整数値を設定する際に、範囲チェックがある。
   メソッドへの直接の引数はチェックされない。

[済] 現状の so としての実装はおかしい
   静的に mruby の本体が結合されている。

[済] ポインタ型を扱うクラスが多すぎる
   使用するクラスだけロードされ、初期化される仕組みが欲しい。

・以下の方法でブリッジを割り付けているが、
    bridge = TsSample.new( "SimpleBridge" )
　以下のように、定数として扱うことができる方がスマートである。
    bridge = TECS::SimpleBridge
  (現状の方が、ブリッジのセルタイプがわかる点が優れている)
  ⇒ もし定数として扱うとして、割付けを C で行うか ruby コードで行うか
     の違いでしかない。以下にあるラッパークラスにより定数の初期化を行
     えばよい。
  ⇒ 定数として扱うと、オブジェクトが最初から最後まで消えないという問
     題がある(それでも少量であるが)

・上記に関連して、VM が異なる場合に、他の VM に割り付けたブリッジ名が
  見えてしまう。アクセス制限が十分でない。
  ブリッジ名のアクセス制限を実現するには、VM を識別する情報が必要にな
  る。mrb_state に p_cellcb を加える方法、mrb_state自身で区別する方法
  が考えられる。要検討。
  なお、セルタイプ単位でのアクセス制 限はできている(てきていないのは、
  同じセルタイプで複数のブリッジセルがある場合に、ブリッジ名のアクセス
  制限)。

・typedef された型名でアクセスできると便利である
  TECSPointer や TECSStruct のクラスに対する alias により、別名として
  アクセスできるとよい。
   ⇒ とりあえず、MrubyBridgePlugin のメッセージとしてわかるようにした。

・send/receive への対応
  mruby と共通のアロケータを指定可能とする (アロケータコンバータプラグインを新規に設けては?)

・構造体メンバーの型制限の緩和
   strign or size_is 指定された char* など

・ruby らしいプログラムから mruby ⇒ TECS ブリッジのクラスへの自動変換するラッパークラスの生成
  (ruby's Array => TECS::Pointer, ruby's Struct => TECS::StructTAG)

[済]ブリッジクラスの new でセルが見つからなかった場合、ruby 例外を発生させる。
  これを free する段階で SEGV が発生するため DATA_SET を早めに行うように変更。
  なお、各メソッドの関数で DATA_PTR が null かどうかをチェックしないが、
  例外が発生したことにより不完全なブリッジは変数に記憶されない（はず）
  ため、問題ない。

