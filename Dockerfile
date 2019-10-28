# Stage 1: Install dependencies
FROM ubuntu:bionic as msp-build
  RUN apt update && apt install -y ninja-build wget unzip vim

  # MSP430-GCC
  RUN mkdir /opt/msp430-gcc && \
      wget http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSPGCC/8_2_0_0/exports/msp430-gcc-8.2.0.52_linux64.tar.bz2 \
      -O - \
      | tar -xjv \
            --directory /opt/msp430-gcc \
            --strip-components 1
  RUN wget http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSPGCC/8_2_0_0/exports/msp430-gcc-support-files-1.207.zip \
        -O /tmp/msp430-supp.zip &&\
      unzip /tmp/msp430-supp.zip -d /tmp && \
      rm /tmp/msp430-supp.zip && \
      mv /tmp/msp430-gcc-support* /opt/msp430-inc

  # CMAKE v3.15.4
  RUN wget https://github.com/Kitware/CMake/releases/download/v3.15.4/cmake-3.15.4-Linux-x86_64.sh &&\
      chmod a+x cmake*.sh &&\
      ./cmake*.sh --skip-license --prefix=/usr/local &&\
      rm cmake*.sh

  ENV MSP430_GCC_ROOT=/opt/msp430-gcc
  ENV MSP430_INC=/opt/msp430-inc
  WORKDIR /opt/src

