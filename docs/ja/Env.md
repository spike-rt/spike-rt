# 開発環境の構築
- ここでは，WSL2(Windows), Mac, Ubuntu(Linux)上で環境を構築することを想定．
- ビルドのみをアプリケーション・コンテナ（Docker など）上で行う方法を紹介する．
- ※　Linuxの場合は，Ubuntuを想定．
- ※　Macの場合は，Docker Desktop for Macの使用を想定．
- ※　Windowsの場合は，UbuntuイメージのWSL2上での作業を想定．Docker Desktop for Windowsでも動作報告あり．

各作業：
| 作業内容 | 実行方法 |
| :--- | :--- |
| コーディング | 任意のエディタ（ホスト） |
| [ビルド](#ビルド環境の構築) | コンパイラ（コンテナ） |
| [書き込み](#書き込み環境の構築) | python3（ホスト） |
| [シリアル通信](#シリアル通信)（任意） | minicomなど（ホスト） |
| [Bluetoothシリアル通信](#Bluetoothシリアル通信)（任意） | Google Chromeなど（ホスト） |


## ビルド環境の構築
### コンテナ・イメージのプル
`slim`版または`rich`版のコンテナをプルする．
通信回線や容量の理由がない限り，`rich`版を選択することを想定する．
```bash
docker pull ghcr.io/spike-rt/spike-rt:rich
```

### コンテナの起動とアタッチ
ビルドの際は，ソースコードをコンテナにマウントして，コンテナに起動及びアタッチする．

#### LinuxまたはWSL2(Windows)の場合
以下を実行．
2行目のオプションでソースコードをマウントし，ワークディレクトリとして指定する．
3行目のオプションでファイルのパーミッションの問題を解決する．
```bash
docker run --rm -it \
  -v $(pwd):$(pwd) -w $(pwd) \
  -u "$(id -u $USER):$(id -g $USER)" -v /etc/passwd:/etc/passwd:ro -v /etc/group:/etc/group:ro \
  ghcr.io/spike-rt/spike-rt:rich /bin/bash
```

#### Macの場合
Docker Desktop for Macでは，以下を実行．
このコマンドでも，ファイルのパーミッションの問題を起こさない模様．
```bash
docker run --rm -it -v $(pwd):$(pwd) -w $(pwd) ghcr.io/spike-rt/spike-rt:rich /bin/bash
```

#### ※ Windows（非 WSL2 であり，Docker Desktop for Windows を直接使う)の場合
ホスト側とコンテナ側のパス表記が異なるため、それぞれをフルパスで指定する．
ここではユーザ名を user1 とし，C:\Users\users\repo に spike-rt を git clone したものとする

```winbatch
+docker run --rm -it -v C:\Users\user1\repo\spike-rt:/home/user1/repo/spike-rt -w /home/user1/repo/spike-rt ghcr.io/spike-rt/spike-rt:rich  /bin/bash
```

## 書き込み環境の構築
Prime Hub への書き込みをホスト側で行うことを想定．  
PyUSB(Python3) を用いて書き込みを行う．

### PyUSBの導入の前準備
WSL2(Windows) と Mac については，前準備が必要．

#### WSL2(Windows) の場合
[USB デバイスを接続する | Microsoft Docs](https://docs.microsoft.com/ja-jp/windows/wsl/connect-usb)に従って，USB/IPによりWSL2からUSBデバイスを接続できるようにする． 

以下によりPrime HubのUSBをWSL2にアタッチする．
```powershell
usbipd wsl attach --busid <busid>
```

`usbipd wsl list`(PowerShell)や`lsusb`(WSL2)などでUSBデバイスが表示されない場合は，`3240/tcp`ポートをWindows側のファイアーウォールがブロックしてないかを確認する．
 
#### Macの場合
PyUSBの依存ライブラリのインストール
```bash
brew install libusb
```

### PyUSBの導入
以下のコマンドで必要パッケージPyUSB([PyPI](https://pypi.org/project/pyusb/), [GitHub](https://github.com/pyusb/pyusb))のインストールを行う．  
ホストのpython環境を汚さないために，venv環境下でインストールする．ここでは，`./tools/python/`以下にインストールすることを． 
```bash
python3 -m venv ./tools/python
./tools/python/bin/pip install pyusb
```

## シリアル通信
USBまたはUSART(Port F)経由のシリアル通信について説明．
Bluetoothの場合は，[Bluetoothシリアル通信](#Bluetoothシリアル通信)を参照．

`minicom`によりHubとシリアル通信することを想定．  
以下などの方法により`minicom`をインストールする．

WSL2(Windows)またはUbuntu(Linux)の場合
```bash
sudo apt install minicom
```

Macの場合
```bash
brew install minicom
```

## Bluetoothシリアル通信
SPIKE-RTでは，Pybricksのデバイスドライバを再利用している．
現状では，Pybricksの[Web IDE](https://beta.pybricks.com/)経由でのBluetooth接続のみに対応している．

Pybricksの[Web IDE](https://beta.pybricks.com/)は，Firefox等には対応していない．
Google ChromeやChromiumブラウザを使用する．
