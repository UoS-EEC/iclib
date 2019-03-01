#ifndef SRC_CONFIG_H_
#define SRC_CONFIG_H_

/* ------ Suspend/Restore Mode ----------------------------------------------*/
#define TRACK_MMDATA
#define TRACK_STACK

/* ------ Section sizes -----------------------------------------------------*/
#define STACK_SIZE 0x100
#define DATA_SIZE 0x400
#define BSS_SIZE 0x400
#define MMDATA_SIZE 0x800

/* ------ Memory manager ----------------------------------------------------*/
#define PAGE_SIZE 128u
#define MAX_DIRTY_PAGES 16

/* ------ Threshold Calculation ---------------------------------------------*/
#define V_C 205    // 0.2 V
#define VMAX 3584  // 3.5V

#endif /* SRC_CONFIG_H_ */
