#include <msp430fr5994.h>
#include <string.h>

#include "config.h"
#include "ic.h"
#include "memory_management.h"

// ------------- CONSTANTS -----------------------------------------------------
extern uint8_t __stack_low, __stack_high;
extern uint8_t __bss_low, __bss_high;
extern uint8_t __data_low, __data_high, __data_loadLow;
extern uint8_t __mmdata_start, __mmdata_end, __mmdata_loadStart;
extern uint8_t __boot_stack_high;
extern uint8_t __npdata_loadLow, __npdata_low, __npdata_high;

// ------------- PERSISTENT VARIABLES ------------------------------------------
#define PERSISTENT __attribute__((section(".fram_vars")))

// Restore/suspend thresholds
uint16_t restore_thr PERSISTENT = 2764;  // 2.7 V initial value
uint16_t suspend_thr PERSISTENT = 2355;  // 2.3 V initial value

// Snapshots
uint16_t register_snapshot[15] PERSISTENT;
uint16_t bss_snapshot[BSS_SIZE] PERSISTENT;
uint16_t data_snapshot[DATA_SIZE] PERSISTENT;
uint16_t stack_snapshot[STACK_SIZE] PERSISTENT;

int suspending PERSISTENT;         /*! Flag to determine whether returning from
                                  suspend() or restore() */
int snapshotValid PERSISTENT = 0;  //! Flag: whether snapshot is valid
int needRestore PERSISTENT = 0;    /*! Flag: whether restore is needed i.e. high
                                      when booting from a power outtage */

/* ------ Function Prototypes -----------------------------------------------*/
static void adc_init(void);
static void gpio_init(void);
static void clock_init(void);
static void restore(void);

/* ------ ASM functions ---------------------------------------------------- */
extern void suspend(uint16_t *regSnapshot);
extern void restore_registers(uint16_t *regSnapshot);

/* ------ Function Declarations ---------------------------------------------*/

void __attribute__((interrupt(RESET_VECTOR))) iclib_boot(void) {
    __set_SP_register(&__boot_stack_high);  // Boot stack

    needRestore = 1;  // Indicate powerup

    WDTCTL = WDTPW | WDTHOLD;  // Stop watchdog timer

    // Initialise
    memcpy(&__npdata_low, &__npdata_loadLow, &__npdata_high - &__npdata_low);
    clock_init();
    gpio_init();
    adc_init();

    __bis_SR_register(LPM4_bits + GIE);  // Enter LPM4 with interrupts enabled
    // Processor sleeps
    // ...
    // Processor wakes up after interrupt
    // *!Remaining code in this function is only executed during the first
    // boot!*
    //
    __set_SP_register(&__stack_high);  // Runtime stack
    memcpy(&__data_low, &__data_loadLow, &__data_high - &__data_low);
    mm_init_lru();

#ifndef TRACK_MMDATA
    uint16_t mmdata_size = &__mmdata_end - &__mmdata_start;
    update_thresholds(mmdata_size, mmdata_size);
#endif

    P6OUT |= BIT0;  // Indicate active
    int main();     // Suppress implicit decl. warning
    main();
}

static void gpio_init(void) {
    // Need to initialize all ports/pins to reduce power consump'n
    P1OUT = 0;  // LEDs on P1.0 and P1.1
    P1DIR = 0xff;
    P2OUT = 0;
    P2DIR = 0xff;
    P3OUT = 0;
    P3DIR = 0xff;
    P4OUT = BIT0;  // Pull-up on board
    P4DIR = 0xff;
    P6OUT = 0;
    P6DIR = 0xff;
    P7OUT = 0;
    P7DIR = 0xff;
    P8OUT = 0;
    P8DIR = 0xff;
    PM5CTL0 &= ~LOCKLPM5;
    PCIFG = 0;  // Clear pending interrupts
}

