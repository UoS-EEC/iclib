#include "test-common.h"
#include <ic.h>
#include <memory_management.h>
#include <msp430fr5994.h>

/* Benchmark includes */
#include "TI_aes_128_encr_only.h"
#define AES_BLOCK_SIZE (16u) // bytes

#include "lipsum.h" // Generated input string

static unsigned char key[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                              0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};

/* ------ Function Declarations ---------------------------------------------*/

int main(void) {
  WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer

  while (1) {
    P1OUT |= BIT2;
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
      for (int i = 0; i < AES_BLOCK_SIZE; i++) {
        ptr[i] = ptr[i] ^ prevBlock[i];
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

    P1OUT &= ~BIT2;
    mm_flush();

    // Delay
    WAIT
  }
}
