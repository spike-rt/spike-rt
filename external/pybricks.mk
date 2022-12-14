#  SPDX-License-Identifier: MIT
#
#  Makefile for Pybricks.
#
#  Copyright (C) 2022 by Embedded and Real-Time Systems Laboratory,
#                        Graduate School of Information Science, Nagoya Univ., JAPAN
#

PYBRICKS_DIR := $(EXTERNAL_DIR)/libpybricks
PYBRICKS_BRICK_DIR := $(abspath $(PYBRICKS_DIR)/bricks/primehub_spike-rt)

ifeq ("$(wildcard $(PYBRICKS_DIR)/README.md)","")
$(info GIT cloning pybricks-micropython submodule)
$(info $(shell git submodule update --init $(PYBRICKS_DIR)))
ifeq ("$(wildcard $(PYBRICKS_DIR)/README.md)","")
$(error failed)
endif
endif

PYBRICKS_LIB := $(PYBRICKS_OBJDIR)/libpybricks.a
PYBRICKS_INCLUDES := -I$(PYBRICKS_DIR)/lib/pbio/include \
										 -I$(PYBRICKS_DIR)/bricks/primehub_spike-rt \
										 -I$(PYBRICKS_DIR)/lib/pbio/platform/prime_hub_spike-rt \
										 -I$(PYBRICKS_DIR)/lib/lego \
										 -I$(PYBRICKS_DIR)/lib/contiki-core

#
#  Rules to build Pybricks.
#
.PHONY: libpybricks.a
libpybricks.a: $(PYBRICKS_OBJDIR)/libpybricks.a

# recipe for building pybricks
.PHONY: $(PYBRICKS_OBJDIR)/libpybricks.a
$(PYBRICKS_OBJDIR)/libpybricks.a:
	$(MAKE) -C $(PYBRICKS_BRICK_DIR) BUILD=$(abspath $(PYBRICKS_OBJDIR))
