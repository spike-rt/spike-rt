#!/bin/bash -xe

QEMU=${QEMU:-"qemu-system-arm"}
FIRMWARE=${FIRMWARE:-"$(pwd)/build/obj-discovery_f413xx_gcc/asp"}

$QEMU -cpu cortex-m4 -machine legospike \
  -nographic \
  -kernel $FIRMWARE \
  -semihosting \
  -serial null -serial null -serial null -serial null -serial null -serial mon:stdio
