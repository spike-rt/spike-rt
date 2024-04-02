# -*- coding: utf-8 -*-
#
#  TOPPERS Configurator by Ruby
#
#  Copyright (C) 2015 by FUJI SOFT INCORPORATED, JAPAN
#  Copyright (C) 2015-2022 by Embedded and Real-Time Systems Laboratory
#              Graduate School of Information Science, Nagoya Univ., JAPAN
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
#  $Id: pass2.rb 197 2022-10-22 12:25:44Z ertl-hiro $
#

#
#		パス2の処理
#

#
#  パス1の生成物の読み込み
#
module Cfg1Out
  #
  #  Sレコードファイルからシンボルの値を取り出す
  #
  def self.GetSymbolValue(symbol, size, signed)
    if @symbolAddress.has_key?(symbol)
      return(@cfg1SRec.get_value(@symbolAddress[symbol], size, signed))
    else
      return(nil)
    end
  end

  #
  #  Sレコードファイルからシンボルの文字列を取り出す
  #
  def self.GetSymbolString(symbol)
    if @symbolAddress.has_key?(symbol) \
			&& !(address = @cfg1SRec.get_value(@symbolAddress[symbol], \
												$sizeOfCharPtr, false)).nil?
      return(@cfg1SRec.get_string(address))
    else
      return(nil)
    end
  end

  #
  #  パス1の生成物（静的API以外の部分）の読み込み
  #
  def self.Read
    # cfg1_out.symsの読み込み
    @symbolAddress = ReadSymbolFile(CFG1_OUT_SYMS)

    # cfg1_out.srecまたはcfg1_out.dumpの読み込み
    begin
      if File.exist?(CFG1_OUT_SREC)
        @cfg1SRec = SRecord.new(CFG1_OUT_SREC, :srec)
      else
        @cfg1SRec = SRecord.new(CFG1_OUT_DUMP, :dump)
      end
    rescue Errno::ENOENT, Errno::EACCES => ex
      abort(ex.message)
    end

    # マジックナンバーの取得とエンディアンの設定
    if @symbolAddress.has_key?(CFG1_MAGIC_NUM)
      $asmLabel = ""
      $cfg1_prefix = CFG1_PREFIX
    elsif @symbolAddress.has_key?("_" + CFG1_MAGIC_NUM)
      $asmLabel = "_"
      $cfg1_prefix = "_" + CFG1_PREFIX
    else
      error_exit("`#{CFG1_MAGIC_NUM}' is not found in `#{CFG1_OUT_SYMS}'")
    end
    $globalVars.push("asmLabel")

    magicNumberData = @cfg1SRec.get_data(@symbolAddress \
							[$asmLabel + CFG1_MAGIC_NUM], 4)
    if (magicNumberData == "12345678")
      $endianLittle = false
    elsif (magicNumberData == "78563412")
      $endianLittle = true
    else
      error_exit("`#{CFG1_MAGIC_NUM}' is invalid " \
							"in `#{CFG1_OUT_SREC}' or `#{CFG1_OUT_DUMP}'")
    end
    $globalVars.push("endianLittle")

    # 固定出力した変数の取得
    $sizeOfSigned = GetSymbolValue($asmLabel + CFG1_SIZEOF_SIGNED, 4, false)
    $sizeOfIntptr = GetSymbolValue($asmLabel + CFG1_SIZEOF_INTPTR, 4, false)
    $sizeOfCharPtr = GetSymbolValue($asmLabel + CFG1_SIZEOF_CHARPTR, 4, false)

    # 値取得シンボルの取得
    $symbolValueTable.each do |symbolName, symbolData|
      symbol = $cfg1_prefix + symbolName
      if symbolData.has_key?(:BOOL)
        value = GetSymbolValue(symbol, $sizeOfSigned, true)
        if !value.nil?
          # C言語の真偽値をRubyの真偽値に変換して取り込む
          symbolData[:VALUE] = (value != 0)
        end
      else
        if symbolData.has_key?(:INTPTR)
          value = GetSymbolValue(symbol, $sizeOfIntptr, \
										symbolData.has_key?(:SIGNED))
        else
          value = GetSymbolValue(symbol, $sizeOfSigned, \
										symbolData.has_key?(:SIGNED))
        end
        if !value.nil?
          symbolData[:VALUE] = value
        end
      end
    end

    # SILによるエンディアン定義のチェック
    if $symbolValueTable["SIL_ENDIAN_BIG"].has_key?(:VALUE)
      if $symbolValueTable["SIL_ENDIAN_LITTLE"].has_key?(:VALUE)
        error_exit("Both SIL_ENDIAN_BIG and SIL_ENDIAN_LITTLE are defined.")
      else
        if $endianLittle
          error_exit("Definition of SIL_ENDIAN_BIG seems to be wrong.")
        end
      end
    else
      if $symbolValueTable["SIL_ENDIAN_LITTLE"].has_key?(:VALUE)
        if !$endianLittle
          error_exit("Definition of SIL_ENDIAN_LITTLE seems to be wrong.")
        end
      else
        # 両方が未定義の場合のエラーチェックは，sil.hで実施する
      end
    end

    #
    #  コンフィギュレーション情報を格納するハッシュの初期化
    #
    $cfgData = {}
    @objidValues = {}
    $apiDefinition.each do |apiName, apiDef|
      if apiDef.has_key?(:API)
        if !$cfgData.has_key?(apiDef[:API].to_sym)
          $cfgData[apiDef[:API].to_sym] = {}
        end
      end
      apiDef[:PARAM].each do |apiParam|
        if apiParam.has_key?(:NAME) && apiParam.has_key?(:ID_DEF)
          @objidValues[apiParam[:NAME]] = {}
        end
      end
    end
    $globalVars.push("cfgData")
  end

  #
  #  パラメータの値を取り出す
  #
  #  生成スクリプト内で追加された静的APIの場合には，apiIndexがnilになる．
  #
  def self.GetParamValue(paramName, param, apiIndex, index, apiParam, cfgInfo)
    if apiParam.has_key?(:ID_DEF)				# オブジェクト識別名（定義）
      value = @objidValues[paramName][param]
    elsif apiParam.has_key?(:ID_REF)			# オブジェクト識別名（参照）
      if @objidValues[paramName].has_key?(param)
        value = @objidValues[paramName][param]
      else
        apiDef = $apiDefinition[cfgInfo[:APINAME]]
        error("#{apiDef.has_key?(:KEYPAR) ? "E_NOEXS" : "E_ID"}: " \
					"`#{param}' in #{cfgInfo[:APINAME]} is not defined", \
					"#{cfgInfo[:_FILE_]}:#{cfgInfo[:_LINE_]}:")
        value = nil
      end
    elsif apiParam.has_key?(:STRING)			# 文字列パラメータ
      if !apiIndex.nil?
        symbol = "#{$cfg1_prefix}valueof_#{paramName}_#{apiIndex}#{index}"
        return(GetSymbolString(symbol))
      else
        return(param)
      end
    elsif apiParam.has_key?(:EXPTYPE)			# 整数定数式パラメータ
      if !apiIndex.nil?
        symbol = "#{$cfg1_prefix}valueof_#{paramName}_#{apiIndex}#{index}"
        if apiParam.has_key?(:INTPTR)
          value = GetSymbolValue(symbol, $sizeOfIntptr, \
										apiParam.has_key?(:SIGNED))
        else
          value = GetSymbolValue(symbol, $sizeOfSigned, \
										apiParam.has_key?(:SIGNED))
        end
      else
        if param.is_a?(NumStr)
          return(param)
        else
          return(NumStr.new(param))
        end
      end
    else										# 一般定数式パラメータ
      return(param)
    end
    return(NumStr.new(value, param))
  end

  #
  #  指定したフェーズのためのパス1の生成物の読み込み
  #
  def self.ReadPhase(phase)
    #
    #  オブジェクトIDの割当て
    #
    # 割り当てたオブジェクトIDは，@objidValuesに保持する．@objidValuesは，
    # 2重のハッシュ（ハッシュのハッシュ）である．
    #
    # 具体的には，@objidValuesは，オブジェクトIDのパラメータ名（例えば，セ
    # マフォIDであれば"semid"．これを保持する変数名は，objidParamNameとす
    # る）をキーとし，そのオブジェクトIDの割当て表（これを保持する変数名
    # は，objidListとする）を値とするハッシュである．オブジェクトIDの割当
    # て表は，オブジェクト名（これを保持する変数名は，objNameとする）をキー
    # とし，そのID番号（これを保持する変数名は，objidNumberとする）を値と
    # するハッシュである．
    #
    # 例えば，セマフォSEM1のID番号が1の場合には，次のように設定される．
    #   @objidValues["semid"]["SEM1"] == 1
    #

    # ID番号割り当ての前処理
    $cfgFileInfo.each do |cfgInfo|
      # 静的API以外は読み飛ばす
      next unless cfgInfo.has_key?(:APINAME)

      apiDef = $apiDefinition[cfgInfo[:APINAME]]
      # 異なるフェーズの静的APIは読み飛ばす
      next if apiDef[:PHASE] != phase

      apiIndex = cfgInfo[:INDEX]
      if !apiIndex.nil?
        # シンボルファイルに静的APIのインデックスが存在しなければ読み飛
        # ばす（ifdef等で消えた静的API）
        symbol = "#{$cfg1_prefix}static_api_#{apiIndex}"
        next unless @symbolAddress.has_key?(symbol)
      end

      apiDef[:PARAM].each do |apiParam|
        if apiParam.has_key?(:NAME) && apiParam.has_key?(:ID_DEF)
          objidParamName = apiParam[:NAME]
          objName = cfgInfo[objidParamName]
          if $inputObjid.has_key?(objName)
            # ID番号入力ファイルに定義されていた場合
            @objidValues[objidParamName][objName] = $inputObjid[objName]
          else
            @objidValues[objidParamName][objName] = nil
          end
        end
      end
    end

    # ID番号の割当て処理
    @objidValues.each do |objidParamName, objidList|
      # 未使用のID番号のリスト（使用したものから消していく）
      unusedObjidList = (1.upto(objidList.keys.size)).to_a

      # 割り当て済みのID番号の処理
      objidList.each do |objName, objidNumber|
        if $inputObjid.has_key?(objName)
          objidIndex = unusedObjidList.index($inputObjid[objName])
          if objidIndex.nil?
            # ID番号入力ファイルで指定された値が不正
            error_exit("value of `#{objName}' in ID input file is illegal")
          else
            # 未使用のID番号のリストから削除
            unusedObjidList.delete_at(objidIndex)
          end
        end
      end

      # ID番号の割当て
      objidList.each do |objName, objidNumber|
        if objidList[objName].nil?
          # 以下で，@objidValuesを書き換えている
          objidList[objName] = unusedObjidList.shift
        end
      end
    end

    #
    #  静的APIデータをコンフィギュレーションデータ（$cfgData）に格納
    #
    $cfgFileInfo.each do |cfgInfo|
      # 静的API以外は読み飛ばす
      next unless cfgInfo.has_key?(:APINAME)

      apiDef = $apiDefinition[cfgInfo[:APINAME]]
      # 異なるフェーズの静的APIは読み飛ばす
      next if apiDef[:PHASE] != phase

      apiSym = apiDef[:API].to_sym
      apiIndex = cfgInfo[:INDEX]
      if !apiIndex.nil?
        # シンボルファイルに静的APIのインデックスが存在しなければ読み飛
        # ばす（ifdef等で消えた静的API）
        symbol = "#{$cfg1_prefix}static_api_#{apiIndex}"
        next unless @symbolAddress.has_key?(symbol)
      end

      # パラメータの値をハッシュ形式に格納
      params = {}
      apiDef[:PARAM].each do |apiParam|
        next unless apiParam.has_key?(:NAME)
        paramName = apiParam[:NAME]
        next unless cfgInfo.has_key?(paramName)		# パラメータがない場合
        paramData = cfgInfo[paramName]

        if apiParam.has_key?(:LIST)
          params[paramName.to_sym] = []
          paramData.each.with_index(1) do |param, index|
            params[paramName.to_sym].push(GetParamValue(paramName, param, \
								apiIndex, "_#{index}", apiParam, cfgInfo))
          end
        else
          params[paramName.to_sym] = GetParamValue(paramName, paramData, \
										apiIndex, "", apiParam, cfgInfo)
        end
      end

      # ドメインIDを追加
      if cfgInfo.has_key?(:DOMAIN)
        domainName = cfgInfo[:DOMAIN]
        params[:domain] = $domainId[domainName]
      end

      # クラスIDを追加
      if cfgInfo.has_key?(:CLSIDX) && $classId.has_key?(cfgInfo[:CLSIDX])
        params[:class] = $classId[cfgInfo[:CLSIDX]]
      end

      # API名，ファイル名，行番号を追加
      params[:apiname] = cfgInfo[:APINAME]
      params[:_file_] = cfgInfo[:_FILE_]
      params[:_line_] = cfgInfo[:_LINE_]

      # 登録キーを決定する
      if apiDef.has_key?(:KEYPAR)
        keyParam = params[apiDef[:KEYPAR].to_sym]
        key = keyParam.val
        if $cfgData[apiSym].has_key?(key)
          # 登録キーの重複
          error("E_OBJ: #{apiDef[:KEYPAR]} `#{keyParam}' " \
								"is duplicated in #{cfgInfo[:APINAME]}",
								"#{cfgInfo[:_FILE_]}:#{cfgInfo[:_LINE_]}:")
        end
      else
        key = $cfgData[apiSym].count + 1
      end
      $cfgData[apiSym][key] = params
    end
  end

  #
  #  ID番号の割当て結果の上書き
  #
  def self.SetObjidList(objidParamName, objidList)
    @objidValues[objidParamName] = objidList
  end

  #
  #  ID番号出力ファイルの生成
  #
  def self.OutputId(fileName)
    idOutputFile = GenFile.new(fileName)
    @objidValues.each do |objidParamName, objidList|
      objidList.each do |objName, objidNumber|
        idOutputFile.add("#{objName} #{objidNumber}")
      end
    end
  end
