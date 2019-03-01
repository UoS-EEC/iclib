#include <ic.h>
#include <memory_management.h>
#include <msp430fr5994.h>

#include "input.h"

int16_t output[MATSIZE][MATSIZE] MMDATA;

/**
 * Naive implementation of matrix multiply using memory management. Very
 * inefficient.
 */
void matmult(int m, int n, int16_t a[m][n], int16_t b[m][n],
             int16_t out[m][n]) {
    int i, j, k;
    // Initialize output to 0
    for (i = 0; i < m; ++i) {
        mm_acquire_array((u8 *)&out[i], sizeof(int) * n, MM_READWRITE);
        for (j = 0; j < n; ++j) {
            out[i][j] = 0;
        }
        mm_release_array((u8 *)&out[i], sizeof(int) * n);
    }

    for (i = 0; i < m; ++i) {
        mm_acquire_array((u8 *)&out[i][0], sizeof(int) * n, MM_READWRITE);
        mm_acquire_array((u8 *)&a[i][0], sizeof(int) * m, MM_READWRITE);
        for (j = 0; j < n; ++j) {
            for (k = 0; k < m; ++k) {
                mm_acquire_array((u8 *)&b[k][j], sizeof(int), MM_READWRITE);
                out[i][j] += a[i][k] * b[k][j];
                mm_release_array((u8 *)&b[k][j], sizeof(int));
            }
        }
        mm_release_array((u8 *)&out[i][0], sizeof(int) * n);
        mm_release_array((u8 *)&a[i][0], sizeof(int) * m);
    }
}

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;  // Stop watchdog timer
    ic_init();

    while (1) {
        matmult(MATSIZE, MATSIZE, a, b, output);
        // Toggle pin to show that computation has finished
        P1OUT |= BIT2;
        for (int i = 0; i < 100; i++) {
        }
        P1OUT &= ~BIT2;
    }
}