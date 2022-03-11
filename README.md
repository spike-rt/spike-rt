### GCC
- gcc-arm-none-eabi-5_4-2016q3
  - https://launchpad.net/gcc-arm-embedded/5.0/5-2016-q3-update

### ターゲットバイナリのビルド
```shell-sessions
$ mkdir -p build/obj-primehub_pybricks
$ cd build/obj-primehub_gcc
$ ../../asp3/configure.rb -T primehub_gcc -a ../../sample/ -A idle
$ make
```

### 使用しているOSS
- TOPPERS/ASP3カーネル Nucleo F401RE簡易パッケージ 
  - https://www.toppers.jp/download.cgi/asp3_nucleo_f401re_gcc-20211228.zip
  - 「TOPPERS第3世代カーネル（ITRON系）統合仕様書」, https://www.toppers.jp/docs/tech/tgki_spec-350.pdf
-  STM32Cube MCU Full Package for the STM32F4 series
  - https://github.com/STMicroelectronics/STM32CubeF4/tree/v1.26.2
- QEMU
  - v6.1.0 よりフォーク
