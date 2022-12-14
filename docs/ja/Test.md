# テスト
SPIKE-RT 自体のテストを実行する方法を説明する．

## テストの実装
[test/](../../test)以下にテストが実装されている．テストの実現にテストフレームワークUnityを用いている．  
Unityについては[external/Unity/README.md](https://github.com/ThrowTheSwitch/Unity/tree/0b899aec14d3a9abb2bf260ac355f0f28630a6a3)を参照．

## テストのビルド
アプリケーションと同様にビルドできる．
[DevelopApp.md](DevelopApp.md) を参考にビルドしてもよいが，スクリプト[scripts/build-test.sh](../../scripts/build-test.sh)によりビルド作業を簡略化できる．
```bash
./scripts/build-test.sh
```

## PUP デバイスの接続
[test/test_config.h](../..//test/test_config.h)に従って，PUP デバイスの接続を行う．  
ただし，接続しなくても，LED マトリックスディスプレイに`a`が表示されることを以て動作確認することも可能．

## 書き込み
[DevelopApp.md#書き込み](DevelopApp.md#書き込み)を参照．ただし，obj-primehub_appを`obj-primehub_test`と読み替える．

## シリアル通信
Port FのUSART経由のシリアル通信でテスト結果を確認可能．
以下により接続．
```bash
sudo minicom -D /dev/ttyACM0 -b 115200
```

## Hub の再起動& 動作確認
Hub　の再起動をし，10秒ほど待って，シリアル出力から以下のような表示が得られたら，テストが成功
```bash
.....

-----------------------
7 Tests 0 Failures 0 Ignored 
```
