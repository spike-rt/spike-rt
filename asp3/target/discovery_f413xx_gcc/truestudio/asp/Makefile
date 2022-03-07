#
#  TOPPERS/ASP Kernel
#      Toyohashi Open Platform for Embedded Real-Time Systems/
#      Advanced Standard Profile Kernel
# 
#  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
#                              Toyohashi Univ. of Technology, JAPAN
#  Copyright (C) 2006-2019 by Embedded and Real-Time Systems Laboratory
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
#  $Id: Makefile 1150 2019-01-14 07:56:41Z ertl-hiro $
# 

#
#  ターゲットの指定（Makefile.targetで上書きされるのを防ぐため）
#
all:

#
#  ターゲット略称の定義
#
TARGET = nucleo_f401re_gcc

#
#  プログラミング言語の定義
#
SRCLANG = c
ifeq ($(SRCLANG),c)
	LIBS = -lc
endif
ifeq ($(SRCLANG),c++)
	USE_CXX = true
	CXXLIBS = -lstdc++ -lm -lc
	CXXRTS = cxxrt.o newlibrt.o
endif

#
#  ソースファイルのディレクトリの定義
#
SRCDIR = ../../../../

#
#  オブジェクトファイル名の拡張子の設定
#
OBJEXT = elf

#
#  カーネルライブラリ（libkernel.a）のディレクトリ名
#  （カーネルライブラリもmake対象にする時は，空に定義する）
#
KERNEL_LIB = 

#
#  カーネルを関数単位でコンパイルするかどうかの定義
#
KERNEL_FUNCOBJS = 

#
#  TECSを外すかどうかの定義
#
OMIT_TECS = 

#
#  TECS関係ファイルのディレクトリの定義
#
TECSDIR = $(SRCDIR)/tecsgen

#
#  トレースログを取得するかどうかの定義
#
ENABLE_TRACE = 

#
#  開発ツール（コンパイラ等）のディレクトリの定義
#
DEVTOOLDIR = 

#
#  ユーティリティプログラムの名称
#
CFG = ruby $(SRCDIR)/cfg/cfg.rb
TECSGEN = ruby $(TECSDIR)/tecsgen.rb

#
#  オブジェクトファイル名の定義
#
OBJNAME = asp
ifdef OBJEXT
	OBJFILE = $(OBJNAME).$(OBJEXT)
	CFG1_OUT = cfg1_out.$(OBJEXT)
else
	OBJFILE = $(OBJNAME)
	CFG1_OUT = cfg1_out
endif

#
#  中間オブジェクトファイルと依存関係ファイルを置くディレクトリの定義
#
OBJDIR = objs
DEPDIR = objs

#
#  ターゲット依存部のディレクトリの定義
#
TARGETDIR = $(SRCDIR)/target/$(TARGET)

#
#  ターゲット依存の定義のインクルード
#
include $(TARGETDIR)/Makefile.target

#
#  TECS生成ファイルのディレクトリの定義
#
TECSGENDIR = ./gen
ifndef OMIT_TECS
	TECSGEN_TIMESTAMP = $(TECSGENDIR)/tecsgen.timestamp
	INIT_TECS_COBJ = init_tecs.o
endif

#
#  TECSが生成する定義のインクルード
#
ifndef OMIT_TECS
	GEN_DIR = $(TECSGENDIR)
	-include $(TECSGENDIR)/Makefile.tecsgen
endif

#
#  共通コンパイルオプションの定義
#
COPTS := -g  $(COPTS)
ifndef OMIT_WARNING_ALL
	COPTS := -Wall $(COPTS)
endif
ifndef OMIT_OPTIMIZATION
	COPTS := -O2 $(COPTS)
endif
ifdef OMIT_TECS
	CDEFS := -DTOPPERS_OMIT_TECS $(CDEFS)
endif
CDEFS := $(CDEFS) 
INCLUDES := -I. -I$(SRCDIR)/include $(INCLUDES) -I$(SRCDIR)
LDFLAGS := $(LDFLAGS) 
LIBS := $(LIBS) $(CXXLIBS)
CFLAGS = $(COPTS) $(CDEFS) $(INCLUDES)

