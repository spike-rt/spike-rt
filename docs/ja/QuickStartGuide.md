# クイックスタートガイド(WIP!, TODO)
とりあえず，動かしてみたい人のために，サンプル・アプリケーションを動かす方法を説明する．
各作業の詳しい説明は，[DevelopApp.md](DevelopApp.md)を参照．

## ソースコードの入手
```bash
git clone https://github.com/spike-rt/spike-rt.git
cd spike-rt
```

## 開発環境の構築
[Env.md](Env.md)に従って，開発環境を構築する．

## ビルド
### コンフィグ（初回時のみ）
```bash
mkdir -p build/obj-primehub_kernel
cd build/obj-primehub_kernel
../../asp3/configure.rb -T primehub_gcc -f -m ../../common/kernel.mk
cd ../..
```

```bash
mkdir -p build/obj-primehub_led
cd build/obj-primehub_led
../../asp3/configure.rb -T primehub_gcc -L ../obj-primehub_kernel -a ../../sample/led -A led -m ../../common/app.mk
```

### ビルド
```bash
(cd ../obj-primehub_kernel && && make libkernel.a) && rm -rf asp asp.bin && make
```

## 書き込み
```bash
PYTHON3=../../tools/python/bin/python3 sudo ../../scripts/deploy-dfu.sh asp.bin     
```
