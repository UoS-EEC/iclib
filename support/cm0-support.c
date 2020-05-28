/*
 * Copyright (c) 2019-2020, University of Southampton and Contributors.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "cm0-support.h"

#ifdef SIMULATION
#include <fused.h>
#endif

void indicate_workload_begin() {
#ifdef SIMULATION
  SIMPLE_MONITOR = SIMPLE_MONITOR_INDICATE_BEGIN;
#endif
  ;
}

void indicate_workload_end() {
#ifdef SIMULATION
  SIMPLE_MONITOR = SIMPLE_MONITOR_INDICATE_END;
#endif
}

void end_experiment() {
#ifdef SIMULATION
  SIMPLE_MONITOR = SIMPLE_MONITOR_KILL_SIM;
#endif
  while (1)
    ;
}

void wait() {
  for (volatile uint32_t i = 0; i < 10000ll; i++)
    ;  // delay
}

void target_init() { return; }

void disable_interrupt() { __asm__ volatile("cpsid i" :); }

void enable_interrupt() { __asm__ volatile("cpsie i" :); }

__attribute__((naked)) bool get_interrupt_enable() {
  __asm__ volatile(
      "mrs r0, primask\n"
      "bx lr"
      :);
}