#
#  アプリケーションプログラムに関する定義
#
APPLNAME = sample1
APPLDIRS = $(SRCDIR)/sample
APPL_CFG = sample1.cfg
APPL_CDL = sample1.cdl

APPL_DIRS := $(APPLDIRS) $(SRCDIR)/library
APPL_ASMOBJS :=
ifdef USE_CXX
	APPL_CXXOBJS := sample1.o
	APPL_COBJS :=
else
	APPL_COBJS := sample1.o
endif
APPL_COBJS := $(APPL_COBJS) log_output.o vasyslog.o t_perror.o strerror.o
APPL_CFLAGS := $(APPL_CFLAGS)
ifdef APPLDIRS
	INCLUDES := $(INCLUDES) $(foreach dir,$(APPLDIRS),-I$(dir))
endif

#
#  システムサービスに関する定義
#
SYSSVC_DIRS := $(TECSGENDIR) $(SRCDIR)/tecs_kernel \
				$(SYSSVC_DIRS) $(SRCDIR)/syssvc
SYSSVC_ASMOBJS := $(SYSSVC_ASMOBJS)
SYSSVC_COBJS := $(SYSSVC_COBJS)  $(TECS_COBJS) \
				$(INIT_TECS_COBJ) $(CXXRTS)
SYSSVC_CFLAGS := $(SYSSVC_CFLAGS)
INCLUDES := $(INCLUDES) -I$(TECSGENDIR) -I$(SRCDIR)/tecs_kernel

#
#  トレースログ記録のサンプルコードに関する定義
#
ifdef ENABLE_TRACE
	COPTS := $(COPTS) -DTOPPERS_ENABLE_TRACE
	SYSSVC_DIRS := $(SYSSVC_DIRS) $(SRCDIR)/arch/tracelog
endif

#
#  ターゲットファイル
#
.PHONY: all
ifndef OMIT_TECS
all: tecs
	@$(MAKE) check
#	@$(MAKE) check $(OBJNAME).bin
#	@$(MAKE) check $(OBJNAME).srec
else
all: check
#all: check $(OBJNAME).bin
#all: check $(OBJNAME).srec
endif

##### 以下は編集しないこと #####

#
#  コンフィギュレータに関する定義
#
CFG_KERNEL := --kernel asp
CFG_TABS := --api-table $(SRCDIR)/kernel/kernel_api.def \
			--symval-table $(SRCDIR)/kernel/kernel_sym.def $(CFG_TABS)
CFG_ASMOBJS := $(CFG_ASMOBJS)
CFG_COBJS := kernel_cfg.o $(CFG_COBJS)
CFG_CFLAGS := -DTOPPERS_CB_TYPE_ONLY $(CFG_CFLAGS)

CFG2_OUT_SRCS := kernel_cfg.h kernel_cfg.c $(CFG2_OUT_SRCS)

#
#  カーネルに関する定義
#
#  KERNEL_ASMOBJS: カーネルライブラリに含める，ソースがアセンブリ言語の
#				   オブジェクトファイル．
#  KERNEL_COBJS: カーネルのライブラリに含める，ソースがC言語で，ソース
#				 ファイルと1対1に対応するオブジェクトファイル．
#  KERNEL_LCSRCS: カーネルのライブラリに含めるC言語のソースファイルで，
#				  1つのソースファイルから複数のオブジェクトファイルを生
#				  成するもの．
#  KERNEL_LCOBJS: 上のソースファイルから生成されるオブジェクトファイル．
#
KERNEL_DIRS := $(KERNEL_DIRS) $(SRCDIR)/kernel
KERNEL_ASMOBJS := $(KERNEL_ASMOBJS)
KERNEL_COBJS := $(KERNEL_COBJS)
KERNEL_CFLAGS := $(KERNEL_CFLAGS) -I$(SRCDIR)/kernel

#
#  カーネルのファイル構成の定義
#
include $(SRCDIR)/kernel/Makefile.kernel
ifdef KERNEL_FUNCOBJS
	KERNEL_LCSRCS := $(KERNEL_FCSRCS)
	KERNEL_LCOBJS := $(foreach file,$(KERNEL_FCSRCS),$($(file:.c=)))
