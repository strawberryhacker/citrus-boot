/* Copyright (C) StrawberryHacker */

.syntax unified
.cpu cortex-a5
.arm

/* ARMv7 modes */
MODE_MASK  = 0b11111
USER_MODE  = 0b10000
FIQ_MODE   = 0b10001
IRQ_MODE   = 0b10010
SVC_MODE   = 0b10011
ABORT_MODE = 0b10111
UNDEF_MODE = 0b11011
SYS_MODE   = 0b11111

APIC  = 0xFC020000
SAPIC = 0xF803C000

APIC_IVR   = 0x10
APIC_FVR   = 0x14
APIC_EOICR = 0x38

.extern _bss_s
.extern _bss_e
.extern _user_stack_e
.extern _fiq_stack_e
.extern _irq_stack_e
.extern _abort_stack_e
.extern _svc_stack_e
.extern _undef_stack_e
.extern _image_size

.global vector_table
.section .vector_table, "ax", %progbits
vector_table:
    ldr pc, =reset_handler
    ldr pc, =undef_handler
    ldr pc, =svc_handler
    ldr pc, =prefetch_abort_handler
    ldr pc, =data_abort_handler
    .word _image_size
    ldr pc, =irq_handler
    ldr pc, =undef_handler

.section .text

.type reset_handler, %function
reset_handler:
    
    /* Disable IRQ, FIQ and asynchronous interrupts */
    cpsid ifa

    /* Setup all the stacks */
    mrs r0, cpsr
    bic r0, r0, #MODE_MASK
    orr r0, r0, #UNDEF_MODE
    msr cpsr_c, r0
    ldr sp, =_undef_stack_e

    bic r0, r0, #MODE_MASK
    orr r0, r0, #ABORT_MODE
    msr cpsr_c, r0
    ldr sp, =_abort_stack_e

    bic r0, r0, #MODE_MASK
    orr r0, r0, #IRQ_MODE
    msr cpsr_c, r0
    ldr sp, =_irq_stack_e

    bic r0, r0, #MODE_MASK
    orr r0, r0, #FIQ_MODE
    msr cpsr_c, r0
    ldr sp, =_fiq_stack_e

    bic r0, r0, #MODE_MASK
    orr r0, r0, #SYS_MODE
    msr cpsr_c, r0
    ldr sp, =_user_stack_e

    bic r0, r0, #MODE_MASK
    orr r0, r0, #SVC_MODE
    msr cpsr_c, r0
    ldr sp, =_svc_stack_e

    /* Disable I-cache, D-cache and MMU*/
    mrc p15, 0, r0, c1, c0, 0
    bic r0, r0, #(0x1 << 12)
    bic r0, r0, #(0x1 << 2)
    bic r0, r0, #(0x1 << 0)
    mcr p15, 0, r0, c1, c0, 0

    /* Invalidate I-cache */
    mov r0, #2
    mcr p15, 0, r0, c7, c5, 0

    /* Invalidate D-cache */
    mov r0, #0
    mcr p15, 2, r0, c0, c0, 0
    mrc p15, 1, r0, c0, c0, 0
    mov r1, #0x7FFF
    and r1, r1, r0, LSR #13 /* Number of sets - 1 */
    add r1, r1, #1
    mov r2, #0
set_loop:
    /* Number of ways */
    mov r3, #0
way_loop:
    mov r0, r2, LSL #5
    orr r0, r0, r3, LSL #30
    mcr p15, 0, r0, c7, c6, 2
    
    add r3, r3, #1
    cmp r3, #4
    bne way_loop
    add r2, r2, #1
    cmp r2, r1
    bne set_loop

    /* Invalidate the TLB */
    mcr p15, 0, r0, c8, c7, 0

    /* Disable the FPU */
    mov r0, #0
    mcr p15, 0, r0, c1, c0, 2

    /* Remap the internal SRAM from 0x200000 -> 0x000000 */
    ldr r0, =0x00600000
    mov r1, #1
    str r1, [r0]

    /* Switch to system mode */
    mrs r0, cpsr
    bic r0, r0, #MODE_MASK
    orr r0, r0, #SYS_MODE
    msr cpsr_c, r0

    bl __libc_init_array
    isb
    dsb

    bl main
while:
    b while

.type irq_handler, %function
irq_handler:
    sub lr, lr, #4 /* ARM mode */

    /* Save the LR_irq and the SPSR_irq to the SP_irq stack */
    srsfd sp!, #12

    /* Get the interrupt source -> APIC */
    stmfd sp!, {r0 - r3, r12}
    
    ldr r0, =APIC
    ldr r1, [r0, #APIC_IVR]
    str r1, [r0, #APIC_IVR]  /* Due to protected APIC */
    dmb

    cpsie i
    blx r1
    cpsid i

    ldr r0, =APIC
    str r1, [r0, #APIC_EOICR]

    ldmfd sp!, {r0 - r3, r12}
    rfefd sp!

.type fiq_handler, %function
fiq_handler:
    stmfd sp!, {r0 - r3, r12, lr}

    ldr r0, =SAPIC
    ldr r1, [r0, #APIC_IVR]
    str r1, [r0, #APIC_IVR]  /* Due to protected APIC */
    dmb

    cpsie f
    blx r1
    cpsid f

    ldr r0, =SAPIC
    str r1, [r0, #APIC_EOICR]

    ldmfd sp!, {r0 - r3, r12, lr}
    subs pc, lr, #4
