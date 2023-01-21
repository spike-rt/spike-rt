#!/bin/bash -xe

QEMU=${QEMU:-"qemu-system-arm"}
FIRMWARE=${FIRMWARE:-"$(pwd)/build-qemu/obj-discovery_f413xx_test/asp"}

$QEMU -cpu cortex-m4 -machine lego-spike \
  -nographic \
  -kernel $FIRMWARE \
  -semihosting \
  -serial null -serial null -serial null -serial null -serial null -serial mon:stdio
