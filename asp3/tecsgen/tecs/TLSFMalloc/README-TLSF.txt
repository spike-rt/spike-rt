TLSF Malloc アロケータ
                                          2017.4.15
                                          大山
1.概要

TLSF は効率よく、フラグメントが起きにくいアロケータです。 OS 非依存に
実装されているため、組込みシステムへの応用に適しています。この実装では
排他制御を行っていません。複数のタスクで共用するには、排他制御を行う必
要があります。

2. 実装について

1) TLSF アロケータの本体は、TECS ジェネレータパッケージには含まれていません。
以下に公開されている TLSF を入手する必要があります。
これは GPL/LGPL ライセンスで公開されています。
  http://www.gii.upv.es/tlsf/main/docs

2) TLSF には、別に実装され MIT ライセンスで公開されているものもあります。
API が若干異なるため、tTLSFMalloc_inline.h を少し修正する必要があります。
  https://github.com/mattconte/tlsf

3) ETロボコン用パッケージ
mruby-on-ev3rt+tecs_package (ev3rt) には、1) の TLSF が含まれています。
