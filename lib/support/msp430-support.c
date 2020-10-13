/*
 * Copyright (c) 2019-2020, University of Southampton and Contributors.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <msp430fr5994.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "lib/support/support.h"

#ifdef SIMULATION
#include <fused.h>
#endif

#if TEXT_IN_SRAM && DATA_IN_SRAM
#define FRAM_WAIT_MULT 4
#elif TEXT_IN_SRAM
#define FRAM_WAIT_MULT (4 - FRAM_WAIT / 8)
#elif DATA_IN_SRAM
#define FRAM_WAIT_MULT (4 - FRAM_WAIT / 8)
#else
#define FRAM_WAIT_MULT (4 - FRAM_WAIT / 8)
#endif

void gpio_init();
void cs_init();

void indicate_workload_begin() {
  P1OUT |= BIT2;
#ifdef SIMULATION
  SIMPLE_MONITOR = SIMPLE_MONITOR_INDICATE_BEGIN;
#endif
}
void indicate_workload_end() {
  P1OUT &= ~BIT2;
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
  for (volatile uint32_t i = 0; i < FRAM_WAIT_MULT * 10000ll; i++)
    ;  // delay
}

void target_init() {
  cs_init();
  gpio_init();
}

__attribute__((optimize(0))) void cs_init() {
  CSCTL0_H = 0xA5;            // Unlock CS
  FRCTL0_H = 0xA5;            // Unlock FRAM ctrl
  FRCTL0_L = FRAM_WAIT << 4;  // wait states

  CSCTL1 = DCORSEL | DCOFSEL_4;  // 16 MHz

  // Set ACLK = VLO; MCLK = DCO; SMCLK = DCO
  CSCTL2 = SELA_1 + SELS_3 + SELM_3;

  // ACLK, SMCLK, MCLK dividers
  CSCTL3 = DIVA_0 + DIVS_1 + DIVM_1;
}

void gpio_init() {
  // Need to initialize all ports/pins to reduce power consump'n
  P1OUT = 0;  // LEDs on P1.0 and P1.1
  P1DIR = 0xff;
  P2OUT = 0;
  P2DIR = 0xff;
  P3OUT = 0;
  P3DIR = 0xff;
  P4OUT = BIT0;  // Pull-up on board
  P4DIR = 0xff;
  P6OUT = 0;
  P6DIR = 0xff;
  P7OUT = 0;
  P7DIR = 0xff;
  P8OUT = 0;
  P8DIR = 0xff;
  PM5CTL0 &= ~LOCKLPM5;
}

// Reset stack to 0s during boot (for clean backtrace when debugging)
__attribute__((optimize(0))) void __stack_set() {
  extern uint8_t __stack_low, __stack_high;
  register uint16_t *start = (uint16_t *)&__stack_low;
  register uint16_t *end = (uint16_t *)&__stack_high;
  while (start < end) {
    *start++ = 0x00;
  }
}

void enable_interrupt() { __bis_SR_register(GIE); }

void disable_interrupt() { __bic_SR_register(GIE); }

bool get_interrupt_enable() { return __get_SR_register() & GIE; }

// Flush cache
__attribute__((optimize(0))) void cache_flush() {
  // Fill cache with garbage
  __asm__ volatile(" nop \r\n");
  __asm__ volatile(" nop \r\n");
  __asm__ volatile(" nop \r\n");
  __asm__ volatile(" nop \r\n");
  __asm__ volatile(" nop \r\n");
  __asm__ volatile(" nop \r\n");
  __asm__ volatile(" nop \r\n");
  __asm__ volatile(" nop \r\n");
  __asm__ volatile(" nop \r\n");
  __asm__ volatile(" nop \r\n");
  __asm__ volatile(" nop \r\n");
  __asm__ volatile(" nop \r\n");
  __asm__ volatile(" nop \r\n");
  __asm__ volatile(" nop \r\n");
  __asm__ volatile(" nop \r\n");
  __asm__ volatile(" nop \r\n");
  __asm__ volatile(" nop \r\n");
}
