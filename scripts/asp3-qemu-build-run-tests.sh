#!/bin/bash -xe

QEMU=${QEMU:-"qemu-system-arm"}
TARGET=discovery_f413xx_gcc

RUN="
  $QEMU -cpu cortex-m4 -machine legospike \
  -nographic \
  -kernel asp \
  -semihosting \
  -serial null -serial null -serial null -serial null -serial null -serial mon:stdio
"

KERNEL_DIR="build/obj-$TARGET-kernel"
if [ -e $KERNEL_DIR ]; then
  cd $KERNEL_DIR
else
  mkdir -p $KERNEL_DIR && cd $KERNEL_DIR
  ../../../asp3/configure.rb -T $TARGET -f -O "-DTOPPERS_USE_QEMU"
fi
make libkernel.a -j10
cd ../../

rm -f summary.txt
rm -f result.txt

#<< COMMENTOUT
# "test_dlynse" "test_exttsk" "test_flg1" 
for TEST in \
    "test_cpuexc1" "test_cpuexc2" "test_cpuexc3" "test_cpuexc4" "test_cpuexc5" "test_cpuexc6" "test_cpuexc7" "test_cpuexc8" "test_cpuexc9" "test_cpuexc10"
do
  TEST_DIR="build/obj-$TARGET-$TEST"
  if [ -e $TEST_DIR ]; then
    cd $TEST_DIR
  else
    mkdir -p $TEST_DIR
    cd $TEST_DIR
    ../../../asp3/configure.rb -T $TARGET -L ../../$KERNEL_DIR \
       -a ../../../asp3/test -A $TEST -C test_pf.cdl -c test_cpuexc.cfg
  fi
  make -j10
  echo "<---------- $TEST ---------->" >> ../../result.txt
  $RUN >> ../../result.txt
  echo "$TEST : " >> ../../summary.txt
  tail -n 1 ../../result.txt >> ../../summary.txt
  cd ../../
done
#COMMENTOUT

#"test_int1" "test_notify1" 
for TEST in \
    "test_dtq1" "test_hrt1" \
    "test_mutex1" "test_mutex2" "test_mutex3" "test_mutex4" "test_mutex5" "test_mutex6" "test_mutex7" "test_mutex8" \
    "test_raster1" "test_raster2" "test_sem1" "test_sem2" "test_suspend1" "test_sysman1" "test_sysstat1" \
    "test_task1" "test_tmevt1"

do
  TEST_DIR="build/obj-$TARGET-$TEST"
  if [ -e $TEST_DIR ]; then
    cd $TEST_DIR
  else
    mkdir -p $TEST_DIR
    cd $TEST_DIR
    ../../../asp3/configure.rb -T $TARGET -L ../../$KERNEL_DIR \
       -a ../../../asp3/test -A $TEST -C test_pf.cdl -O "-DTOPPERS_USE_QEMU"
  fi
  make -j10
  echo "<---------- $TEST ---------->" >> ../../result.txt
  $RUN >> ../../result.txt
  echo "$TEST : " >> ../../summary.txt
  tail -n 1 ../../result.txt >> ../../summary.txt
  cd ../../
done

