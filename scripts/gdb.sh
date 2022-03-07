#!/bin/bash -xe

GDB=${GDB:-"arm-none-eabi-gdb"}
FIRMWARE=${FIRMWARE:-"$(pwd)/build/obj-primehub_gcc/asp"}

$GDB $FIRMWARE