static void clock_init(void) {
    CSCTL0_H = 0xA5;  // Unlock register

    /* 16 MHz clock [Source: Table 5-6 from datasheet SLASE54B] */
    CSCTL1 = DCORSEL | DCOFSEL_4;  // DCO = 16 MHz

    // Set ACLK = VLO; MCLK = DCO/2; SMCLK = DCO/2;
    CSCTL2 = SELA_1 + SELS_3 + SELM_3;
    CSCTL3 = DIVA_0 + DIVS_1 + DIVM_1;
}

void suspendVM(void) {
    unsigned nBytesToSave;

    uint8_t *src;
    uint8_t *dst;
    size_t len;

    // mmdata
#ifdef TRACK_MMDATA
    // Save modified pages
    nBytesToSave = mm_suspendStatic();
#else
    // Save entire section
    src = &__mmdata_start;
    dst = (uint8_t *)&__mmdata_loadStart;
    len = &__mmdata_end - src;
    memcpy(dst, src, len);
#endif

    // bss
    src = &__bss_low;
    dst = (uint8_t *)bss_snapshot;
    len = &__bss_high - src;
    memcpy(dst, src, len);
    nBytesToSave += len;

    // data
    src = &__data_low;
    dst = (uint8_t *)data_snapshot;
    len = &__data_high - src;
    memcpy(dst, src, len);
    nBytesToSave += len;

    // stack
    // stack_low-----[SP-------stack_high]
#ifdef TRACK_STACK
    src = (uint8_t *)register_snapshot[0];  // Saved SP
#else
    src = &__stack_low;
#endif
    len = &__stack_high - src;
    uint16_t offset = (uint16_t)(src - &__stack_low) / 2;  // word offset
    dst = (uint8_t *)&stack_snapshot[offset];
    memcpy((void *)dst, (void *)src, len);
    nBytesToSave += len;

    suspending = 1;
}

void restore(void) {
    uint8_t *src;
    uint8_t *dst;
    size_t len;

    suspending = 0;

    // data
    dst = &__data_low;
    src = (uint8_t *)data_snapshot;
    len = &__data_high - dst;
    memcpy(dst, src, len);

    // bss
    dst = &__bss_low;
    src = (uint8_t *)bss_snapshot;
    len = &__bss_high - dst;
    memcpy(dst, src, len);

    // mmdata
#ifdef TRACK_MMDATA
    // Restore active pages only
    mm_restoreStatic();
#else
    // Restore entire section
    dst = &__mmdata_start;
    len = &__mmdata_end - dst;
    src = (uint8_t *)&__mmdata_loadStart;
    memcpy(dst, src, len);
#endif

    // stack
#ifdef TRACK_STACK
    dst = (uint8_t *)register_snapshot[0];  // Saved stack pointer
#else
    dst = &__stack_low;  // Save full stack
#endif
    len = &__stack_high - dst;
    uint16_t offset =
        (uint16_t)((uint16_t *)dst - (uint16_t *)&__stack_low);  // word offset
    src = (uint8_t *)&stack_snapshot[offset];
    memcpy(dst, src, len);  // Restore default stack

    restore_registers(register_snapshot);  // Returns to line after suspend()
}

/**
 * Set up ADC in window comparator mode to monitor supply voltage
 */
static void adc_init(void) {
    /*
     * ADC12 Configuration
     * tSample = 16ADC12CLK, tConvert = 14 ADC12CLK cycles
     * Clock: 75kHz
     */

    ADC12CTL0 = ADC12SHT0_2 |  // 12 Cycles sample time
                ADC12MSC |     // Continuous mode
                ADC12ON;

    ADC12CTL1 = ADC12SSEL_0 |    // MODCLK (4.8 MHz)
                ADC12PDIV_3 |    // MODCLK/64 = 75kHz Clock
                ADC12SHS_0 |     // Software trigger for start of conversion
                ADC12CONSEQ_2 |  // Repeat Single Channel
                ADC12SHP_1;      // Automatically trigger conv. after sample

    ADC12CTL2 = ADC12PWRMD_1 |  // Low-power mode
                ADC12RES_2 |    // 8-bit resolution
                ADC12DF_0;      // Binary unsigned output

    ADC12CTL3 = ADC12BATMAP_1;  // 1/2AVcc channel selected for ADC ch A31

    ADC12MCTL0 = ADC12INCH_31 |  // Vcc
                 ADC12VRSEL_1 |  // Measure V between Vcc and GND
                 ADC12WINC_1;    // Comparator window enable

    // Interrupts
    ADC12HI = restore_thr;
    ADC12LO = suspend_thr;
    ADC12IER2 = ADC12HIIE;

    // Configure internal reference
    while (REFCTL0 & REFGENBUSY)
        ;
    REFCTL0 |= REFVSEL_1 | REFON;  // Select internal ref = 2.0V
    while (!(REFCTL0 & REFGENRDY))
        ;

    ADC12CTL0 |= ADC12ENC | ADC12SC;  // Enable & start conversion
}

