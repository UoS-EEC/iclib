#ifndef CM0_IC_H
#define CM0_IC_H

#define PERSISTENT __attribute__((section(".persistent")))
#define MMDATA __attribute__((section(".mmdata")))

#include <stdint.h>
#include "iclib/config.h"

// Boot function
void _start();

#endif
