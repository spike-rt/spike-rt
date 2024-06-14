#!/bin/bash -xe

QEMU=${QEMU:-"qemu-system-arm"}
TARGET=discovery_f413xx_gcc
JOB_NUM=${JOB_NUM:-$(nproc)}

CUR_DIR=$(pwd)
THIS_SCRIPT_DIR=$(cd $(dirname $0);pwd)
SR_TOP=$(cd $THIS_SCRIPT_DIR/../; pwd)

RUN="
  $QEMU -cpu cortex-m4 -machine lego-spike \
  -nographic \
  -kernel asp \
  -semihosting \
  -serial null -serial null -serial null -serial null -serial null -serial mon:stdio
"

KERNEL_DIR="$SR_TOP/build/obj-$TARGET-qemu-kernel"
mkdir -p $KERNEL_DIR 
pushd $KERNEL_DIR
$SR_TOP/asp3/configure.rb -T $TARGET -f -m $SR_TOP/common/kernel.mk -O "-DTOPPERS_USE_QEMU"
make libkernel.a -j $JOB_NUM
popd

rm -f $SR_TOP/summary.txt
rm -f $SR_TOP/result.txt

#<< COMMENTOUT
# "test_dlynse" "test_exttsk" "test_flg1" 
for TEST in \
    "test_cpuexc1" "test_cpuexc2" "test_cpuexc3" "test_cpuexc4" "test_cpuexc5" "test_cpuexc6" "test_cpuexc7" "test_cpuexc8" "test_cpuexc9" "test_cpuexc10"
do
  TEST_DIR="$SR_TOP/build/obj-$TARGET-qemu-$TEST"
  mkdir -p $TEST_DIR
  pushd $TEST_DIR
  $SR_TOP/asp3/configure.rb -T $TARGET -f -m $SR_TOP/common/app.mk -L $KERNEL_DIR \
      -a $SR_TOP/asp3/test -A $TEST -C test_pf.cdl -c test_cpuexc.cfg -O "-DTOPPERS_USE_QEMU"
  make -j $JOB_NUM
  echo "<---------- $TEST ---------->" >> $SR_TOP/result.txt
  $RUN >> $SR_TOP/result.txt
  echo "$TEST : " >> $SR_TOP/summary.txt
  tail -n 1 $SR_TOP/result.txt >> $SR_TOP/summary.txt
  popd
done
#COMMENTOUT

#"test_int1" "test_notify1" 
for TEST in \
    "test_dtq1" "test_hrt1" \
    "test_mutex1" "test_mutex2" "test_mutex3" "test_mutex4" "test_mutex5" "test_mutex6" "test_mutex7" "test_mutex8" \
    "test_raster1" "test_raster2" "test_sem1" "test_sem2" "test_suspend1" "test_sysman1" "test_sysstat1" \
    "test_task1" "test_tmevt1"

do
  TEST_DIR="$SR_TOP/build/obj-$TARGET-qemu-$TEST"
  mkdir -p $TEST_DIR
  pushd $TEST_DIR
  $SR_TOP/asp3/configure.rb -T $TARGET -f -m $SR_TOP/common/app.mk -L $KERNEL_DIR \
       -a $SR_TOP/asp3/test -A $TEST -C test_pf.cdl -O "-DTOPPERS_USE_QEMU"
  make -j $JOB_NUM
  echo "<---------- $TEST ---------->" >> ../../result.txt
  $RUN >> $SR_TOP/result.txt
  echo "$TEST : " >> $SR_TOP/summary.txt
  tail -n 1 $SR_TOP/result.txt >> $SR_TOP/summary.txt
  popd
done

