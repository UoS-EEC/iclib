# Efficient Reactive Intermittent Computing

Library, build system and examples for ManagedState, a memory manager that uses paged memory to improve the efficiency of state retention in reactive intermittent computing.

## Setup

### Dependencies
  + [cmake](https://cmake.org/)
  + [MSP 430 GCC toolchain](http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSPGCC/6_0_1_0/index_FDS.html) (tested with [msp430-gcc-7.3.1.24_linux64.tar.bz2](http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSPGCC/6_0_1_0/exports/msp430-gcc-7.3.1.24_linux64.tar.bz2))
  + [MSP430 support files](http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSPGCC/6_0_1_0/exports/msp430-gcc-support-files-1.205.zip)
  + (optional) [mspdebug](https://github.com/dlbeer/mspdebug)
    + Extract libmsp430.so from the [MSP430 Debug stack](http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSPDS/3_13_0_001/index_FDS.html) to e.g. /usr/lib

### Paths
Set environment variables `MSP_GCC_ROOT`, that points to the toolchain directory, and `MSP_GCC_INC` that points to the support file directory. This is done by e.g. adding the following two lines to your `~/.bashrc`:

```bash
export MSP_GCC_ROOT=$HOME/msp430-gcc
export MSP_GCC_INC=$HOME/msp430-inc
export MSPDEBUG_TILIB_PATH=${MSP_GCC_INC}/
```

