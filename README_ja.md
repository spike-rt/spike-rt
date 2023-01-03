[![Build](https://github.com/spike-rt/spike-rt/workflows/Build/badge.svg?branch=main)](https://github.com/spike-rt/spike-rt/workflows/Build/badge.svg?branch=main)
[![Slack(Japanese)](https://img.shields.io/badge/spike--rt-slack--ja-blue?logo=slack)](https://join.slack.com/t/spike-rt/shared_invite/zt-1lzsqg9zm-_C3sPe_hFsXAWErdSVXKjg)

For English document please see [README.md](README.md).

**プレリリースのため，一部動作が不安定だったり，言及なしに破壊的変更が導入される可能性があります．**

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
    - [x] 加速度センサ及びジャイロスコープ
    - [x] スピーカ
    - [x] バッテリ制御
    - [ ] 32 MB 外部フラッシュメモリ
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
  - [ドキュメント](https://spike-rt.github.io/spike-rt/ja/html/modules.html) を参照．

## 使い方
アプリケーションの開発方法などの詳細については，[docs/ja/](docs/ja/) を参照．

## 貢献方法
バグ報告は，GitHub Issuesへ．
パッチの提供などの貢献は，[docs/ja/CONTRIBUTING.md](docs/ja/CONTRIBUTING.md)を参照．

## Slackコミュニティ
[Slack](https://join.slack.com/t/spike-rt/shared_invite/zt-1lzsqg9zm-_C3sPe_hFsXAWErdSVXKjg) （日本語のみ）にて，質問や議論を受け付けています．

## 言語
IssuesやPull RequestsなどのGitHub上では，日本語と英語を受け付けます．
