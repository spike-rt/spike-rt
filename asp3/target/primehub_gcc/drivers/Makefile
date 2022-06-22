LIBPYBRICKS_DIR = $(SRCDIR)/../external/libpybricks
INCLUDES += -I$(TARGETDIR)/drivers \
						-I$(TARGETDIR)/drivers/cbricks \
	          -I$(LIBPYBRICKS_DIR)/lib/pbio/include \
	          -I$(LIBPYBRICKS_DIR)/bricks/primehub_asp3 \
	          -I$(LIBPYBRICKS_DIR)/lib/pbio/platform/prime_hub_asp3 \
	          -I$(LIBPYBRICKS_DIR)/lib/libfixmath/libfixmath \
	          -I$(LIBPYBRICKS_DIR)/lib/lego \

LDFLAGS += -L$(LIBPYBRICKS_DIR)/bricks/primehub_asp3 
LIBS = -lpybricks

KERNEL_DIRS += $(TARGETDIR)/drivers \
							 $(TARGETDIR)/drivers/cbricks \
							 $(TARGETDIR)/drivers/cbricks/pup

KERNEL_COBJS += pybricks.o pup_device.o ultrasonicsensor.o colorsensor.o

.PHONY: libpybricks.a
libpybricks.a:
	make -C $(LIBPYBRICKS_DIR)/bricks/primehub_asp3
