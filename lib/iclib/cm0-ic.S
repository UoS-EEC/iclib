/*
 * Copyright (c) 2018-2020, University of Southampton.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

.file "ic.S"
.cpu cortex-m0
.text
.thumb

/* @function void suspend_stack_and_regs(uint32_t *saved_sp, int * snapshotValid, uint8_t *stackSnapshot)
 * @brief pushes registers to stack, and saves stack pointer to savedSp, then
 *        sleeps.
 */
.globl suspend_stack_and_regs
.type suspend_stack_and_regs, %function
suspend_stack_and_regs:
    // Push core registers to stack
    push {r0-r7,lr}
    mov r3, r8
    mov r4, r9
    mov r5, r10
    mov r6, r11
    mov r7, r12
    push {r3-r7}
    mrs r4, apsr /*# should this be xpsr?*/
    push {r4}

    // Save stack pointer in NVM @r0
    mov r7, sp
    str r7, [r0]

    // Switch to boot stack
    ldr r4, =__boot_stack_high
    mov sp, r4

    // Save main stack to NVM
    // r1: Source = sp
    // r2: Len = __stack_high - sp
    // r0: Destination = stackSnapShot + (stack_size - len)

    push {r0-r3} // Safekeep function arguments
    mov r4, r2 // tmp

    // Source
    mov r1, r7

    // Len
    ldr r2, =__stack_high
    sub r2, r2, r1

    // Destination
    mov r0, r4 // *stackSnapshot
    ldr r4, =__stack_size
    add r0, r0, r4
    sub r0, r0, r2

    bl memcpy
    pop {r0-r3}

    // Set snapshotValid
    mov r4, #1
    str r4, [r1]

    // Optionally shut off power & sleep
    cmp r3, #0
    bgt return_ssar

    bl deassert_keep_alive
    wfe
    b suspend_loop0

    // Return via restore_registers
return_ssar:
    bl restore_registers

suspend_loop0: // Should never get here
    nop
    b suspend_loop0


/* @function void suspend_regs(uint32_t *saved_sp, int * snapshotValid)
 * @brief pushes registers to stack, then sleeps.
 */
.globl suspend_regs
.type suspend_stack_and_regs, %function
suspend_regs:
    // Push core registers to stack
    push {r0-r7,lr}
    mov r3, r8
    mov r4, r9
    mov r5, r10
    mov r6, r11
    mov r7, r12
    push {r3-r7}
    mrs r4, apsr /*# should this be xpsr?*/
    push {r4}

    // Save stack pointer in NVM @r0
    mov r7, sp
    str r7, [r0]

    // Set snapshotValid
    mov r4, #1
    str r4, [r1]

    // Optionally shut off power & sleep
    cmp r2, #0
    bgt return_sr

    // Suspend execution
    bl deassert_keep_alive
    wfe
    b suspend_loop_sr

    // Return via restore_registers
return_sr:
    bl restore_registers

suspend_loop_sr: // Should never get here
    nop
    b suspend_loop_sr

/*
 * @function void suspend_and_flush_cache(uint32_t *saved_sp,
 *                                        int *snapshotValid,
 *                                        bool doReturn)
 * @brief pushes registers to stack and flushes data cache, then
 *        returns if doReturn is true, otherwise sleeps.
 */
.globl suspend_and_flush_cache
.type suspend_and_flush_cache, %function
suspend_and_flush_cache:
    // Push core registers to stack
    push {r0-r7,lr}
    mov r4, r8
    mov r5, r9
    mov r6, r10
    mov r7, r11
    push {r4-r7}
    mov r4, r12
    push {r4}
    mrs r4, apsr /* should this be xpsr?*/
    push {r4}

    // Save stack pointer in NVM @r0
    mov r7, sp
    str r7, [r0]

    // Flush cache
    mov r4, #0x2 // DCACHE_CTRL_FLUSH
    ldr r5, =0x40000020 // DCACHE_CTRL_CSR
    str r4, [r5]
  safc_loop0: // while(flushing)
    ldr r4, [r5]
    cmp r4, #0x2
    beq safc_loop0

    // Set snapshotValid
    mov r4, #1
    str r4, [r1]

    // Flush again to persist snapshotValid
    mov r4, #0x2 // DCACHE_CTRL_FLUSH
    ldr r5, =0x40000020 // DCACHE_CTRL_CSR
    str r4, [r5]
  safc_loop1: // while(flushing)
    ldr r4, [r5]
    cmp r4, #0x2
    beq safc_loop1

    // Optionally shut off power & sleep
    cmp r2, #0
    bgt return_safc

    // Suspend execution
    bl deassert_keep_alive
    wfe
    b suspend_loop_safc

    // Return via restore_registers
return_safc:
    bl restore_registers

suspend_loop_safc: // Should never get here
    nop
    b suspend_loop_safc

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
    pop {r3-r7}
    mov r8, r3
    mov r9, r4
    mov r10, r5
    mov r11, r6
    mov r12, r7
    pop {r0-r7, pc}
