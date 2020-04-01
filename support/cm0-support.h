/*
 * Copyright (c) 2018-2020, University of Southampton.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _SUPPORT_H
#define _SUPPORT_H

#include <fused.h>
#include <stdbool.h>
#include "support/cm0.h"

// Initialize target
void target_init();

// Wait between workload iterations
void wait();

// Indicate start of workload
void indicate_begin();

// Indicate end of workload
void indicate_end();

// Assert & report test failures
void fused_assert(bool c);

// End simulation
void fused_end_simulation();

#endif
