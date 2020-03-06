#include <stdint.h>
#include <string.h>
#include <fused.h>
#include "support/cm0.h"
#include "support/cm0-support.h"

void indicate_begin() {
   SIMPLE_MONITOR = SIMPLE_MONITOR_INDICATE_BEGIN;
;}

void indicate_end() {
  SIMPLE_MONITOR = SIMPLE_MONITOR_INDICATE_END;
}

void fused_end_simulation() {
  SIMPLE_MONITOR = SIMPLE_MONITOR_KILL_SIM;
  while(1); // Just in case
}

void wait() {
  for (volatile uint32_t i = 0; i < 10000ll; i++)
    ; // delay
}

void fused_assert (bool c) {
  if (!c) {
    SIMPLE_MONITOR = SIMPLE_MONITOR_TEST_FAIL;
    while(1);
  }
}

void target_init() { return; }

/*
__attribute__((optimize(1), naked, used, section(".ftext"))) void _start() {
  // Boot data (if necessary)
  extern uint8_t __data_low, __data_high, __data_loadLow;
  if ((&__data_loadLow != &__data_low) && (&__data_low < &__data_high)) {
    memcpy(&__data_low, &__data_loadLow, &__data_high - &__data_low + 1);
  }

  int main();
  main();
}
*/