else
	KERNEL_CFLAGS := -DALLFUNC $(KERNEL_CFLAGS)
	KERNEL_COBJS := $(KERNEL_COBJS) \
					$(foreach file,$(KERNEL_FCSRCS),$(file:.c=.o))
endif
ifdef TARGET_OFFSET_TRB
	OFFSET_H = offset.h
endif
ifndef TARGET_KERNEL_TRB
	TARGET_KERNEL_TRB := $(TARGETDIR)/target_kernel.trb
endif
ifndef TARGET_CHECK_TRB
	TARGET_CHECK_TRB := $(TARGETDIR)/target_check.trb
endif
ifndef TARGET_KERNEL_CFG
	TARGET_KERNEL_CFG := $(TARGETDIR)/target_kernel.cfg
endif

#
#  ソースファイルのあるディレクトリに関する定義
#
vpath %.c $(KERNEL_DIRS) $(SYSSVC_DIRS) $(APPL_DIRS)
vpath %.S $(KERNEL_DIRS) $(SYSSVC_DIRS) $(APPL_DIRS)
vpath %.cfg $(APPL_DIRS)
vpath %.cdl $(APPL_DIRS)

#
#  中間オブジェクトファイルを置くディレクトリの処理
#
APPL_ASMOBJS   := $(addprefix $(OBJDIR)/, $(APPL_ASMOBJS))
APPL_CXXOBJS   := $(addprefix $(OBJDIR)/, $(APPL_CXXOBJS))
APPL_COBJS     := $(addprefix $(OBJDIR)/, $(APPL_COBJS))
SYSSVC_ASMOBJS := $(addprefix $(OBJDIR)/, $(SYSSVC_ASMOBJS))
SYSSVC_COBJS   := $(addprefix $(OBJDIR)/, $(SYSSVC_COBJS))
KERNEL_ASMOBJS := $(addprefix $(OBJDIR)/, $(KERNEL_ASMOBJS))
KERNEL_COBJS   := $(addprefix $(OBJDIR)/, $(KERNEL_COBJS))
KERNEL_LCOBJS  := $(addprefix $(OBJDIR)/, $(KERNEL_LCOBJS))
CFG_ASMOBJS    := $(addprefix $(OBJDIR)/, $(CFG_ASMOBJS))
CFG_COBJS      := $(addprefix $(OBJDIR)/, $(CFG_COBJS))

#
#  コンパイルのための変数の定義
#
APPL_OBJS = $(APPL_ASMOBJS) $(APPL_COBJS) $(APPL_CXXOBJS)
SYSSVC_OBJS = $(SYSSVC_ASMOBJS) $(SYSSVC_COBJS)
KERNEL_LIB_OBJS = $(KERNEL_ASMOBJS) $(KERNEL_COBJS) $(KERNEL_LCOBJS)
CFG_OBJS = $(CFG_ASMOBJS) $(CFG_COBJS)
ALL_OBJS = $(START_OBJS) $(APPL_OBJS) $(SYSSVC_OBJS) $(CFG_OBJS) \
											$(END_OBJS) $(HIDDEN_OBJS)
ALL_LIBS = -lkernel $(LIBS)
ifdef KERNEL_LIB
	LIBS_DEP = $(KERNEL_LIB)/libkernel.a $(filter %.a,$(LIBS))
	OBJ_LDFLAGS := $(OBJ_LDFLAGS) -L$(KERNEL_LIB)
	REALCLEAN_FILES := libkernel.a $(REALCLEAN_FILES)
else
	LIBS_DEP = libkernel.a $(filter %.a,$(LIBS))
	OBJ_LDFLAGS := $(OBJ_LDFLAGS) -L.
endif

ifdef TEXT_START_ADDRESS
	LDFLAGS := $(LDFLAGS) -Wl,-Ttext,$(TEXT_START_ADDRESS)
endif
ifdef DATA_START_ADDRESS
	LDFLAGS := $(LDFLAGS) -Wl,-Tdata,$(DATA_START_ADDRESS)
endif
ifdef LDSCRIPT
	LDFLAGS := $(LDFLAGS) -Wl,-T,$(LDSCRIPT)
endif

#
#  tecsgenからCプリプロセッサを呼び出す際のオプションの定義
#
TECS_CPP = $(CC) $(CDEFS) $(INCLUDES) $(SYSSVC_CFLAGS) -D TECSGEN -E

