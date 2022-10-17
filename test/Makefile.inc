include $(EXTERNAL_DIR)/unity.mk

APPL_DIRS += $(Unity_SRCDIR)
APPL_COBJS += $(Unity_COBJS)

HUB_COBJS = test_light.o test_display.o test_button.o test_battery.o
APPL_COBJS += $(addprefix hub/, $(HUB_COBJS))
PUP_COBJS = test_pup.o test_motor.o test_ultrasonicsensor.o test_colorsensor.o test_forcesensor.o
APPL_COBJS += $(addprefix pup/, $(PUP_COBJS))

APPL_CFLAGS += $(Unity_CFLAGS) -DUNITY_INCLUDE_CONFIG_H 
