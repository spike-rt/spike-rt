FROM ruby:2.7.4-bullseye

ARG GCC_URL=https://launchpad.net/gcc-arm-embedded/5.0/5-2016-q3-update/+download/gcc-arm-none-eabi-5_4-2016q3-20160926-linux.tar.bz2

# Install GNU Arm Toolchain
RUN cd /usr/local && curl -OL ${GCC_URL} && tar -xf gcc-arm-none-eabi-*.tar.bz2 && rm -rf gcc-arm-none-eabi-*.tar.bz2
ENV PATH $PATH:/usr/local/gcc-arm-none-eabi-5_4-2016q3/bin

# Install requirements
RUN gem install shell
RUN apt update && apt install -y libc6-i386  && rm -rf /var/lib/apt/lists/*
