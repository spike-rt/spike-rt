#  SPDX-License-Identifier: MIT
#
#  Makefile for Unity.
#
#  Copyright (C) 2022 by Embedded and Real-Time Systems Laboratory,
#                        Graduate School of Information Science, Nagoya Univ., JAPAN
#

# EXTERNAL_DIR := $(dir $(lastword $(MAKEFILE_LIST)))
Unity_DIR := $(EXTERNAL_DIR)/Unity

ifeq ("$(wildcard $(Unity_DIR)/README.md)","")
$(info GIT cloning Unity submodule)
$(info $(shell git submodule update --init $(Unity_DIR)))
ifeq ("$(wildcard $(Unity_DIR)/README.md)","")
$(error failed)
endif
endif

Unity_SRCDIR := $(Unity_DIR)/src \
	              $(Unity_DIR)/extras/fixture/src \
	              $(Unity_DIR)/extras/memory/src
Unity_COBJS := unity.o unity_fixture.o unity_memory.o #unity_output_Spy.o
#Unity_CFLAGS := -DUNITY_FIXTURE_NO_EXTRAS
