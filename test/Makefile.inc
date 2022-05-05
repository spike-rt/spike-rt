include $(EXTERNAL_DIR)/Makefile.unity

APPL_DIRS += $(Unity_SRCDIR)
APPL_COBJS += $(Unity_COBJS)

HUB_COBJS = test_display.o
APPL_COBJS += $(addprefix hub/, $(HUB_COBJS))
PUP_COBJS = test_pup.o test_motor.o test_forcesensor.o test_ultrasonicsensor.o
APPL_COBJS += $(addprefix pup/, $(PUP_COBJS))

APPL_CFLAGS += $(Unity_CFLAGS) -DUNITY_INCLUDE_CONFIG_H 
