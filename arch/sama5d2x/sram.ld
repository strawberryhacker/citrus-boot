/* Copyright (C) StrawberryHacker */

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm","elf32-littlearm")
OUTPUT_ARCH(arm)
SEARCH_DIR(.)
ENTRY(vector_table)

/*
 * The entire bootloader is loaded into internal SRAM at address 0x00200000 by 
 * the ROM code. Therefore the entire binary will be linked at this address.
 * The internal SRAM is 128 kB but page 137 in the datasheet sets a limit of 
 * 64 kB when loading from an SD card
 */
MEMORY {
    sram (rwx) : ORIGIN = 0x00200000, LENGTH = 0x10000 /* 64 kB */
}

USER_STACK  = 512;
FIQ_STACK   = 64;
IRQ_STACK   = 64;
ABORT_STACK = 64;
SVC_STACK   = 64;
UNDEF_STACK = 64;

SECTIONS {
    .vector_table : {
        _image_s = .;
        KEEP(*(.vector_table))
    } > sram

    .text : {
        . = ALIGN(4);
        _text_s = .;
        *(.text)
        *(.text.*)
        *(.glue_7)
        *(.glue_7t)
        
        . = ALIGN(4);
        KEEP(*(.init))
        KEEP(*(.fini))

        . = ALIGN(4);
        _text_e = .;
    } > sram

    /* Black C magic */
    .ARM.extab : {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > sram

    .ARM.exidx : {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > sram

    .preinit_array : {
        KEEP(*(.preinit_array))
    } > sram

    .init_array : {
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
    } > sram

    .fini_array : {
        KEEP(*(.fini_array))
		KEEP(*(SORT(.fini_array.*)))
    } > sram

    /* Constant data */
    .rodata : {
        . = ALIGN(4);
        _rodata_s = .;
        KEEP(*(.rodata))
        KEEP(*(.rodata*))
        . = ALIGN(4);
        _rodata_e = .;
    } > sram

    /* Initialized data and ram-functions. No relocation needed */
    .data : {
        . = ALIGN(4);
        _data_s = .;
        KEEP(*(.data))
        KEEP(*(.data*))
        KEEP(*(.ramfunc))
        KEEP(*(.ramfunc*))
        . = ALIGN(4);
        _data_e = .;

        /* End of loadable memory */
        _image_e = .;
    } > sram

    /* Uninitialized data */
    .bss (NOLOAD) : {
        . = ALIGN(4);
        _bss_s = .;
        *(.bss)
        *(.bss*)
        . = ALIGN(4);
        _bss_e = .;
    } > sram

    /* Stacks */
    .stack (NOLOAD) : {
        . = ALIGN(8);

        /* User stack */
        _user_stack_s = .;
        . += USER_STACK;
        . = ALIGN(8);
        _user_stack_e = .;

        /* User stack */
        _fiq_stack_s = .;
        . += FIQ_STACK;
        . = ALIGN(8);
        _fiq_stack_e = .;

        /* User stack */
        _irq_stack_s = .;
        . += IRQ_STACK;
        . = ALIGN(8);
        _irq_stack_e = .;

        /* User stack */
        _abort_stack_s = .;
        . += ABORT_STACK;
        . = ALIGN(8);
        _abort_stack_e = .;

        /* User stack */
        _svc_stack_s = .;
        . += SVC_STACK;
        . = ALIGN(8);
        _svc_stack_e = .;

        /* User stack */
        _undef_stack_s = .;
        . += UNDEF_STACK;
        . = ALIGN(8);
        _undef_stack_e = .;
    } > sram
}

_image_size = _image_e - _image_s;
end = .;
