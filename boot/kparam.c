/* Copyright (C) strawberryhacker */

#include <c-boot/kparam.h>

/* Allocate space for the kernel parameters */
static union kparam kernel_parameters[2] __attribute__((section(".data")))= {
    {
        /* Main kernel memory has to go first */
        .mem = {
            .type = KPARAM_MEM,
            .param_size = sizeof(union kparam),
            .size = 0x8000000,
            .start = 0x20000000
        }
    },
    {
        .end = {
            .type = KPARAM_END,
            .param_size = sizeof(union kparam)
        }
    }
};

u32 get_kparam_paddr(void)
{
    return (u32)kernel_parameters;
}

void relocate_kparam(u32 addr)
{
    u32 size = sizeof(kernel_parameters);
    const u8* src = (const u8 *)kernel_parameters;
    u8* dest = (u8 *)addr;

    while (size--) {
        *dest++ = *src++;
    }
}
