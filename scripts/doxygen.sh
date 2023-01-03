#!/bin/bash -xe

# Script generating API docs by Doxygen

DOXYGEN=${DOXYGEN:-doxygen}
pushd drivers/
mkdir -p build-doxygen
$DOXYGEN Doxyfile-en
$DOXYGEN Doxyfile-ja
popd
