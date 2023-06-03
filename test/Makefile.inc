include $(EXTERNAL_DIR)/unity.mk

APPL_DIRS += $(Unity_SRCDIR)
APPL_COBJS += $(Unity_COBJS)

SERIAL_COBJS = test_SerialAsyncPort.o test_usart_serial.o test_usb_serial.o test_bluetooth_serial.o
APPL_COBJS += $(addprefix serial/, $(SERIAL_COBJS))
HUB_COBJS = test_light.o test_display.o test_button.o test_battery.o test_imu.o test_speaker.o test_bluetooth.o
APPL_COBJS += $(addprefix hub/, $(HUB_COBJS))
PUP_COBJS = test_pup.o test_ultrasonicsensor.o test_colorsensor.o test_forcesensor.o test_motor.o
APPL_COBJS += $(addprefix pup/, $(PUP_COBJS))

NEWLIB_COBJS = test_stdlib.o test_stdio.o
APPL_COBJS += $(addprefix newlib/, $(NEWLIB_COBJS))

APPL_CFLAGS += $(Unity_CFLAGS) -DUNITY_INCLUDE_CONFIG_H 
