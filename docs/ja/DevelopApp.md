# アプリケーションの開発方法
アプリケーション開発における一連の流れを説明する．

## ソースコードの入手
以下により，ソースコードをクローンする．
```bash
git clone git@github.com:spike-rt/spike-rt.git
cd spike-rt
git submodule update --init ./external/
```
以下，特に断りの無い限りトップディレクトリが`spike-rt` のディレクトリであるとする．
また，アプリケーション・ソースコードのディレクトリが `spike-rt/$appdir`に配置されているとする．


## 開発環境の構築
[Env.md](Env.md)に従って，開発環境を構築する．

## ビルド
コンテナ上でターゲットバイナリの生成を行う．
使用するMakefile のテンプレートが異なるため，カーネルとアプリケーションを分割してコンフィグ及びビルドする．

### コンテナの起動及びシェルへのアタッチ
```bash
docker run --rm -it \
  -v $(pwd):$(pwd) -w $(pwd) \
  -u "$(id -u $USER):$(id -g $USER)" -v /etc/passwd:/etc/passwd:ro -v /etc/group:/etc/group:ro \
  ghcr.io/spike-rt/spike-rt:rich /bin/bash
```

### コンフィグ（初回時のみ）
ビルドの前に テンプレートからビルド用 Makefile の生成を行う．初回時のみ必要である．
カーネルとアプリケーションを分割して，ビルドすることを想定している．

カーネル用 Makefile の生成
```bash
mkdir -p build/obj-primehub_kernel
cd build/obj-primehub_kernel
../../asp3/configure.rb -T primehub_gcc -f -m ../../common/kernel.mk
cd ../..
```

アプリケーション用 Makefile の生成
```bash
mkdir -p build/obj-primehub_$appname
cd build/obj-primehub_$appname
../../asp3/configure.rb -T primehub_gcc -L ../obj-primehub_kernel -a ../../$appdir/ -A $appname -m ../../common/app.mk
cd ../..
```

### ビルド
ホスト側で以下を実行し，カレントディレクトリをアプリのビルドディレクトに移動する．
```bash
cd build/obj-primehub_$appname
```
以下により，
```bash
(cd ../obj-primehub_kernel && && make libkernel.a) && rm -rf asp asp.bin && make
```
再ビルド・再リンクする場合もこのコマンドで良い．

### 例
モータのサンプル・アプリケーション`sample/motor`のビルド例．

```bash
docker run --rm -it -v $(pwd):$(pwd) -w $(pwd) spike-rt-builder /bin/bash

mkdir -p build/obj-primehub_kernel
cd build/obj-primehub_kernel
../../asp3/configure.rb -T primehub_gcc -f -m ../../common/kernel.mk
make libkernel.a
cd -

mkdir -p build/obj-primehub_motor
cd build/obj-primehub_motor
../../asp3/configure.rb -T primehub_gcc -L ../obj-primehub_kernel -a ../../sample/motor/ -A motor -m ../../common/app.mk

make
cd -

exit
```


## 書き込み
ホスト側で以下を実行し，カレントディレクトリをアプリのビルドディレクトに移動する．（既に移動している場合は不要）
```bash
cd build/obj-primehub_$appname
```

### HubのDFUモードへの遷移
- USBケーブルを抜き，Hubの電源を切る．
- Bluetoothボタンを押しながらUSBを接続する．
- ボタンのLEDが点滅するまで，Bluetoothボタンを押し続ける．

### 書き込みの実行
(TODO: これは，Mac でも動作可能？)
ここでは，USBの権限の問題から，管理者権限により書き込みを行う．
`PYTHON3`でpythonの実行ファイルを指定し，以下により，Hubに書き込む．
```bash
PYTHON3=../../tools/python/bin/python3 sudo ../../scripts/deploy-dfu.sh asp.bin     
```

以下のように`make deploy-dfu`を管理者権限で実行することでも可能だが，make の依存関係の問題で上手く動かないことがある．
```bash
sudo make deploy-dfu PYTHON3=../../tools/python/bin/python3
```

## USBシリアルの接続
(TODO: これは，Mac でも動作可能？)
USBシリアルに接続し，ログ出力を確認することができる．
一旦，Hub の電源をONにしてからでないと，接続できないことに注意する．
```bash
sudo minicom -D /dev/ttyACM0 -b 115200
```
