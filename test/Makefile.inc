include $(EXTERNAL_DIR)/Makefile.unity

APPL_DIRS += $(Unity_SRCDIR)
APPL_COBJS += $(Unity_COBJS)
APPL_COBJS += pb_device.o forcesensor.o led.o motor.o

APPL_CFLAGS += $(Unity_CFLAGS) -DUNITY_INCLUDE_CONFIG_H 
