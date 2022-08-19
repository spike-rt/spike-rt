# 開発環境の構築
- ここでは，WSL2 (Windows), Mac, Ubuntu(Linux)上で環境を構築することを想定．
- ビルドのみをアプリケーション・コンテナ（Docker など）上で行う方法を紹介する．
- ※　Windowsの場合は，UbuntuイメージのWSL2上での作業を想定．
- ※　Linuxの場合は，Ubuntuを想定．

各作業：
| 作業内容 | 実行方法 |
| :--- | :--- |
| コーディング | 任意のエディタ（ホスト） |
| [ビルド](#ビルド環境の構築) | コンパイラ（コンテナ） |
| [書き込み](#書き込み環境の構築) | python3（ホスト） |
| [シリアル接続](#シリアル接続)（任意） | minicomなど（ホスト） |


## ビルド環境の構築
### アプリケーション・コンテナエンジンのインストール


### コンテナ・イメージのビルド
```bash
docker build tools/ -t spike-rt-builder
```

### コンテナの起動とアタッチ
ビルドの際は，以下によりソースコードをコンテナにマウントして，コンテナに起動及びアタッチする．
```bash
docker run --rm -it -v $(pwd):$(pwd) -w $(pwd) spike-rt-builder /bin/bash
```

Windows(非 WSL2 であり、Windows の Docker Desktop を直接使う)の場合(ホスト側と
コンテナ側のパス表記が異なるため、それぞれをフルパスで指定する。ここではユーザ名を user1 とし、C:\Users\users\repo に spike-rt を git clone したものとする)

```winbatch
+docker run --rm -it -v C:\Users\user1\repo\spike-rt:/home/user1/repo/spike-rt -w /home/user1/repo/spike-rt spike-rt-builder /bin/bash
```


## 書き込み環境の構築
Prime Hubへの書き込みをホスト側で行うことを想定．  
PyUSB(Python3) を用いて書き込みを行う．

### PyUSBの導入の前準備
WSL2(Windows)とMacについては，前準備が必要．

#### WSL2(Windows)の場合
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

## シリアル接続
`minicom`によりHubとシリアル通信することを想定．  
以下などの方法により`minicom`をインストールする．

WSL2(WIndows)またはUbuntu(Linux)の場合
```bash
sudo apt install minicom
```

Macの場合
```bash
brew install minicom
```