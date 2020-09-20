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

APIC_BASE  = 0xFC020000
SAPIC_BASE = 0xF803C000

APIC_IVR   = 0x10
APIC_FVR   = 0x14
APIC_EOICR = 0x38
APIC_CISR  = 0x34
APIC_SMR   = 0x04

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
    ldr pc, =undef_handler_core
    ldr pc, =svc_handler
    ldr pc, =prefetch_abort_handler
    ldr pc, =data_abort_handler_core
    .word _image_size
    ldr pc, =irq_handler
    ldr pc, =fiq_handler

.section .text

.type undef_handler_core, %function
undef_handler_core:
    sub lr, lr, #8
    mov lr, r0
    bl undef_handler

.type data_abort_handler_core, %function
data_abort_handler_core:
    bkpt
    sub lr, lr, #8
    mov lr, r0
    bl data_abort_handler

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

    /* Initialize the .bss segment */
    ldr r0, =_bss_s
    ldr r1, =_bss_e
    mov r2, #0
_bss_loop:
    cmp r0, r1
    strcc r2, [r0], #4
    bcc _bss_loop

    bl __libc_init_array
    isb
    dsb

    bl main
while:
    b while

.type irq_handler, %function
irq_handler:
    /*
     * The ARM PC allways points to the instructions being fetched. We need to 
     * adjust the link register in order to point to the instruction being 
     * executed. In ARM mode this is done by subracting 4 and in Thumb mode by
     * subtracting 2. Currently, only ARM code is supported
     */
    sub lr, lr, #4

    /*
     * The problem with nested interrupt handling is that CPSR is automatically
     * saved to SPSR_irq upon taking an exception. If another interrupt triggers
     * before the first one has finished executing the CPSR will be saved again
     * thus overwriting and corrupting the SPSR_irq. Therefore we will need to
     * push the SPSR_irq to the irq_stack before disabling the PSR I flag. The
     * following instruction stores the LR and SPSR_irq in the SP for the mode
     * specified by the last option
     */
    srsfd sp!, #SYS_MODE /* Store return state */
    cps #SYS_MODE

    /* Store AAPCS registers on the kernel stack */
    push {r0 - r3, r12}

    /*
     * The AAPCS for ABI requires the SP to be aligned with 8 bytes due to 
     * maximizing the 64-bit AXI matrix performance. The SP is always word 
     * aligned so we only need to care about bit 3
     */
    and r1, sp, #4
    sub sp, sp, r1
    push {r1, lr}

    /*
     * Get the interrupt source from the APIC. This includes reading the IVR. 
     * If a debugger is attatched it might read the IVR during the debug 
     * session. If that is the case we have two scenarios:
     *
     *   - if the pending interrupt has a higher priority than the current 
     *     interrupt the current interrupt is automaticall stacked
     *   - if no interrupt are active the read is wrong and the SPU value
     *     is returned instead
     *
     * to prevent this we use protect mode, which requires the read-only IVR 
     * to be written before the interrupt is acked
     */
    ldr r1, =APIC_BASE
    ldr r0, [r1, #APIC_IVR]
    str r1, [r1, #APIC_IVR]

    /* Whithout this line the code craches - the datasheet doesn't say why... */
    ldr r1, [r1, #APIC_SMR]

    /* Branch to handler with interrupts enabled */
    cpsie i
    blx r0
    cpsid i

    /* Acknowledge interrupt */
    ldr r0, =APIC_BASE
    str r0, [r0, #APIC_EOICR]

    pop {r1, lr}
    add sp, sp, r1

    /* Pop the AAPCS registers */
    pop {r0 - r3, r12}

    /* Return from exception */
    rfefd sp!

.type fiq_handler, %function
fiq_handler:
    stmfd sp!, {r0 - r3, r12, lr}

    ldr r0, =SAPIC_BASE
    ldr r1, [r0, APIC_CISR]
    cmp r1, #1
    beq fiq_get_vect

    /* Secure IRQ triggered */
    ldr r1, [r0, #APIC_IVR]
    str r1, [r0, #APIC_IVR]  /* Due to protected SAPIC */
    dmb
    b call_vect
fiq_get_vect:
    /* Secure IRQ triggered */
    ldr r1, [r0, #APIC_FVR]
    
call_vect:
    blx r1

    ldr r0, =SAPIC_BASE
    str r1, [r0, #APIC_EOICR]

    ldmfd sp!, {r0 - r3, r12, lr}
    subs pc, lr, #4