#
#  tecsgenの呼出し
#
.PHONY: tecs
tecs $(TECSGEN_SRCS) $(TECS_HEADERS): $(TECSGEN_TIMESTAMP) ;
$(TECSGEN_TIMESTAMP): $(APPL_CDL) $(TECS_IMPORTS)
	$(TECSGEN) $< -R $(INCLUDES) --cpp "$(TECS_CPP)" -g $(TECSGENDIR)

#
#  カーネルのコンフィギュレーションファイルの生成
#
cfg1_out.c cfg1_out.db: cfg1_out.timestamp ;
cfg1_out.timestamp: $(APPL_CFG) $(TECSGEN_TIMESTAMP)
	$(CFG) --pass 1 $(CFG_KERNEL) $(INCLUDES) $(CFG_TABS) \
						-M $(DEPDIR)/cfg1_out_c.d $(TARGET_KERNEL_CFG) $<

$(CFG1_OUT): $(START_OBJS) $(OBJDIR)/cfg1_out.o $(END_OBJS) $(HIDDEN_OBJS)
	$(LINK) $(CFLAGS) $(LDFLAGS) $(CFG1_OUT_LDFLAGS) -o $(CFG1_OUT) \
						$(START_OBJS) $(OBJDIR)/cfg1_out.o $(LIBS) $(END_OBJS)

cfg1_out.syms: $(CFG1_OUT)
	$(NM) -n $(CFG1_OUT) > cfg1_out.syms

cfg1_out.srec: $(CFG1_OUT)
	$(OBJCOPY) -O srec -S $(CFG1_OUT) cfg1_out.srec

$(CFG2_OUT_SRCS) cfg2_out.db: kernel_cfg.timestamp ;
kernel_cfg.timestamp: cfg1_out.db cfg1_out.syms cfg1_out.srec
	$(CFG) --pass 2 $(CFG_KERNEL) $(INCLUDES) -T $(TARGET_KERNEL_TRB)

#
#  オフセットファイル（offset.h）の生成規則
#
$(OFFSET_H): offset.timestamp ;
offset.timestamp: cfg1_out.db cfg1_out.syms cfg1_out.srec
	$(CFG) --pass 2 -O $(CFG_KERNEL) $(INCLUDES) -T $(TARGET_OFFSET_TRB) \
				--rom-symbol cfg1_out.syms --rom-image cfg1_out.srec

#
#  カーネルライブラリファイルの生成
#
libkernel.a: $(OFFSET_H) $(KERNEL_LIB_OBJS)
	rm -f libkernel.a
	$(AR) -rcs libkernel.a $(KERNEL_LIB_OBJS)
	$(RANLIB) libkernel.a

#
#  並列makeのための依存関係の定義
#
$(APPL_OBJS) $(SYSSVC_OBJS): | kernel_cfg.timestamp
$(APPL_ASMOBJS) $(SYSSVC_ASMOBJS) $(KERNEL_ASMOBJS) $(CFG_ASMOBJS): \
														| offset.timestamp

#
#  特別な依存関係の定義
#
$(OBJDIR)/tBannerMain.o: $(filter-out $(OBJDIR)/tBannerMain.o,$(ALL_OBJS)) \
																$(LIBS_DEP)

#
#  全体のリンク
#
$(OBJFILE): $(ALL_OBJS) $(LIBS_DEP)
	$(LINK) $(CFLAGS) $(LDFLAGS) $(OBJ_LDFLAGS) -o $(OBJFILE) \
			$(START_OBJS) $(APPL_OBJS) $(SYSSVC_OBJS) $(CFG_OBJS) \
			$(ALL_LIBS) $(END_OBJS)

#
#  シンボルファイルの生成
#
$(OBJNAME).syms: $(OBJFILE)
	$(NM) -n $(OBJFILE) > $(OBJNAME).syms

#
#  バイナリファイルの生成
#
$(OBJNAME).bin: $(OBJFILE)
	$(OBJCOPY) -O binary -S $(OBJFILE) $(OBJNAME).bin

