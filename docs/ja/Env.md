# 開発環境の構築
WSL2 (Windows), Mac, Linux で環境を構築することが可能である・
ここでは，Dockerコンテナを用いる．もちろん，`tools/Dockerfile` を参考にして，ホストやシステム・コンテナなどに一括して環境構築しても良い．
アプリケーション・コンテナの制約から以下のようにビルドのみをコンテナ上で行うことを想定．
また，書き込みには，python3 を用いる．
| 作業内容 | 実行方法 |
| :--- | :--- |
| コーディング | 任意のエディタ（ホスト） |
| ビルド | コンテナ |
| 書き込み | python3（ホスト） |
| シリアル接続（任意） | ホスト |

## ビルド環境の構築
### コンテナ・イメージのビルド
```bash
docker build tools/ -t spike-rt-builder
```

### コンテナの起動
ビルドの際は，以下によりソースコードをコンテナにマウントして，コンテナに起動及びアタッチする．
```bash
docker run --rm -it -v $(pwd):$(pwd) -w $(pwd) spike-rt-builder /bin/bash
```

## 書き込み環境の構築
ホスト側で行うことを想定．python3 を用いて書き込みを行う．
以下のコマンドで必要パッケージ[pyusb](https://pypi.org/project/pyusb/)のインストールを行う．ホストのpython環境を汚さないために，venv環境下でインストールする．
ここでは，`./tools/python/`以下にインストールすることを想定する． 
```bash
python3 -m venv ./tools/python
./tools/python/bin/pip install pyusb
```