end

#
#  ドメイン関連の処理
#
def DomainProc
  #
  #  ドメインデータ（$domData）を生成
  #
  $domData = {}
  $domainId.each do |domainName, domainVal|
    domid = NumStr.new(domainVal, domainName)
    $domData[domainVal] = { :domid => domid }
  end
  $globalVars.push("domData")
end

#
#  クラス関連の処理
#
def ClassProc
  #
  #  クラス定義ファイルを実行する
  #
  $classFileNames.each do |classFileName|
    IncludeTrb(classFileName)
  end

  #
  #  クラスのリストの加工
  #
  $clsData.each do |_, params|
    bitmap = 0
    params[:affinityPrcList].each do |prcid|
      bitmap |= (1 << (prcid - 1))
    end
    params[:affinityPrcBitmap] = bitmap
  end

  #
  #  クラス記述がエラーの場合に以降のエラーを防ぐために使用するクラス
  #
  $TCLS_ERROR = NumStr.new($clsData.keys.first, "")

  #
  #  クラスID情報（$classId）の生成
  #
  $classId = {}
  $cfgFileInfo.each do |cfgInfo|
    if cfgInfo.has_key?(:CLSSTR)
      symbol = "#{$cfg1_prefix}clsid_#{cfgInfo[:CLSIDX]}"
      value = Cfg1Out.GetSymbolValue(symbol, $sizeOfSigned, true)
      if !value.nil?
        if $clsData.has_key?(value)
          $classId[cfgInfo[:CLSIDX]] = NumStr.new(value, cfgInfo[:CLSSTR])
        else
          error("E_ID: illegal class `#{cfgInfo[:CLSSTR]}'",
								"#{cfgInfo[:_FILE_]}:#{cfgInfo[:_LINE_]}:")
          # 以降のエラーの抑止
          $classId[cfgInfo[:CLSIDX]] = $TCLS_ERROR
        end
      end
    end
  end
  $globalVars.push("classId")
