#  SPDX-License-Identifier: MIT
#
#  Makefile for tlsf.
#
#  Copyright (C) 2023 by Embedded and Real-Time Systems Laboratory,
#           	Graduate School of Information Science, Nagoya Univ., JAPAN
#

TLSF_DIR := $(DRIVERS_DIR)/tlsf

ifeq ("$(wildcard $(TLSF_DIR)/README.md)","")
$(info GIT cloning TLSF submodule)
$(info $(shell git submodule update --init $(TLSF_DIR)))
ifeq ("$(wildcard $(TLSF_DIR)/README.md)","")
$(error failed)
endif
endif

TLSF_SRCDIR := $(TLSF_DIR)
TLSF_COBJS := tlsf.o
TLSF_INCLUDES := -I$(TLSF_DIR)
#TLSF_CFLAGS :=
