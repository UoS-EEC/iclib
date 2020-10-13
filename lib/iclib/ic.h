/*
 * Copyright (c) 2018-2020, University of Southampton.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include "lib/iclib/memory-management.h"
#include "lib/support/msp430-support.h"

/* ------ Target-specific includes ----- */
#if defined(MSP430_ARCH)
#include "lib/iclib/msp430-ic.h"
#elif defined(CM0_ARCH)
#include "lib/iclib/cm0-ic.h"
#endif

/* ------ Memory allocation macros ------ */
#define MMDATA __attribute__((section(".mmdata")))
#define PERSISTENT __attribute__((section(".persistent")))

/* ------ Extern functions ------ */

/**
 * @brief Update restore and suspend thresholds based on number of bytes to be
 * suspended/restored
 *
 * @param n_suspend
 * @param n_restore
 */
void ic_update_thresholds(unsigned n_suspend, unsigned n_restore);
