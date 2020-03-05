.file "ic.S"
.cpu cortex-m0
.text
.thumb

/* @function void suspend(uint32_t *saved_sp)
 * @brief pushes registers to stack, and saves stack pointer to savedSp, then
 *        sleeps.
 * Argument is in r0 by Arm procedure call standard
 */
.globl suspend
.type suspend, %function
suspend:
    push {r0-r7,lr}
    mrs r1, apsr /*# should this be xpsr?*/
    push {r1}
    mov r1, sp
    str r1, [r0, #0]
    wfe
    bx lr

/*
 * @function void restore_registers(uint16_t *saved_sp)
 * @brief Restores register values and returns to saved return address
 */
.globl restore_registers
.type restore_registers, %function
restore_registers:
    /* get stack pointer and recover registers */
    ldr r1, [r0, #0]
    mov sp, r1
    pop {r1}
    msr apsr_nzcvq, r1
    pop {r0}
    mov lr, r0
    pop {r0-r7}
    bx lr
