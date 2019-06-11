# Efficient Reactive Intermittent Computing

Library, build system and examples for ManagedState, a memory manager that uses paged memory to improve the efficiency of state retention in reactive intermittent computing.


Set up for use with MSP430FR5994, but can readily be ported for similar platforms.

ManagedState was presented in an academic paper,

> "Efficient State Retention through Paged Memory Management for Reactive Transient Computing",<br />
> Sivert T. Sliper, Domenico Balsamo, Nikos Nikoleris, William Wang, Alex S. Weddell and Geoff V. Merrett,<br />
> IEEE Design Automation Conference (DAC 56), Las Vegas, 2019.<br />

The paper is available at [DOI 10.1145/33167813317812](https://doi.org/10.1145/3316781.3317812), and the supporting dataset at [DOI 10.5258/SOTON/D0835](http://dx.doi.org/10.5258/SOTON/D0835).

## Setup

### Dependencies
  + [cmake](https://cmake.org/)
  + [MSP 430 GCC toolchain](http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSPGCC/6_0_1_0/index_FDS.html) (tested with [msp430-gcc-7.3.1.24_linux64.tar.bz2](http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSPGCC/6_0_1_0/exports/msp430-gcc-7.3.1.24_linux64.tar.bz2))
  + [MSP430 support files](http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSPGCC/6_0_1_0/exports/msp430-gcc-support-files-1.205.zip)
  + (optional) [mspdebug](https://github.com/dlbeer/mspdebug)
    + Extract libmsp430.so from the [MSP430 Debug stack](http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSPDS/3_13_0_001/index_FDS.html) to e.g. `/usr/lib`

### Paths
Set environment variables `MSP430_GCC_ROOT`, that points to the toolchain directory, and `MSP430_INC` that points to the support file directory. This is done by e.g. adding the following two lines to your `~/.bashrc`:

```bash
export MSP430_GCC_ROOT=<path/to/MSP430 GCC folder>
export MSP430_INC=<path/to/MSP430 support files>
```

## Getting started
First, build `libic.a`:
```bash
cd iclib
mkdir build
cd build
cmake ..
make
cp libic.a ..
```

Then, build an example, e.g `aes`

```bash
cd aes128
mkdir build
cd build
cmake ..
make
```

and program it to the development board:

```bash
make upload_aes
```
