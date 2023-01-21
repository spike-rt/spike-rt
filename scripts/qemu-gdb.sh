#!/bin/bash -xe

QEMU=${QEMU:-"$(pwd)/qemu-system-arm"}
FIRMWARE=${FIRMWARE:-"$(pwd)/build-qemu/obj-discovery_f413xx_test/asp"}

$QEMU -cpu cortex-m4 -machine legospike \
  -nographic \
  -kernel $FIRMWARE \
  -S -s \
  -semihosting \
  -serial null -serial null -serial null -serial null -serial null -serial mon:stdio