void __attribute__((__interrupt__(ADC12_B_VECTOR))) adc12_isr(void) {
    __disable_interrupt();

    switch (__even_in_range(ADC12IV, ADC12IV__ADC12RDYIFG)) {
        case ADC12IV__NONE:
            break;                 // Vector  0:  No interrupt
        case ADC12IV__ADC12HIIFG:  // High Interrupt - Restore

            // Disable high interrupt and enable low interrupt
            ADC12IER2 = ADC12LOIE;

            if (needRestore) {
                if (snapshotValid) {  // Restore from snapshot
                    P1OUT |= BIT2;    // dbg
                    restore();  // **Restore returns to line after suspend()**
                } else {
                    // Boot in iclib_boot
                }
                needRestore = 0;
            } else {            // Survived power-outage, no need to restore
                P6OUT |= BIT0;  // Indicate active
            }

            __bic_SR_register_on_exit(LPM4_bits);  // Wake up on return
            break;
        case ADC12IV__ADC12LOIFG:  // Low Interrupt - Suspend

            // Disable low interrupt and enable high
            ADC12IER2 = ADC12HIIE;

            P1OUT |= BIT4;
            //        P1OUT &= ~BIT5;
            P6OUT &= ~BIT0;  // Indicate not active
            snapshotValid = 0;
            suspend(register_snapshot);

            //!! Execution enters this line either:
            // 1. when returning from suspend(), 2. when returning from
            // restore()
            P1OUT &= ~BIT4;
            if (suspending) {  // Returning from suspend(), go to sleep
                snapshotValid = 1;
                __bis_SR_register_on_exit(LPM4_bits);  // Sleep on return
            } else {  // Returning from Restore(), continue execution
                P1OUT &= ~(BIT2 | BIT4);
                P6OUT |= BIT0;                         // Indicate active
                __bic_SR_register_on_exit(LPM4_bits);  // Wake up on return
            }
            break;
        default:
            break;
    }

    ADC12IFGR2 = 0;                  // Clear Interrupt flags
    __bis_SR_register_on_exit(GIE);  // enable global interrupts
}

void update_thresholds(uint16_t n_suspend, uint16_t n_restore) {
    static const uint32_t factor = DVDT;
    static uint16_t suspend_old = 0;
    static uint16_t restore_old = 0;

    if (n_suspend == suspend_old && n_restore == restore_old) {
        return;  // No need for updates
    }

    const uint16_t untracked =
        (uint16_t)((&__data_high - &__data_low) + (&__bss_high - &__bss_low) +
                   (&__stack_high - &__stack_low));

    // newVS = (1024*V_ON + factor*bytes_to_save)/1024
    uint32_t newVS = factor * (uint32_t)(untracked + n_suspend);
    newVS >>= 10;  // /1024
    newVS += VON;  // Offset (minimum voltage)

    // newVR = newVS + V_C + factor*bytes_to_restore/1024
    uint32_t newVR = factor * (uint32_t)(untracked + n_restore);
    newVR >>= 10;          // /1024
    newVR += newVS + V_C;  // hysteresis over suspend threshold

    if (newVR > VMAX) {
        while (1)
            ;  // Error: No safe restore thr found
    }

    restore_thr = (uint16_t)newVR;
    suspend_thr = (uint16_t)newVS;

    ADC12HI = restore_thr > VMAX ? VMAX : restore_thr;
    ADC12LO = suspend_thr;
}
