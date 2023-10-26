mkdir -p build
( cd build
  ../../../asp3/configure.rb -T primehub_gcc -L ../../../build/obj-primehub_kernel -a ../ -A ${1:-idle} -m ../../../common/app.mk
)
