#ifndef IC_SRC
#define IC_SRC

#define PERSISTENT __attribute__((section(".persistent")))
#define MMDATA __attribute__((section(".mmdata")))

#include <stdint.h>
#include "iclib/config.h"

void _start();

#endif /* IC_SRC */
