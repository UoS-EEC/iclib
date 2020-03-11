/*
 * Copyright (c) 2018-2020, University of Southampton.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "support/support.h"
#include "iclib/ic.h"
#include "crc.h"

// u8 input [64*AES_BLOCK_SIZE] MMDATA; // Random inputs
#include "lipsum.h" // Generated input string

int main(void) {
  while (1) {
    indicate_begin();
    for (volatile int i = 0; i < 20; i++) {
      uint32_t result __attribute__((unused)) = crc32buf(input, sizeof(input));
    }
    indicate_end();
    mm_flush();
    wait();
  }
}
