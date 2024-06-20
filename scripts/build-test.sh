#!/bin/bash -xe
JOB_NUM=${JOB_NUM:-$(nproc)}

CUR_DIR=$(pwd)
THIS_SCRIPT_DIR=$(cd $(dirname $0);pwd)
SR_TOP=$(cd $THIS_SCRIPT_DIR/../; pwd)
BUILD_TOP=${BUILD_TOP:-"$SR_TOP/build"}


mkdir -p $BUILD_TOP
BUILD_TOP=$(cd $BUILD_TOP; pwd)

pushd $BUILD_TOP

mkdir -p obj-primehub_kernel
pushd obj-primehub_kernel 
$SR_TOP/asp3/configure.rb -T primehub_gcc -f -m $SR_TOP/common/kernel.mk
make libkernel.a -j $JOB_NUM
popd


mkdir -p obj-primehub_test
pushd obj-primehub_test
$SR_TOP/asp3/configure.rb -T primehub_gcc -L $BUILD_TOP/obj-primehub_kernel -a $SR_TOP/test -A test -m $SR_TOP/common/app.mk
make -j $JOB_NUM
popd

popd
