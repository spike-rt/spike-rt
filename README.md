### GCC
- gcc-arm-none-eabi-5_4-2016q3
  - https://launchpad.net/gcc-arm-embedded/5.0/5-2016-q3-update

### ターゲットバイナリのビルド
```shell-sessions
$ mkdir -p build/obj-primehub_gcc
$ cd build/obj-primehub_gcc
$ ../asp3/configure.rb -T primehub_gcc -f
$ make
```
```shell-sessions
$ mkdir obj
$ cd obj
$ ../asp3/configure.rb -T primehub_gcc -f -A pybricks-micropython -a -L 
$ make
```

### QEMU
#### 依存パッケージのインストール
[Hosts/Linux - QEMU](https://wiki.qemu.org/Hosts/Linux) 参考に依存パッケージをインストールする．
#### QEMUのビルド
```shell-sessions
$ mkdir -p build/qemu-primehub
$ cd build/qemu-primehub
$ ../../qemu-primehub/configure --prefix=/home/ubuntu/tools/ --target-list=arm-softmmu,aarch64-softmmu
```

#### ターゲットバイナリをQEMU上で実行
```shell-sessions
$ qemu-system-arm -cpu cortex-m4 -machine legospike \
-nographic \
-kernel /home/ubuntu/asp3-primehub/obj-primehub_gcc/asp \
-serial null -serial null -serial null -serial null -serial null -serial null -serial mon:stdio
```


### 使用しているOSS
- TOPPERS/ASP3カーネル Nucleo F401RE簡易パッケージ 
  - https://www.toppers.jp/download.cgi/asp3_nucleo_f401re_gcc-20200414.zip
  - 「TOPPERS第3世代カーネル（ITRON系）統合仕様書」, https://www.toppers.jp/docs/tech/tgki_spec-350.pdf
-  STM32Cube MCU Full Package for the STM32F4 series
  - https://github.com/STMicroelectronics/STM32CubeF4/tree/v1.26.2
- QEMU
  - v6.1.0 よりフォーク
