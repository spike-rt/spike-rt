#!/bin/bash -xe
BUILD_DIR=${BUILD_DIR:-build}
JOB_NUM=${JOB_NUM:-$(nproc)}

mkdir -p $BUILD_DIR
cd $BUILD_DIR

mkdir -p obj-primehub_kernel
(cd obj-primehub_kernel && ../../asp3/configure.rb -T primehub_gcc -f)

mkdir -p obj-primehub_test
(cd obj-primehub_test && ../../asp3/configure.rb -T primehub_gcc -L ../obj-primehub_kernel -a ../../test/ -A test -m ../../common/Makefile.appl)

(cd obj-primehub_kernel && make libkernel.a -j $JOB_NUM)
(cd obj-primehub_test && make libpybricks.a -j $JOB_NUM && make -j $JOB_NUM && make asp.bin -j $JOB_NUM)

cd ..
