#!/usr/bin/env ruby
# -*- coding: utf-8 -*-
#
#  TOPPERS Software
#      Toyohashi Open Platform for Embedded Real-Time Systems
# 
#  Copyright (C) 2016-2022 by Embedded and Real-Time Systems Laboratory
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
#  $Id: testcfg.rb 1739 2022-12-05 14:45:14Z ertl-hiro $
# 

Encoding.default_external = 'utf-8'
require "optparse"
require "pp"

#  オプションの定義
#
#  -c，--copy			生成されたファイルをテスト期待値とする

#
#  オプションの処理
#
$copy = false;
OptionParser.new do |opt|
  opt.on("-c", "--copy", "copy the generated files to the expected files") do
    $copy = true
  end
  opt.parse!(ARGV)
end

#
#  テストプログラム毎に必要なオプションの定義
#
CFG_TEST_SPEC = {
  # ビルドに成功するテスト
  "cfg_all1"   => { },
}

PASS3_TEST_SPEC = {
  # パス3でエラーになるテスト
  "pass3_all1" => { },
}

PASS2_TEST_SPEC = {
  # パス2でエラーになるテスト
  "pass2_cfg1" => { },
  "pass2_int1" => { },
  "pass2_obj1" => { },
  "pass2_obj2" => { },
  "pass2_task1" => { },

  # サブ優先度機能拡張パッケージの機能テストプログラム
  "pass2_subprio1" => { },
}

PASS1_TEST_SPEC = {
  # パス1でエラーになるテスト
  "pass1_cfg1" => { },
}

#
#  カーネルライブラリの作成
#
def BuildKernel
  if !Dir.exist?("KERNELLIB")
    Dir.mkdir("KERNELLIB")
  end

  Dir.chdir("KERNELLIB") do
    puts("== building: KERNELLIB ==")
    configCommand = "ruby #{$usedSrcDir}/configure.rb"
    configCommand += " #{$targetOptions[0]}"
    configCommand += " -c empty.cfg"
    puts(configCommand)
    system(configCommand)
    system("touch empty.cfg")
    system("make libkernel.a")
    if File.exist?("Makefile.bak")
      File.delete("Makefile.bak")
    end
  end
end

#
#  テストプログラムの作成
#
def BuildTest(test, testSpec, errorRedirect, mkdirFlag=false, rmFlag=false)
  objDir = test.tr("a-z_", "A-Z-")

  if !Dir.exist?(objDir)
    if mkdirFlag
      Dir.mkdir(objDir)
    else
      return
    end
  end

  Dir.chdir(objDir) do
    if rmFlag
      puts("== removing timestamp files: #{objDir} ==")
      system("rm *.timestamp")
    end

    puts("== building: #{objDir} ==")
    configCommand = "ruby #{$usedSrcDir}/configure.rb"
    if testSpec.has_key?(:TARGET)
      configCommand += " #{$targetOptions[testSpec[:TARGET]]}"
    else
      configCommand += " #{$targetOptions[0]}"
    end
    configCommand += " -a #{$usedSrcDir}/test_cfg"

    if (!testSpec.has_key?(:TARGET) || testSpec[:TARGET] == 0)
      configCommand += " -L ../KERNELLIB"
    end
    if testSpec.has_key?(:SRC)
      configCommand += " -A #{testSpec[:SRC]}"
    else
      configCommand += " -A #{test}"
    end
    if testSpec.has_key?(:CFG)
      configCommand += " -c #{testSpec[:CFG]}.cfg"
    end
    if testSpec.has_key?(:SYSOBJ)
      configCommand += " -S \"" \
			+ testSpec[:SYSOBJ].split(/\s+/).map{|f| f+".o"}.join(" ") \
			+ "\""
    end
    if testSpec.has_key?(:APPLOBJ)
      configCommand += " -U \"" \
			+ testSpec[:APPLOBJ].split(/\s+/).map{|f| f+".o"}.join(" ") \
			+ "\""
    end
    if testSpec.has_key?(:DEFS)
      configCommand += " -O \"#{testSpec[:DEFS]}\""
    end
    if testSpec.has_key?(:OPTS)
      configCommand += " #{testSpec[:OPTS]}"
    end
    puts(configCommand)
    system(configCommand)

    if testSpec.has_key?(:ML_MANUAL)
      # 手動メモリ配置の場合は，リンカスクリプトをコピーする
      cpCommand = "cp #{$usedSrcDir}/test_cfg/#{test}/ldscript.ld ."
      puts(cpCommand)
      system(cpCommand)
    end

    makeCommand = "make"
    if errorRedirect
      makeCommand += " 2> error.txt"
    end
    system(makeCommand)

    if File.exist?("Makefile.bak")
      File.delete("Makefile.bak")
    end
  end
end

#
#  全テストプログラムの作成
#
def BuildAllTest()
  CFG_TEST_SPEC.each do |test, testSpec|
    BuildTest(test, testSpec, false)
  end
  PASS3_TEST_SPEC.each do |test, testSpec|
    BuildTest(test, testSpec, true)
  end
  PASS2_TEST_SPEC.each do |test, testSpec|
    BuildTest(test, testSpec, true)
  end
  PASS1_TEST_SPEC.each do |test, testSpec|
    BuildTest(test, testSpec, true)
  end
end

#
#  ファイルの比較
#
def diffFile(test, filename, objDir)
  diffCommand = "diff #{$usedSrcDir}/test_cfg/#{test}/#{filename} #{filename}"
  puts(diffCommand)
  ret = system(diffCommand)
  if !ret
    if ($copy)
      cpCommand = "cp #{filename} #{$usedSrcDir}/test_cfg/#{test}/"
      puts(cpCommand)
      system(cpCommand)
    else
      puts("#TODO# cp #{objDir}/#{filename} #{$srcDir}/test_cfg/#{test}/")
    end
  end
