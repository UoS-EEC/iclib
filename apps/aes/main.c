/*
 * Copyright (c) 2018-2020, University of Southampton.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "TI_aes_128_encr_only.h"
#include "lib/iclib/ic.h"
#include "lib/support/support.h"

#define AES_BLOCK_SIZE (16u) // bytes

#include "lipsum.h" // Input string

static unsigned char key[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                              0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};

/* ------ Function Declarations ---------------------------------------------*/

void main(void) {
  for (volatile unsigned i = 0; i < 3; i++) {
    indicate_workload_begin();
    // AES128 in Cipher Block Chaining mode
    uint8_t *prevBlock;
    uint8_t *ptr = input;

    // Acquire and encrypt first block
    mm_acquire_array(ptr, AES_BLOCK_SIZE, MM_READWRITE);
    aes_encrypt(ptr, key);
    prevBlock = ptr;
    ptr += AES_BLOCK_SIZE;

    // Encrypt remaining blocks
    while (ptr < input + sizeof(input)) {
      // Acquire current block
      mm_acquire_array(ptr, AES_BLOCK_SIZE, MM_READWRITE);

      // CBC - Cipher Block Chaining mode
      for (int j = 0; j < AES_BLOCK_SIZE; j++) {
        ptr[j] = ptr[j] ^ prevBlock[j];
      }

      // Release previous block
      mm_release_array(prevBlock, AES_BLOCK_SIZE);

      // Encrypt current block
      aes_encrypt(ptr, key);

      prevBlock = ptr;
      ptr += AES_BLOCK_SIZE;
    }

    // Release last block
    mm_release_array(prevBlock, AES_BLOCK_SIZE);

    indicate_workload_end();
    mm_flush();

    // Delay
    wait();
  }
  end_experiment();
}
