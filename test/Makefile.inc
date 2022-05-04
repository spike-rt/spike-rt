include $(EXTERNAL_DIR)/Makefile.unity

APPL_DIRS += $(Unity_SRCDIR)
APPL_COBJS += $(Unity_COBJS)
APPL_COBJS += led.o motor.o pup.o forcesensor.o test_ultrasonicsensor.o

APPL_CFLAGS += $(Unity_CFLAGS) -DUNITY_INCLUDE_CONFIG_H 
