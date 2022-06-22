#!/bin/bash -xe

# Script generating API docs by Doxygen

DOXYGEN=${DOXYGEN:-doxygen}
cd asp3/target/primehub_gcc/drivers/
mkdir -p build
$DOXYGEN Doxyfile-en
$DOXYGEN Doxyfile-ja
