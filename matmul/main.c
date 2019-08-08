#include <ic.h>
#include <memory_management.h>
#include <msp430fr5994.h>

#include "input.h"

int16_t output[MATSIZE][MATSIZE] MMDATA = {0};

/**
 * Naive implementation of matrix multiply using memory management. Very
 * inefficient.
 */
void matmult(int m, int n, int16_t a[m][n], int16_t b[m][n],
             int16_t out[m][n]) {
    uint8_t i, j, k;

    for (i = 0; i < m; ++i) {
        mm_acquire_array((uint8_t *)&out[i][0], sizeof(int) * n, MM_READWRITE);
        mm_acquire_array((uint8_t *)&a[i][0], sizeof(int) * m, MM_READWRITE);
        for (j = 0; j < n; ++j) {
            out[i][j] = 0;
            for (k = 0; k < m; ++k) {
                mm_acquire_array((uint8_t *)&b[k][j], sizeof(int),
                                 MM_READWRITE);
                out[i][j] += a[i][k] * b[k][j];
                mm_release_array((uint8_t *)&b[k][j], sizeof(int));
            }
        }
        mm_release_array((uint8_t *)&out[i][0], sizeof(int) * n);
        mm_release_array((uint8_t *)&a[i][0], sizeof(int) * m);
    }
}

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;  // Stop watchdog timer

    while (1) {
        P1OUT |= BIT2;
        matmult(MATSIZE, MATSIZE, a, b, output);
        P1OUT &= ~BIT2;
        // Clear mmdata
        mm_flush();
        // Toggle pin to show that computation has finished
        for (volatile long int i = 0; i < (16 - FRAM_WAIT) * 10000; i++) {
        }
    }
}
