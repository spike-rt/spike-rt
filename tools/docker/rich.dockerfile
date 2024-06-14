FROM ubuntu:22.04

ARG GCC_URL=https://developer.arm.com/-/media/Files/downloads/gnu-rm/10.3-2021.10/gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2
ARG Doxygen_URL=https://www.doxygen.nl/files/doxygen-1.9.5.linux.bin.tar.gz
ARG CMake_URL=https://github.com/Kitware/CMake/releases/download/v3.24.3/cmake-3.24.3-linux-x86_64.tar.gz
ARG QEMU_URL=https://github.com/spike-rt/qemu/releases/download/v7.2.0-lego-spike.0/qemu-system-arm-bin-7.2.0-lego-spike.0-linux-x86_64.tar.gz

# Install required apt packages
RUN apt update \
    && apt install -y --no-install-recommends \
          curl build-essential libncurses5 python3 git \
          ruby3.0 \
    && rm -rf /var/lib/apt/lists/*

# Install GNU Arm Toolchain
RUN curl -L ${GCC_URL} -o gcc-arm-none-eabi.tar.bz2 \
    && mkdir -p  /usr/local/gcc-arm-none-eabi \
    && tar -xf gcc-arm-none-eabi.tar.bz2 -C /usr/local/gcc-arm-none-eabi --strip-components=1 \
    && rm -rf gcc-arm-none-eabi.tar.bz2
ENV PATH $PATH:/usr/local/gcc-arm-none-eabi/bin

# Install required Ruby gem
RUN gem install shell

# Install CMake
RUN curl -L ${CMake_URL} -o cmake.tar.gz \
    && mkdir -p  /usr/local/cmake \
    && tar -xf cmake.tar.gz -C /usr/local/cmake --strip-components=1 \
    && rm -rf cmake.tar.gz
ENV PATH $PATH:/usr/local/cmake/bin

# Install Doxygen
RUN curl -L ${Doxygen_URL} -o doxygen.tar.gz \
    && mkdir -p  /usr/local/doxygen \
    && tar -xf doxygen.tar.gz -C /usr/local/doxygen --strip-components=1 \
    && rm -rf doxygen.tar.gz
ENV PATH $PATH:/usr/local/doxygen/bin

# Install QEMU
RUN curl -L ${QEMU_URL} -o qemu-system-arm-bin.tar.gz \
    && mkdir -p  /usr/local/qemu/bin \
    && tar -xf qemu-system-arm-bin.tar.gz -C /usr/local/qemu/bin \
    && rm -rf qemu-system-arm-bin.tar.gz
ENV PATH $PATH:/usr/local/qemu/bin
