For English document please see [README.md](README.md).

# SPIKE-RT
[LEGO® Education SPIKE™ Prime](https://education.lego.com/ja-jp/products/-spike-/45678#spike%E3%83%97%E3%83%A9%E3%82%A4%E3%83%A0) 向け RTOS ベースのソフトウェアプラットフォーム．

## 機能
- [TOPPERS/ASP3](https://toppers.jp/asp3-kernel.html) リアルタイム OS．  
  - µITRON4.0仕様の拡張仕様であるTOPPERS第3世代カーネル（ITRON系）仕様．
- C 言語プログラミング環境．
- デバイスドライバ．
  - Hub 本体の機能
    - [x] 5x5 LED マトリックスディスプレイ
    - [x] ライト
    - [x] ボタン
    - [ ] 加速度センサ及びジャイロスコープ
    - [x] スピーカ
    - [x] バッテリ制御
    - [ ] 32 MB 外部フラッシュメモリ（次の Pybricks のリリースで対応予定）
    - [x] USB Serial
    - [x] Bluetooth
  - PUP デバイス
    - [x] アンギュラーモータ
    - [x] カラーセンサ
    - [x] 超音波センサ
    - [x] フォースセンサ
- USB DFU による書き込み．

## API
- ASP3 API
  - 「[TOPPERS第3世代カーネル（ITRON系）統合仕様書](https://www.toppers.jp/docs/tech/tgki_spec-350.pdf)」を参照．
- C 標準ライブラリ（Newlib）
- SPIKE API
  - Pybricks の API を参考にした アプリケーションからデバイスを制御するための API．
  - ドキュメント(TODO: Doxygen への URL) を参照．

## 詳細
アプリケーションの開発方法などの詳細については，[docs/ja/](docs/ja/) を参照．
