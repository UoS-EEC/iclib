#include "support/cm0.h"
#include "cmsis/core_cm0.h"
#include "iclib/config.h"
#include "iclib/cm0_ic.h"
#include <string.h>

//#include "memory_management.h"
#ifdef ALLOCATEDSTATE
//#include "dvdb-AS.h"
#else
//#include "dvdb-MS.h"
#endif

// ------------- CONSTANTS -----------------------------------------------------
extern uint8_t __stack_low, __stack_high;
extern uint8_t __data_low, __data_high, __data_loadLow;
extern uint8_t __mmdata_low, __mmdata_high, __mmdata_loadLow;
extern uint8_t __boot_stack_high;
//extern uint8_t __npdata_loadLow, __npdata_low, __npdata_high;

// ------------- Globals -------------------------------------------------------

// ------------- PERSISTENT VARIABLES ------------------------------------------

// Snapshots
uint32_t saved_stack_pointer PERSISTENT;
uint32_t stack_snapshot[STACK_SIZE / sizeof(uint32_t)] PERSISTENT;

int suspending PERSISTENT;        /*! Flag to determine whether returning from
                                 suspend() or restore() */
int snapshotValid PERSISTENT = 0; //! Flag: whether snapshot is valid
int needRestore PERSISTENT = 0;   /*! Flag: whether restore is needed i.e. high
                                     when booting from a power outtage */

/* ------ Function Prototypes -----------------------------------------------*/

/* ------ ASM functions ---------------------------------------------------- */
extern void suspend(uint32_t *saved_sp);
extern void restore_registers(uint32_t *saved_sp);

/* ------ Function Declarations ---------------------------------------------*/

void __attribute__((optimize(1))) _start() {
#ifndef QUICKRECALL
  // Load npdata (non-persistent data)
  //memcpy(&__npdata_low, &__npdata_loadLow, &__npdata_high - &__npdata_low);
#endif

  needRestore = 1;                    // Indicate powerup

#ifndef QUICKRECALL
  memcpy(&__data_low, &__data_loadLow, &__data_high - &__data_low);
  //mm_init_lru();
  //mm_restore();
#endif

#ifdef ALLOCATEDSTATE
  //uint32_t mmdata_size = &__mmdata_end - &__mmdata_start;
  //ic_update_thresholds(mmdata_size, mmdata_size);
  memcpy(&__mmdata_low, &__mmdata_loadLow, &__mmdata_high - &__mmdata_low);
#endif

  if (snapshotValid){ // Restore stack from snapshot and continue execution
    // stack -- restore from saved SP to stack_high
    for (uint32_t *i= (uint32_t *)saved_stack_pointer; i <= (uint32_t *)&__stack_high; i++) {
      *i = stack_snapshot[i - (uint32_t*)&__stack_low];
    }
    restore_registers(&saved_stack_pointer); // Returns to suspend()
  }

  // First power-up: set SP and start execution
  __set_MSP((uint32_t)&__stack_high);
  int main(); // Suppress implicit decl. warning
  main();

  NVIC_SystemReset(); // Should never get here
}

void __attribute__((optimize("O1"))) suspendVM(void) {
#ifdef QUICKRECALL
  // All state is in NVM
  suspending = 1;
  return;
#endif

  // Save mmdata
  //mm_flush();
  memcpy(&__mmdata_loadLow, &__mmdata_low, &__mmdata_high - &__mmdata_low);

  // data
  memcpy(&__data_loadLow, &__data_low, &__data_high - &__data_low);

  // stack
  // stack_low-----[SP-------stack_high]
  for (uint32_t *i= (uint32_t *)__get_MSP(); i <= (uint32_t *)&__stack_high; i++) {
    stack_snapshot[i - (uint32_t*)&__stack_low] = *i;
  }

  suspending = 1;
  suspend(&saved_stack_pointer);
}

// Suspend interrupt
void Interrupt0Handler() {
    snapshotValid = 0;
    suspendVM();
    needRestore = 0;
    snapshotValid = 1;
}
