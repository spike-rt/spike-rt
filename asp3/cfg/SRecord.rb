# -*- coding: utf-8 -*-
#
#  TOPPERS Configurator by Ruby
#
#  Copyright (C) 2015-2022 by Embedded and Real-Time Systems Laboratory
#              Graduate School of Information Science, Nagoya Univ., JAPAN
#  Copyright (C) 2015 by FUJI SOFT INCORPORATED, JAPAN
#
#  上記著作権者は，以下の(1)〜(4)の条件を満たす場合に限り，本ソフトウェ
#  ア（本ソフトウェアを改変したものを含む．以下同じ）を使用・複製・改
#  変・再配布（以下，利用と呼ぶ）することを無償で許諾する．
#  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
#      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
#      スコード中に含まれていること．
#  (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
#      用できる形で再配布する場合には，再配布に伴うドキュメント（利用
#      者マニュアルなど）に，上記の著作権表示，この利用条件および下記
#      の無保証規定を掲載すること．
#  (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
#      用できない形で再配布する場合には，次のいずれかの条件を満たすこ
#      と．
#    (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
#        作権表示，この利用条件および下記の無保証規定を掲載すること．
#    (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
#        報告すること．
#  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
#      害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
#      また，本ソフトウェアのユーザまたはエンドユーザからのいかなる理
#      由に基づく請求からも，上記著作権者およびTOPPERSプロジェクトを
#      免責すること．
#
#  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
#  よびTOPPERSプロジェクトは，本ソフトウェアに関して，特定の使用目的
#  に対する適合性も含めて，いかなる保証も行わない．また，本ソフトウェ
#  アの利用により直接的または間接的に生じたいかなる損害に関しても，そ
#  の責任を負わない．
#
#  $Id: SRecord.rb 198 2022-11-20 02:57:57Z ertl-hiro $
#

#
#		Sレコードファイル処理クラス
#

#
#  モトローラSレコード形式またはobjdumpコマンドによるダンプ形式のファ
#  イルの内容を変数に読み込み，要求された番地のデータを返す．
#
#  @dumpDataは，先頭番地をキーとし，その番地からのデータ（16進ダンプ形
#  式の文字列）を値とするハッシュである．連続する番地のデータは，1つに
#  まとめて格納する．
#
class SRecord
  def initialize(fileName, format = :srec)
    @dumpData = {}
    File.open(fileName) do |file|
      prevAddress = 0
      prevData = ""
      file.each do |line|
        # ファイルからデータを読み込む
        case format
        when :srec
          address, data = read_line_srec(line)
        when :dump
          address, data = read_line_dump(line)
        else
          error_exit("Unknown file format: #{format}")
        end
        if !address.nil?
          # データを格納する
          if address == prevAddress + prevData.size / 2
            prevData << data
          else
            set_data(prevAddress, prevData) if prevData.size > 0
            prevAddress = address
            prevData = data
          end
        end
      end
      set_data(prevAddress, prevData) if prevData.size > 0
    end
  end

  # 行の読み込み（モトローラSレコード形式）
  def read_line_srec(line)
    # データレコードにより分岐
    case line.slice(0, 2)
    when "S1"
      # データ長（アドレス分[2byte]+チェックサム分1byteを減算）
      length = line.slice(2, 2).hex - 2 - 1

      # アドレス（4文字=2byte）
      address = line.slice(4, 4).hex

      # データ（この時点では文字列で取っておく）
      data = line.slice(8, length * 2)
    when "S2"
      # データ長（アドレス分[3byte]+チェックサム分1byteを減算）
      length = line.slice(2, 2).hex - 3 - 1

      # アドレス（6文字=3byte）
      address = line.slice(4, 6).hex

      # データ（この時点では文字列で取っておく）
      data = line.slice(10, length * 2)
    when "S3"
      # データ長（アドレス分[4byte]+チェックサム分1byteを減算）
      length = line.slice(2, 2).hex - 4 - 1

      # アドレス（8文字=4byte）
      address = line.slice(4, 8).hex

      # データ（この時点では文字列で取っておく）
      data = line.slice(12, length * 2)
    else
      address = nil
    end
    return address, data
  end

  # 行の読み込み（objdumpコマンドによるダンプ形式）
  def read_line_dump(line)
    data = ""
    if line =~ /^ ([0-9a-f]+) (.*)$/
      address = $1.hex
      line = $2
      while line =~ /^([0-9a-f]+) (.*)$/
        data << $1
        line = $2
      end
    else
      address = nil
    end
    return address, data
  end

  # データ取得
  def get_data(address, size)
    endAddress = address + size
    @dumpData.each do |block, blockData|
      if (block <= address && endAddress <= block + blockData.size / 2)
        offset = (address - block) * 2
        return blockData[offset, size * 2]
      end
    end
    return nil
  end

  # データ書込み
  def set_data(address, data)
    endAddress = address + data.size / 2
    @dumpData.each do |block, blockData|
      endBlock = block + blockData.size / 2

      if endBlock < address || endAddress < block
        # 重なりがない
      elsif address < block
        # 新規データの方が先頭番地が小さい
        # ここでは endAddress >= block が成立している
        offset = (endAddress - block) * 2
        data << blockData[offset..-1]
        @dumpData.delete(block)
      else
        # 登録済みデータの方が先頭番地が小さいか同じ
        offset = (address - block) * 2
        address = block
        blockData[offset, data.size] = data
        data = blockData
        @dumpData.delete(block)
      end
    end
    @dumpData[address] = data
  end

  # データのコピー
  def copy_data(fromAddress, toAddress, size)
    copyDataList = {}
    endAddress = fromAddress + size
    @dumpData.each do |block, blockData|
      endBlock = block + blockData.size / 2

      if endBlock <= fromAddress || endAddress <= block
        # 重なりがない
      else
        # 重なっている部分を抽出
        copyFromAddress = [block, fromAddress].max
        copyToAddress = toAddress + (copyFromAddress - fromAddress)
        copySize = [endBlock, endAddress].min - copyFromAddress
        copyDataList[copyToAddress] = get_data(copyFromAddress, copySize)
      end
    end
    copyDataList.each do |copyToAddress, copyData|
      set_data(copyToAddress, copyData)
    end
  end

  # 値としてのデータ取得
  def get_value(address, size, signed)
    targetData = get_data(address, size)
    if targetData.nil?
      return nil
    else
      if $endianLittle
        # リトルエンディアンの場合，バイトオーダーを逆にする
        reverseData = ""
        # 後ろから2文字ずつ抜き出し，並び替える
        while targetData.size > 0
          reverseData << targetData[-2, 2]
          targetData.slice!(-2, 2)
        end
        returnData = reverseData.hex
      else
        returnData = targetData.hex
      end

      # 負の数の処理
      if signed && (returnData & (1 << (size * 8 - 1))) != 0
        returnData -= (1 << (size * 8))
      end
      return returnData
    end
  end

  # 文字列としてのデータ取得
  def get_string(address)
    str = ""
    targetChar = get_data(address, 1).hex
    while (!targetChar.nil? && targetChar != 0)
      str += targetChar.chr
      address += 1
      targetChar = get_data(address, 1).hex
    end
    return str
  end
end
