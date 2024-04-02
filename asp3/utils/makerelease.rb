#!/usr/bin/env ruby
# -*- coding: utf-8 -*-
#
#  TOPPERS Software
#      Toyohashi Open Platform for Embedded Real-Time Systems
# 
#  Copyright (C) 2006-2020 by Embedded and Real-Time Systems Laboratory
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
#  $Id: makerelease.rb 1463 2020-07-18 08:37:21Z ertl-hiro $
# 

require "optparse"
require "fileutils"

#
#  オプションの定義
#
#  -e <dirname>			アーカイブファイルを展開して削除する．dirname
#						は展開するディレクトリ名（省略可能）．

#
#  オプションの処理
#
OptionParser.new("Usage: makerelease.rb [options] MANIFEST-FILE") do |opt|
  opt.on("-e [dirname]",	"expand archive file and delete") do |val|
    $expandDirname = val
  end
  opt.parse!(ARGV)
end

#
#  ".."を含むパスの整形
#
#  例）"yyy/../zzz" → "zzz"
#  例）"xxx/yyy/../zzz" → "xxx/zzz"
#
def canonicalPath(path)
  while path.sub!(/([^\/]+\/\.\.\/)/, "") do end
  return(path)
end

#
#  ファイルの読み込み
#
def readFile(inFileName)
  baseDirectory = File.dirname(inFileName)
  if (baseDirectory == ".")
    baseDirectory = ""
  else
    baseDirectory += "/"
  end

  begin
    inFile = File.open(inFileName)
  rescue Errno::ENOENT, Errno::EACCES => ex
    abort(ex.message)
  end

  inFile.each do |line|
    line.chomp!
    line.sub!(/\s*\#.*$/, "")
    next if /^\s*$/ =~ line

    if /^E_PACKAGE[ \t]+(.*)$/ =~ line
      if $package != ""
        abort("Duplicated E_PACKAGE directive.")
      else
        $package = $1
        $e_package = 1
      end
    elsif /^PACKAGE[ \t]+(.*)$/ =~ line
      if $package != ""
        if !$e_package && $package != $1
          abort("Inconsistent PACKAGE directive.")
        end
      else
        $package = $1
      end
    elsif /^VERSION[ \t]+(.*)$/ =~ line
      if $version != ""
        if !$e_package && $version != $1
          abort("Inconsistent VERSION directive.")
        end
      else
        $version = $1
        if /%date/ =~ $version
          currentTime = Time.now.localtime
          vdate = sprintf("%04d%02d%02d", currentTime.year,
									currentTime.month, currentTime.day)
          $version.sub!(/%date/, vdate)
        end
      end
    elsif /^INCLUDE[ \t]+(.*)$/ =~ line
      readFile(canonicalPath(baseDirectory + $1))
    else
      fileName = $prefix + "/" + canonicalPath(baseDirectory + line)
      if !File.file?("../" + fileName) && !File.directory?("../" + fileName)
        abort("#{fileName} is not a file or a directory.")
      elsif $fileList.index(fileName)
        abort("#{fileName} is duplicated.")
      else 
        $fileList.push(fileName)
      end
    end
  end
end

#
#  初期化
#
$package = ""
$e_package = 0
$version = ""
$fileList = []

#
#  プリフィックス（./カレントディレクトリ名）の取り出し
#
$prefix = "./" + File.basename(Dir.pwd)

#
#  パラメータの取り出し
#
if ARGV.length >= 1
  arg = ARGV.shift.dup
  arg.sub!(/\/$/, "/MANIFEST")
  arg.sub!(/^\.\//, "")
else
  arg = "MANIFEST"
end

#
#  ファイルの読み込み
#
readFile(arg)
if $package == ""
  abort("PACKAGE/E_PACKAGE directive not found.")
end
if $version == ""
  abort("VERSION directive not found.")
end

#
#  RELEASEディレクトリの作成
#
if !File.directory?("RELEASE")
  Dir.mkdir("RELEASE")
end

#
#  アーカイブ（.tar.gz）の作成
#
archiveName = $package + "-" + $version + ".tar.gz"
fileListStr = $fileList.join(" ")
command = "tar cvfz RELEASE/#{archiveName} -C .. #{fileListStr}";
system(command)
puts("== RELEASE/#{archiveName} is generated. ==")

#
#  アーカイブファイルの展開と削除
#
if defined?($expandDirname)
  command = "tar xf RELEASE/#{archiveName}; rm RELEASE/#{archiveName}";
  system(command)

  dirname = $expandDirname || $prefix
  if File.exist?(dirname)
    File.rename(dirname, dirname + ".bak")
    puts("== '#{dirname}' is renamed to '#{dirname}.bak'. ==")
  end
  if !$expandDirname.nil?
    File.rename($prefix, $expandDirname)
  end
  puts("== RELEASE/#{archiveName} is expanded to '#{dirname}'. ==")
  puts("== RELEASE/#{archiveName} is deleted. ==")
end