#
#  Sレコードファイルの生成
#
$(OBJNAME).srec: $(OBJFILE)
	$(OBJCOPY) -O srec -S $(OBJFILE) $(OBJNAME).srec

#
#  エラーチェック処理
#
.PHONY: check
check: check.timestamp ;
check.timestamp: cfg2_out.db $(OBJNAME).syms $(OBJNAME).srec
	$(CFG) --pass 3 $(CFG_KERNEL) -O $(INCLUDES) -T $(TARGET_CHECK_TRB) \
				--rom-symbol $(OBJNAME).syms --rom-image $(OBJNAME).srec
	@echo "configuration check passed"

#
#  コンパイル結果の消去
#
.PHONY: clean
clean:
	rm -f $(OBJDIR)/*.o $(DEPDIR)/*.d $(CLEAN_FILES) check.timestamp
	rm -f $(OBJFILE) $(OBJNAME).syms $(OBJNAME).srec $(OBJNAME).bin
	rm -f kernel_cfg.timestamp $(CFG2_OUT_SRCS) cfg2_out.db
	rm -f offset.timestamp $(OFFSET_H)
	rm -f cfg1_out.syms cfg1_out.srec $(CFG1_OUT)
	rm -f cfg1_out.timestamp cfg1_out.c cfg1_out.db
	rm -rf $(TECSGENDIR)
ifndef KERNEL_LIB
	rm -f libkernel.a
endif

.PHONY: cleankernel
cleankernel:
	rm -f $(OFFSET_H) $(KERNEL_LIB_OBJS)
	rm -f $(KERNEL_LIB_OBJS:$(OBJDIR)/%.o=$(DEPDIR)/%.d)

.PHONY: realclean
realclean: clean
	rm -f $(REALCLEAN_FILES)

#
#  コンフィギュレータが生成したファイルのコンパイルルールの定義
#
#  コンフィギュレータが生成したファイルは，共通のコンパイルオプション
#  のみを付けてコンパイルする．
#
ALL_CFG_COBJS = $(CFG_COBJS) $(OBJDIR)/cfg1_out.o
ALL_CFG_ASMOBJS = $(CFG_ASMOBJS)

$(ALL_CFG_COBJS): $(OBJDIR)/%.o: %.c
	$(CC) -c -o $@ -MD -MP -MF $(DEPDIR)/$*.d $(CFLAGS) $(CFG_CFLAGS) $<

$(ALL_CFG_COBJS:$(OBJDIR)/%.o=%.s): %.s: %.c
	$(CC) -S -o $@ $(CFLAGS) $(CFG_CFLAGS) $<

$(ALL_CFG_ASMOBJS): $(OBJDIR)/%.o: %.S
	$(CC) -c -o $@ -MD -MP -MF $(DEPDIR)/$*.d $(CFLAGS) $(CFG_CFLAGS) $<

#
#  依存関係ファイルのインクルード
#
-include $(DEPDIR)/*.d

#
#  開発ツールのコマンド名の定義
#
ifeq ($(TOOL),gcc)
	#
	#  GNU開発環境用
	#
	GCC_TARGET_PREFIX = arm-atollic-eabi-
	CC := $(GCC_TARGET_PREFIX)gcc
	CXX := $(GCC_TARGET_PREFIX)g++
	AS := $(GCC_TARGET_PREFIX)as
	LD := $(GCC_TARGET_PREFIX)ld
	AR := $(GCC_TARGET_PREFIX)ar
	NM := $(GCC_TARGET_PREFIX)nm
	RANLIB := $(GCC_TARGET_PREFIX)ranlib
	OBJCOPY := $(GCC_TARGET_PREFIX)objcopy
	OBJDUMP := $(GCC_TARGET_PREFIX)objdump
endif

ifdef DEVTOOLDIR
	CC := $(DEVTOOLDIR)/$(CC)
	CXX := $(DEVTOOLDIR)/$(CXX)
	AS := $(DEVTOOLDIR)/$(AS)
	LD := $(DEVTOOLDIR)/$(LD)
	AR := $(DEVTOOLDIR)/$(AR)
	NM := $(DEVTOOLDIR)/$(NM)
	RANLIB := $(DEVTOOLDIR)/$(RANLIB)
	OBJCOPY := $(DEVTOOLDIR)/$(OBJCOPY)
	OBJDUMP := $(DEVTOOLDIR)/$(OBJDUMP)
endif

ifdef USE_CXX
	LINK = $(CXX)
else
	LINK = $(CC)
endif

#
#  コンパイルルールの定義
#
$(KERNEL_COBJS): $(OBJDIR)/%.o: %.c
	$(CC) -c -o $@ -MD -MP -MF $(DEPDIR)/$*.d $(CFLAGS) $(KERNEL_CFLAGS) $<

$(KERNEL_COBJS:$(OBJDIR)/%.o=%.s): %.s: %.c
	$(CC) -S -o $@ $(CFLAGS) $(KERNEL_CFLAGS) $<

$(KERNEL_LCOBJS): $(OBJDIR)/%.o:
	$(CC) -c -o $@ -DTOPPERS_$(*F) -MD -MP -MF $(DEPDIR)/$*.d \
									$(CFLAGS) $(KERNEL_CFLAGS) $<

$(KERNEL_LCOBJS:$(OBJDIR)/%.o=%.s): %.s:
	$(CC) -S -o $@ -DTOPPERS_$(*F) $(CFLAGS) $(KERNEL_CFLAGS) $<

$(KERNEL_ASMOBJS): $(OBJDIR)/%.o: %.S
	$(CC) -c -o $@ -MD -MP -MF $(DEPDIR)/$*.d $(CFLAGS) $(KERNEL_CFLAGS) $<

$(SYSSVC_COBJS): $(OBJDIR)/%.o: %.c
	$(CC) -c -o $@ -MD -MP -MF $(DEPDIR)/$*.d $(CFLAGS) $(SYSSVC_CFLAGS) $<

$(SYSSVC_COBJS:$(OBJDIR)/%.o=%.s): %.s: %.c
	$(CC) -S -o $@ $(CFLAGS) $(SYSSVC_CFLAGS) $<

$(SYSSVC_ASMOBJS): $(OBJDIR)/%.o: %.S
	$(CC) -c -o $@ -MD -MP -MF $(DEPDIR)/$*.d $(CFLAGS) $(SYSSVC_CFLAGS) $<

$(APPL_COBJS): $(OBJDIR)/%.o: %.c
	$(CC) -c -o $@ -MD -MP -MF $(DEPDIR)/$*.d $(CFLAGS) $(APPL_CFLAGS) $<

$(APPL_COBJS:$(OBJDIR)/%.o=%.s): %.s: %.c
	$(CC) -S -o $@ $(CFLAGS) $(APPL_CFLAGS) $<

$(APPL_CXXOBJS): $(OBJDIR)/%.o: %.cpp
	$(CXX) -c -o $@ -MD -MP -MF $(DEPDIR)/$*.d $(CFLAGS) $(APPL_CFLAGS) $<

$(APPL_CXXOBJS:$(OBJDIR)/%.o=%.s): %.s: %.cpp
	$(CXX) -S -o $@ $(CFLAGS) $(APPL_CFLAGS) $<

$(APPL_ASMOBJS): $(OBJDIR)/%.o: %.S
	$(CC) -c -o $@ -MD -MP -MF $(DEPDIR)/$*.d $(CFLAGS) $(APPL_CFLAGS) $<

#
#  デフォルトコンパイルルールを上書き
#
$(OBJDIR)/%.o: %.c
	@echo "*** Default compile rules should not be used."
	$(CC) -c -o $@ -MD -MP -MF $(DEPDIR)/$*.d $(CFLAGS) $<

%.s: %.c
	@echo "*** Default compile rules should not be used."
	$(CC) -S -o $@ $(CFLAGS) $<

$(OBJDIR)/%.o: %.cpp
	@echo "*** Default compile rules should not be used."
	$(CXX) -c -o $@ -MD -MP -MF $(DEPDIR)/$*.d $(CFLAGS) $<

%.s: %.cpp
	@echo "*** Default compile rules should not be used."
	$(CXX) -S -o $@ $(CFLAGS) $<

$(OBJDIR)/%.o: %.S
	@echo "*** Default compile rules should not be used."
	$(CC) -c -o $@ -MD -MP -MF $(DEPDIR)/$*.d $(CFLAGS) $<