end

#
#  テスト結果のチェック（パス3まで通過するテスト）
#
def CfgTest(test, testSpec)
  objDir = test.tr("a-z_", "A-Z-")
  return unless Dir.exist?(objDir)

  Dir.chdir(objDir) do
    puts("== checking: #{objDir} ==")
    diffFile(test, "kernel_cfg.h", objDir)
    diffFile(test, "kernel_cfg.c", objDir)
  end
end

#
#  テスト結果のチェック（途中でエラーになるテスト）
#
def ErrorTest(test, testSpec)
  objDir = test.tr("a-z_", "A-Z-")
  return unless Dir.exist?(objDir)

  Dir.chdir(objDir) do
    puts("== checking: #{objDir} ==")
    diffFile(test, "error.txt", objDir)
  end
end

#
#  全テストプログラムのテスト結果のチェック
#
def ExecAllTest()
  CFG_TEST_SPEC.each do |test, testSpec|
    CfgTest(test, testSpec)
  end
  PASS3_TEST_SPEC.each do |test, testSpec|
    ErrorTest(test, testSpec)
  end
  PASS2_TEST_SPEC.each do |test, testSpec|
    ErrorTest(test, testSpec)
  end
  PASS1_TEST_SPEC.each do |test, testSpec|
    ErrorTest(test, testSpec)
  end
end

#
#  カーネルライブラリのクリーン
#
def CleanKernel
  if Dir.exist?("KERNELLIB")
    Dir.chdir("KERNELLIB") do
      system("make clean")
    end
  end
end

#
#  テストプログラムのクリーン
#
def CleanTest(test, testSpec)
  objDir = test.tr("a-z_", "A-Z-")
  return unless Dir.exist?(objDir)

  Dir.chdir(objDir) do
    system("make clean")
    system("rm error.txt")
  end
end

#
#  全テストプログラムのクリーン
#
def CleanAllTest()
  CFG_TEST_SPEC.each do |test, testSpec|
    CleanTest(test, testSpec)
  end
  PASS3_TEST_SPEC.each do |test, testSpec|
    CleanTest(test, testSpec)
  end
  PASS2_TEST_SPEC.each do |test, testSpec|
    CleanTest(test, testSpec)
  end
  PASS1_TEST_SPEC.each do |test, testSpec|
    CleanTest(test, testSpec)
  end
end

#
#  ソースディレクトリ名を取り出す
#
if /^(.*)\/test_cfg\/testcfg/ =~ $0
  $srcDir = $1
else
  $srcDir = "."
end

if /^\// =~ $srcDir
  $usedSrcDir = $srcDir
else
  $usedSrcDir = "../" + $srcDir
end

#
#  ターゲット依存のオプションを読む
#
$targetOptions = {}
File.open("TARGET_OPTIONS") do |file|
  file.each_line.with_index do |line, index|
    line.chomp!
    $targetOptions[index] = line
  end
end

#
#  パラメータで指定された処理の実行
#
$build_only = false
$exec_only = false
$clean_flag = false
$proc_flag = false

ARGV.each do |param|
  case param
  when "build"
    $build_only = true
  when "exec"
    $exec_only = true
  when "clean"
    $clean_flag = true

  when "kernel"
    if ($clean_flag)
      CleanKernel()
    else
      BuildKernel() unless $exec_only
      # カーネルには，execはない
    end
    $proc_flag = true

  when "all"
    if ($clean_flag)
      CleanAllTest()
    else
      BuildAllTest() unless $exec_only
      ExecAllTest() unless $build_only
    end
    $proc_flag = true

  else
    if CFG_TEST_SPEC.has_key?(param)
      if ($clean_flag)
        CleanTest(param, CFG_TEST_SPEC[param])
      else
        BuildTest(param, CFG_TEST_SPEC[param], false, true) unless $exec_only
        CfgTest(param, CFG_TEST_SPEC[param]) unless $build_only
      end
    elsif PASS3_TEST_SPEC.has_key?(param)
      if ($clean_flag)
        CleanTest(param, PASS3_TEST_SPEC[param])
      else
        BuildTest(param, PASS3_TEST_SPEC[param], true, true) unless $exec_only
        ErrorTest(param, PASS3_TEST_SPEC[param]) unless $build_only
      end
    elsif PASS2_TEST_SPEC.has_key?(param)
      if ($clean_flag)
        CleanTest(param, PASS2_TEST_SPEC[param])
      else
        BuildTest(param, PASS2_TEST_SPEC[param], true, true) unless $exec_only
        ErrorTest(param, PASS2_TEST_SPEC[param]) unless $build_only
      end
    elsif PASS1_TEST_SPEC.has_key?(param)
      if ($clean_flag)
        CleanTest(param, PASS1_TEST_SPEC[param])
      else
        BuildTest(param, PASS1_TEST_SPEC[param], true, true) unless $exec_only
        ErrorTest(param, PASS1_TEST_SPEC[param]) unless $build_only
      end
    else
      puts("invalid parameter: #{param}")
    end
    $proc_flag = true
  end
end
if !$proc_flag
  # デフォルトの処理対象（kernelとall）
  if ($clean_flag)
    CleanKernel()
    CleanAllTest()
  else
    BuildKernel() unless $exec_only
    BuildAllTest() unless $exec_only
    ExecAllTest() unless $build_only
  end
end
