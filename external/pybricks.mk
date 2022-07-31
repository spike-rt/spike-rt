#  SPDX-License-Identifier: MIT
#
#  Makefile for Pybricks.
#
#  Copyright (C) 2022 by Embedded and Real-Time Systems Laboratory,
#                        Graduate School of Information Science, Nagoya Univ., JAPAN
#

PYBRICKS_DIR := $(EXTERNAL_DIR)/libpybricks
PYBRICKS_LIB := $(PYBRICKS_OBJDIR)/libpybricks.a
PYBRICKS_INCLUDES := -I$(PYBRICKS_DIR)/lib/pbio/include \
										 -I$(PYBRICKS_DIR)/bricks/primehub_asp3 \
										 -I$(PYBRICKS_DIR)/lib/pbio/platform/prime_hub_asp3 \
										 -I$(PYBRICKS_DIR)/lib/libfixmath/libfixmath \
										 -I$(PYBRICKS_DIR)/lib/lego

#
#  Rules to build Pybricks.
#
.PHONY: libpybricks.a
libpybricks.a: $(PYBRICKS_OBJDIR)/libpybricks.a

# recipe for building pybricks
.PHONY: $(PYBRICKS_OBJDIR)/libpybricks.a
$(PYBRICKS_OBJDIR)/libpybricks.a:
	$(MAKE) -C $(PYBRICKS_DIR)/bricks/primehub_asp3 BUILD=$(abspath $(PYBRICKS_OBJDIR))
