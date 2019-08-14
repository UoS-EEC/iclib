#include <msp430fr5994.h>

#include "common.h"
#include "config.h"
#include "ic.h"
#include "memory_management.h"
#ifdef ALLOCATEDSTATE
#include "dvdb-AS.h"
#else
#include "dvdb-MS.h"
#endif

// ------------- CONSTANTS -----------------------------------------------------
extern uint8_t __stack_low, __stack_high;
extern uint8_t __bss_low, __bss_high;
extern uint8_t __data_low, __data_high, __data_loadLow;
extern uint8_t __mmdata_start, __mmdata_end, __mmdata_loadStart;
extern uint8_t __boot_stack_high;
extern uint8_t __npdata_loadLow, __npdata_low, __npdata_high;

// ------------- Globals -------------------------------------------------------
static uint16_t *stackTrunk = (uint16_t *)&__stack_low;

// ------------- PERSISTENT VARIABLES ------------------------------------------
#define PERSISTENT __attribute__((section(".fram_vars")))

// Restore/suspend thresholds
uint16_t restore_thr PERSISTENT = 2764 >> 2; // 2.7 V initial value
uint16_t suspend_thr PERSISTENT = 2355 >> 2; // 2.3 V initial value

// Snapshots
uint16_t register_snapshot[15] PERSISTENT;
uint16_t bss_snapshot[BSS_SIZE] PERSISTENT;
uint16_t data_snapshot[DATA_SIZE] PERSISTENT;
uint16_t stack_snapshot[STACK_SIZE] PERSISTENT;

int suspending PERSISTENT;        /*! Flag to determine whether returning from
                                 suspend() or restore() */
int snapshotValid PERSISTENT = 0; //! Flag: whether snapshot is valid
int needRestore PERSISTENT = 0;   /*! Flag: whether restore is needed i.e. high
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

void __attribute__((interrupt(RESET_VECTOR), naked, used, optimize("O0")))
iclib_boot() {
  __set_SP_register(&__boot_stack_high); // Boot stack
  WDTCTL = WDTPW | WDTHOLD;              // Stop watchdog timer

  // Initialise
  fastmemcpy(&__npdata_low, &__npdata_loadLow, &__npdata_high - &__npdata_low);
  clock_init();
  gpio_init();
  adc_init();

  needRestore = 1;                    // Indicate powerup
  __bis_SR_register(LPM4_bits + GIE); // Enter LPM4 with interrupts enabled
  // Processor sleeps
  // ...
  // Processor wakes up after interrupt
  // *!Remaining code in this function is only executed during the first
  // boot!*
  //
  // First time boot: Set SP, load data and initialize LRU
  __set_SP_register(&__stack_high); // Runtime stack
  fastmemcpy(&__data_low, &__data_loadLow, &__data_high - &__data_low);
  mm_init_lru();

#ifdef ALLOCATEDSTATE
  uint16_t mmdata_size = &__mmdata_end - &__mmdata_start;
  ic_update_thresholds(mmdata_size, mmdata_size);
  mm_restore();
#endif

  int main(); // Suppress implicit decl. warning
  main();
}

static void gpio_init(void) {
  // Need to initialize all ports/pins to reduce power consump'n
  P1OUT = 0; // LEDs on P1.0 and P1.1
  P1DIR = 0xff;
  P2OUT = 0;
  P2DIR = 0xff;
  P3OUT = 0;
  P3DIR = 0xff;
  P4OUT = BIT0; // Pull-up on board
  P4DIR = 0xff;
  P7OUT = 0;
  P7DIR = 0xff;
  P8OUT = 0;
  P8DIR = 0xff;

  /* Keep-alive signal to external comparator */
  P6OUT = BIT0;    // resistor enable
  P6REN = BIT0;    // Pull-up mode
  P6DIR = ~(BIT0); // All but 0 to output
  // P6OUT = BIT0;
  // P6DIR = 0xff;

  /* Interrupts from S1 and S2 buttons and P5.7 pin*/
  P5OUT = BIT5 | BIT6;           // Pull-up resistor
  P5REN = BIT5 | BIT6;           // Pull-up mode
  P5DIR = ~(BIT5 | BIT6 | BIT7); // Set all but to output direction
  P5IES = BIT5 | BIT6;           // Falling edge
  P5SEL0 = 0;
  P5SEL1 = 0;
  P5IFG = 0; // Clear pending interrupts
  // P5IE = BIT5 | BIT7;  // Enable restore irq

  // Disable GPIO power-on default high-impedance mode
  PM5CTL0 &= ~LOCKLPM5;
}

