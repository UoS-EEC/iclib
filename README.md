# ICLIB: a library for Reactive Intermittent Computing

Library, build system and examples for three flavours of reactive intermittent 
computing:

  - QuickRecall
  - AllocatedState
  - ManagedState

**QuickRecall** is the simplest method, where all data resides in non-volatile 
memory (FRAM). When a power failure is detected, the device saves CPU registers 
and sleeps. Although simple, this method results in very high power consumption 
during on-periods, because accesses to FRAM are far more expensive than those 
to SRAM.

**AllocatedState** allocates data to SRAM to lower power consumption, and must 
therefore load the `.data` and `.bss` sections, as well as the stack, from FRAM 
to SRAM during  boot, and likewise save them to FRAM when power fails. 
Although improving power consumption during the on-period, loading and saving 
the entire allocated state can get quite expensive.

**ManagedState** improves on *AllocatedState* by tracking active and modified 
pages of memory, so that only active pages need to be loaded during boot, 
and only modified pages need to be saved when power fails. This can 
substantially reduce energy consumption and thereby improve performance over 
the two other methods.


*ManagedState* was presented in an academic paper,

> "Efficient State Retention through Paged Memory Management for Reactive Transient Computing",<br />
> Sivert T. Sliper, Domenico Balsamo, Nikos Nikoleris, William Wang, Alex S. Weddell and Geoff V. Merrett,<br />
> IEEE Design Automation Conference (DAC 56), Las Vegas, 2019<br />

available at [DOI 10.1145/33167813317812](https://doi.org/10.1145/33167813317812).

## Targets

Currently, *ICLIB* supports the *MSPFR5994* platform, and can readily be ported 
to other *MSP430*-based platforms. Support for *Arm Cortex-M0* targets is under 
development.

## Setup

### Dependencies

The following dependencies are needed to build *ICLIB*:

  + [cmake](https://cmake.org/)
  + [MSP 430 GCC toolchain](http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSPGCC/6_0_1_0/index_FDS.html) (tested with [msp430-gcc-7.3.1.24_linux64.tar.bz2](http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSPGCC/6_0_1_0/exports/msp430-gcc-7.3.1.24_linux64.tar.bz2))
  + [MSP430 support files](http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSPGCC/6_0_1_0/exports/msp430-gcc-support-files-1.205.zip)
  + (optional) [mspdebug](https://github.com/dlbeer/mspdebug)
    + Extract libmsp430.so from the [MSP430 Debug stack](http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSPDS/3_13_0_001/index_FDS.html) to e.g. `/usr/lib`

### Paths
Set environment variables `MSP430_GCC_ROOT`, that points to the toolchain 
directory, and `MSP430_INC` that points to the support file directory. This is 
done by e.g. adding the following two lines to your `~/.bashrc`:

```bash
export MSP430_GCC_ROOT=<path/to/MSP430 GCC folder>
export MSP430_INC=<path/to/MSP430 support files>
```

## Getting started
*ICLIB* is built using CMAKE.
To build an application make a build directory, and call CMAKE with options to 
set the target platform. 

```bash
cd iclib
mkdir build
cd build
cmake .. -DTARGET_ARCH=msp430
```

By default, CMAKE is set up to do a `debug` build, for benchmarking you should 
run the `release` build:
```bash
cmake .. -DTARGET_ARCH=msp430 -DCMAKE_BUILD_TYPE=Release
```

Then build an executable, for example `aes` using *ManagedState*:

```bash
make aes-MS-msp430
```

The syntax here is `<app-icmethod-target>`, where `QR` selects *QuickRecall*,
`AS` selects *AllocatedState*, and `MS` selects *ManagedState*.

Finally, on supported platforms, you can upload the executable to the 
microcontroller using

```bash
make upload_aes-MS-msp430
```

Note that this uses `mspdebug`, and will only work on some setups (only tested 
on a laptop running Ubuntu 18.04).