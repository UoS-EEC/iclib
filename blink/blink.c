#include <ic.h>
#include <memory_management.h>
#include <msp430fr5994.h>

int main(int argc, char* argv[]) {
    // Stop the watchdog timer
    WDTCTL = WDTPW + WDTHOLD;

    // Disable the GPIO power-on default high-impedance mode
    PM5CTL0 &= ~LOCKLPM5;
    P1DIR |= BIT0;  // Set the direction of P1.0 to OUTPUT

    // Toggle LEDs forever
    while (1) {
        for (volatile unsigned int i = 0; i < 50000; i++)
            ;           // Delay
        P1OUT ^= BIT0;  // Toggle
    }
}

