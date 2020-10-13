/*
 * Copyright (c) 2018-2020, University of Southampton.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef CM0_ARCH
#define TARGET_WORD uint32_t
#include "lib/support/cm0-support.h"
#elif defined(MSP430_ARCH)
#define TARGE_WORD uint16_t
#include <msp430fr5994.h>
#include "lib/support/msp430-support.h"
#else
#error Target architecture must be defined
#endif

// ------ Target functions ------

// Initialize target
void target_init();

// Wait between workload iterations
void wait();

// Indicate start of workload
void indicate_workload_begin();

// Indicate end of workload
void indicate_workload_end();

// Indicate test fail
void indicate_test_fail();

// End experiment
void end_experiment();

// Global disable interrupts
void disable_interrupt();

// Global enable interrupts
void enable_interrupt();

// Check global interrupt enable state
bool get_interrupt_enable();

// ------ Functions that must be implemented by benchmarks ------

/**
 * @brief Verify benchmark result
 * @param result return value of benchmark
 * @return 0 if success, -1 if no verification done, 1 otherwise
 */
extern int verify_benchmark(int result);

/**
 * @brief Initialise benchmark.
 */
extern void initialise_benchmark(void);

/**
 * @brief Run benchmark.
 * @return benchmark-specific return value. For example result of computation.
 */
extern int benchmark(void) __attribute__((noinline));
