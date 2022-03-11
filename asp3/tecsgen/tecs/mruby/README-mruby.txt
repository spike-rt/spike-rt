mruby
                                              2017.4.17  新規
                                              2017.11.4  
                                              大山 博司

■概要

mruby ディレクトリには、組込み用 mruby VM のセルタイプ、および
MrubyBridgePlugin に対応したモジュールが含まれます．

mruby V1.2.0 に対応しています．V1.1.0 でもビルドすることは可能です．

ここには mruby の本体は含まれません．以下からダウンロードする必要があります．
   http://forum.mruby.org/

■TECS ジェネレータ V1.5 での変更

tMruby.cdl (パス：tecsgen/tecs/mruby/tMruby.cdl) を新設した．
以下を可能にするために tMruby を composite に分解した．
 * mruby スクリプトの周期タスク実行
 * VM ごとに異なる TLSF アロケータコンポーネントを使用

tMruby.cdl では tMruby (ネームスペースパス：nMruby::tMruby) の実装を
新しくした他、周期タスク実行用の tMrubyCyclic (同：nMruby::tMrubyCyclic)
を新設した．tMruby を分解した、tMrubyVM セルタイプと、そのシグニチャ
sMruby (sMruby.cdl内) も新設した．

tMruby.cdl に置き換え可能だが、以下の点でビルド用 Makefile の変更が必要．
 * TLSF ヘッダ tTLSFMalloc_inline.h へのパスを追加
 * TLSF のオブジェクトの追加 (mruby+TECS on EV3 には実装済み)

また、以下の点に留意する必要がある．
 * TLSF は、VM ごとに 1MB を割り付ける 

このため、十分な空きメモリが必要である．
このサイズは VM セルの定義で変更可能 (属性 memoryPoolSize)．

■ コンポーネント概要

tMruby.cdl: (TECS ジェネレータ V1.5.0 以降)
 celltype  tMrubyVM
    VM を実装するためのセルタイプ．メモリアロケータ TLSFMalloc を含ま
    ない. 受け口 eMrubyVM のシグニチャは sMruby．

 composite tMruby
    受け口 eMrubyBody のシグニチャを sTaskBody とする．tTLSFMalloc も
    組み入れ.

 composite tMrubyCyclic
    受け口 eMrubyBody のシグニチャを sTaskBody とする．tTLSFMalloc も
    組み入れ.
    周期タスク起床を前提としている．
    一回目の呼出しでは mrb_open し、ブリッジセルを登録後、mrb_run を呼
    び出してスクリプト実行を行う．スクリプト終了後に mrb_close は呼び
    出されない．
    二回目以降の呼出しでは mrb_run のみ呼び出される．
    終了処理 mrb_close を呼びだす手段は提供していない．突然電源が切ら
    れることを前提としている． (少なくとも最初の実装では)．

sMruby.cdl：
 signature sMruby
   初期化、実行、終了処理に分解したシグニチャ