static void clock_init(void) {
  CSCTL0_H = 0xA5;           // Unlock register
  FRCTL0_H = 0xA5;           // Unlock FRAM ctrl
  FRCTL0_L = FRAM_WAIT << 4; // wait states

  /* 16 MHz clock [Source: Table 5-6 from datasheet SLASE54B] */
  CSCTL1 = DCORSEL | DCOFSEL_4; // DCO = 16 MHz

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
#ifdef ALLOCATEDSTATE
  // Save entire section
  src = &__mmdata_start;
  dst = (uint8_t *)&__mmdata_loadStart;
  len = &__mmdata_end - src;
  fastmemcpy(dst, src, len);
#else
  // Save modified pages
  nBytesToSave = mm_flush();
#endif

  // bss
  src = &__bss_low;
  dst = (uint8_t *)bss_snapshot;
  len = &__bss_high - src;
  fastmemcpy(dst, src, len);
  nBytesToSave += len;

  // data
  src = &__data_low;
  dst = (uint8_t *)data_snapshot;
  len = &__data_high - src;
  fastmemcpy(dst, src, len);
  nBytesToSave += len;

  // stack
  // stack_low-----[SP-------stack_high]
#ifdef TRACK_STACK
  src = (uint8_t *)register_snapshot[0]; // Saved SP
#else
  src = &__stack_low;
#endif
  len = &__stack_high - src;
  uint16_t offset = (uint16_t)(src - &__stack_low) / 2; // word offset
  dst = (uint8_t *)&stack_snapshot[offset];
  fastmemcpy((void *)dst, (void *)src, len);
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
  fastmemcpy(dst, src, len);

  // bss
  dst = &__bss_low;
  src = (uint8_t *)bss_snapshot;
  len = &__bss_high - dst;
  fastmemcpy(dst, src, len);

  // Restore mmdata
  mm_restore();

  // stack
#ifdef TRACK_STACK
  dst = (uint8_t *)register_snapshot[0]; // Saved stack pointer
#else
  dst = &__stack_low; // Save full stack
#endif
  len = &__stack_high - dst;
  uint16_t offset =
      (uint16_t)((uint16_t *)dst - (uint16_t *)&__stack_low); // word offset
  src = (uint8_t *)&stack_snapshot[offset];
  fastmemcpy(dst, src, len); // Restore default stack

  restore_registers(register_snapshot); // Returns to line after suspend()
}

/**
 * Set up ADC in window comparator mode to monitor supply voltage
 */
