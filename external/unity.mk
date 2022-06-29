# EXTERNAL_DIR := $(dir $(lastword $(MAKEFILE_LIST)))
Unity_SRCDIR := $(EXTERNAL_DIR)/Unity/src \
	              $(EXTERNAL_DIR)/Unity/extras/fixture/src \
	              $(EXTERNAL_DIR)/Unity/extras/memory/src
Unity_COBJS := unity.o unity_fixture.o unity_memory.o #unity_output_Spy.o
#Unity_CFLAGS := -DUNITY_FIXTURE_NO_EXTRAS
