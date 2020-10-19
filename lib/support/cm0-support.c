/*
 * Copyright (c) 2019-2020, University of Southampton and Contributors.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "lib/support/cm0-support.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#ifdef SIMULATION
#include <fused.h>
#endif

#define PIN_KILL_SIM (1u << 0)
#define PIN_WORKLOAD_BEGIN (1u << 1)
#define PIN_ATOMIC_BEGIN (1u << 2)
#define PIN_TEST_FAIL (1u << 3)
#define PIN_SWFAIL (1u << 4)
#define PIN_KEEP_ALIVE (1u << 5)

void indicate_workload_begin() {
#ifdef SIMULATION
  SIMPLE_MONITOR = SIMPLE_MONITOR_INDICATE_BEGIN;
#endif
  Gpio->DATA.WORD |= PIN_WORKLOAD_BEGIN;
  ;
}

void indicate_workload_end() {
#ifdef SIMULATION
  SIMPLE_MONITOR = SIMPLE_MONITOR_INDICATE_END;
#endif
  Gpio->DATA.WORD &= ~PIN_WORKLOAD_BEGIN;
}

void indicate_test_fail() {
#ifdef SIMULATION
  SIMPLE_MONITOR = SIMPLE_MONITOR_TEST_FAIL;
#endif
  Gpio->DATA.WORD |= PIN_TEST_FAIL;
  while (1)
    ;
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
    ; // delay
}

void disable_interrupt() { __asm__ volatile("cpsid i" :); }

void enable_interrupt() { __asm__ volatile("cpsie i" :); }

bool get_interrupt_enable() {
  bool res;
  __asm__("mrs %0, primask" : "=r"(res));
  return res;
}

void target_init() {
  Gpio->DATA.WORD = 0;
  Gpio->DIR.WORD = 0xff; // Set output mode on pins
  return;
}

void assert_keep_alive() { Gpio->DATA.WORD |= PIN_KEEP_ALIVE; }

void deassert_keep_alive() { Gpio->DATA.WORD &= ~PIN_KEEP_ALIVE; }