static void adc_init(void) {
  /*
   * ADC12 Configuration
   *  sample and hold time  = 4 cycles
   *  convert time  = 10 cycles
   *  tot cycles = 15
   *  Clock: 75kHz
   *  ==> Sample rate = 5kHz
   */

  ADC12CTL0 &= ~ADC12ENC; // Stop conversion
  ADC12CTL0 &= ~ADC12ON;  // Turn off
  __no_operation();       // Delay (just in case)

  ADC12CTL0 = ADC12SHT0_0 | // 4 Cycles sample time
              ADC12MSC;     // Continuous mode

  ADC12CTL1 = ADC12SSEL_0 |   // MODCLK (4.8 MHz)
              ADC12PDIV_3 |   // MODCLK/64 = 75kHz Clock
              ADC12SHS_0 |    // Software trigger for start of conversion
              ADC12CONSEQ_2 | // Repeat Single Channel
              ADC12SHP_1;     // Automatically trigger conv. after sample

  ADC12CTL2 = ADC12PWRMD_1 | // Low-power mode
              ADC12RES_1 |   // 10-bit resolution
              ADC12DF_0;     // Binary unsigned output

  ADC12CTL3 = ADC12BATMAP_1; // 1/2AVcc channel selected for ADC ch A31

  ADC12MCTL0 =
      ADC12INCH_31 | // Vcc
      ADC12VRSEL_1 | // High reference = REF (2.0V), low reference = AVCC
      ADC12WINC_1;   // Comparator window enable

  // Interrupts
  ADC12HI = restore_thr;
  ADC12LO = suspend_thr;
  ADC12IER2 = ADC12HIIE;

  // Configure internal reference
  while (REFCTL0 & REFGENBUSY)
    ;
  REFCTL0 |= REFVSEL_1 | REFON; // Select internal ref = 2.0V
  while (!(REFCTL0 & REFGENRDY))
    ;

  ADC12CTL0 |= ADC12ON; // Turn on ADC

  while (ADC12CTL1 & ADC12BUSY)
    ;
  ADC12CTL0 |= (ADC12SC | ADC12ENC); // Enable & start conversion
}

void __attribute__((__interrupt__(ADC12_B_VECTOR), optimize("O0")))
adc12_isr(void) {
  __disable_interrupt();

  switch (__even_in_range(ADC12IV, ADC12IV__ADC12RDYIFG)) {
  case ADC12IV__NONE:
    break;                  // Vector  0:  No interrupt
  case ADC12IV__ADC12HIIFG: // High Interrupt - Restore

    // Disable high interrupt and enable low interrupt
    ADC12IER2 = ADC12LOIE;

    if (needRestore) {
      if (snapshotValid) { // Restore from snapshot
        needRestore = 0;
        __set_SP_register(&__boot_stack_high); // Boot stack
        P1OUT |= BIT3;
        restore(); // **Restore returns to line after
                   // suspend()**
      } else {
        // Boot in iclib_boot
      }
    } else { // Survived power-outage, no need to restore
    }

    __bic_SR_register_on_exit(LPM4_bits); // Wake up on return
    break;
  case ADC12IV__ADC12LOIFG: // Low Interrupt - Suspend

    // Disable low interrupt and enable high
    ADC12IER2 = ADC12HIIE;

    P1OUT |= BIT4;
    snapshotValid = 0;
    suspend(register_snapshot);
    P1OUT &= ~(BIT3 | BIT4);

    //!! Execution enters this line either:
    // 1. when returning from suspend(), 2. when returning from
    // restore()
    if (suspending) { // Returning from suspend(), go to sleep
      snapshotValid = 1;
      P1OUT &= ~BIT5; // Clear active
      // P6REN &= ~BIT0;  // Disable pull-up
      P6OUT &= ~BIT0;                       // De-assert keep-alive
      __bis_SR_register_on_exit(LPM4_bits); // Sleep on return
    } else { // Returning from Restore(), continue execution
      __bic_SR_register_on_exit(LPM4_bits); // Wake up on return
    }
    break;
  default:
    break;
  }

  ADC12IFGR2 = 0;                 // Clear Interrupt flags
  __bis_SR_register_on_exit(GIE); // enable global interrupts
}

