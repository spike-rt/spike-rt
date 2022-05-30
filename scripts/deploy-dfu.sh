#!/bin/bash -xe
TOOL_DIR=$( cd "$( dirname "$0" )" && pwd -P )/../tools

PYTHON3=${PYTHON3:-"$TOOL_DIR/python/bin/python3"}

MPTOP=$TOOL_DIR/../external/libpybricks/micropython
DFU=$MPTOP/tools/dfu.py
PYDFU=$MPTOP/tools/pydfu.py

TEXT0_ADDR=0x8008000
DFU_VID=0x0694
DFU_PID=0x0008


echo "DFU Create $1"
$PYTHON3 $DFU -b $TEXT0_ADDR:$1 firmware.dfu

echo "Writing $1 to the board"
$PYTHON3 $PYDFU -u firmware.dfu --vid $DFU_VID --pid $DFU_PID

rm -rf firmware.dfu
