/*
 * Copyright (c) 2018-2020, University of Southampton.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "crc.h"
#include "iclib/ic.h"
#include "support/support.h"

// u8 input [64*AES_BLOCK_SIZE] MMDATA; // Random inputs
#include "lipsum.h"  // Generated input string

int main(void) {
  for (volatile unsigned i = 0; i < 3; ++i) {
    indicate_workload_begin();
    for (volatile int j = 0; j < 20; ++j) {
      uint32_t result __attribute__((unused)) = crc32buf(input, sizeof(input));
    }
    indicate_workload_end();
    mm_flush();
    wait();
  }
  end_experiment();
}