// Port 5 interrupt service routine
void __attribute__((__interrupt__(PORT5_VECTOR))) port5_isr_handler(void) {
  __disable_interrupt();
  // if (__get_SR_register() & LPM4_bits) {  // if left LPM
  if (P5IFG & BIT5) {    // Restore
    P5IE = BIT6 | BIT7;  // Disable restore irq, enable suspend irq
                         // if (needRestore) {
    if (snapshotValid) { // Restore from snapshot
      __set_SP_register(&__boot_stack_high); // Boot stack
      restore(); /* !** Restore returns to line after suspend **! */
    } else {
      // Boot in iclib_boot
    }
    //}
    __bic_SR_register_on_exit(LPM4_bits); // Wake up on return
  } else if (P5IFG & BIT6) {              // suspend (active low)
    P5IE = BIT5 | BIT7; // Disable suspend irq, enable restore irq
    snapshotValid = 0;
    suspend(register_snapshot);
    needRestore = 0;
    snapshotValid = 1;

    if (suspending) { // Returning from suspend(), go to sleep
      __bis_SR_register_on_exit(LPM4_bits); // Sleep upon return

    } else { // Returning from Restore(), continue execution
      __bic_SR_register_on_exit(LPM4_bits); // Wake up on return
    }
  }

  PCIFG &= ~(BIT5 | BIT6 | BIT7); // Clear interrupt flags
  __bis_SR_register_on_exit(GIE);
}

void ic_update_thresholds(uint16_t n_suspend, uint16_t n_restore) {
  static uint16_t suspend_old = 0;
  static uint16_t restore_old = 0;

  uint16_t untracked =
      (uint16_t)((&__data_high - &__data_low) + (&__bss_high - &__bss_low) +
                 (&__stack_high - &__stack_low));

  if (n_suspend == suspend_old && n_restore == restore_old) {
    return; // No need for updates
  }

  // newVS = V_ON + (factor*bytes_to_save)/1024
  // newVR = newVS + V_C + factor*bytes_to_restore/1024

  /*
  uint16_t newVS = (calculate_dvdb(untracked + n_suspend) + VON) >> 2;
  uint16_t newVR = (calculate_dvdb(untracked + n_restore) + newVS + V_C) >> 2;
  */

  uint16_t newVS = vdrop[(untracked + n_suspend) >> 5] + (VON >> 2);
  uint16_t newVR = vdrop[(untracked + n_restore) >> 5] + ((newVS + V_C) >> 2);

  if (newVR > (VMAX >> 2)) {
    while (1)
      ; // Error: No safe restore thr found
  }

  restore_thr = newVR;
  suspend_thr = newVS;

  ADC12HI = newVR;
  ADC12LO = newVS;
}

uint16_t calculate_dvdb(size_t nbytes) {
  return (uint16_t)((DVDT * (uint32_t)nbytes) >> 10);
}

void comparator_init() {
  // Set up 3.0 as comparator input C12
  // Should be tied to vcc/2 externally
  // P3SEL1 = BIT0;
  // P3SEL2 = BIT0;

  // Set up voltage reference
  // Configure internal reference
  // while (REFCTL0 & REFGENBUSY)
  //;
  // REFCTL0 |= REFVSEL_0 | REFON; // Select internal ref = 1.2 V
  // while (!(REFCTL0 & REFGENRDY))
  //;
  //
  // Set up reference voltage generator (resistor ladder)
  // CEREFLx = 0;
  // CEON = 1;
  // CERS = 0b01;
  // CEREF0 = 0b01000;
  // CEMRVS = 1;
  // CEMRVL = 0;
  // CECTL1 = CEMRVS_1 | CEMRVL_0 | CEON | CEPWRMD_2 | DEFDLY_1 | CEEX_0 |
  // CESHORT_0 | CEIES_1 | CEF_1 | CEOUTPOL_0;
  // uint8_t ladder_select = 0b01000;
  // CECTL2 = CEREFL_1 | CERS_1 | CERSEL_1 | ladder_select;
  //
  //// Set up comparator
  // CEPWRMD
  // CEIMSEL = CESHORT = 0 CEEX = 0 CERSEL = 0 CEON = 1 CEF = 0 CEOUTPOL =
  // 0 CEIFG = NEGEDGE;
  //
  // CEINT = CEIE;
}

void __attribute__((no_instrument_function))
__cyg_profile_func_enter(void *this_fn, void *call_site) {}

void __attribute__((no_instrument_function))
__cyg_profile_func_exit(void *this_fn, void *call_site) {
  if ((uint16_t *)__get_SP_register() < stackTrunk) {
    stackTrunk = (uint16_t *)_get_SP_register();
  }
}
