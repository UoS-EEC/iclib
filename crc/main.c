#include "test-common.h"
#include <ic.h>
#include <memory_management.h>
#include <msp430fr5994.h>

/* Benchmark includes */
#include "crc.h"

// u8 input [64*AES_BLOCK_SIZE] MMDATA; // Random inputs
#include "lipsum.h" // Generated input string

int main(void) {
  WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer

  while (1) {
    P1OUT |= BIT2;
    for (volatile int i = 0; i < 20; i++) {
      uint32_t result __attribute__((unused)) = crc32buf(input, sizeof(input));
    }
    P1OUT &= ~BIT2;
    mm_flush();

    // Delay
    WAIT
  }
}