end

#
#  パス2の処理
#
def Pass2
  #
  #  パス1から引き渡される情報をファイルから読み込む
  #
  db = PStore.new(CFG1_OUT_DB)
  db.transaction(true) do
    db.roots.each do |var|
      eval("$#{var} = db[:#{var}]")
    end
  end

  #
  #  パス1の生成物（静的API以外の部分）を読み込む
  #
  Cfg1Out.Read()
  abort if $errorFlag					# エラー発生時はabortする

  #
  #  値取得シンボルをグローバル変数として定義する
  #
  DefineSymbolValue()

  #
  #  ドメイン関連の処理
  #
  if $supportDomain
    DomainProc()
  end

  #
  #  クラス関連の処理
  #
  if $supportClass
    ClassProc()
  end

  #
  #  生成スクリプト（trbファイル）を実行する
  #
  Cfg1Out.ReadPhase(nil)
  $trbFileNames.each do |trbFileName|
    if /^(.+):(\w+)$/ =~ trbFileName
      trbFileName = $1
      Cfg1Out.ReadPhase($2.to_sym)
    end
    IncludeTrb(trbFileName)
  end

  #
  #  ID番号出力ファイルの生成
  #
  if !$idOutputFileName.nil?
    Cfg1Out.OutputId($idOutputFileName)
  end

  #
  #  パス3に引き渡す情報をファイルに生成
  #
  if !$omitOutputDb
    db = PStore.new(CFG2_OUT_DB)
    db.transaction do
      $globalVars.each do |var|
        eval("db[:#{var}] = $#{var}")
      end
    end
  end
end
