#!/bin/bash -xe
BUILD_DIR=${BUILD_DIR:-build-qemu}
JOB_NUM=${JOB_NUM:-$(nproc)}

mkdir -p $BUILD_DIR
cd $BUILD_DIR

mkdir -p obj-discovery_f413xx_test
pushd obj-discovery_f413xx_test
../../asp3/configure.rb -T discovery_f413xx_gcc -a ../../test/ -A test -m ../../common/app.mk -O "-DTOPPERS_USE_QEMU"
popd

# Currently, required to execute `make libpybricks.a` fisrt
# to clone submodules.
(cd obj-discovery_f413xx_test && make libpybricks.a && make -j $JOB_NUM && make asp.bin -j $JOB_NUM)

cd ..
