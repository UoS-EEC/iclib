#ifndef SRC_CONFIG_H_
#define SRC_CONFIG_H_

/* ------ Device settings ---------------------------------------------------*/
#ifndef FRAM_WAIT
#define FRAM_WAIT 15 // Number of wait states on FRAM cache miss (0-15)
#endif

/* ------ Suspend/Restore Mode ----------------------------------------------*/
#define TRACK_MMDATA
#define TRACK_STACK

/* ------ Section sizes -----------------------------------------------------*/
/* Allocates (plenty of) space for snapshot in FRAM. Note that these can be made
 * smaller for most apps*/
#define STACK_SIZE 0x1000
#define BSS_SIZE 0x1000
#define DATA_SIZE 0x2000
#define MMDATA_SIZE 0x2000

/* ------ Memory manager ----------------------------------------------------*/
#define PAGE_SIZE 128u
#define MAX_DIRTY_PAGES 10

/* ------ Threshold Calculation ---------------------------------------------*/
#define VMAX 3665 // 3.58 V maximum operating voltage
#define VON 2100  // On-voltage

// DVDT: 1024 x voltage delta per byte saved/restored
#define DVDT 60 * 5

#define V_C 205 // ~0.2 V Voltage buffer for useful compute

#endif /* SRC_CONFIG_H_ */
