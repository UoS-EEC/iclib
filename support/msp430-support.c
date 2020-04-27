/*
 * Copyright (c) 2018-2020, University of Southampton.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <msp430fr5994.h>
#include <stdint.h>
#include "support/msp430-support.h"

void target_init() {}

void indicate_begin() { P1OUT |= BIT2; }

void indicate_end() { P1OUT &= ~BIT2; }

void wait() {
  for (volatile long int i = 0; i < (16 - FRAM_WAIT) * 2000l; i++)
    ;
}

void __attribute__((section(".ramtext"), naked))
fastmemcpy(uint8_t *dst, uint8_t *src, size_t len) {
  __asm__(
      " push r5\n"
      " tst r14\n"  // Test for len=0
      " jz return\n"
      " mov #2, r5\n"    // r5 = word size
      " xor r15, r15\n"  // Clear r15
      " mov r14, r15\n"  // r15=len
      " and #1, r15\n"   // r15 = len%2
      " sub r15, r14\n"  // r14 = len - len%2
      "loopmemcpy:  \n"
      " mov.w @r13+, @r12\n"
      " add r5, r12 \n"
      " sub r5, r14 \n"
      " jnz loopmemcpy \n"
      " tst r15\n"
      " jz return\n"
      " mov.b @r13, @r12\n"  // move last byte
      "return:\n"
      " pop r5\n"
      " ret\n");
}
