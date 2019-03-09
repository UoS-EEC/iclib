#include <ic.h>
#include <memory_management.h>
#include <msp430fr5994.h>

/* Benchmark includes */
#include "crc.h"

// u8 input [64*AES_BLOCK_SIZE] MMDATA; // Random inputs
#include "lipsum.h"  // Generated input string

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;  // Stop watchdog timer
    ic_init();

    while (1) {
        uint32_t result __attribute__((unused)) =
            crc32buf(input, sizeof(input));
        // Toggle pin to show that computation has finished
        P1OUT |= BIT2;
        for (int i = 0; i < 100; i++) {
        }
        P1OUT &= ~BIT2;
    }
}
